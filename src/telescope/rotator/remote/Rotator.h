//--------------------------------------------------------------------------------------------------
// remote telescope rotator control, using the CANbus interface
#pragma once

#include "../../../Common.h"

#include "../RotatorBase.h"

#ifdef ROTATOR_CAN_CLIENT_PRESENT

#include "../../../lib/canTransport/CanTransportClient.h"
#include "../../../lib/canTransport/CanPayload.h"
#include "../../../libApp/commands/ProcessCmds.h"

class Rotator : public CanTransportClient {
  public:
    Rotator() : CanTransportClient((uint16_t)(CAN_ROTATOR_REQ_ID), (uint16_t)(CAN_ROTATOR_RSP_ID)) {}

    void begin() {}

    bool command(char *reply, char *command, char *parameter,
                 bool *supressFrame, bool *numericReply, CommandError *commandError);

  private:
    bool encodeRequest(uint8_t &opcode, uint8_t &tidop,
                       uint8_t requestPayload[8], uint8_t &requestLen,
                       char *command, char *parameter);

    bool decodeResponse(char *reply, uint8_t opcode,
                        const uint8_t responsePayload[8], uint8_t responseLen,
                        bool &supressFrame, bool &numericReply);
};

extern Rotator rotator;

#endif
