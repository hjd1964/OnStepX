// global ethernet variables
#pragma once

#include "../../Common.h"

#if defined(OPERATIONAL_MODE) && (OPERATIONAL_MODE == ETHERNET_W5100 || OPERATIONAL_MODE == ETHERNET_W5500)
  #ifdef ESP8266
    #ifndef ETHERNET_W5500
      #error "The ESP8266 Ethernet option supports the W5500 only"
    #endif
    #include <Ethernet2.h>  // https://github.com/adafruit/Ethernet2
  #else
    #include <Ethernet.h>
  #endif

  // various global parameters to setup ethernet
  extern bool eth_active;
  extern byte eth_mac[];
  extern bool eth_dhcp_enabled;
  extern IPAddress eth_ip;
  extern IPAddress eth_dns;
  extern IPAddress eth_gw;
  extern IPAddress eth_sn;
#endif
