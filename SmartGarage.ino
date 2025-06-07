// SmartGarage.ino - Main Sketch

#include <WiFi.h>
#include "HomeSpan.h"
#include "Config.h"
#include "Utility.h"
#include "DEV_Services.h"

//GarageDoorAccessory *bigDoor;
//GarageDoorAccessory *smallDoor;

void setup() {
  Serial.begin(115200);

  // DHCP Hostname
  WiFi.setHostname(MODEL);

  homeSpan.setControlPin(CONTROL_PIN)
          .setStatusPin(STATUS_PIN);

  homeSpan.enableWebLog(WEBLOG_MAX_ENTRIES, NTP_SERVER, TZ, WEBLOG_URL);
  homeSpan.enableAutoStartAP()
          .setApSSID(AP_SSID)
          .setApPassword(AP_PASSWORD);

  homeSpan.begin(Category::Bridges,MODEL, MODEL, MODEL);

  new SpanAccessory();
    new Service::AccessoryInformation();
      // new Characteristic::Name((std::string(MODEL) + "-Bridge").c_str());  //this gets ignored anyway
      new Characteristic::Manufacturer(MANUFACTURER);
      new Characteristic::SerialNumber((std::string(SERIAL_NUM) + "brge").c_str());
      new Characteristic::Model(MODEL);
      new Characteristic::FirmwareRevision(FIRMWARE);
      new Characteristic::Identify();

  // --- (Big) Door 1 + Light ---
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Name(DOOR1_TILE_NAME);
      new Characteristic::Manufacturer(MANUFACTURER);
      new Characteristic::SerialNumber((std::string(SERIAL_NUM) + "-d1-" + std::to_string(DOOR1_PIN)).c_str());
      new Characteristic::Model((std::string(MODEL) + " " + std::string(SUB_MODEL)).c_str());
      new Characteristic::FirmwareRevision(FIRMWARE);
      new Characteristic::Identify();

    auto bigDoor = new DEV_GarageDoor(DOOR1_NAME, DOOR1_PIN, DOOR1_REED_PIN);
    auto bigLight = new DEV_GarageDoorLight(DOOR1_LIGHT_NAME, DOOR1_LIGHT_PIN, DOOR1_LIGHT_TIMEOUT);
    bigDoor->attachLight(bigLight);

  // --- (Small) Door 2 + Light ---
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Name(DOOR2_TILE_NAME);
      new Characteristic::Manufacturer(MANUFACTURER);
      new Characteristic::SerialNumber((std::string(SERIAL_NUM) + "-d2-" + std::to_string(DOOR2_PIN)).c_str());
      new Characteristic::Model((std::string(MODEL) + " " + std::string(SUB_MODEL)).c_str());
      new Characteristic::FirmwareRevision(FIRMWARE);
      new Characteristic::Identify();

    auto smallDoor = new DEV_GarageDoor(DOOR2_NAME, DOOR2_PIN, DOOR2_REED_PIN);
    auto smallLight = new DEV_GarageDoorLight(DOOR2_LIGHT_NAME, DOOR2_LIGHT_PIN, DOOR2_LIGHT_TIMEOUT);
    smallDoor->attachLight(smallLight);

  //  bigDoor = new GarageDoorAccessory("Big Door", BIG_DOOR_PIN, BIG_LIGHT_PIN, BIG_REED_PIN, BIG_LIGHT_TIMEOUT);
  //  smallDoor = new GarageDoorAccessory("Small Door", SMALL_DOOR_PIN, SMALL_LIGHT_PIN, SMALL_REED_PIN, SMALL_LIGHT_TIMEOUT);

  homeSpan.autoPoll();
}

void loop() {
  // Example usage:
  // bigDoor->triggerDoorFromLoop();
  // smallDoor->triggerLightFromLoop();
  // smallDoor->halfOpen();
}
