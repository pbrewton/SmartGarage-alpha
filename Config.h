// Config.h

#pragma once

// --- Pin Definitions ---
#define BIG_DOOR_PIN        17
#define BIG_LIGHT_PIN       16
#define SMALL_DOOR_PIN      19
#define SMALL_LIGHT_PIN     18

#define BIG_REED_PIN        14
#define SMALL_REED_PIN      27

#define CONTROL_PIN         0
#define STATUS_PIN          5

// --- Access Point Settings ---
const char *AP_SSID     = "SmartGarage-setup";
const char *AP_PASSWORD = "hookii123";

// --- WebLog Settings ---
#define WEBLOG_LEVEL   50
#define NTP_SERVER     "pool.ntp.org"
#define TIMEZONE       "UTC"
#define WEBLOG_TAG     "log"

// --- Door Timing Configs ---
#define DOOR_ACTIVE_MS       500             // Time GPIO stays HIGH to activate
#define DOOR_OBSTRUCT_TIMEOUT 30000          // Milliseconds to check for obstruction
#define HALF_OPEN_WAIT       4000            // Milliseconds between pulses for half-open

#define BIG_LIGHT_TIMEOUT     300000         // ms (5 minutes)
#define SMALL_LIGHT_TIMEOUT   300000
