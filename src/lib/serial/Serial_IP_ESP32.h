// -----------------------------------------------------------------------------------
// Polling serial IP for ESP32
#pragma once

#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.common.h"
#include "../../HAL/HAL.h"
#include "../../debug/Debug.h"

#if defined(ESP32) && (SERIAL_IP_MODE == STATION || SERIAL_IP_MODE == ACCESS_POINT)

  #include <WiFi.h>
  #include <WiFiClient.h>
  #include <WiFiAP.h>

  class IPSerial : public Stream
  {
    public:
      inline void begin() { begin(9999); }
      void begin(long port);
      
      void end();

      int read(void);

      int available(void);

      int peek(void);

      void flush(void);

      size_t write(uint8_t data);

      virtual size_t write(const uint8_t* data, size_t count) {
        for (int i = 0; i < count; i++) { if (!write(data[i])) return i; }
        return count;
      }
      inline size_t write(unsigned long n) { return write((uint8_t)n); }
      inline size_t write(long n) { return write((uint8_t)n); }
      inline size_t write(unsigned int n) { return write((uint8_t)n); }
      inline size_t write(int n) { return write((uint8_t)n); }

      using Print::write;

    protected:

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

      unsigned long clientTime = 0;
  };

  extern IPSerial ipSerial;
  #define SERIAL_IP ipSerial

#endif
