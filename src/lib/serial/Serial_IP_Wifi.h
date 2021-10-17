// -----------------------------------------------------------------------------------
// Polling serial IP for ESP32
#pragma once

#include "../../Common.h"

#if defined(OPERATIONAL_MODE) && OPERATIONAL_MODE == WIFI && \
    defined(SERIAL_IP_MODE) && (SERIAL_IP_MODE == STATION || SERIAL_IP_MODE == ACCESS_POINT)

  #include "../wifi/WifiManager.h"

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

  class IPSerial : public Stream {
    public:
      void begin(long port, unsigned long clientTimeoutMs, bool persist = false);
      
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
      WiFiServer *cmdSvr;
      WiFiClient cmdSvrClient;

      int port = -1;
      unsigned long clientTimeoutMs;
      unsigned long clientEndTimeMs = 0;
      bool active = false;
      bool persist = false;

      bool accessPointEnabled = SERIAL_IP_MODE == ACCESS_POINT;
      bool stationEnabled     = SERIAL_IP_MODE == STATION || SERIAL_IP_MODE == STATION_DHCP;
      bool stationDhcpEnabled = SERIAL_IP_MODE == STATION_DHCP;

      char wifi_sta_ssid[40] = STA_SSID;
      char wifi_sta_pwd[40] = STA_PASSWORD;

      IPAddress wifi_sta_ip = IPAddress STA_IP_ADDR;
      IPAddress wifi_sta_gw = IPAddress STA_GW_ADDR;
      IPAddress wifi_sta_sn = IPAddress STA_SN_MASK;

      char wifi_ap_ssid[40] = AP_SSID;
      char wifi_ap_pwd[40] = AP_PASSWORD;
      byte wifi_ap_ch = AP_CHANNEL;

      IPAddress wifi_ap_ip = IPAddress AP_IP_ADDR;
      IPAddress wifi_ap_gw = IPAddress AP_GW_ADDR;
      IPAddress wifi_ap_sn = IPAddress AP_SN_MASK;
  };

  #if defined(STANDARD_IPSERIAL_CHANNEL) && STANDARD_IPSERIAL_CHANNEL == ON
    extern IPSerial ipSerial;
    #define SERIAL_IP ipSerial
  #endif

  #if defined(PERSISTENT_IPSERIAL_CHANNEL) && PERSISTENT_IPSERIAL_CHANNEL == ON
    extern IPSerial pipSerial;
    #define SERIAL_PIP pipSerial
  #endif
#endif
