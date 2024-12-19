// -----------------------------------------------------------------------------------
// CAN library
#pragma once

#include "../../../Common.h"

#if defined(CAN_PLUS) && CAN_PLUS == CAN_MCP2515

#include "../CanPlusBase.h"

#ifndef CAN_CLOCK
  #define CAN_CLOCK MCP_8MHZ
#endif

#ifndef CAN_CS_PIN
  // MCP2515 CAN controller CS pin for SPI comms on the microcontroller default interface 
  #define CAN_CS_PIN 5
#endif

#ifndef CAN_INT_PIN
  // MCP2515 CAN controller interrupt pin is required 
  #define CAN_INT_PIN 15
#endif

class CanPlusMCP2515 : public CanPlus {
  public:
    CanPlusMCP2515();
    void init();
    int writePacket(int id, uint8_t *buffer, size_t size);
    void poll();

  private:

};

extern CanPlusMCP2515 canPlus;

#endif
