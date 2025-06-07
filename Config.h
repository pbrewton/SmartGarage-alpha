// Config.h

#pragma once
#define MANUFACTURER        "pbrewton"
#define MODEL               "SmartGarage"
#define SUB_MODEL           "DnL"
#define FIRMWARE            "0.9"
#define SERIAL_NUM          "SG2025"

#define DOOR1_NAME          "Big Door"
#define DOOR1_LIGHT_NAME    "Big Door Light"
//#define DOOR1_LIGHT_NAME    "BDL"
#define DOOR1_TILE_NAME     "Big Door & Light"
#define DOOR1_PIN            17
#define DOOR1_LIGHT_PIN      16
#define DOOR1_REED_PIN       14
//#define DOOR1_LIGHT_TIMEOUT  300000         // ms (5 minutes)
#define DOOR1_LIGHT_TIMEOUT  15000         // 15sec

#define DOOR2_NAME          "Small Door"
#define DOOR2_LIGHT_NAME    "Small Door Light"
//#define DOOR2_LIGHT_NAME    "SDL"
#define DOOR2_TILE_NAME     "Small Door & Light"
#define DOOR2_PIN            19
#define DOOR2_LIGHT_PIN      18
#define DOOR2_REED_PIN       27
#define DOOR2_LIGHT_TIMEOUT  300000

#define CONTROL_PIN         0   // built-in LED
#define STATUS_PIN          5

// --- WiFi Setup AP ---
const char *AP_SSID     = "SmartGarage-setup";
const char *AP_PASSWORD = "hookii123";

// --- WebLog Settings ---
#define WEBLOG_MAX_ENTRIES  50
#define NTP_SERVER          "pool.ntp.org"
#define TZ                  "UTC"
#define WEBLOG_URL          "log"

// --- Door Timing Configs ---
#define DOOR_ACTIVE_MS       500             // Time GPIO stays HIGH to activate
//#define DOOR_OBSTRUCT_TIMEOUT 30000          // Milliseconds to check for obstruction
#define DOOR_OBSTRUCT_TIMEOUT 15000          // Milliseconds to check for obstruction
#define HALF_OPEN_WAIT       4000            // Milliseconds between pulses for half-open

