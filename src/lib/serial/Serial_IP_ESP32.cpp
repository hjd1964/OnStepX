// -----------------------------------------------------------------------------------
// IP communication routines

#include "Serial_IP_ESP32.h"

#if defined(ESP32) && (SERIAL_IP_MODE == STATION || SERIAL_IP_MODE == ACCESS_POINT)

  bool wifiActive = false;

  WiFiServer cmdSvr(9999);
  WiFiClient cmdSvrClient;
  bool port9999Assigned = false;

//  WiFiServer cmdSvrP(9998);
//  WiFiClient cmdSvrClientP;
  bool port9998Assigned = false;

  void IPSerial::begin(long port) {
    if (port != 9999 && port != 9998) return;
    if (port == 9999) { if (port9999Assigned) return; else port9999Assigned = true; }
    if (port == 9998) { if (port9998Assigned) return; else port9998Assigned = true; }

    if (!wifiActive) {
      VLF("MSG: WiFi disconnecting");
      WiFi.disconnect();
      WiFi.softAPdisconnect(true);
      btStop();
      delay(20000);

      VF("MSG: WiFi PORT = "); VL(port);

      VF("MSG: WiFi AP Enabled  = "); VL(accessPointEnabled);
      VF("MSG: WiFi Sta Enabled = "); VL(stationEnabled);
      VF("MSG: WiFi Sta DHCP En = "); VL(stationDhcpEnabled);

      VF("MSG: WiFi STA SSID    = "); VL(wifi_sta_ssid);
      VF("MSG: WiFi STA PWD     = "); VL(wifi_sta_pwd);
      VF("MSG: WiFi STA IP      = "); VL(wifi_sta_ip.toString());
      VF("MSG: WiFi STA GATEWAY = "); VL(wifi_sta_gw.toString());
      VF("MSG: WiFi STA SN      = "); VL(wifi_sta_sn.toString());

      VF("MSG: WiFi AP SSID     = "); VL(wifi_ap_ssid);
      VF("MSG: WiFi AP PWD      = "); VL(wifi_ap_pwd);
      VF("MSG: WiFi AP CH       = "); VL(wifi_ap_ch);
      VF("MSG: WiFi AP IP       = "); VL(wifi_ap_ip.toString());
      VF("MSG: WiFi AP GATEWAY  = "); VL(wifi_ap_gw.toString());
      VF("MSG: WiFi AP SN       = "); VL(wifi_ap_sn.toString());

    TryAgain:
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
        for (int i=0; i<8; i++) if (WiFi.status() != WL_CONNECTED) delay(1000); else break;
        if (WiFi.status() != WL_CONNECTED) {
          VLF("MSG: WiFi Starting Station, failed");
          WiFi.disconnect(); delay(3000);
          VLF("MSG: WiFi Switching to Soft AP mode");
          stationEnabled = false;
          accessPointEnabled = true;
          goto TryAgain;
        }
      }
      delay(1000);

      VLF("MSG: WiFi is ready");
    }
    wifiActive = true;
  }

  void IPSerial::end() {
    if (cmdSvrClient.connected()) {
      cmdSvrClient.stop();
      VLF("MSG: IPSerial::stop() cmdSvrClient stopped");
    }
  }

  int IPSerial::available(void) {
    if (!cmdSvrClient) {
      if (cmdSvr.hasClient()) {
        cmdSvrClient = cmdSvr.available();
        clientTime = millis() + 60000UL;
        VLF("MSG: IPSerial::available() cmdSvrClient started");
      }
      return 0;
    }
    if (!cmdSvrClient.connected()) { 
      VLF("MSG: IPSerial::available() cmdSvrClient not connected, stopping");
      cmdSvrClient.stop();
      return 0;
    }
    if ((long)(clientTime - millis()) < 0) {
      VLF("MSG: IPSerial::available() cmdSvrClient timeout, stopping");
      cmdSvrClient.stop();
      return 0;
    }

    VLF("MSG: IPSerial::available() -> true");

    return cmdSvrClient.available();
  }

  int IPSerial::peek(void) {
    if (!cmdSvrClient) {
      VLF("MSG: IPSerial::peek() no cmdSvrClient");
      return -1;
    }
    if (!cmdSvrClient.connected()) { 
      VLF("MSG: IPSerial::peek() cmdSvrClient not connected");
      return -1;
    }
    if (cmdSvrClient.available() > 0) {
      VLF("MSG: IPSerial::peek() cmdSvrClient no data");
      return -1;
    }

    return cmdSvrClient.peek();
  }

  void IPSerial::flush(void) {
    if (!cmdSvrClient) {
      VLF("MSG: IPSerial::flush() no cmdSvrClient");
      return;
    }
    if (!cmdSvrClient.connected()) { 
      VLF("MSG: IPSerial::flush() cmdSvrClient not connected");
      return;
    }

    cmdSvrClient.flush();
  }

  int IPSerial::read(void) {
    if (!cmdSvrClient) {
      VLF("MSG: IPSerial::read() no cmdSvrClient");
      return -1;
    }
    if (!cmdSvrClient.connected()) { 
      VLF("MSG: IPSerial::read() cmdSvrClient not connected");
      return -1;
    }
    if (cmdSvrClient.available() > 0) {
      VLF("MSG: IPSerial::read() cmdSvrClient no data");
      return -1;
    }

    return cmdSvrClient.read();
  }

  size_t IPSerial::write(uint8_t data) {
    if (!cmdSvrClient) {
      VLF("MSG: IPSerial::write() no cmdSvrClient");
      return 0;
    }
    if (!cmdSvrClient.connected()) { 
      VLF("MSG: IPSerial::write() cmdSvrClient not connected");
      return 0;
    }

    return cmdSvrClient.write(data);
  }

  IPSerial ipSerial;
#endif
