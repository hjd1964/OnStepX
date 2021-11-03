// wifi manager, used by the webserver and wifi serial IP
#include "WifiManager.h"

#if OPERATIONAL_MODE == WIFI

bool WifiManager::init() {
  if (!active) {

    #ifdef NV_WIFI_SETTINGS_BASE
      if (WifiSettingsSize < sizeof(WifiSettings)) { nv.readOnly(true); DL("ERR: WifiManager::init(); WifiSettingsSize error NV subsystem writes disabled"); }

      if (!nv.isKeyValid()) {
        VLF("MSG: WifiManager, writing defaults to NV");
        nv.writeBytes(NV_WIFI_SETTINGS_BASE, &settings, sizeof(WifiSettings));
      }

      nv.readBytes(NV_WIFI_SETTINGS_BASE, &settings, sizeof(WifiSettings));
    #endif

    IPAddress ap_ip = IPAddress(settings.ap.ip);
    IPAddress ap_gw = IPAddress(settings.ap.gw);
    IPAddress ap_sn = IPAddress(settings.ap.sn);

    VF("MSG: WiFi Master Pwd   = "); VL(settings.masterPassword);

    VF("MSG: WiFi AP Enable    = "); VL(settings.accessPointEnabled);
    VF("MSG: WiFi AP SSID      = "); VL(settings.ap.ssid);
    VF("MSG: WiFi AP PWD       = "); VL(settings.ap.pwd);
    VF("MSG: WiFi AP CH        = "); VL(settings.ap.channel);
    VF("MSG: WiFi AP IP        = "); VL(ap_ip.toString());
    VF("MSG: WiFi AP GATEWAY   = "); VL(ap_gw.toString());
    VF("MSG: WiFi AP SN        = "); VL(ap_sn.toString());

    VF("MSG: WiFi Sta Enable   = "); VL(settings.stationEnabled);
    VF("MSG: WiFi Sta Fback AP = "); VL(settings.stationApFallback);
    VF("MSG: WiFi Sta Fback Alt= "); VL(settings.stationAltFallback);

    sta = &settings.sta1;
    IPAddress sta_ip = IPAddress(sta->ip);
    IPAddress sta_gw = IPAddress(sta->gw);
    IPAddress sta_sn = IPAddress(sta->sn);
    IPAddress target = IPAddress(sta->target);

    VF("MSG: WiFi Sta1 DHCP En = "); VL(sta->dhcpEnabled);
    VF("MSG: WiFi Sta1 SSID    = "); VL(sta->ssid);
    VF("MSG: WiFi Sta1 PWD     = "); VL(sta->pwd);
    VF("MSG: WiFi Sta1 IP      = "); VL(sta_ip.toString());
    VF("MSG: WiFi Sta1 GATEWAY = "); VL(sta_gw.toString());
    VF("MSG: WiFi Sta1 SN      = "); VL(sta_sn.toString());
    VF("MSG: WiFi Sta1 TARGET  = "); VL(target.toString());

    sta = &settings.sta2;
    sta_ip = IPAddress(sta->ip);
    sta_gw = IPAddress(sta->gw);
    sta_sn = IPAddress(sta->sn);
    target = IPAddress(sta->target);

    VF("MSG: WiFi Sta2 DHCP En = "); VL(sta->dhcpEnabled);
    VF("MSG: WiFi Sta2 SSID    = "); VL(sta->ssid);
    VF("MSG: WiFi Sta2 PWD     = "); VL(sta->pwd);
    VF("MSG: WiFi Sta2 IP      = "); VL(sta_ip.toString());
    VF("MSG: WiFi Sta2 GATEWAY = "); VL(sta_gw.toString());
    VF("MSG: WiFi Sta2 SN      = "); VL(sta_sn.toString());
    VF("MSG: WiFi Sta2 TARGET  = "); VL(target.toString());

    sta = &settings.sta1;

  TryAgain:
    sta_ip = IPAddress(sta->ip);
    sta_gw = IPAddress(sta->gw);
    sta_sn = IPAddress(sta->sn);

    if (settings.accessPointEnabled && !settings.stationEnabled) {
      VLF("MSG: WiFi starting Soft AP");
      WiFi.softAP(settings.ap.ssid, settings.ap.pwd, settings.ap.channel);
      WiFi.mode(WIFI_AP);
    } else
    if (!settings.accessPointEnabled && settings.stationEnabled) {
      VLF("MSG: WiFi starting Station");
      WiFi.begin(sta->ssid, sta->pwd);
      WiFi.mode(WIFI_STA);
    } else
    if (settings.accessPointEnabled && settings.stationEnabled) {
      VLF("MSG: WiFi starting Soft AP");
      WiFi.softAP(settings.ap.ssid, settings.ap.pwd, settings.ap.channel);
      VLF("MSG: WiFi starting Station");
      WiFi.begin(sta->ssid, sta->pwd);
      WiFi.mode(WIFI_AP_STA);
    }

    delay(100);
    
    if (settings.stationEnabled && !sta->dhcpEnabled) WiFi.config(sta_ip, sta_gw, sta_sn);
    if (settings.accessPointEnabled) WiFi.softAPConfig(ap_ip, ap_gw, ap_sn);

    // wait for connection
    for (int i = 0; i < 8; i++) if (WiFi.status() != WL_CONNECTED) delay(1000); else break;

    if (WiFi.status() != WL_CONNECTED) {
      // if connection fails fall back to the alternate station
      if (settings.stationEnabled && settings.stationAltFallback && sta != &settings.sta2) {
        VLF("MSG: WiFi starting station failed");
        WiFi.disconnect();
        delay(3000);
        VLF("MSG: WiFi switching to alternate station");
        sta = &settings.sta2;
        goto TryAgain;
      }

      // if connection fails fall back to access-point mode
      if (settings.stationEnabled && settings.stationApFallback && !settings.accessPointEnabled) {
        VLF("MSG: WiFi starting station failed");
        WiFi.disconnect();
        delay(3000);
        VLF("MSG: WiFi switching to SoftAP mode");
        settings.stationEnabled = false;
        settings.accessPointEnabled = true;
        goto TryAgain;
      }

      VLF("MSG: WiFi initialization failed");
    } else {
      active = true;
      VLF("MSG: WiFi initialized");      
    }
  }

  return active;
}

void WifiManager::writeSettings() {
  #ifdef NV_WIFI_SETTINGS_BASE
    VLF("MSG: WifiManager, writing settings to NV");
    nv.writeBytes(NV_WIFI_SETTINGS_BASE, &settings, sizeof(WifiSettings));
  #endif
}

WifiManager wifiManager;

#endif
