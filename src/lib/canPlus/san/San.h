// -----------------------------------------------------------------------------------
// CAN library
#pragma once

#include "../../../Common.h"

#if defined(CAN_PLUS) && CAN_PLUS == CAN_SAN

#include "../CanPlusBase.h"

class CanPlusSan : public CanPlus {
  public:
    CanPlusSan();
    void init();
    int beginPacket(int id);
    int endPacket();
    int write(uint8_t byte);
    int write(uint8_t *buffer, size_t size);
    int writePacket(int id, uint8_t *buffer, size_t size);
    void poll(int packetSize);

  private:

};

extern CanPlusSan canPlus;

#endif
