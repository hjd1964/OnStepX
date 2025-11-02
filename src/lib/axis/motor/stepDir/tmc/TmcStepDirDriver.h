// -----------------------------------------------------------------------------------
// axis step/dir motor driver
#pragma once

#include "../StepDirDriver.h"

#ifdef STEP_DIR_MOTOR_PRESENT

// default settings for any TMC drivers that may be present
#ifndef DRIVER_TMC_STEPPER_AUTOGRAD
  #define DRIVER_TMC_STEPPER_AUTOGRAD true // note that AUTOGRAD was false for SPI drivers before!
#endif
#ifndef SERIAL_TMC
  #define SERIAL_TMC SoftSerial // Use software serial w/ TX on M3 (CS) and RX on M4 (MISO) of each axis
#endif
#ifndef SERIAL_TMC_BAUD
  #define SERIAL_TMC_BAUD 115200 // Slower and more timing disrutpions or faster and it doesn't work (platform dependant)
#endif
#ifndef SERIAL_TMC_ADDRESS_MAP
  #define SERIAL_TMC_ADDRESS_MAP(x) (0) // driver addresses are 0 for all axes
#endif

#if SERIAL_TMC == SoftSerial
  #include <SoftwareSerial.h> // must be built into the board libraries
#endif

class TmcStepDirDriver : public StepDirDriver {
  public:
    TmcStepDirDriver(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings,
                     int16_t currentHold, int16_t currentRun, int16_t currentSlewing, int8_t intpol);

    // set up driver
    virtual bool init();

    // returns the number of axis parameters
    uint8_t getParameterCount() { return numParameters; }

    // returns the specified axis parameter
    AxisParameter* getParameter(uint8_t number) { if (number > numParameters) return &invalid; else return parameter[number]; }

    // check if axis parameter is valid
    bool parameterIsValid(AxisParameter* parameter, bool next = false);

  protected:
    // driver current control
    float rSense = 0.11F;
    int16_t currentMax = 0;

    float iHold, iRun, iGoto, iHoldRatio;

    // runtime adjustable settings
    AxisParameter currentHold    = {NAN, NAN, NAN, 0, 20000, AXP_INTEGER, AXPN_CURRENT_HOLD};
    AxisParameter currentRun     = {NAN, NAN, NAN, 0, 20000, AXP_INTEGER, AXPN_CURRENT_RUN};
    AxisParameter currentSlewing = {NAN, NAN, NAN, 0, 20000, AXP_INTEGER, AXPN_CURRENT_GOTO};
    AxisParameter intpol         = {NAN, NAN, NAN, -2, -1, AXP_BOOLEAN, AXPN_INTERPOLATE};

    const int numParameters = 8;
    AxisParameter* parameter[9] = {&invalid, &microsteps, &microstepsSlewing, &currentHold, &currentRun, &currentSlewing, &decay, &decaySlewing, &intpol};
};

#endif
