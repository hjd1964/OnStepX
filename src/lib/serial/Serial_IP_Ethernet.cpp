// -----------------------------------------------------------------------------------
// IP communication routines

#include "Serial_IP_Ethernet.h"

#if defined(OPERATIONAL_MODE) && (OPERATIONAL_MODE == ETHERNET_W5100 || OPERATIONAL_MODE == ETHERNET_W5500)

  bool port9999Assigned = false;
  bool port9998Assigned = false;

  void IPSerial::begin(long port) {
    if (port == 9999) { if (port9999Assigned) return; else port9999Assigned = true; } else
    if (port == 9998) { if (port9998Assigned) return; else port9998Assigned = true; } else return;

    this->port = port;

    if (!eth_active) {
      #ifdef W5500_CS_PIN
        Ethernet.init(W5500_CS_PIN);
      #endif
      Ethernet.begin(eth_mac, eth_ip, eth_dns, eth_gw, eth_sn);

      VF("MSG: Ethernet DHCP En = "); VL(eth_dhcp_enabled);
      VF("MSG: Ethernet IP = "); V(eth_ip[0]); V("."); V(eth_ip[1]); V("."); V(eth_ip[2]); V("."); VL(eth_ip[3]);
      VF("MSG: Ethernet GW = "); V(eth_gw[0]); V("."); V(eth_gw[1]); V("."); V(eth_gw[2]); V("."); VL(eth_gw[3]);
      VF("MSG: Ethernet SN = "); V(eth_sn[0]); V("."); V(eth_sn[1]); V("."); V(eth_sn[2]); V("."); VL(eth_sn[3]);

      #if OPERATIONAL_MODE == ETHERNET_W5500
        VF("MSG: Resetting W5500 using ETH_RESET_PIN ("); V(ETH_RESET_PIN); VL(")");
        pinMode(ETH_RESET_PIN, OUTPUT); 
        digitalWrite(ETH_RESET_PIN, LOW);
        delayMicroseconds(500);
        digitalWrite(ETH_RESET_PIN, HIGH);
        delayMicroseconds(1000);
        delay(1000);
      #endif

      VLF("MSG: Ethernet initialized");

      eth_active = true;
    }

    cmdSvr = new EthernetServer(port);
    cmdSvr->begin();
    VF("MSG: Ethernet started IP commandServer on port "); VL(port);

    // setup for persistent channel
    if (port < 9999) {
      timeout = 120000UL;
      resetTimeout = true;
    }

    delay(1000);
  }

  void IPSerial::restart() {
    cmdSvr->begin();
  }

  void IPSerial::end() {
    if (cmdSvrClient.connected()) {
      #if DEBUG_CMDSERVER == ON
        VLF("MSG: end(), STOP cmdSvrClient.");
      #endif
      cmdSvrClient.stop();
    }
  }

  int IPSerial::available(void) {
    if (!eth_active) return 0;

    if (!cmdSvrClient) {
      cmdSvrClient = cmdSvr->available();
      if (cmdSvrClient) {
        #if DEBUG_CMDSERVER == ON
          VLF("MSG: available(), NEW cmdSvrClient.");
        #endif
        clientTimeout = millis() + timeout;
        cmdSvrClient.setTimeout(1000);
      }
    } else {
      if (!cmdSvrClient.connected()) { 
        #if DEBUG_CMDSERVER == ON
          VLF("MSG: available(), not connected STOP cmdSvrClient.");
        #endif
        cmdSvrClient.stop();
        return 0;
      }
      if ((long)(clientTimeout - millis()) < 0) {
        #if DEBUG_CMDSERVER == ON
          VLF("MSG: available(), timed out STOP cmdSvrClient.");
        #endif
        cmdSvrClient.stop();
        return 0;
      }
    }

    int i = cmdSvrClient.available();

    #if DEBUG_CMDSERVER == ON
      if (i > 0) { VF("MSG: available(), recv. buffer has "); V(i); VLF(" chars."); }
    #endif

    return i;
  }

  int IPSerial::peek(void) {
    if (!eth_active || !cmdSvrClient) return -1;
    return cmdSvrClient.peek();
  }

  void IPSerial::flush(void) {
    if (!eth_active || !cmdSvrClient) return;
    cmdSvrClient.flush();
  }

  int IPSerial::read(void) {
    if (!eth_active || !cmdSvrClient) return -1;
    if (resetTimeout) clientTimeout = millis() + timeout;
    int c = cmdSvrClient.read();
    #if DEBUG_CMDSERVER == ON
      VF("MSG: read(), found: "); VL((char)c);
    #endif
    return c;
  }

  size_t IPSerial::write(uint8_t data) {
    if (!eth_active || !cmdSvrClient) return 0;
    return cmdSvrClient.write(data);
  }

  size_t IPSerial::write(const uint8_t *data, size_t count) {
    if (!eth_active || !cmdSvrClient) return 0;
    return cmdSvrClient.write(data, count);
  }

  #if STANDARD_COMMAND_CHANNEL == ON
    IPSerial ipSerial;
  #endif

  #if PERSISTENT_COMMAND_CHANNEL == ON
    IPSerial pipSerial;
  #endif

#endif
