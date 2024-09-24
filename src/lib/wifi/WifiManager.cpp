// wifi manager, used by the webserver and wifi serial IP
#include "WifiManager.h"

#if OPERATIONAL_MODE == WIFI

#include "../tasks/OnTask.h"

#if STA_AUTO_RECONNECT == true
  void reconnectStationWrapper() { wifiManager.reconnectStation(); }
#endif

bool WifiManager::init() {
  if (!active) {

    #ifdef NV_WIFI_SETTINGS_BASE
      if (WifiSettingsSize < sizeof(WifiSettings)) { nv.initError = true; DL("ERR: WifiManager::init(), WifiSettingsSize error"); }

      if (!nv.hasValidKey() || nv.isNull(NV_WIFI_SETTINGS_BASE, sizeof(WifiSettings))) {
        VLF("MSG: WiFi, writing defaults to NV");
        nv.writeBytes(NV_WIFI_SETTINGS_BASE, &settings, sizeof(WifiSettings));
      }

      nv.readBytes(NV_WIFI_SETTINGS_BASE, &settings, sizeof(WifiSettings));
    #endif

    IPAddress ap_ip = IPAddress(settings.ap.ip);
    IPAddress ap_gw = IPAddress(settings.ap.gw);
    IPAddress ap_sn = IPAddress(settings.ap.sn);

    VF("MSG: WiFi, Master Pwd  = "); VL(settings.masterPassword);

    VF("MSG: WiFi, AP Enable   = "); VL(settings.accessPointEnabled);
    VF("MSG: WiFi, AP Fallback = "); VL(settings.stationApFallback);

    if (settings.accessPointEnabled || settings.stationApFallback) {
      VF("MSG: WiFi, AP SSID     = "); VL(settings.ap.ssid);
      VF("MSG: WiFi, AP PWD      = "); VL(settings.ap.pwd);
      VF("MSG: WiFi, AP CH       = "); VL(settings.ap.channel);
      VF("MSG: WiFi, AP IP       = "); VL(ap_ip.toString());
      VF("MSG: WiFi, AP GATEWAY  = "); VL(ap_gw.toString());
      VF("MSG: WiFi, AP SN       = "); VL(ap_sn.toString());
    }

    sta = &settings.station[stationNumber - 1];

    IPAddress sta_ip = IPAddress(sta->ip);
    IPAddress sta_gw = IPAddress(sta->gw);
    IPAddress sta_sn = IPAddress(sta->sn);

    VF("MSG: WiFi, Sta Enable  = "); VL(settings.stationEnabled);

    if (settings.stationEnabled) {
      VF("MSG: WiFi, Station#    = "); VL(stationNumber);
      VF("MSG: WiFi, Sta DHCP En = "); VL(sta->dhcpEnabled);
      VF("MSG: WiFi, Sta SSID    = "); VL(sta->ssid);
      VF("MSG: WiFi, Sta PWD     = "); VL(sta->pwd);
      VF("MSG: WiFi, Sta IP      = "); VL(sta_ip.toString());
      VF("MSG: WiFi, Sta GATEWAY = "); VL(sta_gw.toString());
      VF("MSG: WiFi, Sta SN      = "); VL(sta_sn.toString());
      IPAddress target = IPAddress(sta->target);
      VF("MSG: WiFi, Sta TARGET  = "); VL(target.toString());
    }

  TryAgain:
    if (settings.accessPointEnabled && !settings.stationEnabled) {
      VLF("MSG: WiFi, starting Soft AP");
      WiFi.softAP(settings.ap.ssid, settings.ap.pwd, settings.ap.channel);
      #if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32C3)
        WiFi.setTxPower(WIFI_POWER_8_5dBm);
      #endif
      WiFi.mode(WIFI_AP);
    } else
    if (!settings.accessPointEnabled && settings.stationEnabled) {
      VLF("MSG: WiFi, starting Station");
      WiFi.begin(sta->ssid, sta->pwd);
      #if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32C3)
        WiFi.setTxPower(WIFI_POWER_8_5dBm);
      #endif
      WiFi.mode(WIFI_STA);
    } else
    if (settings.accessPointEnabled && settings.stationEnabled) {
      VLF("MSG: WiFi, starting Soft AP");
      WiFi.softAP(settings.ap.ssid, settings.ap.pwd, settings.ap.channel);
      VLF("MSG: WiFi, starting Station");
      WiFi.begin(sta->ssid, sta->pwd);
      #if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32C3)
        WiFi.setTxPower(WIFI_POWER_8_5dBm);
      #endif
      WiFi.mode(WIFI_AP_STA);
    }

    delay(100);
    
    if (settings.stationEnabled && !sta->dhcpEnabled) WiFi.config(sta_ip, sta_gw, sta_sn);
    if (settings.accessPointEnabled) WiFi.softAPConfig(ap_ip, ap_gw, ap_sn);

    // wait for connection
    if (settings.stationEnabled) { for (int i = 0; i < 8; i++) if (WiFi.status() != WL_CONNECTED) delay(1000); else break; }

    if (settings.stationEnabled && WiFi.status() != WL_CONNECTED) {

      // if connection fails fall back to access-point mode
      if (settings.stationApFallback && !settings.accessPointEnabled) {
        VLF("MSG: WiFi starting station failed");
        WiFi.disconnect();
        delay(3000);
        VLF("MSG: WiFi, switching to SoftAP mode");
        settings.stationEnabled = false;
        settings.accessPointEnabled = true;
        goto TryAgain;
      }

      if (!settings.accessPointEnabled) {
        VLF("MSG: WiFi, initialization failed");
      } else {
        active = true;
        VLF("MSG: WiFi, AP initialized station failed");
      }
    } else {
      active = true;
      VLF("MSG: WiFi, initialized");

      #if MDNS_SERVER == ON && !defined(ESP8266)
        if (MDNS.begin(MDNS_NAME)) { VLF("MSG: WiFi, mDNS started"); } else { VLF("WRN: WiFi, mDNS start failed!"); }
      #endif

      #if STA_AUTO_RECONNECT == true
        if (settings.stationEnabled) {
          VF("MSG: WiFi, start connection check task (rate 8s priority 7)... ");
          if (tasks.add(8000, 0, true, 7, reconnectStationWrapper, "WifiChk")) { VLF("success"); } else { VLF("FAILED!"); }
        }
      #endif
    }
  }

  return active;
}

#if STA_AUTO_RECONNECT == true
  void WifiManager::reconnectStation() {
    if (WiFi.status() != WL_CONNECTED) {
      VLF("MSG: WiFi, attempting reconnect");
      WiFi.disconnect();
      WiFi.reconnect();
    }
  }
#endif

void WifiManager::setStation(int number) {
  if (number >= 1 && number <= WifiStationCount) stationNumber = number;
  sta = &settings.station[stationNumber - 1];
}

void WifiManager::disconnect() {
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  active = false;
  VLF("MSG: WiFi, disconnected");      
}

void WifiManager::writeSettings() {
  #ifdef NV_WIFI_SETTINGS_BASE
    VLF("MSG: WifiManager, writing settings to NV");
    nv.writeBytes(NV_WIFI_SETTINGS_BASE, &settings, sizeof(WifiSettings));
  #endif
}

WifiManager wifiManager;

#endif
