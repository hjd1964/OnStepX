// -----------------------------------------------------------------------------------
// axis step/dir motor driver

#include "TmcStepDirDriver.h"

#ifdef STEP_DIR_MOTOR_PRESENT

#ifndef TMC2130_RSENSE
#define TMC2130_RSENSE 0.11F
#endif
#ifndef TMC2130_MAX_CURRENT
#define TMC2130_MAX_CURRENT (1700*0.8) // chip typically rated at 1.2A RMS, downrated to 80% due to typical step-stick form
#endif

#ifndef TMC2160_RSENSE
#define TMC2160_RSENSE 0.075F
#endif
#ifndef TMC2160_MAX_CURRENT
#define TMC2160_MAX_CURRENT 4230       // module typically rated at 3.0A RMS
#endif

#ifndef TMC2208_RSENSE
#define TMC2208_RSENSE 0.11F
#endif
#ifndef TMC2208_MAX_CURRENT
#define TMC2208_MAX_CURRENT (1974*0.8) // chip rated at 1.4A RMS, downrated to 80% due to typical step-stick form
#endif

#ifndef TMC2209_RSENSE
#define TMC2209_RSENSE 0.11F
#endif
#ifndef TMC2209_MAX_CURRENT
#define TMC2209_MAX_CURRENT (2820*0.8) // chip rated at 2.0A RMS, downrated to 80% due to typical step-stick form
#endif

#ifndef TMC2226_RSENSE
#define TMC2226_RSENSE 0.11F
#endif
#ifndef TMC2226_MAX_CURRENT
#define TMC2226_MAX_CURRENT (2820*0.8) // chip rated at 2.0A RMS, downrated to 80% due to typical step-stick form
#endif

#ifndef TMC2660_RSENSE
#define TMC2660_RSENSE 0.075F
#endif
#ifndef TMC2660_MAX_CURRENT
#define TMC2660_MAX_CURRENT 2820       // module/chip rated at 2.0A RMS
#endif

#ifndef TMC5160_RSENSE
#define TMC5160_RSENSE 0.075F
#endif
#ifndef TMC5160_MAX_CURRENT
#define TMC5160_MAX_CURRENT 4230       // typical module rated at 3.0A RMS
#endif

#ifndef TMC5161_RSENSE
#define TMC5161_RSENSE 0.075F
#endif
#ifndef TMC5161_MAX_CURRENT
#define TMC5161_MAX_CURRENT (4935*0.8) // chip rated at 3.5A RMS, downrated to 80% due to typical step-stick form
#endif

