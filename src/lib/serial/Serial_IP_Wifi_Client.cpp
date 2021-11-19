// -----------------------------------------------------------------------------------
// IP communication routines

// original work by jesco-t

#include "Serial_IP_Wifi_Client.h"

#if OPERATIONAL_MODE == WIFI && SERIAL_CLIENT == ON

  bool IPSerialClient::begin(long port, unsigned long clientTimeoutMs, bool persist) { 
    if (active) return true;

    // special case where the port is the most common baud rate
    // so a standard call to begin(baud_rate) can still work
    if ((port < 9000 || port >= 10000 || port == 9600) && clientTimeoutMs == 2000 && persist == false) port = 9996;

    this->port = port;

    wifiManager.init();

    this->clientTimeoutMs = clientTimeoutMs;
    this->persist = persist;
    onStep = IPAddress(wifiManager.sta->target);

    VF("MSG: WiFi, waiting for connection");
    for (int i = 0; i < 5; i++) { if (WiFi.status() != WL_CONNECTED) { delay(2000); V("."); } }
    VL("");

    if (WiFi.status() != WL_CONNECTED) {
      VLF("WRN: WiFi, connection to target failed");
      return false;
    }

    delay(1000);
    if (cmdSvrClient.connect(onStep, port)) {
      VF("MSG: WiFi, started client to "); V(onStep.toString()); V(":"); VL(port);
      active = true;
      return true;
    } else {
      VLF("WRN: WiFi, connection to target failed");
      return false;
    }
  }

  void IPSerialClient::end() {
    if (!active) return;
    
    cmdSvrClient.stop();
    VLF("MSG: IPSerial, connection closed");
    WiFi.disconnect();
    VLF("MSG: IPSerial, disconnected");
    active = false;
  }

  bool IPSerialClient::isConnected() {
    if (WiFi.status() == WL_CONNECTED) {
      if (!cmdSvrClient.connected()) {
        if (cmdSvrClient.connect(onStep, port)) {
          VLF("MSG: WiFi, restarted client");
          return true;
        } else {
          VLF("WRN: WiFi, connection to target failed");
          return false;
        }
      } else return true;
    } else { active = false; return false; }
  }

  size_t IPSerialClient::write(uint8_t data) {
    if (!active || !isConnected()) return 0;

    return cmdSvrClient.write(data);
  }

  size_t IPSerialClient::write(const uint8_t *data, size_t quantity) {
    if (!active || !isConnected()) return 0;

    return cmdSvrClient.write(data, quantity);
  }

  int IPSerialClient::available(void) {
    if (!active || !isConnected()) return 0;

    return cmdSvrClient.available();
  }

  int IPSerialClient::read(void) {
    if (!active || !isConnected()) return -1;

    return cmdSvrClient.read();
  }

  int IPSerialClient::peek(void) {
    if (!active || !isConnected()) return -1;

    return cmdSvrClient.peek();
  }

  void IPSerialClient::flush(void) {
    if (!active || !isConnected()) return;

    return cmdSvrClient.flush();
  }

  IPSerialClient SerialIPClient;
#endif
