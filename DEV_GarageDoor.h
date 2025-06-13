// DEV_GarageDoor.h

//@@@ change to // DEV_GarageDoor.h, DEV_Hookii.h

#pragma once
#include <HomeSpan.h>
#include "Config.h"
#include "Utility.h"

//CUSTOM_CHAR_STRING(LastChangeTime, 19A10001-CA30-489F-898A-4D6CD54F9A65, PR+PW+EV, "Last Door Open/Close");
//CUSTOM_CHAR_STRING(BigTotalEvents, 19A10002-CA30-489F-898A-4D6CD54F9B65, PR+PW+EV, "Big Door Open/Close Events");
//CUSTOM_CHAR_STRING(SmallTotalEvents, 19A10002-CA30-489F-898A-4A6CD54F9B65, PR+PW+EV, "Small Door Open/Close Events");


//////////////////////////////////////////////
// Door & Light Button Pulsing Definitions //
////////////////////////////////////////////

struct PulsePin {
  int pin;
  unsigned long duration;
  bool pressed = false;
  unsigned long startTime = 0;

  PulsePin(int pin, unsigned long duration) : pin(pin), duration(duration) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }

  void beginButtonPress() {
    if (!pressed) {
      LOG2("[DEBUG] Pin %d HIGH\n",pin);
      digitalWrite(pin, HIGH);
      startTime = millis();
      pressed = true;
    }
  }

  void endButtonPress() {
    if (pressed && millis() - startTime >= duration) {
      LOG2("[DEBUG] Pin %d LOW\n",pin);
      digitalWrite(pin, LOW);
      pressed = false;
    }
  }
};

//////////////////////////////////
// GarageDoorLight Definitions //
////////////////////////////////

struct DEV_GarageDoorLight : Service::LightBulb {
  const char *name;
  PulsePin pulsePin;
  unsigned long lightTimeoutMs;
  
  SpanCharacteristic *lightSwitchOn;

  bool lightManuallyOn = false;
  unsigned long lightTurnOffTime = 0;

  DEV_GarageDoorLight(const char *name, int lightPin, unsigned long lightTimeoutMs)
      : name(name), pulsePin(lightPin, PULSE_ACTIVE_MS), lightTimeoutMs(lightTimeoutMs) {

    lightSwitchOn = new Characteristic::On(false);
              new Characteristic::ConfiguredName(name);
  }

  boolean update() override {
    toggle(lightSwitchOn->getNewVal());
    return true;
  }

  //@@@ light is not turning off after timeout
  void loop() override {
    pulsePin.endButtonPress();
    lightTurnOffTimer();
  }

  void toggle(bool on) {
    WEBLOG("[HomeKit] %s: Turned %s", name, on ? "ON" : "OFF");
    pulsePin.beginButtonPress();
    lightManuallyOn = on;

    //@@@ needed?
    //lightSwitchOn->setVal(on);
  }

  void turnOnTemporarily() {
    if (!lightManuallyOn) {
      WEBLOG("[SmartGarage] %s: Turned ON via timer", name);
      lightSwitchOn->setVal(true);                       // toggle virtual light switch state. good luck!
      lightTurnOffTime = millis() + lightTimeoutMs;
    }
  }

  void lightTurnOffTimer() {
    if (!lightManuallyOn && lightSwitchOn->getVal() && millis() > lightTurnOffTime) {
      WEBLOG("[SmartGarage] %s: Turned OFF via timer", name);
      lightSwitchOn->setVal(false);
    }
  }

//  bool isOn() {
//    return lightSwitchOn->getVal();
//  }
};

/////////////////////////////
// GarageDoor Definitions //
///////////////////////////

struct DEV_GarageDoor : Service::GarageDoorOpener {
  const char *name;
  PulsePin pulsePin;
  int reedPin;
  DEV_GarageDoorLight *light = nullptr;
  bool lastReedStateClosed = true;

  SpanCharacteristic *currentState;
  SpanCharacteristic *targetState;
  SpanCharacteristic *obstruction;
  SpanCharacteristic *lastChangeChar;
  SpanCharacteristic *cycleCountChar;

  unsigned long doorCommandTime = 0;
  bool waitingForObstructionCheck = false;
  int expectedFinalState;
  unsigned long lastReedEvent = 0;
  const unsigned long reedDebounceMs = REED_DEBOUNCE_MS;
  bool halfOpenActive = false;
  unsigned long halfOpenStartTime = 0;

