// -----------------------------------------------------------------------------------
// CAN transport - Client (single/dual-frame request/response, 1 in-flight per instance)
// -----------------------------------------------------------------------------------

#pragma once

#include "../../Common.h"

#if defined(CAN_PLUS) && CAN_PLUS != OFF

#include "../tasks/OnTask.h"
#include "../canPlus/CanPlus.h"
#include "CanTransportBase.h"

class CanTransportClient : public CanTransport {
public:
  CanTransportClient(uint16_t requestId, uint16_t responseId, uint8_t dualFrameOpcodeStart = 32)
    : CanTransport(requestId, responseId, dualFrameOpcodeStart) {}

  bool init();

  // Changes timeout, default is 10ms
  void setTimeoutMs(uint16_t ms) { timeoutMs = ms; }

protected:
  // Clients do NOT treat inbound frames as requests; they treat them as responses.
  void onFrame(const uint8_t data[8], uint8_t len) override { onResponse(data, len); }

  // Start building a request frame using opCode (0..31).
  // Resets TX cursor and writes tidop as the first byte.
  bool beginNewRequest(uint8_t opCode);

  // Sends the request and waits for the reply.
  // Auto-selects single/dual based on opcode >= opDualResponse.
  // On success:
  //   rxLen/rxPos updated; rxPos=2 (payload start after tidop/status)
  //   readXXX() helpers read from rxBuf via rxPos
  bool transactRequest(bool &handled, bool &suppressFrame, bool &numericReply, CommandError &commandError);

private:
  // Response callback invoked via onFrame().
  void onResponse(const uint8_t data[8], uint8_t len) override;

  // Send request and wait for correlated response (tidop match).
  bool transact(uint8_t expectedTidOp,
                const uint8_t *requestPayload, uint8_t requestLen,
                uint8_t responsePayload[8], uint8_t &responseLen);

  // Send request and wait for two correlated responses (tidop and tidop+1 match).
  bool transact2(uint8_t expectedTidOp,
                 const uint8_t *requestPayload, uint8_t requestLen,
                 uint8_t responsePayload[14], uint8_t &responseLen);

  static inline uint8_t tidopPlus1(uint8_t tidop) {
    const uint8_t op  = (uint8_t)(tidop & 0x1F);
    const uint8_t tid = (uint8_t)((tidop >> 5) & 0x07);
    const uint8_t tid1 = (uint8_t)((tid + 1) & 0x07);
    return (uint8_t)((tid1 << 5) | op);
  }

  uint16_t timeoutMs = 50;

  // Per-instance TID (3 bits). No global coupling.
  uint8_t tid = 0;

  bool callbackRegistered = false;

  // in-flight request correlation mode: 0=idle, 1=single, 2=dual
  volatile uint8_t rspMode = 0;

  // first in-flight response correlation
  volatile bool    rspReady = false;
  volatile uint8_t rspLenLatched = 0;
  volatile uint8_t rspTidOpLatched = 0;
  uint8_t rspBuf[8] = {0};

  // second in-flight response correlation
  volatile bool    rsp1Ready = false;
  volatile uint8_t rsp1LenLatched = 0;
  volatile uint8_t rsp1TidOpLatched = 0;
  uint8_t rsp1Buf[8] = {0};
};

#endif
