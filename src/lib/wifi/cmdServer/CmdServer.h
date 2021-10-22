// wifi IP command server
#pragma once

#include "../../../Common.h"
#include "../WifiManager.h"

#if OPERATIONAL_MODE == WIFI && COMMAND_SERVER != OFF

  class CmdServer {
    public:
      CmdServer(uint32_t port, long clientTimeoutMs, bool persist = false);
      void begin();
      void handleClient();

    private:
      WiFiServer *cmdSvr;
      WiFiClient cmdSvrClient;

      unsigned long clientTimeoutMs;
      unsigned long clientEndTimeMs = 0;
      bool persist;
      long port;
  };

#endif
