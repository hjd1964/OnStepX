// -----------------------------------------------------------------------------------
// CAN transport (base)
// -----------------------------------------------------------------------------------

#include "CanTransportBase.h"

#if defined(CAN_PLUS) && CAN_PLUS != OFF

#include "../CanPlus/CanPlus.h"

CanTransport* CanTransport::s_instances[TRANSPORT_CAN_MAX_INSTANCES] = {nullptr};

CanTransport::CanTransport(uint16_t requestId, uint16_t responseId, uint8_t dualFrameOpcodeStart)
  : reqId(requestId), rspId(responseId), opDualResponse(dualFrameOpcodeStart) {
  tidop  = 0;
  opcode = 0;

  // dual-cursor state
  rxLen = 0;
  rxPos = 0;
  txPos = 0;

  // buffers are fixed arrays; no pointer init needed
  // optional: clear for determinism
  memset(rxBuf, 0, sizeof(rxBuf));
  memset(txBuf, 0, sizeof(txBuf));
}

bool CanTransport::init() {
  if (!canPlus.ready) return false;

  if (slot < 0) slot = allocSlot(this);
  if (slot < 0) return false;

  return true;
}

// -----------------------------------------------------------------------------------
// Dual-cursor pack/unpack helpers (LE on wire)
//
// read*  => rxBuf/rxPos/rxLen
// write* => txBuf/txPos/txMax
// -----------------------------------------------------------------------------------

bool CanTransport::writeBytes(const uint8_t *src, uint8_t n) {
  if (!src) return false;
  if (!allowTx(n)) return false;
  memcpy(&txBuf[txPos], src, n);
  txPos = (uint8_t)(txPos + n);
  return true;
}

int CanTransport::readBytes(uint8_t *dst, uint8_t n) {
  if (!dst) return 0;
  if (!allowRx(n)) return 0;
  memcpy(dst, &rxBuf[rxPos], n);
  rxPos = (uint8_t)(rxPos + n);
  return (int)n;
}

bool CanTransport::writeU8(uint8_t v) {
  if (!allowTx(1)) return false;
  txBuf[txPos++] = v;
  return true;
}

bool CanTransport::readU8(uint8_t &v) {
  if (!allowRx(1)) return false;
  v = rxBuf[rxPos++];
  return true;
}

bool CanTransport::writeU16(uint16_t v) {
  if (!allowTx(2)) return false;
  txBuf[txPos + 0] = (uint8_t)(v & 0xFFu);
  txBuf[txPos + 1] = (uint8_t)((v >> 8) & 0xFFu);
  txPos = (uint8_t)(txPos + 2);
  return true;
}

bool CanTransport::readU16(uint16_t &v) {
  if (!allowRx(2)) return false;
  v = (uint16_t)rxBuf[rxPos + 0]
    | (uint16_t)((uint16_t)rxBuf[rxPos + 1] << 8);
  rxPos = (uint8_t)(rxPos + 2);
  return true;
}

bool CanTransport::writeU32(uint32_t v) {
  if (!allowTx(4)) return false;
  txBuf[txPos + 0] = (uint8_t)(v & 0xFFu);
  txBuf[txPos + 1] = (uint8_t)((v >> 8) & 0xFFu);
  txBuf[txPos + 2] = (uint8_t)((v >> 16) & 0xFFu);
  txBuf[txPos + 3] = (uint8_t)((v >> 24) & 0xFFu);
  txPos = (uint8_t)(txPos + 4);
  return true;
}

bool CanTransport::readU32(uint32_t &v) {
  if (!allowRx(4)) return false;
  v = (uint32_t)rxBuf[rxPos + 0]
    | (uint32_t)((uint32_t)rxBuf[rxPos + 1] << 8)
    | (uint32_t)((uint32_t)rxBuf[rxPos + 2] << 16)
    | (uint32_t)((uint32_t)rxBuf[rxPos + 3] << 24);
  rxPos = (uint8_t)(rxPos + 4);
  return true;
}

