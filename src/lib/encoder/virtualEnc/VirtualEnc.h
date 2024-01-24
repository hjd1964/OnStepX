// Pulse only encoders
#pragma once

#include "../Encoder.h"

#if AXIS1_ENCODER == VIRTUAL || AXIS2_ENCODER == VIRTUAL || AXIS3_ENCODER == VIRTUAL || \
    AXIS4_ENCODER == VIRTUAL || AXIS5_ENCODER == VIRTUAL || AXIS6_ENCODER == VIRTUAL || \
    AXIS7_ENCODER == VIRTUAL || AXIS8_ENCODER == VIRTUAL || AXIS9_ENCODER == VIRTUAL

class VirtualEnc : public Encoder {
  public:
    VirtualEnc(int16_t axis);
    void init();

    int32_t read();
    void write(int32_t count);

    void setVelocity(float countsPerSec);
    void setDirection(volatile int8_t *direction);

  private:
    uint8_t virtualEncoderHandle = 0; 
    long countsPerSec = 0;
    long timerRateMs = 3000/HAL_FRACTIONAL_SEC; // (1.0/HAL_FRACTIONAL_SEC)*3*1000
};

#endif
