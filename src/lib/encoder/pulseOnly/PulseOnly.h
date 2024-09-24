// Pulse only encoders
#pragma once

#include "../Encoder.h"

#if AXIS1_ENCODER == PULSE_ONLY || AXIS2_ENCODER == PULSE_ONLY || AXIS3_ENCODER == PULSE_ONLY || \
    AXIS4_ENCODER == PULSE_ONLY || AXIS5_ENCODER == PULSE_ONLY || AXIS6_ENCODER == PULSE_ONLY || \
    AXIS7_ENCODER == PULSE_ONLY || AXIS8_ENCODER == PULSE_ONLY || AXIS9_ENCODER == PULSE_ONLY

class PulseOnly : public Encoder {
  public:
    PulseOnly(int16_t pulsePin, int16_t *direction, int16_t axis);
    void init();

    int32_t read();
    void write(int32_t count);

    void setDirection(volatile int8_t *direction);

  private:
    int16_t pulsePin;
};

#endif
