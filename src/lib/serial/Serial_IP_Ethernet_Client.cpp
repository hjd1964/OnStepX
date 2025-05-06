// -----------------------------------------------------------------------------------
// IP communication routines

#include "Serial_IP_Ethernet_Client.h"

#if OPERATIONAL_MODE >= ETHERNET_FIRST && OPERATIONAL_MODE <= ETHERNET_LAST && SERIAL_CLIENT != OFF

  #include "../tasks/OnTask.h"

  void pollIPClient() { SerialIPClient.poll(); }

  bool IPSerialClient::begin(long port, unsigned long clientTimeoutMs, bool persist) {
    if (active) return true;

    // special case where the port is the most common baud rate
    // so a standard call to begin(baud_rate) can still work
    if ((port < 9000 || port >= 10000 || port == 9600) && clientTimeoutMs == 2000 && persist == false) port = 9999;

    this->port = port;
    this->clientTimeoutMs = clientTimeoutMs;
    this->persist = persist;

    if (!ethernetManager.init()) {
      DLF("WRN: IPSerialClient, failed to start Ethernet");
      return false;
    }

    delay(1000);

    onStep = IPAddress(ethernetManager.sta->target);
    VF("MSG: IPSerialClient, target "); V(onStep[0]); V("."); V(onStep[1]); V("."); V(onStep[2]); V("."); V(onStep[3]); V(":"); VL(port);

    if (!persist) {
      lastActivityTimeMs = millis();
      VF("MSG: Setup, start IPSerialClient monitor task (rate 1s priority 7)... ");
      if (tasks.add(1000, 0, true, 7, pollIPClient, "PollIP")) { VLF("success"); } else { VLF("FAILED!"); }
    }

    active = true;
    return true;
  }

  void IPSerialClient::end() {
    if (!active) return;
    
    cmdSvrClient.stop();
    VLF("MSG: IPSerialClient, connection to target closed");

    tasks.remove(tasks.getHandleByName("PollIP"));

    active = false;
  }

  bool IPSerialClient::isConnected() {
    lastActivityTimeMs = millis();
    if (Ethernet.linkStatus() != LinkOFF) {
      if (!cmdSvrClient.connected()) {
        cmdSvrClient.stop();
        if (cmdSvrClient.connect(onStep, port)) {
          VLF("MSG: IPSerialClient, connection to target restarted");
          return true;
        } else {
          DLF("WRN: IPSerialClient, connection to target failed");
          return false;
        }
      } else return true;
    } else {
      DLF("WRN: IPSerialClient, connection to target failed no cable"); 
      return false;
    }
  }

  void IPSerialClient::flush(void) {
    if (!active || !isConnected()) return;

    return cmdSvrClient.flush();
  }

  int IPSerialClient::available(void) {
    if (!active || !isConnected()) return 0;

    return cmdSvrClient.available();
  }

  int IPSerialClient::peek(void) {
    if (!active || !isConnected()) return -1;

    return cmdSvrClient.peek();
  }

  int IPSerialClient::read(void) {
    if (!active || !isConnected()) return -1;
    char c = cmdSvrClient.read();
    return c;
  }

  size_t IPSerialClient::write(uint8_t data) {
    if (!active || !isConnected()) return 0;
    return cmdSvrClient.write(data);
  }

  size_t IPSerialClient::write(const uint8_t *data, size_t quantity) {
    if (!active || !isConnected()) return 0;
    return cmdSvrClient.write(data, quantity);
  }

  void IPSerialClient::poll() {
    if ((long)(millis() - lastActivityTimeMs) > (long)clientTimeoutMs && cmdSvrClient.connected()) cmdSvrClient.stop();
  }

  IPSerialClient SerialIPClient;

#endif
