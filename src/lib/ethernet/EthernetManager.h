// ethernet manager, used by the webserver and ethernet serial IP
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

#pragma pack(1)
#define EthernetSettingsSize 40
typedef struct EthernetSettings {
  unsigned char mac[6];
  bool dhcp_enabled;
  IPAddress ip, dns, gw, sn;
} EthernetSettings;
#pragma pack()

class EthernetManager {
  public:
    void init();
    void restart();
    void writeSettings();

    EthernetSettings settings = {MAC, STA_DHCP_ENABLED, STA_IP_ADDR, STA_GW_ADDR, STA_GW_ADDR, STA_SN_MASK};

    bool active = false;
  private:
};

extern EthernetManager ethernetManager;

#endif
