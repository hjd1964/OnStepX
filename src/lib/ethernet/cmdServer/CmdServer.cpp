// ethernet IP command server
#include "CmdServer.h"

#if OPERATIONAL_MODE >= ETHERNET_FIRST && OPERATIONAL_MODE <= ETHERNET_LAST && COMMAND_SERVER != OFF

  #include "../../tasks/OnTask.h"
  #include "../../../libApp/cmd/Cmd.h"

  CmdServer::CmdServer(uint32_t port, long clientTimeoutMs, bool persist) {
    cmdBuffer[0] = 0;
    cmdBufferPos = 0;
    clientEndTimeMs = 0;
    this->clientTimeoutMs = clientTimeoutMs;
    this->persist = persist;
    cmdSvr = new EthernetServer(port);
  }

  void CmdServer::begin() {
    cmdSvr->begin();
  }

  void CmdServer::handleClient() {
    const unsigned long now = millis();

    // drop stale client socket (disconnected or timed out/hung)
    if (cmdSvrClient &&
       (!cmdSvrClient.connected() || (long)(clientEndTimeMs - now) < 0)) {
      cmdSvrClient.stop();
      cmdBuffer[0] = 0;
      cmdBufferPos = 0;
    }

    // new client
    if (!cmdSvrClient) {
      cmdSvrClient = cmdSvr->available();
      if (cmdSvrClient) {
        clientEndTimeMs = now + clientTimeoutMs;
        cmdBuffer[0] = 0;
        cmdBufferPos = 0;
      }
    }

    // check clients for data, if found get the command, pass to OnStep and pickup the response, then return the response to client
    while (cmdSvrClient && cmdSvrClient.connected() && cmdSvrClient.available() > 0) {
      // still active? push back disconnect
      if (persist) clientEndTimeMs = now + clientTimeoutMs;

      // get the data
      int c = cmdSvrClient.read();
      if (c < 0 || c > 255) break;

      // insert into the command buffer
      cmdBuffer[cmdBufferPos] = (char)c;
      cmdBufferPos++;
      if (cmdBufferPos > 39) cmdBufferPos = 39;
      cmdBuffer[cmdBufferPos] = 0;

      // send cmd and pickup the response
      if (c == '#' || (strlen(cmdBuffer) == 1 && c == (char)6)) {
        char result[40] = "";

        onStep.processCommand(cmdBuffer, result, cmdTimeout);

        // pickup response
        if (strlen(result) > 0) {
          if (cmdSvrClient && cmdSvrClient.connected()) {
            cmdSvrClient.print(result); // there was a delay(2); after this
          }
        }

        // reset command buffer
        cmdBuffer[0] = 0;
        cmdBufferPos = 0;
      } else tasks.yield();
    }
  }

#endif
