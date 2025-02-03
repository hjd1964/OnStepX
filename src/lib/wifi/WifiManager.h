// wifi manager, used by the webserver and wifi serial IP
#pragma once

#include "../../Common.h"

#ifndef OPERATIONAL_MODE
#define OPERATIONAL_MODE OFF
#endif

#if OPERATIONAL_MODE == WIFI

#include "WifiManager.defaults.h"

#if defined(ESP32)
  #include <WiFi.h>
  #include <WiFiClient.h>
  #include <WiFiAP.h>
  #if MDNS_SERVER == ON || MDNS_CLIENT == ON
    #include <ESPmDNS.h>
  #endif
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <ESP8266WiFiAP.h>
#else
  #error "Configuration (Config.h): No Wifi support is present for this device"
#endif

typedef struct AccessPointSettings {
  char ssid[32];
  char pwd[32];
  unsigned char channel;
  uint8_t ip[4];
  uint8_t gw[4];
  uint8_t sn[4];
} AccessPointSettings;

typedef struct StationSettings {
  char ssid[33];
  char pwd[64];
  bool dhcpEnabled;
  uint8_t ip[4];
  uint8_t gw[4];
  uint8_t sn[4];
  char host[32];
  uint8_t target[4];
} StationSettings;

#ifndef WifiStationCount
  // number of wifi stations supported, between 1 and 6
  #define WifiStationCount 1
#endif
#define WifiSettingsSize (112 + WifiStationCount*146)
typedef struct WifiSettings {
  char masterPassword[32];

  bool accessPointEnabled;
  AccessPointSettings ap;

  bool stationEnabled;
  bool stationApFallback;
  StationSettings station[WifiStationCount];

} WifiSettings;

class WifiManager {
  public:
    // starts WiFi
    bool init();

    // stops WiFi
    void disconnect();

    #if STA_AUTO_RECONNECT == true
      void reconnectStation();
    #endif

    // set the currently active station
    // \param number from 1 to WifiStationCount
    void setStation(int number);

    // read settings from NV
    void readSettings();

    // write settings to NV
    void writeSettings();

    // currently selected station
    StationSettings *sta;

    // currently selected station number
    int stationNumber = 1;

    // optional station host name lookup overrides target IP
    bool staNameLookup = false;

    // true if the WiFi AP or Station is active
    bool active = false;

    WifiSettings settings = {
      PASSWORD_DEFAULT,

      AP_ENABLED,
      {
        AP_SSID, AP_PASSWORD, AP_CHANNEL,
        AP_IP_ADDR, AP_GW_ADDR, AP_SN_MASK
      },

      STA_ENABLED,
      STA_AP_FALLBACK,

      {
        #if WifiStationCount > 0
        {STA1_SSID, STA1_PASSWORD, STA1_DHCP_ENABLED, STA1_IP_ADDR, STA1_GW_ADDR, STA1_SN_MASK, STA1_HOST_NAME, STA1_TARGET_IP_ADDR},
        #endif
        #if WifiStationCount > 1
        {STA2_SSID, STA2_PASSWORD, STA2_DHCP_ENABLED, STA2_IP_ADDR, STA2_GW_ADDR, STA2_SN_MASK, STA2_HOST_NAME, STA2_TARGET_IP_ADDR},
        #endif
        #if WifiStationCount > 2
        {STA3_SSID, STA3_PASSWORD, STA3_DHCP_ENABLED, STA3_IP_ADDR, STA3_GW_ADDR, STA3_SN_MASK, STA3_HOST_NAME, STA3_TARGET_IP_ADDR},
        #endif
        #if WifiStationCount > 3
        {STA4_SSID, STA4_PASSWORD, STA4_DHCP_ENABLED, STA4_IP_ADDR, STA4_GW_ADDR, STA4_SN_MASK, STA4_HOST_NAME, STA4_TARGET_IP_ADDR},
        #endif
        #if WifiStationCount > 4
        {STA5_SSID, STA5_PASSWORD, STA5_DHCP_ENABLED, STA5_IP_ADDR, STA5_GW_ADDR, STA5_SN_MASK, STA5_HOST_NAME, STA5_TARGET_IP_ADDR},
        #endif
        #if WifiStationCount > 5
        {STA6_SSID, STA6_PASSWORD, STA6_DHCP_ENABLED, STA6_IP_ADDR, STA6_GW_ADDR, STA6_SN_MASK, STA6_HOST_NAME, STA6_TARGET_IP_ADDR},
        #endif

      }
    };

  private:
    bool settingsReady = false;
};

extern WifiManager wifiManager;

#endif
