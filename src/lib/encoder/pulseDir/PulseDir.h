// Pulse/Dir encoders
#pragma once

#include "../EncoderBase.h"

#if AXIS1_ENCODER == PULSE_DIR || AXIS2_ENCODER == PULSE_DIR || AXIS3_ENCODER == PULSE_DIR || \
    AXIS4_ENCODER == PULSE_DIR || AXIS5_ENCODER == PULSE_DIR || AXIS6_ENCODER == PULSE_DIR || \
    AXIS7_ENCODER == PULSE_DIR || AXIS8_ENCODER == PULSE_DIR || AXIS9_ENCODER == PULSE_DIR

// for example:
// PulseDir encoder1(1, AXIS1_ENCODER_A_PIN, AXIS1_ENCODER_B_PIN);

class PulseDir : public Encoder {
  public:
    PulseDir(int16_t axis, int16_t pulsePin, int16_t dirPin);
    bool init();

    int32_t read();
    void write(int32_t position);

    void pulse(const int16_t dirPin);

  private:
    int16_t axis_index;

    int16_t PulsePin
    volatile int16_t DirPin;
};

#endif
