// ethernet IP command server
#pragma once

#include "../../../Common.h"

#if defined(OPERATIONAL_MODE) && (OPERATIONAL_MODE == ETHERNET_W5100 || OPERATIONAL_MODE == ETHERNET_W5500) && \
  ((defined(STANDARD_COMMAND_CHANNEL) && STANDARD_COMMAND_CHANNEL == ON) || \
   (defined(PERSISTENT_COMMAND_CHANNEL) && PERSISTENT_COMMAND_CHANNEL == ON)) && !defined(SERIAL_IP_MODE)

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
