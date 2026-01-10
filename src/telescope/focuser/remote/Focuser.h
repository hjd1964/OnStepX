//--------------------------------------------------------------------------------------------------
// telescope focuser control local node, using the CANbus interface
#pragma once

#include "../../../Common.h"

#include "../FocuserBase.h"

#ifdef FOCUSER_CAN_CLIENT_PRESENT

#include "../../../lib/canTransport/CanTransportClient.h"
#include "../../../lib/canTransport/CanPayload.h"
#include "../../../libApp/commands/ProcessCmds.h"

class Focuser : public CanTransportClient {
  public:
    Focuser() : CanTransportClient((uint16_t)(CAN_FOCUSER_REQ_ID), (uint16_t)(CAN_FOCUSER_RSP_ID)) {}

    void begin();

    bool command(char *reply, char *command, char *parameter,
                 bool *suppressFrame, bool *numericReply, CommandError *commandError);

    // Heartbeat event, indexed by focuser 1..6 => focuserIdx 0..5
    inline void heartbeat(uint8_t focuserIdx) { lastHeartbeatMs[focuserIdx] = hb_stamp_2ms_odd(); }

  private:
    bool encodeRequest(uint8_t &opcode, uint8_t &tidop,
                       uint8_t requestPayload[8], uint8_t &requestLen,
                       char *command, char *parameter);

    bool decodeResponse(char *reply, uint8_t opcode,
                        const uint8_t responsePayload[8], uint8_t responseLen,
                        bool &supressFrame, bool &numericReply);

    static inline uint32_t hb_stamp_2ms_odd() {
      return millis() | 1UL;
    }

    // Heartbeat presence, indexed by focuser 1..6 => focuserIdx 0..5
    inline bool heartbeatFresh(int focuserIdx) const {
      if (focuserIdx < 0 || focuserIdx > 5) return false;
      const uint32_t last = lastHeartbeatMs[focuserIdx];
      if (last == 0U) return false;
      const uint32_t now = hb_stamp_2ms_odd();
      return (uint32_t)(now - last) <= 2000U;
    }

    int active = -1;

    // Heartbeat timestamps (ms), indexed by focuser 1..6 => [0..5]
    volatile uint32_t lastHeartbeatMs[6] = {0, 0, 0, 0, 0, 0};
};

extern Focuser focuser;

#endif
