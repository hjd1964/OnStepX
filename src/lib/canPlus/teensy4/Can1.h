// -----------------------------------------------------------------------------------
// CAN library
#pragma once

#include "../../../Common.h"

#if defined(CAN_PLUS) && CAN_PLUS == CAN1_TEENSY4

#include "../CanPlusBase.h"

#include <FlexCAN_T4.h> // https://github.com/tonton81/FlexCAN_T4

#if !defined(ARDUINO_TEENSY40) && !defined(ARDUINO_TEENSY41) 
  #error "Configuration (Config.h): Setting CAN_PLUS CAN_TEENSY4 requires an Teensy4.0 or Teensy4.1 microcontroller."
#endif

#ifndef CAN_PLUS_DEFAULT
  #define CAN_PLUS_DEFAULT CAN1_TEENSY4
#endif

class CanPlus1Teesny4 : public CanPlus {
  public:
    CanPlus1Teesny4();
    void init();
    int writePacket(int id, uint8_t *buffer, size_t size);
    void poll(const CAN_message_t &msg);
    void events();

  private:

};

#if CAN_PLUS_DEFAULT == CAN1_TEENSY4
  extern CanPlus1Teesny4 canPlus;
#endif

#endif