TmcStepDirDriver::TmcStepDirDriver(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings,
                                   int16_t currentHold, int16_t currentRun, int16_t currentSlewing, int8_t intpol)
                                   :StepDirDriver(axisNumber, Pins, Settings) {

  // get the maximum current and Rsense for this axis
  float user_rSense = 0;
  int16_t user_currentMax = 0;
  switch (axisNumber) {
    case 1:
      #ifdef AXIS1_DRIVER_RSENSE
        user_rSense = AXIS1_DRIVER_RSENSE;
      #endif
      #ifdef AXIS1_DRIVER_MAX_CURRENT
        user_currentMax = AXIS1_DRIVER_MAX_CURRENT;
      #endif
    break;
    case 2:
      #ifdef AXIS2_DRIVER_RSENSE
        user_rSense = AXIS2_DRIVER_RSENSE;
      #endif
      #ifdef AXIS2_DRIVER_MAX_CURRENT
        user_currentMax = AXIS2_DRIVER_MAX_CURRENT;
      #endif
    break;
    case 3:
      #ifdef AXIS3_DRIVER_RSENSE
        user_rSense = AXIS3_DRIVER_RSENSE;
      #endif
      #ifdef AXIS3_DRIVER_MAX_CURRENT
        user_currentMax = AXIS3_DRIVER_MAX_CURRENT;
      #endif
    break;
    case 4:
      #ifdef AXIS4_DRIVER_RSENSE
        user_rSense = AXIS4_DRIVER_RSENSE;
      #endif
      #ifdef AXIS4_DRIVER_MAX_CURRENT
        user_currentMax = AXIS4_DRIVER_MAX_CURRENT;
      #endif
    break;
    case 5:
      #ifdef AXIS5_DRIVER_RSENSE
        user_rSense = AXIS5_DRIVER_RSENSE;
      #endif
      #ifdef AXIS5_DRIVER_MAX_CURRENT
        user_currentMax = AXIS5_DRIVER_MAX_CURRENT;
      #endif
    break;
    case 6:
      #ifdef AXIS6_DRIVER_RSENSE
        user_rSense = AXIS6_DRIVER_RSENSE;
      #endif
      #ifdef AXIS6_DRIVER_MAX_CURRENT
        user_currentMax = AXIS6_DRIVER_MAX_CURRENT;
      #endif
    break;
    case 7:
      #ifdef AXIS7_DRIVER_RSENSE
        user_rSense = AXIS7_DRIVER_RSENSE;
      #endif
      #ifdef AXIS7_DRIVER_MAX_CURRENT
        user_currentMax = AXIS7_DRIVER_MAX_CURRENT;
      #endif
    break;
    case 8:
      #ifdef AXIS8_DRIVER_RSENSE
        user_rSense = AXIS8_DRIVER_RSENSE;
      #endif
      #ifdef AXIS8_DRIVER_MAX_CURRENT
        user_currentMax = AXIS8_DRIVER_MAX_CURRENT;
      #endif
    break;
    case 9:
      #ifdef AXIS9_DRIVER_RSENSE
        user_rSense = AXIS9_DRIVER_RSENSE;
      #endif
      #ifdef AXIS9_DRIVER_MAX_CURRENT
        user_currentMax = AXIS9_DRIVER_MAX_CURRENT;
      #endif
    break;
  }

  if (driverModel == TMC2130) { currentMax = TMC2130_MAX_CURRENT; rSense = TMC2130_RSENSE; } else
  if (driverModel == TMC2160) { currentMax = TMC2160_MAX_CURRENT; rSense = TMC2160_RSENSE; } else
  if (driverModel == TMC2660) { currentMax = TMC2660_MAX_CURRENT; rSense = TMC2660_RSENSE; } else
  if (driverModel == TMC5160) { currentMax = TMC5160_MAX_CURRENT; rSense = TMC5160_RSENSE; } else
  if (driverModel == TMC5161) { currentMax = TMC5161_MAX_CURRENT; rSense = TMC5161_RSENSE; } else
  if (driverModel == TMC2208) { currentMax = TMC2208_MAX_CURRENT; rSense = TMC2208_RSENSE; } else
  if (driverModel == TMC2209) { currentMax = TMC2209_MAX_CURRENT; rSense = TMC2209_RSENSE; } else
  if (driverModel == TMC2226) { currentMax = TMC2226_MAX_CURRENT; rSense = TMC2226_RSENSE; } else
  driverModel = 0;

  if (user_rSense != 0) rSense = user_rSense;
  if (user_currentMax != 0) currentMax = user_currentMax;

  // set current defaults for TMC drivers
  if (lround(currentRun) == OFF) {
    if (driverModel == TMC2130) currentRun = 2500.0F; else currentRun = 300.0F;
  }
  currentHold = lround(currentHold) == OFF ? currentRun/2.0F : currentHold;
  currentSlewing = lround(currentSlewing) == OFF ? currentRun : currentSlewing;

  this->currentHold.valueDefault = currentHold;
  this->currentHold.max = currentMax;
  this->currentRun.valueDefault = currentRun;
  this->currentRun.max = currentMax;
  this->currentSlewing.valueDefault = currentSlewing;
  this->currentSlewing.max = currentMax;
  this->intpol.valueDefault = intpol;

  // setup decay modes
  if (decay.valueDefault == OFF) decay.valueDefault = STEALTHCHOP;
  if (decaySlewing.valueDefault == OFF) decaySlewing.valueDefault = SPREADCYCLE;
  decay.min = SPREADCYCLE;
  decay.max = STEALTHCHOP;
  decaySlewing.min = SPREADCYCLE;
  decaySlewing.max = STEALTHCHOP;
}

bool TmcStepDirDriver::init() {
  if (!StepDirDriver::init()) return false;

  if (!driverModel) {
    DF("WRN:"); D(axisPrefix); DLF("unknown driver model!");
    return false;
  }

  VF("MSG:"); V(axisPrefix); VF("Rsense="); V(rSense); VL("Ohms");
  VF("MSG:"); V(axisPrefix); VF("max current="); V(currentMax); VL("mA");

  iHold = currentHold.value;
  iRun = currentRun.value;
  iGoto = currentSlewing.value;
  iHoldRatio = (float)iHold/iRun;

  VF("MSG:"); V(axisPrefix);
  VF("IHOLD="); V(lround(iHold)); VF("mA, ");
  VF("IRUN="); V(lround(iRun)); VF("mA, ");
  VF("IGOTO="); V(lround(iGoto)); VL("mA");

  // set mode switching support flags
  // use low speed mode switch for TMC drivers
  modeSwitchAllowed = microstepRatio != 1;
  modeSwitchFastAllowed = false;

  return true;
}

bool TmcStepDirDriver::parameterIsValid(AxisParameter* parameter, bool next) {
  if (!StepDirDriver::parameterIsValid(parameter, next)) return false;

  if (parameter == &currentHold || parameter == &currentRun || parameter == &currentSlewing) {
    int currentHoldValue;
    int currentRunValue;
    int currentSlewingValue;
    if (next) {
      currentHoldValue = lround(currentHold.valueNv);
      currentRunValue = lround(currentRun.valueNv);
      currentSlewingValue = lround(currentSlewing.valueNv);
    } else {
      currentHoldValue = lround(currentHold.value);
      currentRunValue = lround(currentRun.value);
      currentSlewingValue = lround(currentSlewing.value);
    }

    if (currentRunValue > currentMax) {
      DF("WRN:"); D(axisPrefix); DF("bad current run="); D(currentRunValue); DF("mA > current max "); D(currentMax); DLF("mA");
      return false;
    }

    if (currentSlewingValue > currentMax) {
      DF("WRN:"); D(axisPrefix); DF("bad current goto="); D(currentSlewingValue); DF("mA > current max "); D(currentMax); DLF("mA");
      return false;
    }

    if (currentHoldValue > currentRunValue) {
      DF("WRN:"); D(axisPrefix); DF("bad current hold="); D(currentHoldValue); DF("mA > current run "); D(currentRunValue); DLF("mA");
      return false;
    }
  }

  return true;
}

#endif
