// -----------------------------------------------------------------------------------
// Web server
#pragma once

#include "../../../Common.h"
#include "../WifiManager.h"

#if OPERATIONAL_MODE == WIFI && WEB_SERVER == ON

  #if defined(ESP32)
    #include <WebServer.h>
  #elif defined(ESP8266)
    #include <ESP8266WebServer.h>
  #else
    #error "Configuration (Config.h): No Wifi support is present for this device"
  #endif

  #if defined(ESP32)
    extern WebServer www;
  #elif defined(ESP8266)
    extern ESP8266WebServer www;
  #endif

  // macros to help with sending webpage data, chunked
  #define sendHtmlStart() www.setContentLength(CONTENT_LENGTH_UNKNOWN); www.sendHeader("Cache-Control","no-cache"); www.send(200, "text/html", String());
  #define sendHtmlC(x) www.sendContent(x);
  #define sendHtml(x) www.sendContent(x); x = "";
  #define sendHtmlDone() www.sendContent("");

#endif
