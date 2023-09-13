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
    #define sendContentAndClear(x) sendContent(x); x = "";
  #elif defined(ESP8266)
    extern ESP8266WebServer www;
    #define sendContentAndClear(x) sendContent(x); x = ""; delay(1);
  #endif

#endif
