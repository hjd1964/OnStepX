// -----------------------------------------------------------------------------------
// IP communication routines

#include "Serial_IP_Wifi.h"

#if defined(OPERATIONAL_MODE) && OPERATIONAL_MODE == WIFI && \
    defined(SERIAL_IP_MODE) && (SERIAL_IP_MODE == STATION || SERIAL_IP_MODE == ACCESS_POINT)

  bool wifiActive = false;
  bool port9999Assigned = false;
  bool port9998Assigned = false;

  void IPSerial::begin(long port) {
    if (port == 9999) { if (port9999Assigned) return; else port9999Assigned = true; } else
    if (port == 9998) { if (port9998Assigned) return; else port9998Assigned = true; } else return;

    this->port = port;

    wifiManager.init();

    cmdSvr = new WiFiServer(port);
    delay(1000);
    cmdSvr->begin();
    cmdSvr->setNoDelay(true);
    VF("MSG: WiFi started IP commandServer on port "); VL(port);

    // setup for persistent channel
    if (port == 9998) {
      timeout = 120000UL;
      resetTimeout = true;
    }

    delay(1000);
  }

  void IPSerial::end() {
    if (cmdSvrClient.connected()) {
      cmdSvrClient.stop();
    }
  }

  int IPSerial::available(void) {
    if (!wifiActive) return 0;

    if (!cmdSvrClient) {
      if (cmdSvr->hasClient()) {
        clientTimeout = millis() + timeout;
        cmdSvrClient = cmdSvr->available();
        cmdSvrClient.setTimeout(1000);
      }
    } else {
      if (!cmdSvrClient.connected()) { 
        cmdSvrClient.stop();
        return 0;
      }
      if ((long)(clientTimeout - millis()) < 0) {
        cmdSvrClient.stop();
        return 0;
      }
    }

    return cmdSvrClient.available();
  }

  int IPSerial::peek(void) {
    if (!wifiActive || !cmdSvrClient) return -1;
    return cmdSvrClient.peek();
  }

  void IPSerial::flush(void) {
    if (!wifiActive || !cmdSvrClient) return;
    cmdSvrClient.flush();
  }

  int IPSerial::read(void) {
    if (!wifiActive || !cmdSvrClient) return -1;
    if (resetTimeout) clientTimeout = millis() + timeout;
    int c = cmdSvrClient.read();
    return c;
  }

  size_t IPSerial::write(uint8_t data) {
    if (!wifiActive || !cmdSvrClient) return 0;
    return cmdSvrClient.write(data);
  }

  size_t IPSerial::write(const uint8_t *data, size_t count) {
    if (!wifiActive || !cmdSvrClient) return 0;
    return cmdSvrClient.write(data, count);
  }

  #if defined(STANDARD_IPSERIAL_CHANNEL) && STANDARD_COMMAND_CHANNEL == ON
    IPSerial ipSerial;
  #endif

  #if defined(PERSISTENT_COMMAND_CHANNEL) && PERSISTENT_COMMAND_CHANNEL == ON
    IPSerial pipSerial;
  #endif

#endif
