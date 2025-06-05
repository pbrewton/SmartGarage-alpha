// SmartGarage.ino - Main Sketch
#include "Config.h"
#include "Utility.h"
#include "GarageDoorAccessory.h"

GarageDoorAccessory *bigGarage;
GarageDoorAccessory *smallGarage;

void setup() {
  Serial.begin(115200);

  homeSpan.setControlPin(CONTROL_PIN)
         .setStatusPin(STATUS_PIN);

  homeSpan.enableWebLog(WEBLOG_LEVEL, NTP_SERVER, TIMEZONE, WEBLOG_TAG); //@@@ change WEBLOG_MAX_ENTRIES, WEBLOG_URL, 
  homeSpan.enableAutoStartAP()
         .setApSSID(AP_SSID)
         .setApPassword(AP_PASSWORD);

  homeSpan.begin(Category::GarageDoorOpeners,"SmartGarage");

  bigGarage = new GarageDoorAccessory("Big Garage", BIG_DOOR_PIN, BIG_LIGHT_PIN, BIG_REED_PIN, BIG_LIGHT_TIMEOUT);
  smallGarage = new GarageDoorAccessory("Small Garage", SMALL_DOOR_PIN, SMALL_LIGHT_PIN, SMALL_REED_PIN, SMALL_LIGHT_TIMEOUT);

  homeSpan.autoPoll();
}

void loop() {

  // Example usage:
  // bigGarage->triggerDoorFromLoop();
  // smallGarage->triggerLightFromLoop();
  // smallGarage->halfOpen();
}
