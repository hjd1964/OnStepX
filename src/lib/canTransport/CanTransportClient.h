// -----------------------------------------------------------------------------------
// CAN transport - Client (single-frame request/response, 1 in-flight per instance)
//
// Wire format:
//   Request:  [ tidop, args... ]                   (1..8 bytes)
//   Response: [ tidop, status, payload... ]        (2..8 bytes)
//     status: [7 handled][6 numericReply][5 suppressFrame][4..0 commandError]
//
// Notes:
// - Uses global canPlus (single CAN interface typical on Arduino-class targets).
// - Supports multiple logical instances by slot/thunk dispatch on response CAN ID.

#pragma once

#include "../../Common.h"

#if defined(CAN_PLUS) && CAN_PLUS != OFF

#include "../tasks/OnTask.h"
#include "../canPlus/CanPlus.h"

#ifndef TRANSPORT_CAN_MAX_INSTANCES
  #define TRANSPORT_CAN_MAX_INSTANCES 8
#endif

class CanTransportClient {
  public:
    CanTransportClient(uint16_t requestId, uint16_t responseId);

    void init();   // registers response callback
    void begin() {} // symmetry; no-op

    void setTimeoutMs(uint16_t ms) { timeoutMs = ms; }

    // Send request and wait for correlated response (tidop match).
    // Returns true if a response was received (not if the command succeeded).
    bool transact(uint8_t expectedTidOp,
                  const uint8_t *requestPayload, uint8_t requestLen,
                  uint8_t responsePayload[8], uint8_t &responseLen);

    inline uint16_t requestCanId() const { return reqId; }
    inline uint16_t responseCanId() const { return rspId; }

    // Status helpers (shared contract)
    static inline uint8_t packStatus(bool handled, bool numericReply, bool suppressFrame, uint8_t commandError) {
      return (uint8_t)((handled ? 0x80 : 0x00) |
                       (numericReply ? 0x40 : 0x00) |
                       (suppressFrame ? 0x20 : 0x00) |
                       (commandError & 0x1F));
    }

    static inline void unpackStatus(uint8_t status, bool &handled, bool &numericReply, bool &suppressFrame, uint8_t &commandError) {
      handled       = (status & 0x80) != 0;
      numericReply  = (status & 0x40) != 0;
      suppressFrame = (status & 0x20) != 0;
      commandError  = (uint8_t)(status & 0x1F);
    }

    static inline uint8_t packTidOp(uint8_t tid, uint8_t op) { return (uint8_t)(((tid & 0x07) << 5) | (op & 0x1F)); }
    static inline uint8_t unpackTid(uint8_t tidop) { return (uint8_t)((tidop >> 5) & 0x07); }
    static inline uint8_t unpackOp(uint8_t tidop)  { return (uint8_t)(tidop & 0x1F); }

  private:
    void onResponse(const uint8_t data[8], uint8_t len);

    // Slot/thunk dispatch (supports multiple instances, no global self)
    int8_t slot = -1;
    static CanTransportClient* s_instances[TRANSPORT_CAN_MAX_INSTANCES];

    static int8_t allocSlot(CanTransportClient *p);

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

    bool callbackRegistered = false;

    const uint16_t reqId;
    const uint16_t rspId;

    uint16_t timeoutMs = 1000;

    // 1 in-flight request correlation
    volatile bool    rspReady = false;
    volatile uint8_t rspLenLatched = 0;
    volatile uint8_t expectedTidOpLatched = 0;
    uint8_t rspBuf[8] = {0};
};

#endif
