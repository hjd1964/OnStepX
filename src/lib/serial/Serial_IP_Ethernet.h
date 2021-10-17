// -----------------------------------------------------------------------------------
// Polling serial IP for Ethernet
#pragma once

#include "../../Common.h"

#if defined(OPERATIONAL_MODE) && (OPERATIONAL_MODE == ETHERNET_W5100 || OPERATIONAL_MODE == ETHERNET_W5500) && \
    defined(SERIAL_IP_MODE) && (SERIAL_IP_MODE == STATION || SERIAL_IP_MODE == ON)

  #include "../ethernet/EthernetManager.h"

  #ifdef ESP8266
    #ifndef ETHERNET_W5500
      #error "The ESP8266 Ethernet option supports the W5500 only"
    #endif
    #include <Ethernet2.h>  // https://github.com/adafruit/Ethernet2
  #else
    #include <Ethernet.h>
  #endif

  class IPSerial : public Stream {
    public:
      void begin(long port, unsigned long clientTimeoutMs, bool persist = false);

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

  #if defined(STANDARD_COMMAND_CHANNEL) && STANDARD_COMMAND_CHANNEL == ON
    extern IPSerial ipSerial;
    #define SERIAL_IP ipSerial
  #endif

  #if defined(PERSISTENT_COMMAND_CHANNEL) && PERSISTENT_COMMAND_CHANNEL == ON
    extern IPSerial pipSerial;
    #define SERIAL_PIP pipSerial
  #endif

#endif
