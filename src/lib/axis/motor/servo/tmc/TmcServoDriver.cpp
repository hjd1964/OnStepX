// -----------------------------------------------------------------------------------
// axis servo motor driver

#include "TmcServoDriver.h"

#ifdef SERVO_MOTOR_PRESENT

TmcServoDriver::TmcServoDriver(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings,
                               int16_t microsteps, int16_t current, int8_t decay, int8_t decaySlewing)
                               :ServoDriver(axisNumber, Pins, Settings) {
  if (axisNumber < 1 || axisNumber > 9) return;

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

  if (driverModel == SERVO_TMC5160) { currentMax = TMC5160_MAX_CURRENT; rSense = TMC5160_RSENSE; } else
  if (driverModel == SERVO_TMC2209) { currentMax = TMC2209_MAX_CURRENT; rSense = TMC2209_RSENSE; } else
  driverModel = 0;

  if (user_rSense != 0) rSense = user_rSense;
  if (user_currentMax != 0) currentMax = user_currentMax;

  // set current defaults for TMC drivers
  if (lround(current) == OFF) current = 300.0F;

  this->currentRun.valueDefault = current;
  this->currentRun.max = currentMax;

  // setup decay modes
  this->decay.valueDefault = decay;
  if (this->decay.valueDefault == OFF) this->decay.valueDefault = STEALTHCHOP;
  this->decaySlewing.valueDefault = decaySlewing;
  if (this->decaySlewing.valueDefault == OFF) this->decaySlewing.valueDefault = SPREADCYCLE;
}

bool TmcServoDriver::init(bool reverse) {
  if (!ServoDriver::init(reverse)) return false;

  if (!driverModel) {
    DF("WRN:"); D(axisPrefix); DLF("unknown driver model!");
    return false;
  }

  VF("MSG:"); V(axisPrefix); VF("Rsense="); V(rSense); VL("Ohms");
  VF("MSG:"); V(axisPrefix); VF("max current="); V(currentMax); VL("mA");

  iRun = this->currentRun.valueDefault;
  iHold = iRun;
  iHoldRatio = (float)1.0F;

  VF("MSG:"); V(axisPrefix);
  VF("IHOLD="); V(lround(iHold)); VF("mA, ");
  VF("IRUN="); V(lround(iRun)); VL("mA");

  return true;
}

#endif
