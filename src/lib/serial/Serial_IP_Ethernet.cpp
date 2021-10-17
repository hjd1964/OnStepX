// -----------------------------------------------------------------------------------
// IP communication routines

#include "Serial_IP_Ethernet.h"

#if defined(OPERATIONAL_MODE) && (OPERATIONAL_MODE == ETHERNET_W5100 || OPERATIONAL_MODE == ETHERNET_W5500) && \
    defined(SERIAL_IP_MODE) && (SERIAL_IP_MODE == STATION || SERIAL_IP_MODE == ON)

  bool port9999Assigned = false;
  bool port9998Assigned = false;

  void IPSerial::begin(long port, unsigned long clientTimeoutMs, bool persist) {
    if (active) return;

    this->port = port;

    ethernetManager.init();

    cmdSvr = new EthernetServer(port);
    cmdSvr->begin();
    VF("MSG: Ethernet started IP commandServer on port "); VL(port);

    this->clientTimeoutMs = clientTimeoutMs;
    this->persist = persist;
    active = true;

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
    if (!ethernetManager.active) return 0;

    if (!cmdSvrClient) {
      cmdSvrClient = cmdSvr->available();
      if (cmdSvrClient) {
        #if DEBUG_CMDSERVER == ON
          VLF("MSG: available(), NEW cmdSvrClient.");
        #endif
        clientEndTimeMs = millis() + clientTimeoutMs;
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
      if ((long)(clientEndTimeMs - millis()) < 0) {
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
    if (!ethernetManager.active || !cmdSvrClient) return -1;
    return cmdSvrClient.peek();
  }

  void IPSerial::flush(void) {
    if (!ethernetManager.active || !cmdSvrClient) return;
    cmdSvrClient.flush();
  }

  int IPSerial::read(void) {
    if (!ethernetManager.active || !cmdSvrClient) return -1;
    if (persist) clientEndTimeMs = millis() + clientTimeoutMs;
    int c = cmdSvrClient.read();
    #if DEBUG_CMDSERVER == ON
      VF("MSG: read(), found: "); VL((char)c);
    #endif
    return c;
  }

  size_t IPSerial::write(uint8_t data) {
    if (!ethernetManager.active || !cmdSvrClient) return 0;
    return cmdSvrClient.write(data);
  }

  size_t IPSerial::write(const uint8_t *data, size_t count) {
    if (!ethernetManager.active || !cmdSvrClient) return 0;
    return cmdSvrClient.write(data, count);
  }

  #if defined(STANDARD_IPSERIAL_CHANNEL) && STANDARD_IPSERIAL_CHANNEL == ON
    IPSerial ipSerial;
  #endif

  #if defined(PERSISTENT_IPSERIAL_CHANNEL) && PERSISTENT_IPSERIAL_CHANNEL == ON
    IPSerial pipSerial;
  #endif

#endif
