// A/B Quadrature encoders
#pragma once

#include "../EncoderBase.h"

#if AXIS1_ENCODER == AB || AXIS2_ENCODER == AB || AXIS3_ENCODER == AB || \
    AXIS4_ENCODER == AB || AXIS5_ENCODER == AB || AXIS6_ENCODER == AB || \
    AXIS7_ENCODER == AB || AXIS8_ENCODER == AB || AXIS9_ENCODER == AB

// for example:
// Quadrature encoder1(AXIS1_ENCODER_A_PIN, AXIS1_ENCODER_B_PIN, 1);

// Phase A: LLHH LLHH LLHH ...
// Phase B: LHHL LHHL LHHL ...
// States:  00 01 11 10 00 01 11 10 ... (forward rotation)
//         00 10 11 01 00 10 11 01 ... (reverse rotation)

class Quadrature : public Encoder {
  public:
    // Constructor
    Quadrature(int16_t APin, int16_t BPin, int16_t axis);

    // Initialization
    bool init();

    // Position read/write
    int32_t read();
    void write(int32_t count);

    // Error management
    void clearErrors();
    uint32_t getErrors();
    uint32_t getWarnings();

    // Interrupt handler (called by both A and B interrupts)
    void handleInterrupt();

  private:
    // Axis configuration
    int16_t axis;
    int16_t APin = OFF;
    int16_t BPin = OFF;

    // State tracking (volatile for interrupt safety)
    volatile bool stateA;
    volatile bool stateB;

    // Position and error counters (volatile for interrupt safety)
    volatile int32_t count = 0;
    volatile uint32_t error = 0;
    volatile uint32_t warn = 0;

    // Ready flag
    volatile bool ready = false;

    // Origin offset for position calculations
    int32_t origin = 0;

  // Note: Removed the separate A() and B() methods since we now use
  // a single handleInterrupt() method to prevent double-counting
};

#endif