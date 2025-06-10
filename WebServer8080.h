// WebServer8080.h
#pragma once
#include <WebServer.h>
#include "status_html.h"

WebServer webServer(8080);

inline void httpLog(const String &httpcode) {
  Serial.println("HTTP " + httpcode + " for " + webServer.uri() + " from " + webServer.client().remoteIP().toString());
}

inline void setupWeb8080(int count) {
  if(count>1) return;

  webServer.on("/", []() {
    //httpLog("200");
    //webServer.send(200, "text/html", "<h1>hello world</h1>");
    httpLog("302");
    webServer.sendHeader("Location", "/status", true);  // true = replace header
    webServer.send(302, "text/plain", "Redirecting to /status");    
  });

  webServer.on("/status", []() {
    httpLog("200");
    webServer.send(200, "text/html", STATUS_HTML);
  });

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
