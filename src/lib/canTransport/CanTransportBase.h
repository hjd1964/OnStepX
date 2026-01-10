// -----------------------------------------------------------------------------------
// CAN transport (base)
//
// Wire format (payload bytes):
//   Request:  [ tidop, args... ]                   (1..8 bytes)
//   Response: [ tidop, status, payload... ]        (2..8 bytes)
//     status: [7 handled][6 numericReply][5 suppressFrame][4..0 commandError]
//
// Notes:
// - Uses global canPlus (single CAN interface typical on Arduino-class targets)
// - This base provides:
//     * dual-cursor pack/unpack (RX cursor for reads, TX cursor for writes)
//     * slot/thunk dispatch so callbacks can route to instances without globals
// - This base does NOT impose a global/shared tidop. tid/op correlation is a
//   per-transaction concern and should be tracked by the client implementation.
// -----------------------------------------------------------------------------------

#pragma once

#include "../../Common.h"

#if defined(CAN_PLUS) && CAN_PLUS != OFF

#include "../commands/CommandErrors.h"

#ifndef TRANSPORT_CAN_MAX_INSTANCES
  #define TRANSPORT_CAN_MAX_INSTANCES 8
#endif

class CanTransport {
public:
  CanTransport(uint16_t requestId, uint16_t responseId, uint8_t dualFrameOpcodeStart = 32);

  virtual bool init();
  virtual void begin() {}

  inline uint8_t opCode() const { return opcode; }
  inline uint16_t requestCanId()  const { return reqId; }
  inline uint16_t responseCanId() const { return rspId; }

  // -------------------------
  // Raw bytes
  //   write* => TX cursor
  //   read*  => RX cursor
  // -------------------------
  bool writeBytes(const uint8_t *src, uint8_t n);
  int readBytes(uint8_t *dst, uint8_t n);
  int readRemainingBytes(uint8_t *dst) { if (rxPos > rxLen) return 0; return readBytes(dst, (uint8_t)(rxLen - rxPos)); }
  
  // -------------------------
  // Unsigned integers (LE on wire)
  // -------------------------
  bool writeU8(uint8_t v);
  bool readU8(uint8_t &v);
  bool writeU16(uint16_t v);
  bool readU16(uint16_t &v);
  bool writeU32(uint32_t v);
  bool readU32(uint32_t &v);

  // -------------------------
  // Signed integers (LE on wire)
  // -------------------------
  inline bool writeI16(int16_t v) { return writeU16((uint16_t)v); }
  inline bool readI16(int16_t &v) { uint16_t u = 0; if (!readU16(u)) return false; v = (int16_t)u; return true; }
  inline bool writeI32(int32_t v) { return writeU32((uint32_t)v); }
  inline bool readI32(int32_t &v) { uint32_t u = 0; if (!readU32(u)) return false; v = (int32_t)u; return true; }

  // -------------------------
  // Floats (IEEE-754, LE on wire)
  // -------------------------
  bool writeF32(float v);
  bool readF32(float &v);

  // -------------------------
  // Fixed-point helpers (int16 payload, NaN sentinel)
  // -------------------------
  bool writeFixedI16(float value, int32_t scale);
  bool readFixedI16(float &value, int32_t scale);
  bool writeFixedI16(double value, int32_t scale);
  bool readFixedI16(double &value, int32_t scale);

  // -------------------------
  // RX cursor accessors (default size/offset/remaining map to RX)
  // -------------------------
  uint8_t size() const { return rxLen; }
  uint8_t offset() const { return rxPos; }
  uint8_t remaining() const { return (rxPos <= rxLen) ? (uint8_t)(rxLen - rxPos) : 0; }

protected:
  // Server-side handler: interpret request bytes and decide payload/status.
  // Reads come from RX cursor; writes go to TX cursor.
  virtual void processCommand() {}

  // Unified ingress point for CAN frames delivered via thunk.
  // Default: treat the frame as a request and call processCommand() directly.
  // Server implementations that need ISR-safe handling should override onFrame().
  virtual void onFrame(const uint8_t data[8], uint8_t len) {
    beginNewRequest(data, len);
    beginNewResponse();
    processCommand();
  }

  virtual void onResponse(const uint8_t data[8], uint8_t len) { (void)data; (void)len; }

  // Optional per-instance periodic processing hook (poll loop / task)
  virtual void process() {}

