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
  rspReady = false;
  rspLenLatched = 0;
  expectedTidOpLatched = expectedTidOp;

  canPlus.txWait();
  if (!canPlus.writePacket((int)requestCanId(), requestPayload, requestLen)) return false;

  const uint32_t startUs   = micros();
  const uint32_t timeoutUs = (uint32_t)timeoutMs * 1000UL;

  while (!rspReady) {
    canPlus.rxBrust();
    if ((uint32_t)(micros() - startUs) > timeoutUs) return false;
    tasks.yield();
  }

  uint8_t gotLen = 0;

  // Atomic snapshot of the response + clear "ready"
  noInterrupts();
  gotLen = rspLenLatched;
  if (gotLen > 8) gotLen = 8;
  for (uint8_t i = 0; i < gotLen; i++) responsePayload[i] = rspBuf[i];
  rspReady = false;
  interrupts();

  responseLen = gotLen;
  return (responseLen >= 2);
}

void CanTransportClient::onResponse(const uint8_t data[8], uint8_t len) {
  if (len > 8) len = 8;
  if (len < 2) return;

  // Correlate on tidop (byte 0 of response)
  if (data[0] != expectedTidOpLatched) return;

  for (uint8_t i = 0; i < len; i++) rspBuf[i] = data[i];
  rspLenLatched = len;
  rspReady = true;
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
