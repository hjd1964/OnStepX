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
#define WifiSettingsSize 236
typedef struct WifiSettings {
  char masterPassword[40];

  bool accessPointEnabled;
  bool stationEnabled;
  bool stationDhcpEnabled;

  char sta_ssid[40];
  char sta_pwd[40];

  uint8_t target1_ip[4];
  uint8_t target2_ip[4];

  uint8_t sta_ip[4];
  uint8_t sta_gw[4];
  uint8_t sta_sn[4];

  char ap_ssid[40];
  char ap_pwd[40];
  unsigned char ap_ch;

  uint8_t ap_ip[4];
  uint8_t ap_gw[4];
  uint8_t ap_sn[4];
} WifiSettings;
#pragma pack()

class WifiManager {
  public:
    void init();
    void writeSettings();

    WifiSettings settings = {
      PASSWORD_DEFAULT,
      AP_ENABLED, STA_ENABLED, STA_DHCP_ENABLED,
      STA_SSID, STA_PASSWORD,
      TARGET_IP_ADDR1,
      TARGET_IP_ADDR2,
      STA_IP_ADDR, STA_GW_ADDR, STA_SN_MASK,
      AP_SSID, AP_PASSWORD, AP_CHANNEL,
      AP_IP_ADDR, AP_GW_ADDR, AP_SN_MASK
    };

  private:
    bool active = false;

    IPAddress ap_ip;
    IPAddress ap_gw;
    IPAddress ap_sn;

    IPAddress sta_ip;
    IPAddress sta_gw;
    IPAddress sta_sn;
};

extern WifiManager wifiManager;

#endif
