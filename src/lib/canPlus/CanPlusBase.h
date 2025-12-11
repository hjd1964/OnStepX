// -----------------------------------------------------------------------------------
// CAN library
#pragma once

#include "../../Common.h"

#if defined(CAN_PLUS) && CAN_PLUS != OFF

#ifndef CAN_MAX_CALLBACKS
  #define CAN_MAX_CALLBACKS           16
#endif

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

class CanPlus {
  public:
    virtual ~CanPlus() = default;
    virtual int beginPacket(int id);
    virtual int endPacket();
    virtual int write(uint8_t byte);
    virtual int write(const uint8_t *buffer, size_t size);
    virtual int writePacket(int id, const uint8_t *buffer, size_t size);
    // RTR policy: supports sending RTR, but drops on receive
    virtual int writePacketRtr(int id, size_t dlc) { UNUSED(id); UNUSED(dlc); return 0; }
    virtual int callbackRegisterId(int id, void (*callback)(uint8_t data[8]));
    virtual int callbackRegisterMessage(int id, uint8_t msg, void (*callback)(uint8_t data[8]));
    virtual int callbackProcess(int id, uint8_t *buffer, size_t size);

    bool ready = false;

    inline void resetStats() {
      noInterrupts();
      tx_ok = 0;
      tx_fail = 0;
      rx_ok = 0;
      rx_drop_ext = 0;
      rx_drop_rtr = 0;
      rx_drop_len = 0;
      interrupts();
    }

    volatile uint32_t rx_ok = 0;
    volatile uint32_t rx_drop_ext = 0;
    volatile uint32_t rx_drop_rtr = 0;
    volatile uint32_t rx_drop_len = 0;
    volatile uint32_t tx_ok = 0;
    volatile uint32_t tx_fail = 0;

  protected:
    int sendId = 0;
    int sendCount = -1;
    uint8_t sendData[8] = {0};
    
    int idCallbackCount = 0;
    int idCallBackId[CAN_MAX_CALLBACKS] = {0};
    void (*idCallback[CAN_MAX_CALLBACKS])(uint8_t data[8]) = {nullptr};

    int msgCallbackCount = 0;
    int msgCallBackId[CAN_MAX_CALLBACKS] = {0};
    int msgCallBackMsg[CAN_MAX_CALLBACKS] = {0};
    void (*msgCallback[CAN_MAX_CALLBACKS])(uint8_t data[8]) = {nullptr};
};

#endif