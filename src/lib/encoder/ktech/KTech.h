// KTech in motor encoder
#pragma once

#include "../EncoderBase.h"

#ifndef KTECH_SINGLE_TURN
  #define KTECH_SINGLE_TURN 65536
#endif

#ifndef KTECH_SLEW_DIRECT
  #define KTECH_SLEW_DIRECT OFF // ON for KTECH trapezoidal move profile or OFF for OnStep move profile
#endif

#if AXIS1_ENCODER == KTECH_IME || AXIS2_ENCODER == KTECH_IME || AXIS3_ENCODER == KTECH_IME || \
    AXIS4_ENCODER == KTECH_IME || AXIS5_ENCODER == KTECH_IME || AXIS6_ENCODER == KTECH_IME || \
    AXIS7_ENCODER == KTECH_IME || AXIS8_ENCODER == KTECH_IME || AXIS9_ENCODER == KTECH_IME

class KTechIME : public Encoder {
  public:
    KTechIME(int16_t axis);
    bool init();

    int32_t read();
    void write(int32_t count);

    void requestPosition();
    void updatePositionCallback(uint8_t data[8]);

  private:
    int canID = 0;
    void (*callback)() = NULL;
    uint32_t lastCountSingleTurn = 0;
    int32_t countTurns = 0;
};

#endif
