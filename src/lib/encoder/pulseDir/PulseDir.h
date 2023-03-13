// Pulse/Dir encoders
#pragma once

#include "../Encoder.h"

#if AXIS1_ENCODER == PULSE_DIR || AXIS2_ENCODER == PULSE_DIR || AXIS3_ENCODER == PULSE_DIR || \
    AXIS4_ENCODER == PULSE_DIR || AXIS5_ENCODER == PULSE_DIR || AXIS6_ENCODER == PULSE_DIR || \
    AXIS7_ENCODER == PULSE_DIR || AXIS8_ENCODER == PULSE_DIR || AXIS9_ENCODER == PULSE_DIR

class PulseDir : public Encoder {
  public:
    PulseDir(int16_t pulsePin, int16_t dirPin, int16_t axis);
    void init();

    int32_t read();
    void write(int32_t count);

  private:
    int16_t pulsePin, dirPin;
};

#endif
