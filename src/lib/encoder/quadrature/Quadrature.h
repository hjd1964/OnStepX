// A/B Quadrature encoders
#pragma once

#include "../Encoder.h"

#if AXIS1_ENCODER == AB || AXIS2_ENCODER == AB || AXIS3_ENCODER == AB || \
    AXIS4_ENCODER == AB || AXIS5_ENCODER == AB || AXIS6_ENCODER == AB || \
    AXIS7_ENCODER == AB || AXIS8_ENCODER == AB || AXIS9_ENCODER == AB

// for example:
// Quadrature encoder1(AXIS1_ENCODER_A_PIN, AXIS1_ENCODER_B_PIN, 1);

// Phase 1: LLHH LLHH
// Phase 2: LHHL LHHL
// ...00 01 11 10 00 01 11 10 00 01 11 10...

class Quadrature : public Encoder {
  public:
    Quadrature(int16_t APin, int16_t BPin, int16_t axis);
    void init();

    int32_t read();
    void write(int32_t count);

    void A(const int16_t pin);
    void B(const int16_t pin);

  private:
    int16_t axis;

    int16_t APin = OFF;
    int16_t BPin = OFF;

    volatile int16_t stateA;
    volatile int16_t stateB;
    volatile int16_t lastA;
    volatile int16_t lastB;
};

#endif
