// -----------------------------------------------------------------------------------
// CAN library
#pragma once

#include "../../../Common.h"

#if defined(CAN_PLUS) && CAN_PLUS == CAN_ESP32

#include "../CanPlusBase.h"

#ifndef ESP32
  #error "Configuration (Config.h): Setting CAN_PLUS CAN_ESP32 requires an ESP32 microcontroller."
#endif

#if CAN_RX_PIN == OFF || CAN_TX_PIN == OFF
  #error "CAN_ESP32 requires CAN_RX_PIN and CAN_TX_PIN to be defined."
#endif

class CanPlusESP32 : public CanPlus {
  public:
    CanPlusESP32();
    void init();
    int writePacket(int id, const uint8_t *buffer, size_t size);
    int writePacketRtr(int id, size_t dlc);

    void rxBurst(uint32_t periodUs = 2000);

    void debugStatus();

    void poll();

  private:
    uint8_t burst = 0;
    uint32_t burstUntilUs = 0;
    bool burstEnabled = false;
};

extern CanPlusESP32 canPlus;

#endif
