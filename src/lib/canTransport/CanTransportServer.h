// -----------------------------------------------------------------------------------
// CAN transport - Server (ISR-safe enqueue; process() executes commands)
//
// Request:   [ tidop, args... ]                       (1..8 bytes)
// Response0: [ tidop,   status, payload0... ]         (2..8 bytes)
// Response1: [ tidop+1, status, payload1... ]         (2..8 bytes)   // dual ops only
//
// Notes:
// - Uses global canPlus
// - ISR-safe: callback only enqueues; process() drains and calls processCommand()
// - process() binds RX via beginNewRequest() (args at rxPos=1) and starts a new TX response via beginNewResponse()
// - Dual-frame responses enabled when: opcode >= opDualResponse
//   In dual mode, frame0 and frame1 are always sent, with DLC matching payload length
// -----------------------------------------------------------------------------------

#pragma once

#include "../../Common.h"

#if defined(CAN_PLUS) && CAN_PLUS != OFF

#include "../tasks/OnTask.h"
#include "../canPlus/CanPlus.h"
#include "CanTransportBase.h"

#ifndef TRANSPORT_CAN_SERVER_RXQ
  #define TRANSPORT_CAN_SERVER_RXQ 16
#endif

class CanTransportServer : public CanTransport {
public:
  CanTransportServer(uint16_t requestId, uint16_t responseId, uint8_t dualFrameOpcodeStart = 32)
    : CanTransport(requestId, responseId, dualFrameOpcodeStart) {}

  bool init(bool startTask = true, uint16_t processPeriodMs = 2);

protected:
  // ISR ingress: enqueue only (do not execute command processing here)
  void onFrame(const uint8_t data[8], uint8_t len) override;

  // Send response using current tidop (captured from the request frame).
  // Uses TX payload bytes written so far (0..12) via txBuf/txPos in base.
  void sendResponse(bool handled, bool suppressFrame, bool numericReply, CommandError commandError);

  void process() override;

private:
  struct Frame {
    uint8_t len;
    uint8_t data[8];
  };

  void enqueueFromCallback(const uint8_t data[8], uint8_t len);
  bool dequeue(Frame &out);

  static inline uint8_t tidopPlus1(uint8_t t) {
    // increment TID (bits 7..5) modulo 8, opcode unchanged
    return (uint8_t)(((t & 0xE0u) + 0x20u) | (t & 0x1Fu));
  }

  // RX queue (written from callback, drained in process())
  volatile uint8_t qHead = 0;
  volatile uint8_t qTail = 0;
  Frame q[TRANSPORT_CAN_SERVER_RXQ];
};

#endif
