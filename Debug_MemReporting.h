// Debug_MemReporting.h
#pragma once
#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <LittleFS.h>

#define CHECK_SPIFFS    0
#define CHECK_LITTLEFS  0

/*
Memory Type       How to Check Usage
-----------       ------------------
SPIFFS/LittleFS   SPIFFS.totalBytes(), SPIFFS.usedBytes()
SRAM (Heap)       ESP.getFreeHeap()
PSRAM             ESP.getFreePsram(), ESP.getPsramSize()
Flash partitions  Tools > Partition Scheme / partitions.csv


Memory Type   Volatile? Typical Size  Main Use                        Speed
-----------   --------- ------------  --------                        -----
Flash         No        4-16MB        Program, data, filesystems      Slow
PROGMEM       No        Flash-based   Store const data in flash       Slow
SRAM (RAM)    Yes       520KB total   Variables, stack, code exec     Very Fast
PSRAM         Yes       4-8MB         Large buffers                   Medium
RTC Memory    Yes       ~8KB          Persist vars across deep sleep  Slow-Fast
EEPROM (emul) No        <4KB          Config/data (in flash)	        Slow

*/


inline void printMemorySummaryEvery(unsigned long interval = 2000) {
  static unsigned long lastStatsPrint = 0;
  unsigned long now = millis();
  if (now - lastStatsPrint >= interval) {
    lastStatsPrint = now;
    Serial.printf("\n=-=-=-=-= Memory Summary [%lums loop] =-=-=-=-=\n", interval);

    #if CHECK_SPIFFS
    //if (SPIFFS.begin(true)) { // Passing true will format if mount fails
      if (SPIFFS.begin(false)) {
        size_t total = SPIFFS.totalBytes();
        size_t used  = SPIFFS.usedBytes();
        float pct = total ? (100.0f * used / total) : 0;
        Serial.printf("[SPIFFS]    Total: %6lu KB, Used: %6lu KB (%.1f%%), Free: %6lu KB\n",
                      (unsigned long)(total/1024), (unsigned long)(used/1024), pct, (unsigned long)((total-used)/1024));
      } else {
        Serial.println("[SPIFFS]   Not mounted or not supported.");
      }
    #endif

    #if CHECK_LITTLEFS
    //if (LittleFS.begin(true)) { // Passing true will format if mount fails
      if (LittleFS.begin(false)) {
        size_t total = LittleFS.totalBytes();
        size_t used  = LittleFS.usedBytes();
        float pct = total ? (100.0f * used / total) : 0;
        Serial.printf("[LittleFS]   Total: %6lu KB, Used: %6lu KB (%.1f%%), Free: %6lu KB\n",
                      (unsigned long)(total/1024), (unsigned long)(used/1024), pct, (unsigned long)((total-used)/1024));
      } else {
        Serial.println("[LittleFS] Not mounted or not supported.");
      }
    #endif

    // --- Heap (SRAM) ---
    uint32_t heapTotal = ESP.getHeapSize();
    uint32_t heapFree  = ESP.getFreeHeap();
    uint32_t heapUsed  = heapTotal - heapFree;
    float    heapPct   = heapTotal ? (100.0f * heapUsed / heapTotal) : 0;
    Serial.printf("[SRAM Heap] Total: %6lu KB, Used: %6lu KB (%.1f%%), Free: %6lu KB\n",
                  (unsigned long)(heapTotal/1024), (unsigned long)(heapUsed/1024), heapPct, (unsigned long)(heapFree/1024));

    // --- PSRAM (if available) ---
    #if defined(ARDUINO_ARCH_ESP32)
      if (psramFound()) {
        uint32_t psramTotal = ESP.getPsramSize();
        uint32_t psramFree  = ESP.getFreePsram();
        uint32_t psramUsed  = psramTotal - psramFree;
        float    psramPct   = psramTotal ? (100.0f * psramUsed / psramTotal) : 0;
        Serial.printf("[PSRAM]     Total: %6lu KB, Used: %6lu KB (%.1f%%), Free: %6lu KB\n",
                      (unsigned long)(psramTotal/1024), (unsigned long)(psramUsed/1024), psramPct, (unsigned long)(psramFree/1024));
      } else {
        Serial.println("[PSRAM]     Not found.");
      }
    #endif
    Serial.println("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");
  }
}
