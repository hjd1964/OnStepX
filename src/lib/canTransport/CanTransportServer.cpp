// -----------------------------------------------------------------------------------
// CAN transport - Server
// -----------------------------------------------------------------------------------

#include "CanTransportServer.h"

#if defined(CAN_PLUS) && CAN_PLUS != OFF

bool CanTransportServer::init(bool startTask, uint16_t processPeriodMs) {
  if (!CanTransport::init()) return false;

  // Register request callback for this CAN ID; thunk routes to onFrame()
  if (!canPlus.callbackRegisterId((int)requestCanId(), thunkForSlot(slot))) return false;

  if (startTask) {
    if (!tasks.add(processPeriodMs, 0, true, 5, &CanTransport::taskThunk, "CanTrS")) return false;
  }

  return true;
}

void CanTransportServer::onFrame(const uint8_t data[8], uint8_t len) {
  enqueueFromCallback(data, len);
}

void CanTransportServer::process() {
  Frame f;
  while (dequeue(f)) {
    if (f.len < 1) continue;

    // Bind RX (copies into base rxBuf, sets tidop/opcode, sets rxPos=1)
    beginNewRequest(f.data, f.len);

    // Fresh TX response (txPos=0)
    beginNewResponse();

    // Execute command in non-ISR context
    processCommand();
  }
}

void CanTransportServer::sendResponse(bool handled, bool suppressFrame, bool numericReply, CommandError commandError) {
  const uint8_t status = packStatus(handled, suppressFrame, numericReply, commandError);

  // Payload bytes written so far (cap at txMax == 12)
  uint8_t plen = txPos;
  if (plen > txMax) plen = txMax;

  const bool dual = (opcode >= opDualResponse);

  // Single-frame mode: clamp payload to 6
  if (!dual && plen > 6) plen = 6;

  // ---------------- frame 0 ----------------
  uint8_t out0[8] = {0};
  out0[0] = tidop;
  out0[1] = status;

  const uint8_t n0 = (plen > 6) ? 6 : plen;        // 0..6
  for (uint8_t i = 0; i < n0; ++i) out0[2 + i] = txBuf[i];

  const uint8_t out0Len = (uint8_t)(2 + n0);       // 2..8
  canPlus.txWait();
  (void)canPlus.writePacket((int)responseCanId(), out0, out0Len);

  // ---------------- optional frame 1 ----------------
  if (dual) {
    const uint8_t tidop1 = tidopPlus1(tidop);

    uint8_t out1[8] = {0};
    out1[0] = tidop1;
    out1[1] = status;

    const uint8_t rem = (plen > 6) ? (uint8_t)(plen - 6) : 0; // 0..6
    const uint8_t n1  = (rem > 6) ? 6 : rem;                 // 0..6

    for (uint8_t i = 0; i < n1; ++i) out1[2 + i] = txBuf[6 + i];

    const uint8_t out1Len = (uint8_t)(2 + n1);               // 2..8 (may be 2)
    canPlus.txWait();
    (void)canPlus.writePacket((int)responseCanId(), out1, out1Len);
  }
}

void CanTransportServer::enqueueFromCallback(const uint8_t data[8], uint8_t len) {
  if (len > 8) len = 8;
  if (len < 1) return;

  const uint8_t head = qHead;
  const uint8_t next = (uint8_t)((head + 1u) % (uint8_t)TRANSPORT_CAN_SERVER_RXQ);
  if (next == qTail) return; // full, drop

  q[head].len = len;
  for (uint8_t i = 0; i < len; i++) q[head].data[i] = data[i];

  qHead = next;
}

bool CanTransportServer::dequeue(Frame &out) {
  const uint8_t tail = qTail;
  if (tail == qHead) return false;

  out = q[tail];
  qTail = (uint8_t)((tail + 1u) % (uint8_t)TRANSPORT_CAN_SERVER_RXQ);
  return true;
}

#endif
