// -----------------------------------------------------------------------------------
// CAN transport - Client
// -----------------------------------------------------------------------------------

#include "CanTransportClient.h"

#if defined(CAN_PLUS) && CAN_PLUS != OFF

#include <string.h>

bool CanTransportClient::init() {
  if (!CanTransport::init()) return false;

  if (callbackRegistered) return true;

  if (canPlus.callbackRegisterId((int)responseCanId(), thunkForSlot(slot))) {
    callbackRegistered = true;
  }

  return callbackRegistered;
}

// -----------------------------------------------------------------------------
// Build request (TX cursor writes into base txBuf/txPos)
// -----------------------------------------------------------------------------
bool CanTransportClient::beginNewRequest(uint8_t opCode) {
  if (!canPlus.ready) return false;

  tid = (uint8_t)((tid + 1) & 0x07);

  opcode = (uint8_t)(opCode & 0x1F);
  tidop  = packTidOp(tid, opcode);

  // Use base TX buffer for request build
  resetTx();

  // First byte on wire is tidop
  return writeU8(tidop);
}

// -----------------------------------------------------------------------------
// Unified transaction: chooses transact() vs transact2() based on opDualResponse
// On success sets RX cursor to payload start (rxPos=2).
//
// IMPORTANT: on failure, does NOT modify handled/suppressFrame/numericReply/commandError.
// -----------------------------------------------------------------------------
bool CanTransportClient::transactRequest(bool &handled, bool &suppressFrame, bool &numericReply, CommandError &commandError) {
  if (!canPlus.ready) return false;

  // Must have at least tidop written, and <= 8 total
  if (txPos < 1 || txPos > 8) return false;

  const uint8_t op = unpackOp(tidop);

  // Build combined response directly into base rxBuf
  uint8_t combinedLen = 0;
  bool ok = false;

  if (op >= opDualResponse) {
    memset(rxBuf, 0, sizeof(rxBuf));
    ok = transact2(tidop, txBuf, txPos, rxBuf, combinedLen);
  } else {
    memset(rxBuf, 0, 8);
    ok = transact(tidop, txBuf, txPos, rxBuf, combinedLen);
  }

  if (!ok) return false;
  if (combinedLen < 2 || combinedLen > sizeof(rxBuf)) return false;
  if (rxBuf[0] != tidop) return false;

  bool h = false;
  bool sf = false;
  bool nr = true;
  CommandError ce = CE_NONE;
  unpackStatus(rxBuf[1], h, sf, nr, ce);

  // Commit outputs only on success
  handled = h;
  suppressFrame = sf;
  numericReply = nr;
  commandError = ce;

  // Commit RX cursor for payload reads
  rxLen = combinedLen;
  rxPos = 2;

  return true;
}

// -----------------------------------------------------------------------------
// Single-frame transaction
// -----------------------------------------------------------------------------
bool CanTransportClient::transact(uint8_t expectedTidOp,
                                  const uint8_t *requestPayload, uint8_t requestLen,
                                  uint8_t responsePayload[8], uint8_t &responseLen) {
  responseLen = 0;
  if (!canPlus.ready) return false;
  if (!requestPayload || requestLen < 1 || requestLen > 8) return false;
  if (!responsePayload) return false;

  rspMode = 1;
  rspReady = false;
  rspLenLatched = 0;
  rspTidOpLatched = expectedTidOp;

  canPlus.txWait();
  if (!canPlus.writePacket((int)requestCanId(), requestPayload, requestLen)) {
    rspMode = 0;
    return false;
  }

  const uint32_t startUs   = micros();
  const uint32_t timeoutUs = (uint32_t)timeoutMs * 1000UL;

  while (!rspReady) {
    canPlus.rxBurst();
    if ((uint32_t)(micros() - startUs) > timeoutUs) {
      rspMode = 0;
      return false;
    }
    tasks.yield();
  }

  responseLen = rspLenLatched;
  memcpy(responsePayload, rspBuf, rspLenLatched);

  rspReady = false;
  rspMode = 0;

  return (responseLen >= 2);
}

// -----------------------------------------------------------------------------
// Dual-frame transaction (waits for both frames)
//
// Combined buffer:
//   [ tidop, status, payload0..., payload1... ]
//   where payload0 = dlc0-2 bytes, payload1 = dlc1-2 bytes
// -----------------------------------------------------------------------------
bool CanTransportClient::transact2(uint8_t expectedTidOp,
                                   const uint8_t *requestPayload, uint8_t requestLen,
                                   uint8_t responsePayload[14], uint8_t &responseLen) {
  responseLen = 0;
  if (!canPlus.ready) return false;
  if (!requestPayload || requestLen < 1 || requestLen > 8) return false;
  if (!responsePayload) return false;

  rspMode = 2;

  rspReady = false;
  rspLenLatched = 0;
  rspTidOpLatched = expectedTidOp;

  rsp1Ready = false;
  rsp1LenLatched = 0;
  rsp1TidOpLatched = tidopPlus1(expectedTidOp);

  canPlus.txWait();
  if (!canPlus.writePacket((int)requestCanId(), requestPayload, requestLen)) {
    rspMode = 0;
    return false;
  }

  const uint32_t startUs   = micros();
  const uint32_t timeoutUs = (uint32_t)timeoutMs * 1000UL;

  while (!(rspReady && rsp1Ready)) {
    canPlus.rxBurst();
    if ((uint32_t)(micros() - startUs) > timeoutUs) {
      rspMode = 0;
      return false;
    }
    tasks.yield();
  }

  if (!(rspLenLatched  >= 2 && rspLenLatched  <= 8)) { rspMode = 0; return false; }
  if (!(rsp1LenLatched >= 2 && rsp1LenLatched <= 8)) { rspMode = 0; return false; }
  if (rspBuf[0] != expectedTidOp) { rspMode = 0; return false; }
  if (rsp1Buf[0] != rsp1TidOpLatched) { rspMode = 0; return false; }

  const uint8_t d0 = (uint8_t)(rspLenLatched  - 2); // 0..6
  const uint8_t d1 = (uint8_t)(rsp1LenLatched - 2); // 0..6

  // Build combined: tidop/status from frame0, then payload0 + payload1
  responsePayload[0] = rspBuf[0];
  responsePayload[1] = rspBuf[1];

  if (d0 > 0) memcpy(&responsePayload[2], &rspBuf[2], d0);
  if (d1 > 0) memcpy(&responsePayload[2 + d0], &rsp1Buf[2], d1);

  responseLen = (uint8_t)(2 + d0 + d1);

  rspReady = false;
  rsp1Ready = false;
  rspMode = 0;

  return (responseLen >= 2);
}

// -----------------------------------------------------------------------------
// Response callback (invoked via onFrame())
// -----------------------------------------------------------------------------
void CanTransportClient::onResponse(const uint8_t data[8], uint8_t len) {
  if (len > 8) len = 8;
  if (len < 2) return;

  if (!rspReady && rspMode > 0 && data[0] == rspTidOpLatched) {
    memcpy(rspBuf, data, len);
    rspLenLatched = len;
    rspReady = true;
    return;
  }

  if (!rsp1Ready && rspMode > 1 && data[0] == rsp1TidOpLatched) {
    memcpy(rsp1Buf, data, len);
    rsp1LenLatched = len;
    rsp1Ready = true;
    return;
  }
}

#endif
