// CW/CCW encoders
#pragma once

#include "../EncoderBase.h"

#if AXIS1_ENCODER == CW_CCW || AXIS2_ENCODER == CW_CCW || AXIS3_ENCODER == CW_CCW || \
    AXIS4_ENCODER == CW_CCW || AXIS5_ENCODER == CW_CCW || AXIS6_ENCODER == CW_CCW || \
    AXIS7_ENCODER == CW_CCW || AXIS8_ENCODER == CW_CCW || AXIS9_ENCODER == CW_CCW

// for example:
// CwCcw encoder1(1, AXIS1_ENCODER_A_PIN, AXIS1_ENCODER_B_PIN);

class CwCcw : public Encoder {
  public:
    CwCcw(int16_t axis, int16_t cwPin, int16_t ccwPin);
    bool init();

    int32_t read();
    void write(int32_t position);

    void cw();
    void ccw();

  private:
    int16_t axis_index;

    int16_t CwPin;
    int16_t CcwPin;
};

#endif
