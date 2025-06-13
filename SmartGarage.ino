// SmartGarage.ino - Main Sketch

//@@@ todo: test setting temp sensor to Name instead of Characteristic::ConfiguredName. same for lightbulb. check for warning in serial monitor

#include <WiFi.h>
#include "HomeSpan.h"
#include "WebServer8080.h"
#include "Config.h"
#include "Utility.h"
#include "DEV_GarageDoor.h"
#include "DEV_Hookii.h"

//#include "Debug_TaskStats.h"
//#include "Debug_MemReporting.h"

DEV_GarageDoor* smallDoorPtr = nullptr;

void setup() {
  Serial.begin(115200);

  // DHCP Hostname
  WiFi.setHostname(MODEL);

  homeSpan.setLogLevel(1);  // @@@ COMMENT OUT for Prod, so it defaults to 0

  homeSpan.setControlPin(CONTROL_PIN)
          .setStatusPin(STATUS_PIN)
          .enableWebLog(WEBLOG_MAX_ENTRIES, NTP_SERVER, TZ, WEBLOG_URL)
          .setApSSID(AP_SSID)
          .setApPassword(AP_PASSWORD)
  //        .setQRID("SMGA")            // QR Code & Tag generator: https://github.com/SimonGolms/homekit-code
          .enableAutoStartAP();

  homeSpan.begin(Category::Bridges,MODEL, MODEL, MODEL);
  homeSpan.setConnectionCallback(setupWeb8080);

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

    smallDoorPtr = new DEV_GarageDoor(DOOR2_NAME, DOOR2_PIN, DOOR2_REED_PIN);
    auto smallLight = new DEV_GarageDoorLight(DOOR2_LIGHT_NAME, DOOR2_LIGHT_PIN, DOOR2_LIGHT_TIMEOUT);
    smallDoorPtr->attachLight(smallLight);


  /////////////////////
  // Hookii Sensors //
  ///////////////////
  /* @@@re-ename after door testing*/
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name("HookiiSensors");
      new Characteristic::Model("ThisIsAtestModel");
    new Service::ContactSensor();
      new Characteristic::ConfiguredName("MowerStatus");
      new Characteristic::ContactSensorState(1);
      new Characteristic::StatusActive(1);
      new Characteristic::StatusFault(0);
      new Characteristic::StatusTampered(0);
      new Characteristic::StatusLowBattery(1);
    new Service::BatteryService();
      new Characteristic::ConfiguredName("BatteryLevel");
      new Characteristic::BatteryLevel(82);
      new Characteristic::ChargingState(1);
      new Characteristic::StatusLowBattery(0);
    new Service::TemperatureSensor();
      new Characteristic::ConfiguredName("batteryTemperature");
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
/*
    new Service::Fan();
      new Characteristic::ConfiguredName("Mower Status");
      new Characteristic::Active(1);
      new Characteristic::CurrentFanState(2);
      new Characteristic::TargetFanState(1);
      new Characteristic::RotationSpeed(30);
        new Service::BatteryService();
          new Characteristic::BatteryLevel(82);
          new Characteristic::ChargingState(1);
          new Characteristic::StatusLowBattery(0);
*/

/*
    new Service::Switch();
      new Characteristic::ConfiguredName("MowerStatus");
      new Characteristic::On(false);
        new Service::BatteryService();
          new Characteristic::BatteryLevel(82);
          new Characteristic::ChargingState(1);
          new Characteristic::StatusLowBattery(0);
*/

/*
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name("Hookii Battery Level");
    new Service::BatteryService();
      new Characteristic::BatteryLevel(75);
      new Characteristic::ChargingState(1);
      new Characteristic::StatusLowBattery(0);
*/

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
/* @@@re-ename after door testing */







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
  webServer.handleClient(); // http://localhost:8080

  static bool hasCalledHalfOpen = false;
  static bool hasCalledClose = false;
  static unsigned long startMillis = millis();

  if (!hasCalledHalfOpen && (millis() - startMillis > 25000)) {  // 10,000 ms = 10 seconds
    if (smallDoorPtr) {
      smallDoorPtr->halfOpen(HOOKII_EVENT);
      hasCalledHalfOpen = true;
      Serial.println("Called smallDoorPtr->halfOpen() after 20 seconds!");
    }
  }
  if (!hasCalledClose && (millis() - startMillis > 60000)) {  // 10,000 ms = 10 seconds
    if (smallDoorPtr) {
      smallDoorPtr->triggerDoorClose(HOOKII_EVENT);
      hasCalledClose = true;
      Serial.println("Called smallDoorPtr->triggerDoorClose() after 60 seconds!");
    }
  }

//  printMemorySummaryEvery(5000); // Print stats every 5 seconds
//  printTaskStatsEvery(5000);     // Print stats every 5 seconds

} // end of loop()
