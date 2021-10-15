// -----------------------------------------------------------------------------------
// Web server

#include "WebServer.h"

#if defined(OPERATIONAL_MODE) && OPERATIONAL_MODE == WIFI && \
   (defined(WEB_SERVER) && WEB_SERVER == ON)

  #if defined(ESP32)
    WebServer www(80);
  #elif defined(ESP8266)
    ESP8266WebServer www(80);
  #endif

#endif
