// -----------------------------------------------------------------------------------
// axis step/dir motor driver

#include "TmcStepDirDriverNSG.h"

#ifdef STEP_DIR_MOTOR_PRESENT

TmcStepDirDriverNSG::TmcStepDirDriverNSG(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings,
                                        int16_t currentHold, int16_t currentRun, int16_t currentSlewing, int8_t intpol)
                                        :TmcStepDirDriver(axisNumber, Pins, Settings, currentHold, currentRun, currentSlewing, intpol) {

}

#endif
