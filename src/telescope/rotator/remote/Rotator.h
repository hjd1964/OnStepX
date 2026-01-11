//--------------------------------------------------------------------------------------------------
// remote telescope rotator control, using the CANbus interface
#pragma once

#include "../../../Common.h"

#include "../RotatorBase.h"

#ifdef ROTATOR_CAN_CLIENT_PRESENT

#include "../../../lib/canTransport/CanTransportClient.h"
#include "../../../libApp/commands/ProcessCmds.h"

class Rotator : public CanTransportClient {
  public:
    Rotator() : CanTransportClient((uint16_t)(CAN_ROTATOR_REQ_ID), (uint16_t)(CAN_ROTATOR_RSP_ID)) {}

    void begin();

    bool command(char *reply, char *command, char *parameter,
                 bool *suppressFrame, bool *numericReply, CommandError *commandError);

    // Heartbeat event
    inline void heartbeat() { lastHeartbeatMs = hb_stamp_2ms_odd(); }

  private:
    bool encodeRequest(char *command, char *parameter);
    bool decodeResponse(char *reply);

    static inline uint32_t hb_stamp_2ms_odd() {
      return millis() | 1UL;
    }

    // Heartbeat presence
    inline bool heartbeatFresh() const {
      const uint32_t last = ATOMIC_LOAD(lastHeartbeatMs);
      if (last == 0U) return false;
      const uint32_t now = hb_stamp_2ms_odd();
      return (uint32_t)(now - last) <= 2000U;
    }

    // Heartbeat timestamps (ms)
    volatile uint32_t lastHeartbeatMs = 0;
};

extern Rotator rotator;

#endif
