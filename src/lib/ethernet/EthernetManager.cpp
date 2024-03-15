// ethernet manager, used by the webserver and ethernet serial IP
#include "EthernetManager.h"

#if defined(OPERATIONAL_MODE) && (OPERATIONAL_MODE == ETHERNET_W5100 || OPERATIONAL_MODE == ETHERNET_W5500)

#include "../tasks/OnTask.h"

#if MDNS_SERVER == ON
  enum MdnsReady {MD_WAIT, MD_READY, MD_FAIL};

  EthernetUDP udp;
  MDNS mdns(udp);

  void mdnsPoll() {
    static MdnsReady mdnsReady = MD_WAIT;
    if (mdnsReady == MD_WAIT && millis() > 5000) {
      if (mdns.begin(Ethernet.localIP(), MDNS_NAME)) {
        VLF("MSG: Ethernet, mDNS started");
        mdnsReady = MD_READY;
      } else {
        VLF("WRN: Ethernet, mDNS start failed!");
        mdnsReady = MD_FAIL;
      }
    }
    if (mdnsReady == MD_READY) mdns.run();
  }
#endif

bool EthernetManager::init() {
  if (!active) {
    #ifdef NV_ETHERNET_SETTINGS_BASE
      if (EthernetSettingsSize < sizeof(EthernetSettings)) { nv.initError = true; DL("ERR: EthernetManager::init(); EthernetSettingsSize error"); }

      if (!nv.hasValidKey() || nv.isNull(NV_ETHERNET_SETTINGS_BASE, sizeof(EthernetSettings))) {
        VLF("MSG: Ethernet, writing defaults to NV");
        nv.writeBytes(NV_ETHERNET_SETTINGS_BASE, &settings, sizeof(EthernetSettings));
      }

      nv.readBytes(NV_ETHERNET_SETTINGS_BASE, &settings, sizeof(EthernetSettings));
    #endif

    VF("MSG: Ethernet, DHCP En = "); VL(settings.dhcpEnabled);
    VF("MSG: Ethernet, IP      = "); V(settings.ip[0]); V("."); V(settings.ip[1]); V("."); V(settings.ip[2]); V("."); VL(settings.ip[3]);
    VF("MSG: Ethernet, GW      = "); V(settings.gw[0]); V("."); V(settings.gw[1]); V("."); V(settings.gw[2]); V("."); VL(settings.gw[3]);
    VF("MSG: Ethernet, SN      = "); V(settings.sn[0]); V("."); V(settings.sn[1]); V("."); V(settings.sn[2]); V("."); VL(settings.sn[3]);
    VF("MSG: Ethernet, TARGET  = "); V(settings.target[0]); V("."); V(settings.target[1]); V("."); V(settings.target[2]); V("."); VL(settings.target[3]);

    #if defined(ETHERNET_CS_PIN) && ETHERNET_CS_PIN != OFF
      VF("MSG: Ethernet, device ETHERNET_CS_PIN ("); V(ETHERNET_CS_PIN); VL(")");
      Ethernet.init(ETHERNET_CS_PIN);
    #endif

    if (ETHERNET_RESET_PIN != OFF) {
      VF("MSG: Ethernet, device ETHERNET_RESET_PIN ("); V(ETHERNET_RESET_PIN); VL(")");
      pinMode(ETHERNET_RESET_PIN, OUTPUT); 
      digitalWrite(ETHERNET_RESET_PIN, LOW);
      delay(1000);
      digitalWrite(ETHERNET_RESET_PIN, HIGH);
      delay(1000);
    }

    if (settings.dhcpEnabled) {
      Ethernet.begin(settings.mac);
    } else {
      Ethernet.begin(settings.mac, settings.ip, settings.dns, settings.gw, settings.sn);
    }
    active = true;

    VLF("MSG: Ethernet, initialized");

    #if MDNS_SERVER == ON
      VF("MSG: Ethernet, starting mDNS polling");
      VF(" task (rate 5ms priority 7)... ");
      if (tasks.add(5, 0, true, 7, mdnsPoll, "mdPoll")) { VL("success"); } else { VL("FAILED!"); }
    #endif
  }
  return active;
}

void EthernetManager::restart() {
  if (ETHERNET_RESET_PIN != OFF) {
    VF("MSG: Ethernet, device ETHERNET_RESET_PIN ("); V(ETHERNET_RESET_PIN); VL(")");
    pinMode(ETHERNET_RESET_PIN, OUTPUT); 
    digitalWrite(ETHERNET_RESET_PIN, LOW);
    delay(1000);
    digitalWrite(ETHERNET_RESET_PIN, HIGH);
    delay(1000);
  }

  VLF("MSG: Ethernet, restart");
  if (settings.dhcpEnabled) {
    active = Ethernet.begin(settings.mac);
  } else {
    Ethernet.begin(settings.mac, settings.ip, settings.dns, settings.gw, settings.sn);
    active = true;
  }
}

void EthernetManager::writeSettings() {
  #ifdef NV_ETHERNET_SETTINGS_BASE
    VLF("MSG: Ethernet, writing settings to NV");
    nv.writeBytes(NV_ETHERNET_SETTINGS_BASE, &settings, sizeof(EthernetSettings));
  #endif
}

EthernetManager ethernetManager;

#endif
