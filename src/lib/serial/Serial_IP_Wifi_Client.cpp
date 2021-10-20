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
    if ((port < 9000 || port >= 10000 || port == 9600) && clientTimeoutMs == 2000 && persist == false) port = 9999;

    this->port = port;

    wifiManager.init();

    this->clientTimeoutMs = clientTimeoutMs;
    this->persist = persist;
    IPAddress onStep = IPAddress(wifiManager.settings.target1_ip);

    VF("MSG: IPSerial, Target = "); VL(onStep.toString());

    delay(1000);
    if (cmdSvrClient.connect(onStep, port)) {
      active = true;
      VL("WRN: IPSerial, connection to target failed"); 
    }
  }

  void IPSerial::end() {
    cmdSvrClient.stop();
    VL("MSG: IPSerial, connection closed.");
    WiFi.disconnect();
    VL("MSG: IPSerial, disconnected.");
  }

  void IPSerial::paused(bool state){
    // dummy, not needed for WiFi implementation
  }

  bool IPSerial::isConnected(){
    return WiFi.status() == WL_CONNECTED; 
  }

  size_t IPSerial::write(uint8_t data) {
    if (cmdSvrClient.connected()) {
      D("LX200 command: "); DL((const char)data);
      cmdSvrClient.println((const char)data);
    } 

    return 1; 
  }

  size_t IPSerial::write(const uint8_t *data, size_t quantity) {
    if (cmdSvrClient.connected()) {
      D("LX200 command: "); DL((const char*)data);
      cmdSvrClient.println((const char*)data);
    } 
    return 1;
  }

  int IPSerial::available(void) {
    int a = cmdSvrClient.available();
    return a;
  }

  int IPSerial::read(void) {
    int c = cmdSvrClient.read();
    //D("OnStep response: "); DL(c);
    if (c == 0) c = -1;
    return c;
  }

  int IPSerial::peek(void) {
    // dummy, not needed for WiFi implementation
    return 1;
  }

  void IPSerial::flush(void) {
    // dummy, not needed for WiFi implementation
  }

  #if defined(STANDARD_IPSERIAL_CHANNEL) && STANDARD_IPSERIAL_CHANNEL == ON
    IPSerial ipSerial;
  #endif

  #if defined(PERSISTENT_IPSERIAL_CHANNEL) && PERSISTENT_IPSERIAL_CHANNEL == ON
    IPSerial pipSerial;
  #endif

#endif
