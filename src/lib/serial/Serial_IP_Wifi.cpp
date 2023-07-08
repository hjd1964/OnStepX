// -----------------------------------------------------------------------------------
// IP communication routines

#include "Serial_IP_Wifi.h"

#if OPERATIONAL_MODE == WIFI && SERIAL_SERVER != OFF

  void IPSerial::begin(long port, unsigned long clientTimeoutMs, bool persist) {
    if (active) return;

    // special case where the port is the most common baud rate
    // so a standard call to begin(baud_rate) can still work
    if ((port < 9000 || port >= 10000 || port == 9600) && clientTimeoutMs == 2000 && persist == false) port = 9999;

    this->port = port;

    wifiManager.init();

    cmdSvr = new WiFiServer(port);
    delay(1000);

    cmdSvr->begin();
    cmdSvr->setNoDelay(true);
    VF("MSG: WiFi, started IP commandServer on port "); VL(port);

    this->clientTimeoutMs = clientTimeoutMs;
    this->persist = persist;
    active = true;

    delay(1000);
  }

  void IPSerial::end() {
    if (cmdSvrClient.connected()) {
      #if DEBUG_CMDSERVER == ON
        VLF("MSG: end(), STOP cmdSvrClient");
      #endif
      cmdSvrClient.stop();
    }
  }

  int IPSerial::available(void) {
    if (!active) return 0;

    if (!cmdSvrClient) {
      if (cmdSvr->hasClient()) {
        cmdSvrClient = cmdSvr->available();
        clientEndTimeMs = millis() + clientTimeoutMs;
      }
      if (cmdSvrClient) {
        #if DEBUG_CMDSERVER == ON
          VLF("MSG: available(), NEW cmdSvrClient");
        #endif
      } else return 0;
    } else {
      if (!cmdSvrClient.connected()) { 
        #if DEBUG_CMDSERVER == ON
          VLF("MSG: available(), not connected STOP cmdSvrClient");
        #endif
        cmdSvrClient.stop();
        return 0;
      }
      if ((long)(clientEndTimeMs - millis()) < 0) {
        #if DEBUG_CMDSERVER == ON
          VLF("MSG: available(), timed out STOP cmdSvrClient");
        #endif
        cmdSvrClient.stop();
        return 0;
      }
    }

    int i = cmdSvrClient.available();

    #if DEBUG_CMDSERVER == ON
      if (i > 0) { VF("MSG: available(), recv. buffer has "); V(i); VLF(" chars"); }
    #endif

    return i;
  }

  int IPSerial::peek(void) {
    if (!active || !cmdSvrClient) return -1;
    return cmdSvrClient.peek();
  }

  void IPSerial::flush(void) {
    if (!active || !cmdSvrClient) return;
    cmdSvrClient.flush();
  }

  int IPSerial::read(void) {
    if (!active || !cmdSvrClient) return -1;
    if (persist) clientEndTimeMs = millis() + clientTimeoutMs;
    int c = cmdSvrClient.read();
    #if DEBUG_CMDSERVER == ON
      VF("MSG: read(), found: "); VL((char)c);
    #endif
    return c;
  }

  size_t IPSerial::write(uint8_t data) {
    if (!active || !cmdSvrClient) return 0;
    return cmdSvrClient.write(data);
  }

  size_t IPSerial::write(const uint8_t *data, size_t count) {
    if (!active || !cmdSvrClient) return 0;
    return cmdSvrClient.write(data, count);
  }

  #if SERIAL_SERVER == STANDARD || SERIAL_SERVER == BOTH
    IPSerial SerialIP;
    #define SERIAL_SIP SerialIP
  #endif

  #if SERIAL_SERVER == PERSISTENT || SERIAL_SERVER == BOTH
    IPSerial SerialPIP1;
    IPSerial SerialPIP2;
    IPSerial SerialPIP3;
  #endif

#endif
