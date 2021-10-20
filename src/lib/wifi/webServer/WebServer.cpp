// -----------------------------------------------------------------------------------
// Web server

#include "WebServer.h"

#if OPERATIONAL_MODE == WIFI && WEB_SERVER == ON

  #if defined(ESP32)
    WebServer www(80);
  #elif defined(ESP8266)
    ESP8266WebServer www(80);
  #endif

#endif
