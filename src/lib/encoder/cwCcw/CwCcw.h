// CW/CCW encoders
#pragma once

#include "../Encoder.h"

#if AXIS1_ENCODER == CW_CCW || AXIS2_ENCODER == CW_CCW || AXIS3_ENCODER == CW_CCW || \
    AXIS4_ENCODER == CW_CCW || AXIS5_ENCODER == CW_CCW || AXIS6_ENCODER == CW_CCW || \
    AXIS7_ENCODER == CW_CCW || AXIS8_ENCODER == CW_CCW || AXIS9_ENCODER == CW_CCW

class CwCcw : public Encoder {
  public:
    CwCcw(int16_t cwPin, int16_t ccwPin, int16_t axis);
    void init();

    int32_t read();
    void write(int32_t count);

  private:
    int16_t cwPin, ccwPin;
};

#endif
