// -----------------------------------------------------------------------------------
// IP communication routines

#include "Serial_IP_Wifi.h"

#if defined(OPERATIONAL_MODE) && OPERATIONAL_MODE == WIFI && \
    defined(SERIAL_IP_MODE) && (SERIAL_IP_MODE == STATION || SERIAL_IP_MODE == ACCESS_POINT)

  void IPSerial::begin(long port, unsigned long clientTimeoutMs, bool persist) {
    if (active) return;

    this->port = port;

    wifiManager.init();

    cmdSvr = new WiFiServer(port);
    delay(1000);

    cmdSvr->begin();
    cmdSvr->setNoDelay(true);
    VF("MSG: WiFi started IP commandServer on port "); VL(port);

    this->clientTimeoutMs = clientTimeoutMs;
    this->persist = persist;
    active = true;

    delay(1000);
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
    if (!active) return 0;

    if (!cmdSvrClient) {
      if (cmdSvr->hasClient()) {
        #if DEBUG_CMDSERVER == ON
          VLF("MSG: available(), NEW cmdSvrClient.");
        #endif
        cmdSvrClient = cmdSvr->available();
        clientEndTimeMs = millis() + clientTimeoutMs;
//        cmdSvrClient.setTimeout(1000);
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

  #if defined(STANDARD_IPSERIAL_CHANNEL) && STANDARD_COMMAND_CHANNEL == ON
    IPSerial ipSerial;
  #endif

  #if defined(PERSISTENT_COMMAND_CHANNEL) && PERSISTENT_COMMAND_CHANNEL == ON
    IPSerial pipSerial;
  #endif

#endif
