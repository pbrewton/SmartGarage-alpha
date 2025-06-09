// SmartGarage.ino - Main Sketch

//@@@ todo: test setting temp sensor to Name instead of Characteristic::ConfiguredName. same for lightbulb. check for warning in serial monitor

#include <WiFi.h>
#include "HomeSpan.h"
#include "Config.h"
#include "Utility.h"
#include "DEV_GarageDoor.h"
#include "DEV_Hookii.h" 

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

  /////////////
  // Bridge //
  ///////////
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      // new Characteristic::Name((std::string(MODEL) + "-Bridge").c_str());  //this gets ignored anyway
      new Characteristic::Manufacturer(MANUFACTURER);
      new Characteristic::SerialNumber((std::string(SERIAL_NUM) + "brge").c_str());
      new Characteristic::Model(MODEL);
      new Characteristic::FirmwareRevision(FIRMWARE);

  ///////////////////////////
  // (Big) Door 1 + Light //
  /////////////////////////
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name(DOOR1_TILE_NAME);
      new Characteristic::Manufacturer(MANUFACTURER);
      new Characteristic::SerialNumber((std::string(SERIAL_NUM) + "-d1-" + std::to_string(DOOR1_PIN)).c_str());
      new Characteristic::Model((std::string(MODEL) + " " + std::string(SUB_MODEL)).c_str());
      new Characteristic::FirmwareRevision(FIRMWARE);

    auto bigDoor = new DEV_GarageDoor(DOOR1_NAME, DOOR1_PIN, DOOR1_REED_PIN);
    auto bigLight = new DEV_GarageDoorLight(DOOR1_LIGHT_NAME, DOOR1_LIGHT_PIN, DOOR1_LIGHT_TIMEOUT);
    bigDoor->attachLight(bigLight);

  /////////////////////////////
  // (Small) Door 2 + Light //
  ///////////////////////////
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name(DOOR2_TILE_NAME);
      new Characteristic::Manufacturer(MANUFACTURER);
      new Characteristic::SerialNumber((std::string(SERIAL_NUM) + "-d2-" + std::to_string(DOOR2_PIN)).c_str());
      new Characteristic::Model((std::string(MODEL) + " " + std::string(SUB_MODEL)).c_str());
      new Characteristic::FirmwareRevision(FIRMWARE);

    auto smallDoor = new DEV_GarageDoor(DOOR2_NAME, DOOR2_PIN, DOOR2_REED_PIN);
    auto smallLight = new DEV_GarageDoorLight(DOOR2_LIGHT_NAME, DOOR2_LIGHT_PIN, DOOR2_LIGHT_TIMEOUT);
    smallDoor->attachLight(smallLight);

    //@@@ ask ChatGPT is better way of doing this - calling door open/close from loop()
  //  bigDoor = new GarageDoorAccessory("Big Door", BIG_DOOR_PIN, BIG_LIGHT_PIN, BIG_REED_PIN, BIG_LIGHT_TIMEOUT);
  //  smallDoor = new GarageDoorAccessory("Small Door", SMALL_DOOR_PIN, SMALL_LIGHT_PIN, SMALL_REED_PIN, SMALL_LIGHT_TIMEOUT);


  /////////////////////
  // Hookii Sensors //
  ///////////////////
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name("Hookii Sensors");
    new Service::TemperatureSensor();
      new Characteristic::ConfiguredName("battery");
      new Characteristic::CurrentTemperature(29.0);
    new Service::TemperatureSensor();
      new Characteristic::ConfiguredName("knifeDiscMotor");
      new Characteristic::CurrentTemperature(30.0);
    new Service::TemperatureSensor();
      new Characteristic::ConfiguredName("liftMotor");
      new Characteristic::CurrentTemperature(46.0);
    new Service::TemperatureSensor();
      new Characteristic::ConfiguredName("leftDriveMotor");
      new Characteristic::CurrentTemperature(41.0);
    new Service::TemperatureSensor();
      new Characteristic::ConfiguredName("rightDriveMotor");
      new Characteristic::CurrentTemperature(24.0);

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name("Battery Level");
    new Service::BatteryService();
      new Characteristic::BatteryLevel(75);
      new Characteristic::ChargingState(1);
      new Characteristic::StatusLowBattery(0);

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name("Mow-or-Charge");
    new Service::Switch();
      new Characteristic::On(false); // default to OFF - if switch
        //new Service::BatteryService();
        //  new Characteristic::BatteryLevel(85);
        //  new Characteristic::ChargingState(1);
        //  new Characteristic::StatusLowBattery(0);


 /*  // @@@ below is from example 12-ServiceLoops.ino.  todo: migrate above to below's example, in DEV_Hookii.h

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Temp Sensor");
    new DEV_TempSensor();                                                                // Create a Temperature Sensor (see DEV_Sensors.h for definition)

  //new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Air Quality");  
    new DEV_AirQualitySensor();                                                          // Create an Air Quality Sensor (see DEV_Sensors.h for definition)
 */
  


  homeSpan.autoPoll();

} // end of setup()

void loop() {
  // Example usage:
  // bigDoor->triggerDoorFromLoop();
  // smallDoor->triggerLightFromLoop();
  // smallDoor->halfOpen(); //@@@ try this

} // end of loop()
