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

      virtual size_t write(uint8_t);
      virtual size_t write(const uint8_t *, size_t);
      virtual int available(void);
      virtual int read(void);
      virtual int peek(void);
      virtual void flush(void);

      inline size_t write(unsigned long n) { return write((uint8_t)n); }
      inline size_t write(long n) { return write((uint8_t)n); }
      inline size_t write(unsigned int n) { return write((uint8_t)n); }
      inline size_t write(int n) { return write((uint8_t)n); }
      virtual int availableForWrite() { return 1; }
      using Print::write;

    private:
      WiFiClient cmdSvrClient;
      IPAddress onStep;

      bool active = false;
      bool persist = false;
      int port = -1;
      unsigned long clientTimeoutMs;
  };

  extern IPSerialClient SerialIPClient;
  #define SERIAL_IP SerialIPClient
#endif