// Config.h

#pragma once
#define MANUFACTURER        "pbrewton"
#define MODEL               "SmartGarage"
#define SUB_MODEL           "DnL"
#define FIRMWARE            "0.9"
#define SERIAL_NUM          "SG2025"

#define DOOR1_NAME          "Big Door"
#define DOOR1_LIGHT_NAME    "Big Door Light"
//#define DOOR1_LIGHT_NAME  "BDL"
#define DOOR1_TILE_NAME     "Big Door & Light"
#define DOOR1_PIN            4
#define DOOR1_LIGHT_PIN      2
#define DOOR1_REED_PIN       14
//#define DOOR1_LIGHT_TIMEOUT  300000         // ms (5 minutes)
#define DOOR1_LIGHT_TIMEOUT  10000         // 10sec

#define DOOR2_NAME          "Small Door"
#define DOOR2_LIGHT_NAME    "Small Door Light"
//#define DOOR2_LIGHT_NAME  "SDL"
#define DOOR2_TILE_NAME     "Small Door & Light"
#define DOOR2_PIN            19
#define DOOR2_LIGHT_PIN      18
#define DOOR2_REED_PIN       27
//#define DOOR2_LIGHT_TIMEOUT  300000
#define DOOR2_LIGHT_TIMEOUT  10000

#define CONTROL_PIN          0   // built-in LED
#define STATUS_PIN           5

// --- WiFi Setup AP ---
const char *AP_SSID     =   "SmartGarage-setup";
const char *AP_PASSWORD =   "hookii123";

// --- WebLog Settings ---
#define WEBLOG_MAX_ENTRIES  50
#define NTP_SERVER          "pool.ntp.org"
#define TZ                  "UTC"
#define WEBLOG_URL          "log"

// --- Door Timing Configs ---
#define PULSE_ACTIVE_MS            500            // How long GPIO stays HIGH / Button is pressed
//#define DOOR_OBSTRUCT_TIMEOUT_MS 30000          // Elapsed time after door open/close to check for obstruction (30 seconds)
#define DOOR_OBSTRUCT_TIMEOUT_MS   15000          // Elapsed time after door open/close to check for obstruction (15 seconds)
#define HALF_OPEN_WAIT_MS          4000           // How long before stopping a partial door open
#define REED_DEBOUNCE_MS           1000           // Milliseconds to debounce reed switch
