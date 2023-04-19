// -----------------------------------------------------------------------------------
// Polling serial IP for Ethernet
#pragma once

#include "../../Common.h"
#include "../ethernet/EthernetManager.h"

#if (OPERATIONAL_MODE == ETHERNET_W5100 || OPERATIONAL_MODE == ETHERNET_W5500) && \
    SERIAL_SERVER != OFF

  #ifdef ESP8266
    #ifndef ETHERNET_W5500
      #error "The ESP8266 Ethernet option supports the W5500 only"
    #endif
    #include <Ethernet2.h>  // https://github.com/adafruit/Ethernet2
  #else
    #include <Ethernet.h>   // built-in library or my https://github.com/hjd1964/Ethernet for ESP32 and ASCOM Alpaca support
  #endif

  class IPSerial : public Stream {
    public:
      void begin(long port, unsigned long clientTimeoutMs = 2000, bool persist = false);

      void restart();

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
      EthernetServer *cmdSvr;
      EthernetClient cmdSvrClient;

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
    #define SERIAL_PIP1 SerialPIP1
  #endif

#endif
