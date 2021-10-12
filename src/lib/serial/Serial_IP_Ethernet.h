// -----------------------------------------------------------------------------------
// Polling serial IP for Ethernet
#pragma once

#include "../../Common.h"

#if defined(OPERATIONAL_MODE) && (OPERATIONAL_MODE == ETHERNET_W5100 || OPERATIONAL_MODE == ETHERNET_W5500)

  #include "../ethernet/Ethernet.h"

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
      inline void begin() { begin(9999); }
      void begin(long port);

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
      unsigned long timeout = 60000;
      unsigned long clientTimeout = 0;
      bool resetTimeout = false;
  };

  #if STANDARD_COMMAND_CHANNEL == ON
    extern IPSerial ipSerial;
    #define SERIAL_IP ipSerial
  #endif

  #if PERSISTENT_COMMAND_CHANNEL == ON
    extern IPSerial pipSerial;
    #define SERIAL_PIP pipSerial
  #endif

#endif