  DEV_GarageDoor(const char *name, int doorPin, int reedPin)
      : name(name), pulsePin(doorPin, PULSE_ACTIVE_MS), reedPin(reedPin) {
    pinMode(reedPin, INPUT);

    lastReedStateClosed = readReedClosed();
                   new Characteristic::ConfiguredName(name);
    currentState = new Characteristic::CurrentDoorState(lastReedStateClosed ? Characteristic::CurrentDoorState::CLOSED
                                                                      : Characteristic::CurrentDoorState::OPEN);
    targetState =  new Characteristic::TargetDoorState(lastReedStateClosed ? Characteristic::TargetDoorState::CLOSED
                                                                     : Characteristic::TargetDoorState::OPEN);
    obstruction =  new Characteristic::ObstructionDetected(Characteristic::ObstructionDetected::NOT_DETECTED);
    //lastChangeChar =  new Characteristic::LastChangeTime();
    //lastChangeChar->setVal(0);
    //cycleCountChar = new Characteristic::TotalEvents("0");
    //cycleCountChar = new SpanCharacteristic("19A10002-CA30-489F-898A-4D6CD54F9B65", "string", PR+PW+EV, 0, 0, "Total Open/Close Events");
    /* still ain't working, so disabled for now
    if (strcmp(name, DOOR1_NAME) == 0) {
        cycleCountChar = new SpanCharacteristic(
          "19A10002-CA30-489F-898A-4A6CD54F9B65",  // Unique Big Door UUID
          "string",
          PR+PW+EV, 0, 0, "Big Door Open/Close Events"
        );
    } else {
        cycleCountChar = new SpanCharacteristic(
          "19A10002-CA30-489F-898A-4D6CD54F9B65",  // Unique Small Door UUID
          "string",
          PR+PW+EV, 0, 0, "Small Door Open/Close Events"
        );
    }
    cycleCountChar->setVal("0");
    */
  }

  void attachLight(DEV_GarageDoorLight *lightAccessory) {
    light = lightAccessory;
  }

  boolean update() override {
    if (targetState->updated()){
      halfOpenActive = false;
      triggerDoor(HOMEKIT_EVENT);
    } else {
      LOG0("[DEBUG] update() called on DEV_GarageDoor, but it wasn't targetState->updated(). name=%s\n", name);
      LOG0("  targetState updated: %d\n", targetState->updated());
      LOG0("  currentState updated: %d\n", currentState->updated());
      LOG0("  obstruction updated: %d\n", obstruction->updated());
    }
    return true;
  }

  void loop() override {
    pulsePin.endButtonPress();
    if (!waitingForObstructionCheck) monitorReedSwitch();
    checkForObstruction();
    halfOpenHelper();
  }

  void triggerDoor(EventSource source) {
    pulsePin.beginButtonPress();

    bool isCurrentlyClosed = readReedClosed();
    const char *direction = isCurrentlyClosed ? "Started Opening" : "Started Closing";
    logEvent(source, name, direction);

    expectedFinalState = isCurrentlyClosed ? Characteristic::CurrentDoorState::OPEN
                                           : Characteristic::CurrentDoorState::CLOSED;

    currentState->setVal(isCurrentlyClosed ? Characteristic::CurrentDoorState::OPENING
                                           : Characteristic::CurrentDoorState::CLOSING);

    //@@@ we may not need this, as targetState is set by HomeKit
    //targetState->setVal(expectedFinalState == Characteristic::CurrentDoorState::CLOSED ? 1 : 0);
    obstruction->setVal(false);
    doorCommandTime = millis();
    waitingForObstructionCheck = true;

    //updateChangeMeta();
  }

  void triggerDoorClose(EventSource source) {
    if (readReedClosed()) {
      WEBLOG("%s Close request received, but %s is already Closed", logLabel(source), name);
      return;
    }
    pulsePin.beginButtonPress();
    logEvent(source, name, "Started Closing");
    expectedFinalState = Characteristic::CurrentDoorState::CLOSED;
    currentState->setVal(Characteristic::CurrentDoorState::CLOSING);
    targetState->setVal(expectedFinalState);
    doorCommandTime = millis();
    obstruction->setVal(false);
    waitingForObstructionCheck = true;
    
  }

