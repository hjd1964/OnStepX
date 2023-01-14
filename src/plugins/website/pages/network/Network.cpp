// -----------------------------------------------------------------------------------
// Setup Network
#include "Network.h"

#include "../../../../lib/ethernet/EthernetManager.h"
#include "../../../../lib/wifi/WifiManager.h"
#include "../Page.h"
#include "../Pages.common.h"

extern NVS nv;

extern int webTimeout;
extern int cmdTimeout;

void processNetworkGet();

bool restartRequired = false;
bool loginRequired = true;
byte temp_ip[4] = {0,0,0,0};
byte temp_sn[4] = {0,0,0,0};
byte temp_gw[4] = {0,0,0,0};

void handleNetwork() {
  char temp[420]  = "";
  char temp1[140] = "";

  SERIAL_ONSTEP.setTimeout(webTimeout);
  onStep.serialRecvFlush();
  
  processNetworkGet();

  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/html", String());

  // send a standard http response header
  String data = FPSTR(html_head_begin);
  data.concat(FPSTR(html_main_css_begin));
  www.sendContentAndClear(data);
  data.concat(FPSTR(html_main_css_core));
  www.sendContentAndClear(data);
  data.concat(FPSTR(html_main_css_end));
  data.concat(FPSTR(html_head_end));
  www.sendContentAndClear(data);

  // show this page
  data.concat(FPSTR(html_body_begin));
  pageHeader(PAGE_NETWORK);
  data.concat(FPSTR(html_onstep_page_begin));

  // OnStep wasn't found, show warning and info.
  if (!status.onStepFound) {
    data.concat(FPSTR(html_bad_comms_message));
    data.concat(FPSTR(html_page_and_body_end));
    www.sendContentAndClear(data);
    www.sendContent("");
    return;
  }

  // page contents
  data.concat("<div>");

  if (restartRequired) {
    restartRequired = false;
    data.concat(FPSTR(html_reboot));
  } else
  if (loginRequired) {
    restartRequired = false;
    data.concat(FPSTR(html_login));
  } else {
    #if defined(NV_TIMEOUT_CMD) && defined(NV_TIMEOUT_WEB)
      sprintf_P(temp, htmL_NETWORKSerial, cmdTimeout, webTimeout); data.concat(temp);
    #endif

    #if OPERATIONAL_MODE == WIFI
      sprintf_P(temp, htmL_NETWORKSSID1, wifiManager.sta->ssid, ""); data.concat(temp);

      uint8_t macsta[6] = {0,0,0,0,0,0};
      WiFi.macAddress(macsta);
      temp1[0] = 0;
      for (int i = 0; i < 6; i++) {
        char temp2[200];
        sprintf(temp2, "%s%02x:", temp1, macsta[i]);
        strcpy(temp1, temp2);
      }
      temp1[strlen(temp1) - 1] = 0;

      sprintf_P(temp,htmL_NET_MAC,"sta", temp1); data.concat(temp);
      www.sendContentAndClear(data);
      sprintf_P(temp,htmL_NET_IP, "sta", (int)wifiManager.sta->ip[0], "sta", (int)wifiManager.sta->ip[1], "sta", (int)wifiManager.sta->ip[2], "sta", (int)wifiManager.sta->ip[3]); data.concat(temp);
      sprintf_P(temp,htmL_NET_GW, "sta", (int)wifiManager.sta->gw[0], "sta", (int)wifiManager.sta->gw[1], "sta", (int)wifiManager.sta->gw[2], "sta", (int)wifiManager.sta->gw[3]); data.concat(temp);
      sprintf_P(temp,htmL_NET_SN, "sta", (int)wifiManager.sta->sn[0], "sta", (int)wifiManager.sta->sn[1], "sta", (int)wifiManager.sta->sn[2], "sta", (int)wifiManager.sta->sn[3]); data.concat(temp);

      sprintf_P(temp,htmL_NETWORKSSID2, wifiManager.sta->dhcpEnabled ? "checked" : "",wifiManager.settings.stationEnabled ? "checked" : ""); data.concat(temp);
      data.concat(FPSTR(htmL_NETWORKSSID3A));
      sprintf_P(temp,htmL_NETWORKSSID3B, wifiManager.settings.ap.ssid, "", wifiManager.settings.ap.channel); data.concat(temp);
      www.sendContentAndClear(data);
    
      uint8_t macap[6] = {0,0,0,0,0,0}; WiFi.softAPmacAddress(macap); temp1[0] = 0;
      for (int i = 0; i < 6; i++) {
        char temp2[200];
        sprintf(temp2, "%s%02x:", temp1, macap[i]);
        strcpy(temp1, temp2);
      }
      temp1[strlen(temp1) - 1] = 0;
      
      sprintf_P(temp,htmL_NET_MAC,"ap", temp1); data.concat(temp);
      www.sendContentAndClear(data);
      sprintf_P(temp,htmL_NET_IP, "ap", (int)wifiManager.settings.ap.ip[0], "ap", (int)wifiManager.settings.ap.ip[1], "ap", (int)wifiManager.settings.ap.ip[2], "ap", (int)wifiManager.settings.ap.ip[3]); data.concat(temp);
      sprintf_P(temp,htmL_NET_GW, "ap", (int)wifiManager.settings.ap.gw[0], "ap", (int)wifiManager.settings.ap.gw[1], "ap", (int)wifiManager.settings.ap.gw[2], "ap", (int)wifiManager.settings.ap.gw[3]); data.concat(temp);
      sprintf_P(temp,htmL_NET_SN, "ap", (int)wifiManager.settings.ap.sn[0], "ap", (int)wifiManager.settings.ap.sn[1], "ap", (int)wifiManager.settings.ap.sn[2], "ap", (int)wifiManager.settings.ap.sn[3]); data.concat(temp);

      sprintf_P(temp,htmL_NETWORKSSID7, wifiManager.settings.accessPointEnabled ? "checked" : ""); data.concat(temp);
    #elif OPERATIONAL_MODE == ETHERNET_W5100 || OPERATIONAL_MODE == ETHERNET_W5500

      data.concat(FPSTR(htmL_NETWORK_ETH_BEG)); temp1[0] = 0;
      for (int i = 0; i < 6; i++) { sprintf(temp1, "%s%02x:", temp1, ethernetManager.settings.mac[i]); } temp1[strlen(temp1) - 1] = 0;

      sprintf_P(temp,htmL_NET_MAC,"eth", temp1); data.concat(temp);
      www.sendContentAndClear(data);
      sprintf_P(temp,htmL_NET_IP, "eth", (int)ethernetManager.settings.ip[0], "eth", (int)ethernetManager.settings.ip[1], "eth", (int)ethernetManager.settings.ip[2], "eth", (int)ethernetManager.settings.ip[3]); data.concat(temp);
      sprintf_P(temp,htmL_NET_GW, "eth", (int)ethernetManager.settings.gw[0], "eth", (int)ethernetManager.settings.gw[1], "eth", (int)ethernetManager.settings.gw[2], "eth", (int)ethernetManager.settings.gw[3]); data.concat(temp);
      sprintf_P(temp,htmL_NET_SN, "eth", (int)ethernetManager.settings.sn[0], "eth", (int)ethernetManager.settings.sn[1], "eth", (int)ethernetManager.settings.sn[2], "eth", (int)ethernetManager.settings.sn[3]); data.concat(temp);

      data.concat(FPSTR(htmL_NETWORK_ETH_END));
      www.sendContentAndClear(data);
    #endif

    data.concat(FPSTR(html_logout));
  }
  
  strcpy(temp,"</div></div></body></html>");
  data.concat(temp);

  www.sendContentAndClear(data);
  www.sendContent("");
}

