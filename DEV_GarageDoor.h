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
  unsigned long lightTimeoutMs;
  PulsePin pulsePin;
  
  SpanCharacteristic *lightOn;

  bool lightManuallyOn = false;
  unsigned long lightTurnOffTime = 0;

  DEV_GarageDoorLight(const char *name, int lightPin, unsigned long lightTimeoutMs)
    : name(name), lightTimeoutMs(lightTimeoutMs), pulsePin(lightPin, PULSE_ACTIVE_MS) {

              new Characteristic::ConfiguredName(name);
    lightOn = new Characteristic::On(false);
  }

  boolean update() override {
    toggle(lightOn->getNewVal(), false, HOMEKIT_EVENT);
    return true;
  }

  //@@@ light is not turning of after timeout
  //@@@ also, convert this to milis() pulse instead of high/low, before troubleshooting above
  void loop() override {
    pulsePin.endButtonPress();
    if (!lightManuallyOn && lightOn->getVal() && millis() > lightTurnOffTime) {
      //toggle(false, true, SMARTGARAGE_EVENT);
      WEBLOG("[SmartGarage][timer] %s: Turned OFF", name);
      lightOn->setVal(false);
    }
  }

  void toggle(bool on, bool viaTimer, EventSource source) {
    pulsePin.beginButtonPress();
    if (source == HOMEKIT_EVENT) {
      lightManuallyOn = on;
    }

    if (viaTimer) {
      WEBLOG("[SmartGarage][timer] %s: %s", name, on ? "ON" : "OFF");
    } else {
      WEBLOG("[HomeKit][manual] %s: %s", name, on ? "ON" : "OFF");
    }
    lightOn->setVal(on);        // toggle virtual light switch state. good luck!
    if (on && !lightManuallyOn)
      lightTurnOffTime = millis() + lightTimeoutMs;
  }

  void turnOnTemporarily() {
    if (!lightManuallyOn) {
      //toggle(true, true, SMARTGARAGE_EVENT);
      WEBLOG("[SmartGarage][timer] %s: Turned ON", name);
      lightOn->setVal(true);
      lightTurnOffTime = millis() + lightTimeoutMs;
    }
  }

  bool isOn() {
    return lightOn->getVal();
  }
};

/////////////////////////////
// GarageDoor Definitions //
///////////////////////////

struct DEV_GarageDoor : Service::GarageDoorOpener {
  const char *name;
  int reedPin;
  PulsePin pulsePin;
  DEV_GarageDoorLight *light = nullptr;
  bool lastReedState = true;

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

