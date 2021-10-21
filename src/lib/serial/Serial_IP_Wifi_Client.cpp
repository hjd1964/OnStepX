// -----------------------------------------------------------------------------------
// IP communication routines

// original work by jesco-t

#include "Serial_IP_Wifi_Client.h"

#if (SERIAL_IP_MODE == STATION || SERIAL_IP_MODE == ACCESS_POINT) && defined(SERIAL_IP_CLIENT) && \
    OPERATIONAL_MODE == WIFI

  void IPSerial::begin(long port, unsigned long clientTimeoutMs, bool persist) { 
    if (active) return;

    // special case where the port is the most common baud rate
    // so a standard call to begin(baud_rate) can still work
    if ((port < 9000 || port >= 10000 || port == 9600) && clientTimeoutMs == 2000 && persist == false) port = 9998;

    this->port = port;

    wifiManager.init();

    this->clientTimeoutMs = clientTimeoutMs;
    this->persist = persist;
    onStep = IPAddress(wifiManager.settings.target1_ip);

    VF("MSG: WiFi waiting for connection");
    while (WiFi.status() != WL_CONNECTED) { delay(500); V("."); }
    VL("");

    delay(1000);
    if (cmdSvrClient.connect(onStep, port)) {
      VF("MSG: WiFi started client to "); V(onStep.toString()); V(":"); VL(port);
      active = true;
    } else VL("WRN: WiFi connection to target failed"); 
  }

  void IPSerial::end() {
    cmdSvrClient.stop();
    VL("MSG: IPSerial, connection closed");
    WiFi.disconnect();
    VL("MSG: IPSerial, disconnected");
  }

  bool IPSerial::isConnected() {
    if (WiFi.status() == WL_CONNECTED) {
      if (!cmdSvrClient.connected()) {
        if (cmdSvrClient.connect(onStep, port)) {
          VLF("MSG: WiFi restarted client");
          return true;
        } else {
          VLF("WRN: WiFi connection to target failed");
          return false;
        }
      } else return true;
    } else return false;
  }

  size_t IPSerial::write(uint8_t data) {
    if (!active || !isConnected()) return 0;

    return cmdSvrClient.write(data);
  }

  size_t IPSerial::write(const uint8_t *data, size_t quantity) {
    if (!active || !isConnected()) return 0;

    return cmdSvrClient.write(data, quantity);
  }

  int IPSerial::available(void) {
    if (!active || !isConnected()) return 0;

    return cmdSvrClient.available();
  }

  int IPSerial::read(void) {
    if (!active || !isConnected()) return -1;

    return cmdSvrClient.read();
  }

  int IPSerial::peek(void) {
    if (!active || !isConnected()) return -1;

    return cmdSvrClient.peek();
  }

  void IPSerial::flush(void) {
    if (!active || !isConnected()) return;

    return cmdSvrClient.flush();
  }

  #if defined(STANDARD_IPSERIAL_CHANNEL) && STANDARD_IPSERIAL_CHANNEL == ON
    IPSerial SerialIP;
  #endif

#endif
