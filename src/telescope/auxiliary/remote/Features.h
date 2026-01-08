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
    Features() : CanTransportClient((uint16_t)(CAN_FEATURES_REQ_ID),
                                    (uint16_t)(CAN_FEATURES_RSP_ID)) {}

    // initialize the aux features
    void begin();

    bool command(char *reply, char *command, char *parameter,
                 bool *supressFrame, bool *numericReply, CommandError *commandError);

    // Heartbeat event
    inline void heartbeat() { lastHeartbeatMs = hb_stamp_2ms_odd(); }

  private:
    bool encodeRequest(uint8_t &opcode, uint8_t &tidop,
                       uint8_t requestPayload[8], uint8_t &requestLen,
                       char *command, char *parameter);

    bool decodeResponse(char *reply, uint8_t opcode,
                        const uint8_t *responsePayload, uint8_t responseLen,
                        bool &supressFrame, bool &numericReply);

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