  DEV_GarageDoor(const char *name, int doorPin, int reedPin)
      : name(name), reedPin(reedPin), pulsePin(doorPin, PULSE_ACTIVE_MS) {
    pinMode(reedPin, INPUT);

    lastReedState = readReedClosed();
                      new Characteristic::ConfiguredName(name);
    currentState =    new Characteristic::CurrentDoorState(lastReedState ? Characteristic::CurrentDoorState::CLOSED : Characteristic::CurrentDoorState::OPEN);
    targetState =     new Characteristic::TargetDoorState(lastReedState ? Characteristic::TargetDoorState::CLOSED : Characteristic::TargetDoorState::OPEN);
    obstruction =     new Characteristic::ObstructionDetected(false);
    //lastChangeChar =  new Characteristic::LastChangeTime();
    //lastChangeChar->setVal(0);
    //cycleCountChar = new Characteristic::TotalEvents("0");
    //cycleCountChar = new SpanCharacteristic("19A10002-CA30-489F-898A-4D6CD54F9B65", "string", PR+PW+EV, 0, 0, "Total Open/Close Events");
    /*
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
    if (targetState->updated()) triggerDoor(HOMEKIT_EVENT);
    return true;
  }

  void loop() override {
    pulsePin.endButtonPress();
    checkReedSwitch();
    checkObstructionTimeout();
  }

  void triggerDoor(EventSource source) {
    pulsePin.beginButtonPress();

    bool isCurrentlyClosed = readReedClosed();
    const char *direction = isCurrentlyClosed ? "Started Opening" : "Started Closing";
    logEvent(source, direction, name);

    expectedFinalState = isCurrentlyClosed
        ? Characteristic::CurrentDoorState::OPEN
        : Characteristic::CurrentDoorState::CLOSED;

    currentState->setVal(
        isCurrentlyClosed
            ? Characteristic::CurrentDoorState::OPENING
            : Characteristic::CurrentDoorState::CLOSING);

    targetState->setVal(expectedFinalState == Characteristic::CurrentDoorState::CLOSED ? 1 : 0);
    obstruction->setVal(false);

    doorCommandTime = millis();
    waitingForObstructionCheck = true;

    //updateChangeMeta();
  }

  void triggerDoorFromLoop() {
    triggerDoor(SMARTGARAGE_EVENT);
  }

  void halfOpen() {
    if (!readReedClosed()) {
      WEBLOG("[SmartGarage] Half-open aborted: %s already open", name);
      return;
    }

    logEvent(SMARTGARAGE_EVENT, "Half-Open Start", name);
    pulsePin.beginButtonPress();
    delay(HALF_OPEN_WAIT); //@@@ milis() conversion
    pulsePin.beginButtonPress();
    expectedFinalState = Characteristic::CurrentDoorState::OPEN;
    currentState->setVal(Characteristic::CurrentDoorState::OPENING);
    targetState->setVal(0);

    doorCommandTime = millis();
    waitingForObstructionCheck = true;

    //updateChangeMeta();
  }

  bool isClosed() {
    return readReedClosed();
  }

private:
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

  void checkReedSwitch() {
    bool state = readReedClosed();
    unsigned long now = millis();
    if (state != lastReedState) {
      if (now - lastReedEvent > reedDebounceMs) {
        lastReedState = state;
        lastReedEvent = now;
        currentState->setVal(state ? Characteristic::CurrentDoorState::CLOSED : Characteristic::CurrentDoorState::OPEN);
        targetState->setVal(state ? Characteristic::CurrentDoorState::CLOSED : Characteristic::CurrentDoorState::OPEN);
        logEvent(SMARTGARAGE_EVENT, state ? "Door Closed" : "Door Opened", name);
      } else {
        LOG2("[DEBUG] Reed Switch debounce ignored on %s | State: %s | Delta: %lu ms\n", name, state ? "Closed" : "Opened", now - lastReedEvent);
      }
    }
  }

  void checkObstructionTimeout() {
    if (waitingForObstructionCheck && (millis() - doorCommandTime > DOOR_OBSTRUCT_TIMEOUT)) {
      bool match = readReedClosed() == (expectedFinalState == Characteristic::CurrentDoorState::CLOSED);
      obstruction->setVal(!match);
      if (!match) {
        char msg[80];
        snprintf(msg, sizeof(msg), "Obstruction Detected. Should be %s, but is %s",
                expectedFinalState ? "closed" : "open",
                readReedClosed() ? "closed" : "open");
        logEvent(SMARTGARAGE_EVENT, msg, name);
        bool state = readReedClosed()
          ? Characteristic::CurrentDoorState::CLOSED
          : Characteristic::CurrentDoorState::OPEN;
        currentState->setVal(state ? Characteristic::CurrentDoorState::CLOSED : Characteristic::CurrentDoorState::OPEN);
        targetState->setVal(state ? Characteristic::CurrentDoorState::CLOSED : Characteristic::CurrentDoorState::OPEN);
      } else {
        //logEvent(SMARTGARAGE_EVENT, "Obstruction NOT Detected", name); //@@@ say "Finished (opening)?(closing): DOOR_NAME"
      }

      if (light && !light->lightManuallyOn) {
        light->turnOnTemporarily();
      }
  
      waitingForObstructionCheck = false;
    }
  }
};
