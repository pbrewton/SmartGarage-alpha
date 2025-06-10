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
  HOOKII_EVENT,
  SMARTGARAGE_EVENT,
  EXTERNAL_EVENT
};

inline const char* logLabel(EventSource source) {
  switch (source) {
    case HOMEKIT_EVENT:      return "[HomeKit]";
    case HOOKII_EVENT:       return "[Hookii]";
    case SMARTGARAGE_EVENT:  return "[SmartGarage]";
    case EXTERNAL_EVENT:     return "[External]";
    default:                 return "[Unknown]";
  }
}

inline void logEvent(EventSource source, const char *device, const char *msg) {
  const char *prefix = logLabel(source);
  WEBLOG("%s %s: %s", prefix, device, msg);
}
