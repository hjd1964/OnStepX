// KTech in motor encoder (designed for the MS4010v3 with 16bit encoder)
#pragma once

#include "../EncoderBase.h"

#ifndef KTECH_SINGLE_TURN
  #define KTECH_SINGLE_TURN 65536
#endif

// for example:
// KTechIME encoder1(1);

#if AXIS1_ENCODER == KTECH_IME || AXIS2_ENCODER == KTECH_IME || AXIS3_ENCODER == KTECH_IME || \
    AXIS4_ENCODER == KTECH_IME || AXIS5_ENCODER == KTECH_IME || AXIS6_ENCODER == KTECH_IME || \
    AXIS7_ENCODER == KTECH_IME || AXIS8_ENCODER == KTECH_IME || AXIS9_ENCODER == KTECH_IME

class KTechIME : public Encoder {
  public:
    KTechIME(int16_t axis);
    bool init();

    int32_t read();
    void write(int32_t position);

    bool supportsTimeAlignedMotorSteps() const { return true; }
    void updatePositionCallback(uint8_t data[8]);
    void updateVelocityCallback(uint8_t data[8]);

    void requestPosition();

  private:
    int16_t axis_index = 0;

    bool firstCall = true;

    int canID = 0;
    void (*callback)() = NULL;

    volatile uint16_t lastCountSingleTurn = 0;
    volatile long countTurns = 0;

    volatile long motorStepsAtLastUpdate = 0;

    uint32_t lastUpdateByPositionCommand = 0;
    uint32_t lastUpdateByVelocityCommand = 0;
};

#endif
