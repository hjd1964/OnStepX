//--------------------------------------------------------------------------------------------------
// remote telescope auxiliary FEATURES control, using the CANbus interface
#pragma once

#include "../../../Common.h"

#include "../FeaturesBase.h"

#ifdef FEATURES_CAN_CLIENT_PRESENT

#include "../../../lib/canTransport/CanTransportClient.h"
#include "../../../libApp/commands/ProcessCmds.h"

class Features : public CanTransportClient {
public:
  Features()
    : CanTransportClient((uint16_t)(CAN_FEATURES_REQ_ID), (uint16_t)(CAN_FEATURES_RSP_ID), 0x1E) {}

  // initialize the aux features
  void begin();

  bool command(char *reply, char *command, char *parameter,
               bool *suppressFrame, bool *numericReply, CommandError *commandError);

  // Heartbeat event
  inline void heartbeat() { lastHeartbeatMs = hb_stamp_2ms_odd(); }

private:
    bool encodeRequest(char *command, char *parameter);
    bool decodeResponse(char *reply);

  static inline uint32_t hb_stamp_2ms_odd() { return millis() | 1UL; }

  inline bool heartbeatFresh() const {
    const uint32_t last = ATOMIC_LOAD(lastHeartbeatMs);
    if (last == 0U) return false;
    const uint32_t now = hb_stamp_2ms_odd();
    return (uint32_t)(now - last) <= 2000U;
  }

  volatile uint32_t lastHeartbeatMs = 0;
};

extern Features features;

#endif
