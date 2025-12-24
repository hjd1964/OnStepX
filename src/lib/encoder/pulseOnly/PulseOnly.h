// Pulse only encoders
#pragma once

#include "../EncoderBase.h"

#if AXIS1_ENCODER == PULSE_ONLY || AXIS2_ENCODER == PULSE_ONLY || AXIS3_ENCODER == PULSE_ONLY || \
    AXIS4_ENCODER == PULSE_ONLY || AXIS5_ENCODER == PULSE_ONLY || AXIS6_ENCODER == PULSE_ONLY || \
    AXIS7_ENCODER == PULSE_ONLY || AXIS8_ENCODER == PULSE_ONLY || AXIS9_ENCODER == PULSE_ONLY

// for example:
// PulseOnly encoder1(1, int16_t axis, AXIS1_ENCODER_A_PIN, &direction);

class PulseOnly : public Encoder {
  public:
    PulseOnly(int16_t axis, int16_t pulsePin, volatile int8_t *direction);
    bool init();

    int32_t read();
    void write(int32_t position);

    void setDirection(volatile int8_t *direction);

    void pulse();

  private:
    int16_t axis_index;

    int16_t PulsePin;
    volatile int8_t *pulseDirection;
};

#endif
