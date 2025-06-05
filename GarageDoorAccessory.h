// GarageDoorAccessory.h

#pragma once
#include "Config.h"
#include "Utility.h"

class GarageDoorAccessory : public Service::AccessoryInformation {
private:
  const char* name;
  int doorPin, lightPin, reedPin;
  unsigned long lightTimeoutMs;

  bool lastReedState = true;  // true = closed (since NC), assumes closed at boot

  Characteristic::Name *accessoryName;
  SpanCharacteristic *currentState;
  SpanCharacteristic *targetState;
  SpanCharacteristic *obstruction;
  SpanCharacteristic *lightOn;
  SpanCharacteristic *lastChangeChar;
  SpanCharacteristic *cycleCountChar;

  unsigned long doorCommandTime = 0;
  bool waitingForObstructionCheck = false;
  int expectedFinalState;

  bool lightManuallyOn = false;
  unsigned long lightTurnOffTime = 0;

public:
  GarageDoorAccessory(const char *name, int doorPin, int lightPin, int reedPin, unsigned long lightTimeoutMs) :
    Service::AccessoryInformation(),
    name(name),
    doorPin(doorPin), lightPin(lightPin), reedPin(reedPin), lightTimeoutMs(lightTimeoutMs)
  {
    accessoryName = new Characteristic::Name(name);
    new Characteristic::Manufacturer("SmartGarage");
    new Characteristic::Model("Garage+Light");
    new Characteristic::SerialNumber("SG2025-" + String(doorPin));
    new Characteristic::FirmwareRevision("1.0");

    pinMode(doorPin, OUTPUT); digitalWrite(doorPin, LOW);
    pinMode(lightPin, OUTPUT); digitalWrite(lightPin, LOW);
    pinMode(reedPin, INPUT_PULLUP);

    // Garage Door Service
    new Service::GarageDoorOpener();
    currentState = new Characteristic::CurrentDoorState(Characteristic::CurrentDoorState::CLOSED);
    targetState = new Characteristic::TargetDoorState(Characteristic::TargetDoorState::CLOSED);
    obstruction = new Characteristic::ObstructionDetected(false);

    lastChangeChar = new Characteristic::Custom<String>(
      "19A10001-CA30-489F-898A-4D6CD54F9A65", formatTime(0), PERSISTENT, "Last Door Open/Close");

    cycleCountChar = new Characteristic::Custom<int>(
      "19A10002-CA30-489F-898A-4D6CD54F9A65", 0, PERSISTENT, "Total Open/Close Events");

    // Light Service
    new Service::LightBulb();
    lightOn = new Characteristic::On(false);
  }

  boolean update() override {
    if (targetState->updated()) {
      triggerDoor(HOMEKIT_EVENT);
    }

    if (lightOn->updated()) {
      toggleLight(lightOn->getNewVal(), HOMEKIT_EVENT);
    }

    return true;
  }

  void loop() override {
    checkReedSwitch();
    checkObstructionTimeout();
    checkLightTimer();
  }

  void triggerDoor(EventSource source) {
    logEvent(source, "Door Trigger", name);
    digitalWrite(doorPin, HIGH);
    delay(DOOR_ACTIVE_MS);
    digitalWrite(doorPin, LOW);

    int current = readReedClosed() ? Characteristic::CurrentDoorState::CLOSED
                                   : Characteristic::CurrentDoorState::OPEN;

    expectedFinalState = current == Characteristic::CurrentDoorState::CLOSED ?
                         Characteristic::CurrentDoorState::OPEN : Characteristic::CurrentDoorState::CLOSED;

    currentState->setVal(Characteristic::CurrentDoorState::OPENING);
    targetState->setVal(expectedFinalState == Characteristic::CurrentDoorState::CLOSED ? 1 : 0);

    doorCommandTime = millis();
    waitingForObstructionCheck = true;

    // door motion = light on (and auto off after timeout)
    if (!lightManuallyOn) {
      toggleLight(true, SMARTGARAGE_EVENT);
      lightTurnOffTime = millis() + lightTimeoutMs;
    }

    updateChangeMeta();
  }

  void toggleLight(bool on, EventSource source) {
    digitalWrite(lightPin, on ? HIGH : LOW);
    lightManuallyOn = (source == HOMEKIT_EVENT);
    logEvent(source, on ? "Light ON" : "Light OFF", name);
    lightOn->setVal(on);

    if (on && !lightManuallyOn) {
      lightTurnOffTime = millis() + lightTimeoutMs;
    }
  }

  void triggerDoorFromLoop() {
    triggerDoor(SMARTGARAGE_EVENT);
  }

  void triggerLightFromLoop() {
    toggleLight(!lightOn->getVal(), SMARTGARAGE_EVENT);
  }

  void halfOpen() {
    if (!readReedClosed()) {
      WEBLOG("[SmartGarage] Half-open aborted: %s already open", name);
      return;
    }

    logEvent(SMARTGARAGE_EVENT, "Half-Open Start", name);
    digitalWrite(doorPin, HIGH); delay(DOOR_ACTIVE_MS); digitalWrite(doorPin, LOW);
    delay(HALF_OPEN_WAIT);
    digitalWrite(doorPin, HIGH); delay(DOOR_ACTIVE_MS); digitalWrite(doorPin, LOW);

    expectedFinalState = Characteristic::CurrentDoorState::OPEN;
    currentState->setVal(Characteristic::CurrentDoorState::OPENING);
    targetState->setVal(0); // target OPEN

    doorCommandTime = millis();
    waitingForObstructionCheck = true;

    updateChangeMeta();
  }

private:
  bool readReedClosed() {
    return digitalRead(reedPin) == LOW;  // NC, LOW means closed
  }

  void updateChangeMeta() {
    cycleCountChar->setVal(cycleCountChar->getVal<int>() + 1);
    lastChangeChar->setVal(formatTime(time(nullptr)));
  }

  void checkReedSwitch() {
    bool closed = readReedClosed();
    if (closed != lastReedState) {
      lastReedState = closed;
      currentState->setVal(closed ? Characteristic::CurrentDoorState::CLOSED
                                  : Characteristic::CurrentDoorState::OPEN);
      logEvent(EXTERNAL_EVENT, closed ? "Door Closed" : "Door Opened", name);
    }
  }

  void checkObstructionTimeout() {
    if (waitingForObstructionCheck && (millis() - doorCommandTime > DOOR_OBSTRUCT_TIMEOUT)) {
      bool match = readReedClosed() == (expectedFinalState == Characteristic::CurrentDoorState::CLOSED);
      obstruction->setVal(!match);
      if (!match)
        WEBLOG("[SmartGarage] Obstruction Detected on %s", name);
      waitingForObstructionCheck = false;
    }
  }

  void checkLightTimer() {
    if (!lightManuallyOn && lightOn->getVal() && millis() > lightTurnOffTime) {
      toggleLight(false, SMARTGARAGE_EVENT);
    }
  }
};
