// -----------------------------------------------------------------------------------
// IP communication routines

#include "Serial_IP_Wifi.h"

#if defined(OPERATIONAL_MODE) && OPERATIONAL_MODE == WIFI && \
    defined(SERIAL_IP_MODE) && (SERIAL_IP_MODE == STATION || SERIAL_IP_MODE == ACCESS_POINT)

  bool wifiActive = false;
  bool port9999Assigned = false;
  bool port9998Assigned = false;

  void IPSerial::begin(long port) {
    if (port == 9999) { if (port9999Assigned) return; else port9999Assigned = true; } else
    if (port == 9998) { if (port9998Assigned) return; else port9998Assigned = true; } else return;

    this->port = port;

    cmdSvr = new WiFiServer(port);

    if (!wifiActive) {
      VLF("MSG: WiFi disconnecting");
      WiFi.disconnect();
      WiFi.softAPdisconnect(true);
      btStop();
      delay(100);

      VF("MSG: WiFi STA Enabled = "); VL(stationEnabled);
      VF("MSG: WiFi STA DHCP En = "); VL(stationDhcpEnabled);

      VF("MSG: WiFi STA SSID    = "); VL(wifi_sta_ssid);
      VF("MSG: WiFi STA PWD     = "); VL(wifi_sta_pwd);
      VF("MSG: WiFi STA IP      = "); VL(wifi_sta_ip.toString());
      VF("MSG: WiFi STA GATEWAY = "); VL(wifi_sta_gw.toString());
      VF("MSG: WiFi STA SN      = "); VL(wifi_sta_sn.toString());

      VF("MSG: WiFi AP Enabled  = "); VL(accessPointEnabled);
      VF("MSG: WiFi AP SSID     = "); VL(wifi_ap_ssid);
      VF("MSG: WiFi AP PWD      = "); VL(wifi_ap_pwd);
      VF("MSG: WiFi AP CH       = "); VL(wifi_ap_ch);
      VF("MSG: WiFi AP IP       = "); VL(wifi_ap_ip.toString());
      VF("MSG: WiFi AP GATEWAY  = "); VL(wifi_ap_gw.toString());
      VF("MSG: WiFi AP SN       = "); VL(wifi_ap_sn.toString());

    TryAgain:

      delay(1000);
      if (accessPointEnabled && !stationEnabled) {
        VLF("MSG: WiFi Starting Soft AP");
        WiFi.softAP(wifi_ap_ssid, wifi_ap_pwd, wifi_ap_ch);
        WiFi.mode(WIFI_AP);
      } else
      if (!accessPointEnabled && stationEnabled) {
        VLF("MSG: WiFi Starting Station");
        WiFi.begin(wifi_sta_ssid, wifi_sta_pwd);
        WiFi.mode(WIFI_STA);
      } else
      if (accessPointEnabled && stationEnabled) {
        VLF("MSG: WiFi Starting Soft AP");
        WiFi.softAP(wifi_ap_ssid, wifi_ap_pwd, wifi_ap_ch);
        VLF("MSG: WiFi Starting Station");
        WiFi.begin(wifi_sta_ssid, wifi_sta_pwd);
        WiFi.mode(WIFI_AP_STA);
      }

      delay(1000);
      VLF("MSG: WiFi Setting configurion");
      if (stationEnabled && !stationDhcpEnabled) WiFi.config(wifi_sta_ip, wifi_sta_gw, wifi_sta_sn);
      if (accessPointEnabled) WiFi.softAPConfig(wifi_ap_ip, wifi_ap_gw, wifi_ap_sn);

      // wait for connection in station mode, if it fails fall back to access-point mode
      if (!accessPointEnabled && stationEnabled) {
        for (int i = 0; i < 8; i++) if (WiFi.status() != WL_CONNECTED) delay(1000); else break;
        if (WiFi.status() != WL_CONNECTED) {
          VLF("MSG: WiFi Starting Station, failed");
          WiFi.disconnect(); delay(3000);
          VLF("MSG: WiFi Switching to Soft AP mode");
          stationEnabled = false;
          accessPointEnabled = true;
          goto TryAgain;
        }
      }

      VLF("MSG: WiFi is ready");
      wifiActive = true;
    }

    delay(1000);
    cmdSvr->begin();
    cmdSvr->setNoDelay(true);
    VF("MSG: WiFi started IP commandServer on port "); VL(port);

    // setup for persistent channel
    if (port == 9998) {
      timeout = 120000UL;
      resetTimeout = true;
    }

    delay(1000);
  }

  void IPSerial::end() {
    if (cmdSvrClient.connected()) {
      cmdSvrClient.stop();
    }
  }

  int IPSerial::available(void) {
    if (!wifiActive) return 0;

    if (!cmdSvrClient) {
      if (cmdSvr->hasClient()) {
        clientTimeout = millis() + timeout;
        cmdSvrClient = cmdSvr->available();
        cmdSvrClient.setTimeout(1000);
      }
    } else {
      if (!cmdSvrClient.connected()) { 
        cmdSvrClient.stop();
        return 0;
      }
      if ((long)(clientTimeout - millis()) < 0) {
        cmdSvrClient.stop();
        return 0;
      }
    }

    return cmdSvrClient.available();
  }

  int IPSerial::peek(void) {
    if (!wifiActive || !cmdSvrClient) return -1;
    return cmdSvrClient.peek();
  }

  void IPSerial::flush(void) {
    if (!wifiActive || !cmdSvrClient) return;
    cmdSvrClient.flush();
  }

  int IPSerial::read(void) {
    if (!wifiActive || !cmdSvrClient) return -1;
    if (resetTimeout) clientTimeout = millis() + timeout;
    int c = cmdSvrClient.read();
    return c;
  }

  size_t IPSerial::write(uint8_t data) {
    if (!wifiActive || !cmdSvrClient) return 0;
    return cmdSvrClient.write(data);
  }

  size_t IPSerial::write(const uint8_t *data, size_t count) {
    if (!wifiActive || !cmdSvrClient) return 0;
    return cmdSvrClient.write(data, count);
  }

  #if defined(STANDARD_IPSERIAL_CHANNEL) && STANDARD_COMMAND_CHANNEL == ON
    IPSerial ipSerial;
  #endif

  #if defined(PERSISTENT_COMMAND_CHANNEL) && PERSISTENT_COMMAND_CHANNEL == ON
    IPSerial pipSerial;
  #endif

#endif
