// ethernet manager, used by the webserver and ethernet serial IP
#pragma once

#include "../../Common.h"

#ifndef OPERATIONAL_MODE
#define OPERATIONAL_MODE OFF
#endif

#if OPERATIONAL_MODE >= ETHERNET_FIRST && OPERATIONAL_MODE <= ETHERNET_LAST

#include "EthernetManager.defaults.h"

#ifdef ESP8266
  #ifndef ETHERNET_W5500
    #error "The ESP8266 Ethernet option supports the W5500 only"
  #endif
  #include <Ethernet2.h>     // https://github.com/adafruit/Ethernet2
#else
  #if OPERATIONAL_MODE == ETHERNET_TEENSY41
    #include <NativeEthernet.h>      // built-in library or my https://github.com/hjd1964/Ethernet for ESP32 and ASCOM Alpaca support
  #else
    #include <Ethernet.h>      // built-in library or my https://github.com/hjd1964/Ethernet for ESP32 and ASCOM Alpaca support
    #if MDNS_SERVER == ON
      #include <EthernetUdp.h> // built-in library
      #include <ArduinoMDNS.h> // https://www.arduino.cc/reference/en/libraries/arduinomdns/
    #endif
  #endif
#endif

typedef struct EthernetStationSettings {
  char host[32];
  uint8_t target[4];
} EthernetStationSettings;

#ifndef EthernetStationCount
  // number of ethernet stations supported, between 1 and 6
  #define EthernetStationCount 1
#endif
#define EthernetSettingsSize (72 + 36*EthernetStationCount)
typedef struct EthernetSettings {
  char masterPassword[32];

  unsigned char mac[6];
  bool dhcpEnabled;
  IPAddress ip, dns, gw, sn;

  EthernetStationSettings station[EthernetStationCount];
} EthernetSettings;

class EthernetManager {
  public:
    // starts Ethernet
    bool init();

    // resets Ethernet
    void restart();

    // Ethernet disconnect is ignored!
    void disconnect();

    // set the currently active station
    // \param number from 1 to WifiStationCount
    void setStation(int number);

    void readSettings();
    void writeSettings();

    // currently selected station
    EthernetStationSettings *sta;

    // currently selected station number
    int stationNumber = 1;

    // optional station host name lookup overrides target IP (not implemented!)
    bool staNameLookup = false;

    EthernetSettings settings = {
      PASSWORD_DEFAULT,
      MAC,
      STA_DHCP_ENABLED,
      STA_IP_ADDR, STA_GW_ADDR, STA_GW_ADDR, STA_SN_MASK,
      {
      #if EthernetStationCount > 0
        {STA1_HOST_NAME, STA1_TARGET_IP_ADDR},
      #endif
      #if EthernetStationCount > 1
        {STA2_HOST_NAME, STA2_TARGET_IP_ADDR},
      #endif
      #if EthernetStationCount > 2
        {STA3_HOST_NAME, STA3_TARGET_IP_ADDR},
      #endif
      #if EthernetStationCount > 3
        {STA4_HOST_NAME, STA4_TARGET_IP_ADDR},
      #endif
      #if EthernetStationCount > 4
        {STA5_HOST_NAME, STA5_TARGET_IP_ADDR},
      #endif
      #if EthernetStationCount > 5
        {STA6_HOST_NAME, STA6_TARGET_IP_ADDR},
      #endif
      }
    };

    bool active = false;
    bool settingsReady = false;
  private:
};

extern EthernetManager ethernetManager;

#endif
