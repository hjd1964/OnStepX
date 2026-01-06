// -----------------------------------------------------------------------------------
// CAN transport - Server (single-frame request/response, ISR-safe enqueue)
//
// Request:  [ tidop, args... ]
// Response: [ tidop, status, payload... ]

#pragma once

#include "../../Common.h"

#if defined(CAN_PLUS) && CAN_PLUS != OFF

#include "../tasks/OnTask.h"
#include "../canPlus/CanPlus.h"

#ifndef TRANSPORT_CAN_MAX_INSTANCES
  #define TRANSPORT_CAN_MAX_INSTANCES 8
#endif

#ifndef TRANSPORT_CAN_SERVER_RXQ
  #define TRANSPORT_CAN_SERVER_RXQ 16
#endif

class CanTransportServer {
  public:
    CanTransportServer(uint16_t requestId, uint16_t responseId);

    bool init(bool startTask = true, uint16_t processPeriodMs = 2);
    void begin() {} // symmetry; no-op

    void process(); // call from task or poll loop

  protected:
    // Implement in derived class: interpret request bytes and decide payload/status
    virtual void processCommand(const uint8_t data[8], uint8_t len) = 0;

    // Send response with this transport's response CAN ID
    void sendResponse(uint8_t tidop, uint8_t status, const uint8_t *payload, uint8_t payloadLen);

    // Status helper (shared contract)
    static inline uint8_t packStatus(bool handled, bool numericReply, bool suppressFrame, uint8_t commandError) {
      return (uint8_t)((handled ? 0x80 : 0x00) |
                       (numericReply ? 0x40 : 0x00) |
                       (suppressFrame ? 0x20 : 0x00) |
                       (commandError & 0x1F));
    }

  private:
    struct Frame {
      uint8_t len;
      uint8_t data[8];
    };

    void enqueueFromCallback(const uint8_t data[8], uint8_t len);
    bool dequeue(Frame &out);

    // Slot/thunk dispatch (no global self)
    int8_t slot = -1;
    static CanTransportServer* s_instances[TRANSPORT_CAN_MAX_INSTANCES];

    static int8_t allocSlot(CanTransportServer *p);

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

    static void taskThunk();

  private:
    const uint16_t reqId;
    const uint16_t rspId;

    uint8_t taskHandle = 0;

    // RX queue (written from callback, drained in process())
    volatile uint8_t qHead = 0;
    volatile uint8_t qTail = 0;
    Frame q[TRANSPORT_CAN_SERVER_RXQ];
};

#endif
