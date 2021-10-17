// wifi IP command server
#pragma once

#include "../../../Common.h"

#if defined(OPERATIONAL_MODE) && OPERATIONAL_MODE == WIFI && \
  ((defined(STANDARD_COMMAND_CHANNEL) && STANDARD_COMMAND_CHANNEL == ON) || \
   (defined(PERSISTENT_COMMAND_CHANNEL) && PERSISTENT_COMMAND_CHANNEL == ON)) && !defined(SERIAL_IP_MODE)

  #include "../WifiManager.h"

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
