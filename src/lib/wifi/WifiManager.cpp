// wifi manager, used by the webserver and wifi serial IP
#include "WifiManager.h"

#if OPERATIONAL_MODE == WIFI

#include "../tasks/OnTask.h"
#include "../nv/Nv.h"

#if STA_AUTO_RECONNECT == true
  void reconnectStationWrapper() { wifiManager.reconnectStation(); }
#endif

bool WifiManager::init() {
  if (!active) {

    readSettings();

    setStation(stationNumber);

    IPAddress ap_ip = IPAddress(settings.ap.ip);
    IPAddress ap_gw = IPAddress(settings.ap.gw);
    IPAddress ap_sn = IPAddress(settings.ap.sn);

    IPAddress sta_ip = IPAddress(sta->ip);
    IPAddress sta_gw = IPAddress(sta->gw);
    IPAddress sta_sn = IPAddress(sta->sn);

    char name[32] = HOST_NAME;
    strtohostname(name);

    #if MDNS_SERVER == ON
      WiFi.hostname(name);
    #endif

  TryAgain:
    if (settings.accessPointEnabled && !settings.stationEnabled) {
      VF("MSG: WiFi, starting Soft AP for SSID "); V(settings.ap.ssid); V(" PWD "); V(settings.ap.pwd); V(" CH "); VL(settings.ap.channel);
      WiFi.softAP(settings.ap.ssid, settings.ap.pwd, settings.ap.channel);
      #if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32C3)
        WiFi.setTxPower(WIFI_POWER_8_5dBm);
      #endif
      WiFi.mode(WIFI_AP);
    } else
    if (!settings.accessPointEnabled && settings.stationEnabled) {
      VF("MSG: WiFi, starting Station for SSID "); V(sta->ssid); V(" PWD "); VL(sta->pwd);
      WiFi.begin(sta->ssid, sta->pwd);
      #if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32C3)
        WiFi.setTxPower(WIFI_POWER_8_5dBm);
      #endif
      WiFi.mode(WIFI_STA);
    } else
    if (settings.accessPointEnabled && settings.stationEnabled) {
      VF("MSG: WiFi, starting Soft AP for SSID "); V(settings.ap.ssid); V(" PWD "); V(settings.ap.pwd); V(" CH "); VL(settings.ap.channel);
      WiFi.softAP(settings.ap.ssid, settings.ap.pwd, settings.ap.channel);
      VF("MSG: WiFi, starting Station for SSID "); V(sta->ssid); V(" PWD "); VL(sta->pwd);
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
    if (settings.stationEnabled) {
      for (int i = 0; i < 8; i++) if (WiFi.status() != WL_CONNECTED) delay(1000); else break;
    }

    if (settings.stationEnabled && WiFi.status() != WL_CONNECTED) {

      // if connection fails fall back to access-point mode
      if (settings.stationApFallback && !settings.accessPointEnabled) {
        VLF("MSG: WiFi, starting station failed");
        WiFi.disconnect();
        delay(3000);
        VLF("MSG: WiFi, switching to SoftAP mode");
        settings.stationEnabled = false;
        settings.accessPointEnabled = true;
        goto TryAgain;
      }

      // no fallback but the AP is still enabled
      if (settings.accessPointEnabled) {
        active = true;
        VLF("MSG: WiFi, started AP but station failed");
      } else {
        // the station failed to connect and the AP isn't enabled
        DLF("WRN: WiFi, starting station failed");
        WiFi.disconnect();
      }
    } else {
      active = true;
      VLF("MSG: WiFi, started");

      #if MDNS_SERVER == ON && !defined(ESP8266)
        sstrcpy(name, MDNS_NAME, 32);
        strtohostname2(name);
        if (MDNS.begin(name)) { VF("MSG: WiFi, mDNS started for "); VL(name); } else { DF("WRN: WiFi, mDNS start FAILED for "); DL(name); }
      #endif

      if (staNameLookup && strlen(wifiManager.sta->host) > 0) {
        IPAddress ip;
        char name[32] = "";
        strcpy(name, wifiManager.sta->host);
        strtohostname(name);

        if (WiFi.hostByName(name, ip)) {
          VF("MSG: WiFi, host name "); V(name); VF(" DNS resolved to "); VL(ip.toString().c_str());
          ip4toip4(wifiManager.sta->target, ip);
        } else {
          VF("MSG: WiFi, host name "); V(name); VLF(" DNS resolution failed!");
          #if MDNS_CLIENT == ON && !defined(ESP8266)
            strtohostname2(name);
            ip = MDNS.queryHost(name);
            if (validip4(ip)) {
              ip4toip4(wifiManager.sta->target, ip);
              VF("MSG: WiFi, host name "); V(name); VF(" mDNS resolved to "); VL(ip.toString().c_str());
            } else {
              VF("MSG: WiFi, host name "); V(name); VLF(" mDNS resolution failed!");
            }
          #endif
        }
      }

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

void WifiManager::readSettings() {
  if (settingsReady) return;

  #ifdef NV_WIFI_SETTINGS_BASE
    if (WifiSettingsSize < sizeof(WifiSettings)) { nv.initError = true; DL("ERR: WifiManager::init(), WifiSettingsSize error"); }

    if (!nv.hasValidKey() || nv.isNull(NV_WIFI_SETTINGS_BASE, sizeof(WifiSettings))) {
      VLF("MSG: WiFi, writing defaults to NV");
      nv.writeBytes(NV_WIFI_SETTINGS_BASE, &settings, sizeof(WifiSettings));
    }

    nv.readBytes(NV_WIFI_SETTINGS_BASE, &settings, sizeof(WifiSettings));
  #endif

  #if DEBUG != OFF
    IPAddress ap_ip = IPAddress(settings.ap.ip);
    IPAddress ap_gw = IPAddress(settings.ap.gw);
    IPAddress ap_sn = IPAddress(settings.ap.sn);

    VF("MSG: WiFi, Master Pwd   = "); VL(settings.masterPassword);

    VF("MSG: WiFi, AP Enable    = "); VL(settings.accessPointEnabled);
    VF("MSG: WiFi, AP Fallback  = "); VL(settings.stationApFallback);

    VF("MSG: WiFi, AP SSID      = "); VL(settings.ap.ssid);
    VF("MSG: WiFi, AP PWD       = "); VL(settings.ap.pwd);
    VF("MSG: WiFi, AP CH        = "); VL(settings.ap.channel);
    VF("MSG: WiFi, AP IP        = "); VL(ap_ip.toString());
    VF("MSG: WiFi, AP GATEWAY   = "); VL(ap_gw.toString());
    VF("MSG: WiFi, AP SN        = "); VL(ap_sn.toString());

    int currentStationNumber = stationNumber;

    VF("MSG: WiFi, Sta Enable   = "); VL(settings.stationEnabled);

    VF("MSG: WiFi, Sta Select   = "); VL(stationNumber);

    for (int station = 1; station <= WifiStationCount; station++) {
      setStation(station);

      IPAddress sta_ip = IPAddress(sta->ip); UNUSED(sta_ip);
      IPAddress sta_gw = IPAddress(sta->gw); UNUSED(sta_gw);
      IPAddress sta_sn = IPAddress(sta->sn); UNUSED(sta_sn);
      IPAddress target = IPAddress(sta->target); UNUSED(target);

      VF("MSG: WiFi, Sta"); V(stationNumber); VF(" SSID    = "); VL(sta->ssid);
      VF("MSG: WiFi, Sta"); V(stationNumber); VF(" PWD     = "); VL(sta->pwd);
      VF("MSG: WiFi, Sta"); V(stationNumber); VF(" DHCP En = "); VL(sta->dhcpEnabled);
      VF("MSG: WiFi, Sta"); V(stationNumber); VF(" IP      = "); VL(sta_ip.toString());
      VF("MSG: WiFi, Sta"); V(stationNumber); VF(" GATEWAY = "); VL(sta_gw.toString());
      VF("MSG: WiFi, Sta"); V(stationNumber); VF(" SN      = "); VL(sta_sn.toString());
      VF("MSG: WiFi, Sta"); V(stationNumber); VF(" NAME    = "); VL(sta->host);
      VF("MSG: WiFi, Sta"); V(stationNumber); VF(" TARGET  = "); VL(target.toString());
    }
    stationNumber = currentStationNumber;
  #endif

  settingsReady = true;
}

void WifiManager::writeSettings() {
  if (!settingsReady) return;
  
  #ifdef NV_WIFI_SETTINGS_BASE
    VLF("MSG: WifiManager, writing settings to NV");
    nv.writeBytes(NV_WIFI_SETTINGS_BASE, &settings, sizeof(WifiSettings));
  #endif
}

WifiManager wifiManager;

#endif
