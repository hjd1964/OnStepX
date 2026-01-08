// -----------------------------------------------------------------------------------
// CAN transport - Client

#include "CanTransportClient.h"

#if defined(CAN_PLUS) && CAN_PLUS != OFF

CanTransportClient* CanTransportClient::s_instances[TRANSPORT_CAN_MAX_INSTANCES] = { nullptr };

CanTransportClient::CanTransportClient(uint16_t requestId, uint16_t responseId)
  : reqId(requestId), rspId(responseId) {}

// Register response callback once per instance.
// Safe to call init() multiple times; it will only allocate/register once.
void CanTransportClient::init() {
  if (!canPlus.ready) return;

  // Allocate slot once
  if (slot < 0) slot = allocSlot(this);
  if (slot < 0) return;

  // Register callback once
  if (callbackRegistered) return;

  // Register the slot-specific thunk for this response CAN ID
  const bool ok = canPlus.callbackRegisterId((int)responseCanId(), thunkForSlot(slot));
  if (ok) callbackRegistered = true;
}

bool CanTransportClient::transact(uint8_t expectedTidOp,
                                  const uint8_t *requestPayload, uint8_t requestLen,
                                  uint8_t responsePayload[8], uint8_t &responseLen) {
  responseLen = 0;
  if (!canPlus.ready) return false;
  if (!requestPayload || requestLen < 1 || requestLen > 8) return false;
  if (!responsePayload) return false;

  // Arm correlation before TX
  rspMode = 1;
  rspReady = false;
  rspLenLatched = 0;
  rspTidOpLatched = expectedTidOp;

  canPlus.txWait();
  if (!canPlus.writePacket((int)requestCanId(), requestPayload, requestLen)) { rspMode = 0; return false; }

  const uint32_t startUs   = micros();
  const uint32_t timeoutUs = (uint32_t)timeoutMs * 1000UL;

  while (!rspReady) {
    canPlus.rxBrust();
    if ((uint32_t)(micros() - startUs) > timeoutUs) { rspMode = 0; return false; }
    tasks.yield();
  }

  responseLen = rspLenLatched;
  for (uint8_t i = 0; i < rspLenLatched; i++) responsePayload[i] = rspBuf[i];
  rspReady = false;

  rspMode = 0;

  return (responseLen >= 2);
}

bool CanTransportClient::transact2(uint8_t expectedTidOp,
                                   const uint8_t *requestPayload, uint8_t requestLen,
                                   uint8_t responsePayload[14], uint8_t &responseLen) {
  responseLen = 0;
  if (!canPlus.ready) return false;
  if (!requestPayload || requestLen < 1 || requestLen > 8) return false;
  if (!responsePayload) return false;

  // Arm correlation before TX
  rspMode = 2;
  rspReady = false;
  rspLenLatched = 0;
  rspTidOpLatched = expectedTidOp;

  // Arm correlation before TX (for second frame) 
  rsp1Ready = false;
  rsp1LenLatched = 0;
  rsp1TidOpLatched = ((expectedTidOp & 0b11100000) + 0b00100000) | (expectedTidOp & 0b00011111);

  canPlus.txWait();
  if (!canPlus.writePacket((int)requestCanId(), requestPayload, requestLen)) { rspMode = 0; return false; }

  const uint32_t startUs   = micros();
  const uint32_t timeoutUs = (uint32_t)timeoutMs * 1000UL;

  while (!(rspReady && rsp1Ready)) {
    canPlus.rxBrust();
    if ((uint32_t)(micros() - startUs) > timeoutUs) { rspMode = 0; return false; }
    tasks.yield();
  }

  // Pack the first frame into the caller's 14-byte output
  if (rspLenLatched >= 2 && rspLenLatched <= 8) {
    for (uint8_t i = 0; i < rspLenLatched; i++) responsePayload[i] = rspBuf[i];
    responseLen = rspLenLatched;

    // Pack the second frame into the caller's 14-byte output (if it has any data content)
    if (responseLen == 8 && rsp1LenLatched > 2 && rsp1LenLatched <= 8) {
      uint8_t rsp1DataLength = (uint8_t)(rsp1LenLatched - 2);
      for (uint8_t i = 0; i < rsp1DataLength; i++) responsePayload[8 + i] = rsp1Buf[i + 2];
      responseLen = 8 + rsp1DataLength;
    }

  } else responseLen = 0;

  rspReady = false;
  rsp1Ready = false;

  rspMode = 0;

  return (responseLen >= 2);
}

void CanTransportClient::onResponse(const uint8_t data[8], uint8_t len) {
  if (len > 8) len = 8;
  if (len < 2) return;

  // Correlate on tidop (byte 1 of response)
  if (!rspReady && rspMode > 0 && data[0] == rspTidOpLatched) {
    for (uint8_t i = 0; i < len; i++) rspBuf[i] = data[i];
    rspLenLatched = len;
    rspReady = true;
  }

  // Correlate on tidop (optional byte 2 of response)
  if (!rsp1Ready && rspMode > 1 && data[0] == rsp1TidOpLatched) {
    for (uint8_t i = 0; i < len; i++) rsp1Buf[i] = data[i];
    rsp1LenLatched = len;
    rsp1Ready = true;
  }
}

// -------------------- slot allocation --------------------

int8_t CanTransportClient::allocSlot(CanTransportClient *p) {
  for (int8_t i = 0; i < (int8_t)TRANSPORT_CAN_MAX_INSTANCES; i++) {
    if (s_instances[i] == nullptr) { s_instances[i] = p; return i; }
  }
  return -1;
}

CanTransportClient::ThunkFn CanTransportClient::thunkForSlot(int8_t s) {
  switch (s) {
    default: return &CanTransportClient::onThunk0;
    case 0:  return &CanTransportClient::onThunk0;
    case 1:  return &CanTransportClient::onThunk1;
    case 2:  return &CanTransportClient::onThunk2;
    case 3:  return &CanTransportClient::onThunk3;
    case 4:  return &CanTransportClient::onThunk4;
    case 5:  return &CanTransportClient::onThunk5;
    case 6:  return &CanTransportClient::onThunk6;
    case 7:  return &CanTransportClient::onThunk7;
  }
}

// -------------------- thunks --------------------

#define TRANSPORT_CAN_CLIENT_THUNK(N) \
  void CanTransportClient::onThunk##N(uint8_t data[8], uint8_t len) { \
    CanTransportClient *p = s_instances[N]; \
    if (p) p->onResponse((const uint8_t*)data, len); \
  }

TRANSPORT_CAN_CLIENT_THUNK(0)
TRANSPORT_CAN_CLIENT_THUNK(1)
TRANSPORT_CAN_CLIENT_THUNK(2)
TRANSPORT_CAN_CLIENT_THUNK(3)
TRANSPORT_CAN_CLIENT_THUNK(4)
TRANSPORT_CAN_CLIENT_THUNK(5)
TRANSPORT_CAN_CLIENT_THUNK(6)
TRANSPORT_CAN_CLIENT_THUNK(7)

#undef TRANSPORT_CAN_CLIENT_THUNK

#endif
