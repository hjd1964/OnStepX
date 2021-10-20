// wifi manager, used by the webserver and wifi serial IP
#include "WifiManager.h"

#if OPERATIONAL_MODE == WIFI

void WifiManager::init() {
  if (!active) {

    #ifdef NV_WIFI_SETTINGS_BASE
      if (WifiSettingsSize < sizeof(WifiSettings)) { nv.readOnly(true); DL("ERR: WifiManager::init(); WifiSettingsSize error NV subsystem writes disabled"); }

      if (!nv.isKeyValid()) {
        VLF("MSG: WifiManager, writing defaults to NV");
        nv.writeBytes(NV_WIFI_SETTINGS_BASE, &settings, sizeof(WifiSettings));
      }

      nv.readBytes(NV_WIFI_SETTINGS_BASE, &settings, sizeof(WifiSettings));
    #endif

    ap_ip = IPAddress(settings.ap_ip);
    ap_gw = IPAddress(settings.ap_gw);
    ap_sn = IPAddress(settings.ap_sn);

    sta_ip = IPAddress(settings.sta_ip);
    sta_gw = IPAddress(settings.sta_gw);
    sta_sn = IPAddress(settings.sta_sn);

    VF("MSG: WiFi Master Pwd  = "); VL(settings.masterPassword);

    VF("MSG: WiFi AP Enabled  = "); VL(settings.accessPointEnabled);
    VF("MSG: WiFi Sta Enabled = "); VL(settings.stationEnabled);
    VF("MSG: WiFi Sta DHCP En = "); VL(settings.stationDhcpEnabled);

    VF("MSG: WiFi STA SSID    = "); VL(settings.sta_ssid);
    VF("MSG: WiFi STA PWD     = "); VL(settings.sta_pwd);

    VF("MSG: WiFi STA IP      = "); VL(sta_ip.toString());
    VF("MSG: WiFi STA GATEWAY = "); VL(sta_gw.toString());
    VF("MSG: WiFi STA SN      = "); VL(sta_sn.toString());

    VF("MSG: WiFi AP SSID     = "); VL(settings.ap_ssid);
    VF("MSG: WiFi AP PWD      = "); VL(settings.ap_pwd);
    VF("MSG: WiFi AP CH       = "); VL(settings.ap_ch);
    
    VF("MSG: WiFi AP IP       = "); VL(ap_ip.toString());
    VF("MSG: WiFi AP GATEWAY  = "); VL(ap_gw.toString());
    VF("MSG: WiFi AP SN       = "); VL(ap_sn.toString());

  TryAgain:
    if (settings.accessPointEnabled && !settings.stationEnabled) {
      VLF("MSG: WiFi Starting Soft AP");
      WiFi.softAP(settings.ap_ssid, settings.ap_pwd, settings.ap_ch);
      WiFi.mode(WIFI_AP);
    } else
    if (!settings.accessPointEnabled && settings.stationEnabled) {
      VLF("MSG: WiFi Starting Station");
      WiFi.begin(settings.sta_ssid, settings.sta_pwd);
      WiFi.mode(WIFI_STA);
    } else
    if (settings.accessPointEnabled && settings.stationEnabled) {
      VLF("MSG: WiFi Starting Soft AP");
      WiFi.softAP(settings.ap_ssid, settings.ap_pwd, settings.ap_ch);
      VLF("MSG: WiFi Starting Station");
      WiFi.begin(settings.sta_ssid, settings.sta_pwd);
      WiFi.mode(WIFI_AP_STA);
    }

    delay(100);
    
    if (settings.stationEnabled && !settings.stationDhcpEnabled) WiFi.config(sta_ip, sta_gw, sta_sn);
    if (settings.accessPointEnabled) WiFi.softAPConfig(ap_ip, ap_gw, ap_sn);

    // wait for connection in station mode, if it fails fall back to access-point mode
    if (!settings.accessPointEnabled && settings.stationEnabled) {
      for (int i = 0; i < 8; i++) if (WiFi.status() != WL_CONNECTED) delay(1000); else break;
      if (WiFi.status() != WL_CONNECTED) {
        VLF("MSG: WiFi Starting Station, failed");
        WiFi.disconnect(); delay(3000);
        VLF("MSG: WiFi Switching to Soft AP mode");
        settings.stationEnabled = false;
        settings.accessPointEnabled = true;
        goto TryAgain;
      }
    }

    VLF("MSG: WiFi initialized");
    active = true;
  }
}

void WifiManager::writeSettings() {
  #ifdef NV_WIFI_SETTINGS_BASE
    VLF("MSG: WifiManager, writing settings to NV");
    nv.writeBytes(NV_WIFI_SETTINGS_BASE, &settings, sizeof(WifiSettings));
  #endif
}

WifiManager wifiManager;

#endif
