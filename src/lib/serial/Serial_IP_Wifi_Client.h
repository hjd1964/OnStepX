// -----------------------------------------------------------------------------------
// IP communication routines

// original work by jesco-t

#pragma once

#include "../../Common.h"
#include "../wifi/WifiManager.h"

#if OPERATIONAL_MODE == WIFI && SERIAL_CLIENT == ON

  #if defined(ESP32)
    #include <WiFi.h>
    #include <WiFiClient.h>
    #include <WiFiAP.h>
  #elif defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <WiFiClient.h>
    #include <ESP8266WiFiAP.h>
  #else
    #error "Configuration (Config.h): No Wifi support is present for this device"
  #endif

  class IPSerialClient : public Stream {
    public:
      bool begin(long port, unsigned long clientTimeoutMs = 2000, bool persist = false);
      void end();
      bool isConnected();

      void flush(void);
      int available(void);
      int peek(void);
      int read(void);
      int availableForWrite() { return 1; }
      size_t write(uint8_t data);
      size_t write(const uint8_t* data, size_t count);
      inline size_t write(unsigned long n) { return write((uint8_t)n); }
      inline size_t write(long n) { return write((uint8_t)n); }
      inline size_t write(unsigned int n) { return write((uint8_t)n); }
      inline size_t write(int n) { return write((uint8_t)n); }

      using Print::write;

      void poll();

    private:
      WiFiClient cmdSvrClient;
      IPAddress onStep;

      int port = -1;
      unsigned long lastActivityTimeMs = 0;
      unsigned long clientTimeoutMs;
      bool active = false;
      bool persist = false;
  };

  extern IPSerialClient SerialIPClient;
  #define SERIAL_IP SerialIPClient
#endif