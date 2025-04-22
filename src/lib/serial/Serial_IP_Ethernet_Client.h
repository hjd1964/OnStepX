// -----------------------------------------------------------------------------------
// Polling serial IP for Ethernet
#pragma once

#include "../../Common.h"
#include "../ethernet/EthernetManager.h"

#if OPERATIONAL_MODE >= ETHERNET_FIRST && OPERATIONAL_MODE <= ETHERNET_LAST && SERIAL_CLIENT != OFF

  #ifdef ESP8266
    #ifndef ETHERNET_W5500
      #error "The ESP8266 Ethernet option supports the W5500 only"
    #endif
    #include <Ethernet2.h>  // https://github.com/adafruit/Ethernet2
  #else
    #if OPERATIONAL_MODE == ETHERNET_TEENSY41
      #include <NativeEthernet.h>
    #else
      #include <Ethernet.h>   // built-in library or my https://github.com/hjd1964/Ethernet for ESP32 and ASCOM Alpaca support
    #endif
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
      EthernetClient cmdSvrClient;
      IPAddress onStep;

      int port = -1;
      unsigned long lastActivityTimeMs = 0;
      unsigned long clientTimeoutMs;
      unsigned long clientEndTimeMs = 0;
      bool active = false;
      bool persist = false;
  };

  extern IPSerialClient SerialIPClient;
  #define SERIAL_IP SerialIPClient

#endif
