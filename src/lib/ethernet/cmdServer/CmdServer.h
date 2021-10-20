// ethernet IP command server
#pragma once

#include "../../../Common.h"

#if !defined(SERIAL_IP_MODE) && \
    (OPERATIONAL_MODE == ETHERNET_W5100 || OPERATIONAL_MODE == ETHERNET_W5500) && \
    (STANDARD_COMMAND_CHANNEL == ON || PERSISTENT_COMMAND_CHANNEL == ON)

  #include "../EthernetManager.h"

  class CmdServer {
    public:
      CmdServer(uint32_t port, long clientTimeoutMs, bool persist = false);
      void begin();
      void handleClient();

    private:
      EthernetServer *cmdSvr;
      EthernetClient cmdSvrClient;

      unsigned long clientTimeoutMs;
      unsigned long clientEndTimeMs = 0;
      bool persist;
  };

#endif