  void halfOpen(EventSource source) {
    if (!readReedClosed()) {
      WEBLOG("%s Half-Open request received, but %s is already Open", logLabel(source), name);
      return;
    }

    logEvent(source, name, "Half-Open Started");
    pulsePin.beginButtonPress(); // 1 of 2 button presses
    expectedFinalState = Characteristic::CurrentDoorState::OPEN;
    currentState->setVal(Characteristic::CurrentDoorState::OPENING);
    targetState->setVal(expectedFinalState);
    halfOpenStartTime = millis();
    doorCommandTime = millis();
    obstruction->setVal(false);
    waitingForObstructionCheck = true;
    halfOpenActive = true;

    //updateChangeMeta();
  }

  void halfOpenHelper() {
    // Handle 2nd button press for halfOpen() routing
    if (halfOpenActive && millis() - halfOpenStartTime > HALF_OPEN_WAIT_MS) {
      if (readReedClosed()) {
        WEBLOG("%s Half-Open failed. %s is still closed", logLabel(SMARTGARAGE_EVENT), name);
        halfOpenActive = false;
        return;
      }
      logEvent(SMARTGARAGE_EVENT, name, "Half-Open Finished");
      pulsePin.beginButtonPress(); // 2 of 2 button presses
      halfOpenActive = false;
    }
  }


private:
  // NC (Normally Closed) reed switch is LOW when door closed, HIGH when door open
  // readReedClosed() returns true when door is closed
  bool readReedClosed() {
    return digitalRead(reedPin) == LOW;
  }

  /*
  void updateChangeMeta() {
    //cycleCountChar->setVal(cycleCountChar->getVal<int>() + 1);    // @@@set max and min (see serial monitor errors)  @@@view in EVE
    const char* valStr = cycleCountChar->getString();
    int count = atoi(valStr);
    count++;
    char buf[12];  // plenty of space for int
    snprintf(buf, sizeof(buf), "%d", count);
    cycleCountChar->setVal(buf); 
    // lastChangeChar->setVal(formatTime(time(nullptr)).c_str()); // @@@ was throwing an error, so disabled for now
  }
  */

  void monitorReedSwitch() {
    bool currentReedStateClosed = readReedClosed();
    unsigned long now = millis();
    if (currentReedStateClosed != lastReedStateClosed) {
      if (now - lastReedEvent > reedDebounceMs) {
        lastReedStateClosed = currentReedStateClosed;
        lastReedEvent = now;
        currentState->setVal(currentReedStateClosed ? Characteristic::CurrentDoorState::CLOSED : Characteristic::CurrentDoorState::OPEN);
        targetState->setVal(currentState->getVal());
        logEvent(SMARTGARAGE_EVENT, name, currentReedStateClosed ? "Door Closed" : "Door Opened");
      } else {
        LOG2("[DEBUG] Reed Switch debounce ignored on %s | State: %s | Delta: %lu ms\n", name, currentReedStateClosed ? "Closed" : "Opened", now - lastReedEvent);
      }
    }
  }

  void checkForObstruction() {
    if (!waitingForObstructionCheck || (millis() - doorCommandTime < DOOR_OBSTRUCT_TIMEOUT_MS))
        return;

    bool actualClosed = readReedClosed();
    bool expectedClosed = (expectedFinalState == Characteristic::CurrentDoorState::CLOSED);
    bool match = (actualClosed == expectedClosed);

    obstruction->setVal(!match);

    if (!match) {
      char msg[80];
      snprintf(msg, sizeof(msg), "Obstruction Detected. Should be %s, but is %s",
              expectedClosed ? "Closed" : "Open",
              actualClosed   ? "Closed" : "Open");
      logEvent(SMARTGARAGE_EVENT, name, msg);

      // Update states to reflect the actual (unexpected) state
      currentState->setVal(actualClosed ? Characteristic::CurrentDoorState::CLOSED
                                      : Characteristic::CurrentDoorState::OPEN);
      targetState->setVal(currentState->getVal());
    } else {
      // debug:
      // logEvent(SMARTGARAGE_EVENT, name, "Obstruction NOT Detected");
    }

    // Wait and turn the timerLight indicator on in Home App until after door stops closing/opening,
    // otherwise in the combined tile, messages about light bulb prevents door opening/closing messages.
    // stupid Apple!
    if (light && !light->lightManuallyOn) {
      light->turnOnTemporarily();
    }

    waitingForObstructionCheck = false;
  }

};
