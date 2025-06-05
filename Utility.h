// Utility.h

#pragma once
#include <HomeSpan.h>
#include "time.h"

inline String formatTime(time_t rawTime) {
  struct tm *timeInfo = localtime(&rawTime);
  char buf[32];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", timeInfo);
  return String(buf);
}

enum EventSource {
  HOMEKIT_EVENT,
  SMARTGARAGE_EVENT,
  EXTERNAL_EVENT
};

inline void logEvent(EventSource source, const char *action, const char *target) {
  const char *prefix = "";
  switch (source) {
    case HOMEKIT_EVENT:      prefix = "[HomeKit]"; break;
    case SMARTGARAGE_EVENT:  prefix = "[SmartGarage]"; break;
    case EXTERNAL_EVENT:     prefix = "[External]"; break;
  }

  WEBLOG("%s %s: %s", prefix, action, target);
}
