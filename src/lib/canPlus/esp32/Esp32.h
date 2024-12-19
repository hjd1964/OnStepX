// -----------------------------------------------------------------------------------
// CAN library
#pragma once

#include "../../../Common.h"

#if defined(CAN_PLUS) && CAN_PLUS == CAN_ESP32

#include "../CanPlusBase.h"

#ifndef ESP32
  #error "Configuration (Config.h): Setting CAN_PLUS CAN_ESP32 requires an ESP32 microcontroller."
#endif

class CanPlusESP32 : public CanPlus {
  public:
    CanPlusESP32();
    void init();
    int writePacket(int id, uint8_t *buffer, size_t size);
    void poll();

  private:

};

extern CanPlusESP32 canPlus;

#endif
