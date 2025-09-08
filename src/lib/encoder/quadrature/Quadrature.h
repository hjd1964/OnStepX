// A/B Quadrature encoders
#pragma once

#include "../EncoderBase.h"

#if AXIS1_ENCODER == AB || AXIS2_ENCODER == AB || AXIS3_ENCODER == AB || \
    AXIS4_ENCODER == AB || AXIS5_ENCODER == AB || AXIS6_ENCODER == AB || \
    AXIS7_ENCODER == AB || AXIS8_ENCODER == AB || AXIS9_ENCODER == AB

// for example:
// Quadrature encoder1(AXIS1_ENCODER_A_PIN, AXIS1_ENCODER_B_PIN, 1);

// Phase 1: LLHH LLHH
// Phase 2: LHHL LHHL
// ...00 01 11 10 00 01 11 10 00 01 11 10...
// 

// quadrature encoder behavior for invalid signal state

#ifndef ENCODER_AB_STRICT
  #define ENCODER_AB_STRICT OFF
#endif

#if ENCODER_AB_STRICT == OFF
  // current A and/or B signal didn't toggle, just assume it happened and use last dir state
  #define QUAD_F1 warn++
  #define QUAD_F2 warn++
#else
  // current A and/or B signal didn't toggle, skip counting and flag error
  #define QUAD_F1 dir = 0; error++
  #define QUAD_F2 dir = 0; error++
#endif

class Quadrature : public Encoder {
  public:
    Quadrature(int16_t axis, int16_t APin, int16_t BPin);
    bool init();

    int32_t read();
    void write(int32_t count);

    void A(const int16_t pin);
    void B(const int16_t pin);

  private:
    int16_t axis_index;

    volatile int32_t quadratureCount = 0;

    int16_t APin = OFF;
    int16_t BPin = OFF;

    volatile int16_t stateA;
    volatile int16_t stateB;
    volatile int16_t lastA;
    volatile int16_t lastB;
    volatile int16_t dir;
};

#endif
