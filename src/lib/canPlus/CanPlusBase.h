// -----------------------------------------------------------------------------------
// CAN library
#pragma once

#include "../../Common.h"

#if defined(CAN_PLUS) && CAN_PLUS != OFF

#ifndef CAN_MAX_CALLBACKS
  #define CAN_MAX_CALLBACKS 8
#endif

#ifndef CAN_BAUD
  #define CAN_BAUD 500000
#endif

#ifndef CAN_RECV_RATE_MS
  // recv. message queue processing rate (when recv. message callbacks aren't implemented)
  #define CAN_RECV_RATE_MS 5
#endif

#ifndef CAN_SEND_RATE_MS
  // for clients how quickly to send messages so we don't overwhelm the recv. queue
  #define CAN_SEND_RATE_MS 25
#endif

class CanPlus {
  public:
    virtual int beginPacket(int id);
    virtual int endPacket();
    virtual int write(uint8_t byte);
    virtual int write(uint8_t *buffer, size_t size);
    virtual int writePacket(int id, uint8_t *buffer, size_t size);
    virtual int callbackRegisterId(int id, void (*callback)(uint8_t data[8]));
    virtual int callbackRegisterMessage(int id, uint8_t msg, void (*callback)(uint8_t data[8]));
    virtual int callbackProcess(int id, uint8_t *buffer, size_t size);
    bool ready = false;

  protected:
    int sendId = 0;
    int sendCount = -1;
    uint8_t sendData[8];

    int idCallbackCount = 0;
    int idCallBackId[CAN_MAX_CALLBACKS];
    void (*volatile idCallback[CAN_MAX_CALLBACKS])(uint8_t data[CAN_MAX_CALLBACKS]);

    int msgCallbackCount = 0;
    int msgCallBackId[CAN_MAX_CALLBACKS];
    int msgCallBackMsg[CAN_MAX_CALLBACKS];
    void (*volatile msgCallback[CAN_MAX_CALLBACKS])(uint8_t data[CAN_MAX_CALLBACKS]);
};

#endif