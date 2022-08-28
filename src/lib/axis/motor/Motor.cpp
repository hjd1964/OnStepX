// -----------------------------------------------------------------------------------
// Generic motor controller

#include "Motor.h"

#ifdef MOTOR_PRESENT

// get motor default parameters
void Motor::getDefaultParameters(float *param1, float *param2, float *param3, float *param4, float *param5, float *param6){
  *param1 = default_param1;
  *param2 = default_param2;
  *param3 = default_param3;
  *param4 = default_param4;
  *param5 = default_param5;
  *param6 = default_param6;
}

// set motor default parameters
void Motor::setDefaultParameters(float param1, float param2, float param3, float param4, float param5, float param6){
  default_param1 = param1;
  default_param2 = param2;
  default_param3 = param3;
  default_param4 = param4;
  default_param5 = param5;
  default_param6 = param6;
}

// resets motor and target angular position in steps, also zeros backlash and index 
void Motor::resetPositionSteps(long value) {
  indexSteps    = 0;
  noInterrupts();
  motorSteps    = value;
  targetSteps   = value;
  backlashSteps = 0;
  interrupts();
}

// resets target position to the motor position
void Motor::resetTargetToMotorPosition() {
  noInterrupts();
  targetSteps = motorSteps;
  interrupts();
}

// get motor position in steps (including backlash)
long Motor::getMotorPositionSteps() {
  noInterrupts();
  long steps = motorSteps + backlashSteps;
  interrupts();
  return steps;
}

// get instrument coordinate, in steps
long Motor::getInstrumentCoordinateSteps() {
  noInterrupts();
  long steps = motorSteps + indexSteps;
  interrupts();
  return steps;
}

// set instrument coordinate, in steps
void Motor::setInstrumentCoordinateSteps(long value) {
  noInterrupts();
  indexSteps = value - motorSteps;
  interrupts();
}

// set instrument park coordinate, in steps
// should only be called when the axis is not moving
void Motor::setInstrumentCoordinateParkSteps(long value, int modulo) {
  if (driverType == STEP_DIR) {
    long steps = value - motorSteps;
    steps -= modulo*2L;
    for (int l = 0; l < modulo*4; l++) { if (steps % (modulo*4L) == 0) break; steps++; }
    indexSteps = steps;
  } else setInstrumentCoordinateSteps(value);
  V(axisPrefix); VF("setInstrumentCoordinateParkSteps at "); V(indexSteps); VF(" (was "); V(value - motorSteps); VL(")");
}

// get target coordinate (with index), in steps
long Motor::getTargetCoordinateSteps() {
  noInterrupts();
  long steps = targetSteps + indexSteps;
  interrupts();
  return steps;
}

// set target coordinate (with index), in steps
void Motor::setTargetCoordinateSteps(long value) {
  noInterrupts();
  targetSteps = value - indexSteps;
  interrupts();
}

// set target park coordinate, in steps (taking into account stepper motor cogging when powered off)
// should only be called when the axis is not moving
void Motor::setTargetCoordinateParkSteps(long value, int modulo) {
  if (driverType == STEP_DIR) {
    long steps = value - indexSteps;
    steps -= modulo*2L;
    for (int l = 0; l < modulo*4; l++) { if (steps % (modulo*4L) == 0) break; steps++; }
    noInterrupts();
    targetSteps = steps;
    interrupts();
  } else setTargetCoordinateSteps(value);
  V(axisPrefix); VF("setTargetCoordinateParkSteps at "); V(targetSteps); VF(" (was "); V(value - indexSteps); VL(")");
}

// get backlash amount in steps
long Motor::getBacklashSteps() {
  noInterrupts();
  uint16_t backlash = backlashAmountSteps;
  interrupts();
  return backlash;
}

// set backlash amount in steps
void Motor::setBacklashSteps(long value) {
  noInterrupts();
  backlashAmountSteps = value;
  interrupts();
}

// mark origin coordinate for autoGoto as current location
void Motor::markOriginCoordinateSteps() {
  noInterrupts();
  originSteps = motorSteps;
  interrupts();
}

// distance to target in steps (+/-)
long Motor::getTargetDistanceSteps() {
  noInterrupts();
  long dist = targetSteps - motorSteps;
  interrupts();
  return dist;
}

// distance to origin or target, whichever is closer, in steps
long Motor::getOriginOrTargetDistanceSteps() {
  noInterrupts();
  long steps = motorSteps;
  interrupts();
  long distanceOrigin = labs(originSteps - steps);
  long distanceTarget = labs(targetSteps - steps);
  if (distanceOrigin < distanceTarget) return distanceOrigin; else return distanceTarget;
}

// returns 1 if distance to origin is closer else -1 if target is closer
int Motor::getRampDirection() {
  noInterrupts();
  long steps = motorSteps;
  interrupts();
  long distanceOrigin = labs(originSteps - steps);
  long distanceTarget = labs(targetSteps - steps);
  if (distanceOrigin < distanceTarget) return 1; else return -1;
}

// set backlash frequency in steps per second
void Motor::setBacklashFrequencySteps(float frequency) {
  backlashFrequency = frequency;
}

// get the current direction of motion
Direction Motor::getDirection() {
  if (getFrequencySteps() != 0.0F) {
    if (step >= 1) return DIR_FORWARD;
    if (step <= -1) return DIR_REVERSE;
  }
  return DIR_NONE;
}

void Motor::disableBacklash() {
  noInterrupts();
  backlashAmountStepsStore = backlashAmountSteps;
  motorSteps += backlashSteps;
  backlashStepsStore = backlashSteps;
  backlashSteps = 0;
  interrupts();
}

void Motor::enableBacklash() {
  noInterrupts();
  backlashSteps = backlashStepsStore;
  motorSteps -= backlashSteps;
  backlashAmountSteps = backlashAmountStepsStore;
  interrupts();
  backlashStepsStore = 0;
  backlashAmountStepsStore = 0;
}

#endif
