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
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <ESP8266WiFiAP.h>
#else
  #error "Configuration (Config.h): No Wifi support is present for this device"
#endif

#pragma pack(1)

typedef struct AccessPointSettings {
  char ssid[40];
  char pwd[40];
  unsigned char channel;
  uint8_t ip[4];
  uint8_t gw[4];
  uint8_t sn[4];
} AccessPointSettings;

typedef struct StationSettings {
  char ssid[40];
  char pwd[40];
  bool dhcpEnabled;
  uint8_t target[4];
  uint8_t ip[4];
  uint8_t gw[4];
  uint8_t sn[4];
} StationSettings;

#define WifiSettingsSize 331
typedef struct WifiSettings {
  char masterPassword[40];

  bool accessPointEnabled;
  AccessPointSettings ap;

  bool stationEnabled;
  bool stationApFallback;
  bool stationAltFallback;
  StationSettings sta1;
  StationSettings sta2;

} WifiSettings;
#pragma pack()

class WifiManager {
  public:
    bool init();
    void writeSettings();

    StationSettings *sta;

    WifiSettings settings = {
      PASSWORD_DEFAULT,

      AP_ENABLED,
      {
        AP_SSID, AP_PASSWORD, AP_CHANNEL,
        AP_IP_ADDR, AP_GW_ADDR, AP_SN_MASK
      },

      STA_ENABLED,
      STA_AP_FALLBACK,
      STA_ALT_FALLBACK,
      {
        STA_SSID, STA_PASSWORD, STA_DHCP_ENABLED,
        STA_TARGET_IP_ADDR, STA_IP_ADDR, STA_GW_ADDR, STA_SN_MASK
      },

      {
        STA_SSID_ALT, STA_PASSWORD_ALT, STA_DHCP_ENABLED_ALT,
        STA_TARGET_IP_ADDR_ALT, STA_IP_ADDR_ALT, STA_GW_ADDR_ALT, STA_SN_MASK_ALT
      }
    };

  private:
    bool active = false;
};

extern WifiManager wifiManager;

#endif
