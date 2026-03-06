// -----------------------------------------------------------------------------------
// axis step/dir motor driver
#pragma once

#include "TmcStepDirDriver.h"

#ifdef STEP_DIR_MOTOR_PRESENT

class TmcStepDirDriverNSG : public TmcStepDirDriver {
  public:
    TmcStepDirDriverNSG(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings,
                       int16_t currentHold, int16_t currentRun, int16_t currentSlewing, int8_t intpol);

    // returns the number of axis parameters
    uint8_t getParameterCount() { return numParameters; }

    // returns the specified axis parameter
    AxisParameter* getParameter(uint8_t number) { if (number > numParameters) return &invalid; else return parameter[number]; }

  protected:

    const int numParameters = 7;
    AxisParameter* parameter[8] = {&invalid, &microsteps, &microstepsSlewing,
                                   &currentHold, &currentRun, &currentSlewing,
                                   &decay, &decaySlewing
//                                   ,&intpol
                                   };
};

#endif
