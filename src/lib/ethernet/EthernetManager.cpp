// ethernet manager, used by the webserver and ethernet serial IP
#include "EthernetManager.h"

#if OPERATIONAL_MODE >= ETHERNET_FIRST && OPERATIONAL_MODE <= ETHERNET_LAST

#include "../tasks/OnTask.h"
#include "../nv/Nv.h"

#if MDNS_SERVER == ON
  enum MdnsReady {MD_WAIT, MD_READY, MD_FAIL};

  #if OPERATIONAL_MODE != ETHERNET_TEENSY41
    EthernetUDP udp;
    MDNS mdns(udp);
  #endif

  void mdnsPoll() {
    static MdnsReady mdnsReady = MD_WAIT;
    if (mdnsReady == MD_WAIT && millis() > 5000) {
      char name[] = MDNS_NAME;
      strtohostname2(name);
      #if OPERATIONAL_MODE == ETHERNET_TEENSY41
        MDNS.begin(name, 1);
        VF("MSG: Ethernet, mDNS started for "); VL(name);
        // MDNS.addService("_http._tcp", 80); // adding a webserver service would look like this
        mdnsReady = MD_READY;
      #else
        if (mdns.begin(Ethernet.localIP(), name)) {
          VF("MSG: Ethernet, mDNS started for "); VL(name);
          mdnsReady = MD_READY;
        } else {
          DF("WRN: Ethernet, mDNS start FAILED for "); DL(name);
          mdnsReady = MD_FAIL;
        }
      #endif
    }

    if (mdnsReady == MD_READY) {
      #if OPERATIONAL_MODE != ETHERNET_TEENSY41
        mdns.run();
      #endif
    }
  }
#endif

bool EthernetManager::init() {
  if (!active) {
    readSettings();

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

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      DLF("WRN: Ethernet, no hardware");
      return false;
    }

    if (Ethernet.linkStatus() == LinkOFF) {
      DLF("WRN: Ethernet, no cable");
      return false;
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

void EthernetManager::disconnect() {
  VLF("MSG: Ethernet, disconnect ignored!");      
}

void EthernetManager::setStation(int number) {
  if (number >= 1 && number <= EthernetStationCount) stationNumber = number;
  sta = &settings.station[stationNumber - 1];
}

void EthernetManager::readSettings() {
  if (settingsReady) return;

  #ifdef NV_ETHERNET_SETTINGS_BASE
    if (EthernetSettingsSize < sizeof(EthernetSettings)) { nv.initError = true; DL("ERR: EthernetManager::init(); EthernetSettingsSize error"); }

    if (!nv.hasValidKey() || nv.isNull(NV_ETHERNET_SETTINGS_BASE, sizeof(EthernetSettings))) {
      VLF("MSG: Ethernet, writing defaults to NV");
      nv.writeBytes(NV_ETHERNET_SETTINGS_BASE, &settings, sizeof(EthernetSettings));
    }

    nv.readBytes(NV_ETHERNET_SETTINGS_BASE, &settings, sizeof(EthernetSettings));
  #endif

  #if DEBUG != OFF
    int currentStationNumber = stationNumber;

    VF("MSG: Ethernet, DHCP En     = "); VL(settings.dhcpEnabled);
    VF("MSG: Ethernet, IP          = "); V(settings.ip[0]); V("."); V(settings.ip[1]); V("."); V(settings.ip[2]); V("."); VL(settings.ip[3]);
    VF("MSG: Ethernet, GW          = "); V(settings.gw[0]); V("."); V(settings.gw[1]); V("."); V(settings.gw[2]); V("."); VL(settings.gw[3]);
    VF("MSG: Ethernet, SN          = "); V(settings.sn[0]); V("."); V(settings.sn[1]); V("."); V(settings.sn[2]); V("."); VL(settings.sn[3]);

    for (int station = 1; station <= EthernetStationCount; station++) {
      setStation(station);

      VF("MSG: Ethernet, Sta"); V(stationNumber); VF(" NAME   = "); VL(sta->host);
      VF("MSG: Ethernet, Sta"); V(stationNumber); VF(" TARGET = "); V(sta->target[0]); V("."); V(sta->target[1]); V("."); V(sta->target[2]); V("."); VL(sta->target[3]);
    }
    stationNumber = currentStationNumber;
  #endif

  settingsReady = true;
}

void EthernetManager::writeSettings() {
  if (!settingsReady) return;

  #ifdef NV_ETHERNET_SETTINGS_BASE
    VLF("MSG: Ethernet, writing settings to NV");
    nv.writeBytes(NV_ETHERNET_SETTINGS_BASE, &settings, sizeof(EthernetSettings));
  #endif
}

EthernetManager ethernetManager;

#endif
