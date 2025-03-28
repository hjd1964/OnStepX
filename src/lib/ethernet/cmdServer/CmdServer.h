// ethernet IP command server
#pragma once

#include "../../../Common.h"
#include "../EthernetManager.h"

#if OPERATIONAL_MODE >= ETHERNET_FIRST && OPERATIONAL_MODE <= ETHERNET_LAST && COMMAND_SERVER != OFF

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
