// Debug_TaskStats.h
#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// use from loop() with a pause in printTaskStatsEvery(ms)
// or from setup() with printTaskStatsEvery(0)

inline void printTaskStatsEvery(unsigned long interval = 2000) {
  static unsigned long lastStatsPrint = 0;
  unsigned long now = millis();
  if (now - lastStatsPrint >= interval) {
    lastStatsPrint = now;
    char pcWriteBuffer[1024];
    Serial.printf("\n<><><><> Task Stats [%lums loop] <><><><>\n", interval);
    Serial.print("Task            Abs Time      % Time\n");
    vTaskGetRunTimeStats(pcWriteBuffer);
    Serial.println(pcWriteBuffer);
  Serial.println("<><><><><><><><><><><><><><><><><><><><><>");
  }
}