  // -------------------------
  // RX/TX buffer binding + cursors
  // -------------------------
  inline void beginNewRequest(const uint8_t data[8], uint8_t len) {
    if (!data) { rxLen = 0; rxPos = 0; return; }
    if (len > (uint8_t)sizeof(rxBuf)) len = (uint8_t)sizeof(rxBuf);
    memcpy(rxBuf, data, len);
    rxLen = len;

    tidop  = (len >= 1) ? rxBuf[0] : 0;
    opcode = unpackOp(tidop);

    // args start after tidop
    rxPos = 1;
  }

  inline void beginNewResponse() {
    txPos = 0;
  }

  inline uint8_t responsePayloadLen() const { return txPos; }
  inline const uint8_t* responsePayload() const { return txBuf; }
  inline uint8_t* responsePayloadMut() { return txBuf; }

  // Optional cursor control
  inline void resetRx() { rxPos = 0; }
  inline bool seekRx(uint8_t newOffset) { if (newOffset > rxLen) return false; rxPos = newOffset; return true; }
  inline void resetTx() { txPos = 0; }
  inline bool seekTx(uint8_t newOffset) { if (newOffset > txMax) return false; txPos = newOffset; return true; }

  // -------------------------
  // TidOp helpers
  // -------------------------
  static inline uint8_t packTidOp(uint8_t tid, uint8_t op) { return (uint8_t)(((tid & 0x07) << 5) | (op & 0x1F)); }
  static inline uint8_t unpackTid(uint8_t tidop) { return (uint8_t)((tidop >> 5) & 0x07); }
  static inline uint8_t unpackOp (uint8_t tidop) { return (uint8_t)(tidop & 0x1F); }

  // -------------------------
  // Status helpers
  // -------------------------
  static inline uint8_t packStatus(bool handled, bool suppressFrame, bool numericReply, CommandError commandError) {
    return (uint8_t)((handled ? 0x80:0x00) |
                     (numericReply ? 0x40:0x00) |
                     (suppressFrame ? 0x20:0x00) |
                     ((uint8_t)commandError & 0x1F));
  }

  static inline void unpackStatus(uint8_t status, bool &handled, bool &suppressFrame, bool &numericReply, CommandError &commandError) {
    handled       = (status & 0x80) != 0;
    numericReply  = (status & 0x40) != 0;
    suppressFrame = (status & 0x20) != 0;
    commandError  = (CommandError)(status & 0x1F);
  }

  // -------------------------
  // Cursor bounds check (RX/TX)
  // -------------------------
  inline bool allowRx(uint8_t n) const {
    if (rxPos > rxLen) return false;
    return (uint8_t)(rxLen - rxPos) >= n;
  }

  inline bool allowTx(uint8_t n) const {
    if (txPos > txMax) return false;
    return (uint8_t)(txMax - txPos) >= n;
  }

  // -------------------------
  // Slot/thunk dispatch (no global self)
  // -------------------------
  int8_t slot = -1;
  static CanTransport* s_instances[TRANSPORT_CAN_MAX_INSTANCES];

  static int8_t allocSlot(CanTransport *p);

  static void taskThunk();

  static void onThunk0(uint8_t data[8], uint8_t len);
  static void onThunk1(uint8_t data[8], uint8_t len);
  static void onThunk2(uint8_t data[8], uint8_t len);
  static void onThunk3(uint8_t data[8], uint8_t len);
  static void onThunk4(uint8_t data[8], uint8_t len);
  static void onThunk5(uint8_t data[8], uint8_t len);
  static void onThunk6(uint8_t data[8], uint8_t len);
  static void onThunk7(uint8_t data[8], uint8_t len);

  typedef void (*ThunkFn)(uint8_t data[8], uint8_t len);
  static ThunkFn thunkForSlot(int8_t s);

  // -------------------------
  // IDs
  // -------------------------
  const uint16_t reqId;
  const uint16_t rspId;
  const uint8_t  opDualResponse;

  // -------------------------
  // Dual-cursor buffers
  //   RX: holds last request/response frame bytes for decoding
  //   TX: holds payload bytes (max 12) for encoding
  // -------------------------
  uint8_t rxBuf[14] = {0};
  volatile uint8_t rxLen = 0;
  volatile uint8_t rxPos = 0;

  uint8_t txBuf[12] = {0};
  static constexpr uint8_t txMax = 12;
  volatile uint8_t txPos = 0;

  static constexpr int16_t NAN_SENTINEL = (int16_t)0x8000;

  // Per-instance scratch
  uint8_t tidop = 0;
  uint8_t opcode = 0;
};

#endif
