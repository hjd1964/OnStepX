// -----------------------------------------------------------------------------------
// CAN library
#pragma once

#include "../../Common.h"

#if defined(CAN_PLUS) && CAN_PLUS != OFF

#ifndef CAN_MAX_CALLBACKS
  #define CAN_MAX_CALLBACKS 8
#endif

#if defined(CAN_PLUS) && CAN_PLUS != OFF
  #ifndef CAN_BAUD
  #define CAN_BAUD                      500000                    // 500000 baud default
  #endif
  #ifndef CAN_SEND_RATE_MS
  #define CAN_SEND_RATE_MS              25                        // 40 Hz CAN controller send message processing rate
  #endif
  #ifndef CAN_RECV_RATE_MS
  #define CAN_RECV_RATE_MS              5                         // 200 Hz CAN controller recv. message processing rate
  #endif
  #ifndef CAN_RX_PIN
  #define CAN_RX_PIN                    OFF                       // for ESP32 CAN interface
  #endif
  #ifndef CAN_TX_PIN
  #define CAN_TX_PIN                    OFF                       // for ESP32 CAN interface
  #endif
  #ifndef CAN_CS_PIN
  #define CAN_CS_PIN                    OFF                       // for MCP2515 SPI CAN controller
  #endif
  #ifndef CAN_INT_PIN
  #define CAN_INT_PIN                   OFF                       // for MCP2515 SPI CAN controller
  #endif
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