// -----------------------------------------------------------------------------------
// CAN library
#pragma once

#include "../../../Common.h"

#if defined(CAN_PLUS) && CAN_PLUS == CAN_MCP2515

#include "../CanPlusBase.h"

class CanPlusMCP2515 : public CanPlus {
  public:
    CanPlusMCP2515();
    void init();

    int writePacket(int id, const uint8_t *buffer, size_t size) override;
    int writePacketRtr(int id, size_t dlc) override;

    void rxBrust(uint32_t periodUs = 2000);

    void poll();

  private:
    uint8_t burst = 0;
    uint32_t burstUntilUs = 0;
    bool burstEnabled = false;
};

extern CanPlusMCP2515 canPlus;

#endif