bool CanTransport::writeF32(float v) {
  uint32_t u = 0;
  static_assert(sizeof(float) == sizeof(uint32_t), "float must be 32-bit");
  memcpy(&u, &v, sizeof(u));
  return writeU32(u);
}

bool CanTransport::readF32(float &v) {
  uint32_t u = 0;
  if (!readU32(u)) return false;
  static_assert(sizeof(float) == sizeof(uint32_t), "float must be 32-bit");
  memcpy(&v, &u, sizeof(v));
  return true;
}

bool CanTransport::writeFixedI16(float value, int32_t scale) {
  if (scale == 0) return false;
  if (isnan(value)) return writeI16(NAN_SENTINEL);

  const float scaled = value * (float)scale;
  const float adj = (scaled >= 0.0f) ? 0.5f : -0.5f;
  const int32_t v = (int32_t)(scaled + adj);

  // reserve INT16_MIN for NaN sentinel; overflow => sentinel
  if (v < -32767 || v > 32767) return writeI16(NAN_SENTINEL);

  return writeI16((int16_t)v);
}

bool CanTransport::readFixedI16(float &value, int32_t scale) {
  if (scale == 0) return false;

  int16_t v = 0;
  if (!readI16(v)) return false;

  if (v == NAN_SENTINEL) { value = NAN; return true; }

  value = ((float)v) / (float)scale;
  return true;
}

bool CanTransport::writeFixedI16(double value, int32_t scale) {
  if (scale == 0) return false;
  if (isnan(value)) return writeI16(NAN_SENTINEL);

  const double scaled = value * (double)scale;
  const double adj = (scaled >= 0.0) ? 0.5 : -0.5;
  const int32_t v = (int32_t)(scaled + adj);

  if (v < -32767 || v > 32767) return writeI16(NAN_SENTINEL);

  return writeI16((int16_t)v);
}

bool CanTransport::readFixedI16(double &value, int32_t scale) {
  if (scale == 0) return false;

  int16_t v = 0;
  if (!readI16(v)) return false;

  if (v == NAN_SENTINEL) { value = NAN; return true; }

  value = ((double)v) / (double)scale;
  return true;
}

// -------------------- slot allocation --------------------

int8_t CanTransport::allocSlot(CanTransport *p) {
  for (int8_t i = 0; i < (int8_t)TRANSPORT_CAN_MAX_INSTANCES; i++) {
    if (s_instances[i] == nullptr) { s_instances[i] = p; return i; }
  }
  return -1;
}

void CanTransport::taskThunk() {
  // Pump RX once per scheduler tick/task invocation
  canPlus.rxBurst();

  // Run all instances (cheap); instances with no work return quickly
  for (int i = 0; i < (int)TRANSPORT_CAN_MAX_INSTANCES; i++) {
    if (s_instances[i]) s_instances[i]->process();
  }
}

CanTransport::ThunkFn CanTransport::thunkForSlot(int8_t s) {
  switch (s) {
    case 0:  return &CanTransport::onThunk0;
    case 1:  return &CanTransport::onThunk1;
    case 2:  return &CanTransport::onThunk2;
    case 3:  return &CanTransport::onThunk3;
    case 4:  return &CanTransport::onThunk4;
    case 5:  return &CanTransport::onThunk5;
    case 6:  return &CanTransport::onThunk6;
    case 7:  return &CanTransport::onThunk7;
    default: return &CanTransport::onThunk0;
  }
}

// -------------------- thunks --------------------

#define TRANSPORT_CAN_THUNK(N) \
  void CanTransport::onThunk##N(uint8_t data[8], uint8_t len) { \
    CanTransport *p = s_instances[N]; \
    if (p) p->onFrame((const uint8_t*)data, len); \
  }

TRANSPORT_CAN_THUNK(0)
TRANSPORT_CAN_THUNK(1)
TRANSPORT_CAN_THUNK(2)
TRANSPORT_CAN_THUNK(3)
TRANSPORT_CAN_THUNK(4)
TRANSPORT_CAN_THUNK(5)
TRANSPORT_CAN_THUNK(6)
TRANSPORT_CAN_THUNK(7)

#undef TRANSPORT_CAN_THUNK

#endif
