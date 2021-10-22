// -----------------------------------------------------------------------------------
// Polling serial IP for ESP32
#pragma once

#include "../../Common.h"
#include "../wifi/WifiManager.h"

#if OPERATIONAL_MODE == WIFI && SERIAL_SERVER != OFF

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

  class IPSerial : public Stream {
    public:
      void begin(long port, unsigned long clientTimeoutMs = 2000, bool persist = false);
      
      void end();

      int read(void);

      int available(void);

      int peek(void);

      void flush(void);

      size_t write(uint8_t data);

      size_t write(const uint8_t* data, size_t count);

      inline size_t write(unsigned long n) { return write((uint8_t)n); }
      inline size_t write(long n) { return write((uint8_t)n); }
      inline size_t write(unsigned int n) { return write((uint8_t)n); }
      inline size_t write(int n) { return write((uint8_t)n); }

      using Print::write;

    private:
      WiFiServer *cmdSvr;
      WiFiClient cmdSvrClient;

      int port = -1;
      unsigned long clientTimeoutMs;
      unsigned long clientEndTimeMs = 0;
      bool active = false;
      bool persist = false;
  };

  #if SERIAL_SERVER == STANDARD || SERIAL_SERVER == BOTH
    extern IPSerial SerialIP;
    #define SERIAL_SIP SerialIP
  #endif

  #if SERIAL_SERVER == PERSISTENT || SERIAL_SERVER == BOTH
    extern IPSerial SerialPIP1;
    extern IPSerial SerialPIP2;
    extern IPSerial SerialPIP3;
    #define SERIAL_PIP1 SerialPIP1
    #define SERIAL_PIP2 SerialPIP2
    #define SERIAL_PIP3 SerialPIP3
  #endif

#endif
