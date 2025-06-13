// WebServer8080.h
#pragma once
#include <WebServer.h>
#include <Update.h>
#include "html.h"

WebServer webServer(8080);

inline void httpLog(const String &httpcode) {
  Serial.println("HTTP " + httpcode + " for " + webServer.uri() + " from " + webServer.client().remoteIP().toString());
}

// --- OTA Update Handler ---
inline void handleUpdateUpload() {
  HTTPUpload& upload = webServer.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("Update: %s\n", upload.filename.c_str());
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    /* flashing firmware to ESP*/
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) { //true to set the size to the current progress
      Serial.printf("Update Success: %u\nRebooting...", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
  }
}

inline void handleUpdate() {
  HTTPUpload& upload = webServer.upload();
  if (upload.status == UPLOAD_FILE_END) {
    if (Update.hasError()) {
      webServer.send(200, "text/plain", "Firmware Update Failed  :-(");
    } else {
      webServer.send(200, "text/plain", "Success! Rebooting...<br>(You may now close this page.)");
      delay(1000);
      ESP.restart();
    }
  } else if (upload.status == UPLOAD_FILE_START) {
    webServer.sendHeader("Connection", "close");
  }
}

inline void setupWeb8080(int count) {
  if(count>1) return;

/*
  // HTTP 302 Found - temporarily moved (redirect)
  webServer.on("/", []() {
    httpLog("302");
    webServer.sendHeader("Location", "/status", true);  // true = replace header
    webServer.send(302, "text/plain", "Redirecting to /status");    
  });
*/
  webServer.on("/", []() {
    httpLog("200");
    webServer.send_P(200, "text/html", HOME_HTML);
  });

  webServer.on("/status", []() {
    httpLog("200");
    webServer.send_P(200, "text/html", STATUS_HTML);
  });

  // firmware update
  webServer.on("/update", HTTP_GET, []() {
    webServer.send_P(200, "text/html", UPDATE_HTML);
  });
  webServer.on("/upload", HTTP_POST, handleUpdate, handleUpdateUpload);

  webServer.on("/favicon.ico", []() {
    httpLog("404");
    webServer.send(404, "text/plain", "Not found");
  });

  webServer.onNotFound([]() {
    httpLog("404");
    webServer.send(404, "text/plain", "Not found");
  });

  Serial.println("Starting Status Server on port 8080...");
  webServer.begin();
}