void processNetworkGet() {
  String v, v1;
  
  bool updateNV = false;

  // Login --------------------------------------------------------------------

  #if OPERATIONAL_MODE == WIFI
    v = www.arg("login");
    if (!v.equals(EmptyStr)) {
      if (!strcmp(wifiManager.settings.masterPassword, (char*)v.c_str())) loginRequired = false;
    }

    v = www.arg("logout");
    if (!v.equals(EmptyStr)) loginRequired = true;
    if (loginRequired) return;

    v = www.arg("webpwd");
    if (!v.equals(EmptyStr)) {
      strcpy(wifiManager.settings.masterPassword, (char*)v.c_str());
      updateNV = true;
    }
  #elif OPERATIONAL_MODE == ETHERNET_W5100 || OPERATIONAL_MODE == ETHERNET_W5500
    v = www.arg("login");
    if (!v.equals(EmptyStr)) {
      if (!strcmp(ethernetManager.settings.masterPassword, (char*)v.c_str())) loginRequired = false;
    }

    v = www.arg("logout");
    if (!v.equals(EmptyStr)) loginRequired = true;
    if (loginRequired) return;

    v = www.arg("webpwd");
    if (!v.equals(EmptyStr)) {
      strcpy(ethernetManager.settings.masterPassword, (char*)v.c_str());
      updateNV = true;
    }
  #endif

  // Timeouts -----------------------------------------------------------------
  #if defined(NV_TIMEOUT_CMD) && defined(NV_TIMEOUT_WEB)
    // Cmd channel timeout
    v = www.arg("ccto");
    if (!v.equals(EmptyStr)) {
      cmdTimeout = v.toInt();
      nv.update(NV_TIMEOUT_CMD, (int16_t)cmdTimeout);
    }

    // Web channel timeout
    v = www.arg("wcto");
    if (!v.equals(EmptyStr)) {
      webTimeout = v.toInt();
      nv.update(NV_TIMEOUT_WEB, (int16_t)webTimeout);
    }
  #endif

  #if OPERATIONAL_MODE == WIFI
    // --------------------------------------------------------------------------
    // Station MAC
    v = www.arg("stmac");
    if (!v.equals(EmptyStr)) {
      // 5c:cf:7f:0f:ad:85
      // first the length should be 17
      if (v.length() == 17) {
        // seperators all in place
        if (v.charAt(2) == ':' && v.charAt(5) == ':' && v.charAt(8) == ':' && v.charAt(11) == ':' && v.charAt(14) == ':') {
          // digits all in 0..9, A..F and validate
          v.toUpperCase();
          uint8_t mac[6];
          int imac[6];
          imac[0] = hexToInt(v.substring(0, 2)); imac[1] = hexToInt(v.substring(3, 2)); imac[2] = hexToInt(v.substring(6, 2));
          imac[3] = hexToInt(v.substring(9, 2)); imac[4] = hexToInt(v.substring(12, 2)); imac[5] = hexToInt(v.substring(15, 2));
          if (imac[0] >= 0 && imac[1] >= 0 && imac[2] >= 0 && imac[3] >= 0 && imac[4] >= 0 && imac[5] >= 0) {
            mac[0] = imac[0]; mac[1] = imac[1]; mac[2] = imac[2]; mac[3] = imac[3]; mac[4] = imac[4]; mac[5] = imac[5]; 
            WiFi.macAddress(mac); restartRequired = true; 
          }
        }
      }
    }

    // Station SSID
    v = www.arg("stssid");
    v1 = v;
    if (!v.equals(EmptyStr)) {
      if (!strcmp(wifiManager.sta->ssid, (char*)v.c_str())) restartRequired = true;
      strcpy(wifiManager.sta->ssid, (char*)v.c_str());

      // if this section was submitted set the stationEnabled default to false
      wifiManager.sta->dhcpEnabled = false;
      wifiManager.settings.stationEnabled = false;
    }

    // Station password
    v = www.arg("stpwd");
    if (!v.equals(EmptyStr)) {
      if (!strcmp(wifiManager.sta->pwd, (char*)v.c_str())) restartRequired = true;
      strcpy(wifiManager.sta->pwd, (char*)v.c_str());
    }

    // Station dhcp enabled
    v = www.arg("stadhcp");
    if (!v.equals(EmptyStr)) {
      wifiManager.sta->dhcpEnabled = v.toInt();
    }

    // Station enabled
    v = www.arg("staen");
    if (!v.equals(EmptyStr)) {
      wifiManager.settings.stationEnabled = v.toInt();
    }

    // Station ip
    v = www.arg("staip1"); if (!v.equals(EmptyStr)) wifiManager.sta->ip[0] = v.toInt();
    v = www.arg("staip2"); if (!v.equals(EmptyStr)) wifiManager.sta->ip[1] = v.toInt();
    v = www.arg("staip3"); if (!v.equals(EmptyStr)) wifiManager.sta->ip[2] = v.toInt();
    v = www.arg("staip4"); if (!v.equals(EmptyStr)) wifiManager.sta->ip[3] = v.toInt();

    // Station SubNet
    v = www.arg("stasn1"); if (!v.equals(EmptyStr)) wifiManager.sta->sn[0] = v.toInt();
    v = www.arg("stasn2"); if (!v.equals(EmptyStr)) wifiManager.sta->sn[1] = v.toInt();
    v = www.arg("stasn3"); if (!v.equals(EmptyStr)) wifiManager.sta->sn[2] = v.toInt();
    v = www.arg("stasn4"); if (!v.equals(EmptyStr)) wifiManager.sta->sn[3] = v.toInt();

    // Station Gateway
    v = www.arg("stagw1"); if (!v.equals(EmptyStr)) wifiManager.sta->gw[0] = v.toInt();
    v = www.arg("stagw2"); if (!v.equals(EmptyStr)) wifiManager.sta->gw[1] = v.toInt();
    v = www.arg("stagw3"); if (!v.equals(EmptyStr)) wifiManager.sta->gw[2] = v.toInt();
    v = www.arg("stagw4"); if (!v.equals(EmptyStr)) wifiManager.sta->gw[3] = v.toInt();
      
    if (!v1.equals(EmptyStr)) {
      updateNV = true;
      restartRequired = true;
    }

    // --------------------------------------------------------------------------
    // Access-Point MAC
    v = www.arg("apmac");
    if (!v.equals(EmptyStr)) {
      // 5c:cf:7f:0f:ad:85
      // first the length should be 17
      if (v.length() == 17) {
        // seperators all in place
        if (v.charAt(2) == ':' && v.charAt(5) == ':' && v.charAt(8) == ':' && v.charAt(11) == ':' && v.charAt(14) == ':') {
          // digits all in 0..9,A..F and validate
          v.toUpperCase();
          uint8_t mac[6];
          int imac[6];
          imac[0] = hexToInt(v.substring(0, 2)); imac[1] = hexToInt(v.substring(3, 2)); imac[2] = hexToInt(v.substring(6, 2));
          imac[3] = hexToInt(v.substring(9, 2)); imac[4] = hexToInt(v.substring(12, 2)); imac[5] = hexToInt(v.substring(15, 2));
          if (imac[0] >= 0 && imac[1] >= 0 && imac[2] >= 0 && imac[3] >= 0 && imac[4] >= 0 && imac[5] >= 0) {
            mac[0] = imac[0]; mac[1] = imac[1]; mac[2] = imac[2]; mac[3] = imac[3]; mac[4] = imac[4]; mac[5] = imac[5];
            WiFi.softAPmacAddress(mac); restartRequired = true; 
          }
        }
      }
    }

    // Access-Point SSID
    v = www.arg("apssid");
    if (!v.equals(EmptyStr)) {
      if (!strcmp(wifiManager.settings.ap.ssid, (char*)v.c_str())) restartRequired = true;
      strcpy(wifiManager.settings.ap.ssid, (char*)v.c_str());

      // if this section was submitted set the accessPointEnabled default to false
      wifiManager.settings.accessPointEnabled = false;
    }

    // Access-Point password
    v = www.arg("appwd");
    if (!v.equals(EmptyStr)) {
      if (!strcmp(wifiManager.settings.ap.pwd, (char*)v.c_str())) restartRequired = true;
      strcpy(wifiManager.settings.ap.pwd, (char*)v.c_str());
    }

    // Access-Point channel
    v = www.arg("apch");
    if (!v.equals(EmptyStr)) {
      if (wifiManager.settings.ap.channel != v.toInt()) restartRequired = true;
      wifiManager.settings.ap.channel = v.toInt();
    }

    // Access-Point enabled
    v = www.arg("apen");
    if (!v.equals(EmptyStr)) {
      wifiManager.settings.accessPointEnabled = v.toInt();
    }

    // Access-Point ip
    v = www.arg("apip1"); if (!v.equals(EmptyStr)) wifiManager.settings.ap.ip[0] = v.toInt();
    v = www.arg("apip2"); if (!v.equals(EmptyStr)) wifiManager.settings.ap.ip[1] = v.toInt();
    v = www.arg("apip3"); if (!v.equals(EmptyStr)) wifiManager.settings.ap.ip[2] = v.toInt();
    v = www.arg("apip4"); if (!v.equals(EmptyStr)) wifiManager.settings.ap.ip[3] = v.toInt();

    // Access-Point SubNet
    v = www.arg("apsn1"); if (!v.equals(EmptyStr)) wifiManager.settings.ap.sn[0] = v.toInt();
    v = www.arg("apsn2"); if (!v.equals(EmptyStr)) wifiManager.settings.ap.sn[1] = v.toInt();
    v = www.arg("apsn3"); if (!v.equals(EmptyStr)) wifiManager.settings.ap.sn[2] = v.toInt();
    v = www.arg("apsn4"); if (!v.equals(EmptyStr)) wifiManager.settings.ap.sn[3] = v.toInt();

    // Access-Point Gateway
    v = www.arg("apgw1"); if (!v.equals(EmptyStr)) wifiManager.settings.ap.gw[0] = v.toInt();
    v = www.arg("apgw2"); if (!v.equals(EmptyStr)) wifiManager.settings.ap.gw[1] = v.toInt();
    v = www.arg("apgw3"); if (!v.equals(EmptyStr)) wifiManager.settings.ap.gw[2] = v.toInt();
    v = www.arg("apgw4"); if (!v.equals(EmptyStr)) wifiManager.settings.ap.gw[3] = v.toInt();

    if (!v.equals(EmptyStr)) {
      updateNV = true;
      restartRequired = true;
    }

    if (updateNV) {
      nv.writeBytes(NV_WIFI_SETTINGS_BASE, &wifiManager.settings, sizeof(WifiSettings));
    }
  #elif OPERATIONAL_MODE == ETHERNET_W5100 || OPERATIONAL_MODE == ETHERNET_W5500
    // Ethernet ip
    v = www.arg("ethip1"); if (!v.equals(EmptyStr)) ethernetManager.settings.ip[0] = v.toInt();
    v = www.arg("ethip2"); if (!v.equals(EmptyStr)) ethernetManager.settings.ip[1] = v.toInt();
    v = www.arg("ethip3"); if (!v.equals(EmptyStr)) ethernetManager.settings.ip[2] = v.toInt();
    v = www.arg("ethip4"); if (!v.equals(EmptyStr)) ethernetManager.settings.ip[3] = v.toInt();

    // Ethernet SubNet
    v = www.arg("ethsn1"); if (!v.equals(EmptyStr)) ethernetManager.settings.sn[0] = v.toInt();
    v = www.arg("ethsn2"); if (!v.equals(EmptyStr)) ethernetManager.settings.sn[1] = v.toInt();
    v = www.arg("ethsn3"); if (!v.equals(EmptyStr)) ethernetManager.settings.sn[2] = v.toInt();
    v = www.arg("ethsn4"); if (!v.equals(EmptyStr)) ethernetManager.settings.sn[3] = v.toInt();

    // Ethernet Gateway
    v = www.arg("ethgw1"); if (!v.equals(EmptyStr)) ethernetManager.settings.gw[0] = v.toInt();
    v = www.arg("ethgw2"); if (!v.equals(EmptyStr)) ethernetManager.settings.gw[1] = v.toInt();
    v = www.arg("ethgw3"); if (!v.equals(EmptyStr)) ethernetManager.settings.gw[2] = v.toInt();
    v = www.arg("ethgw4"); if (!v.equals(EmptyStr)) ethernetManager.settings.gw[3] = v.toInt();

    if (!v.equals(EmptyStr)) {
      updateNV = true;
      restartRequired = true;
    }

    if (updateNV) {
      nv.writeBytes(NV_ETHERNET_SETTINGS_BASE, &ethernetManager.settings, sizeof(EthernetSettings));
    }
  #endif
}
