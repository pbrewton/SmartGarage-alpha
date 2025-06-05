// SmartGarage.ino - Main Sketch

#include "Config.h"
#include "Utility.h"
#include "GarageDoorAccessory.h"

GarageDoorAccessory *bigDoor;
GarageDoorAccessory *smallDoor;

void setup() {
  Serial.begin(115200);

homeSpan.setControlPin(CONTROL_PIN)
         .setStatusPin(STATUS_PIN);

  homeSpan.enableWebLog(WEBLOG_MAX_ENTRIES, NTP_SERVER, TZ, WEBLOG_URL);
  homeSpan.enableAutoStartAP()
         .setApSSID(AP_SSID)
         .setApPassword(AP_PASSWORD);

  homeSpan.begin(Category::GarageDoorOpeners,"SmartGarage","SmartGarage","SmartGarage");

  bigDoor = new GarageDoorAccessory("Big Door", BIG_DOOR_PIN, BIG_LIGHT_PIN, BIG_REED_PIN, BIG_LIGHT_TIMEOUT);
  smallDoor = new GarageDoorAccessory("Small Door", SMALL_DOOR_PIN, SMALL_LIGHT_PIN, SMALL_REED_PIN, SMALL_LIGHT_TIMEOUT);

  homeSpan.autoPoll();
}

void loop() {

  // Example usage:
  // bigDoor->triggerDoorFromLoop();
  // smallDoor->triggerLightFromLoop();
  // smallDoor->halfOpen();
}
