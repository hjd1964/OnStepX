// -----------------------------------------------------------------------------------
// Generic motor controller

#include "Motor.h"

#ifdef MOTOR_PRESENT

Motor::Motor(uint8_t axisNumber, int8_t reverse) {
  this->axisNumber = axisNumber;
  this->reverse.valueDefault = reverse == ON;
}

bool Motor::init() {
  VF("MSG:"); V(axisPrefix); VLF("motor init");
  normalizedReverse = (bool)lround(reverse.value);
  return true;
}

// returns the specified axis parameter by name
AxisParameter* Motor::getParameterByName(const char* name) {
  for (int i = 1; i <= getParameterCount(); i++) {
    if (strcmp(getParameter(i)->name, name) == 0) { return getParameter(i); }
  }
  return &invalid;
}

// check if parameter is valid
bool Motor::parameterIsValid(AxisParameter* parameter, bool next) {
  float value;
  if (next) value = parameter->valueNv; else value = parameter->value;
  if (value < parameter->min) return false;
  if (value > parameter->max) return false;
  if (parameter->type == AXP_POW2) {
    if (value != 1.0F && value != 2.0F && value != 4.0F &&
        value != 8.0F && value != 16.0F && value != 32.0F &&
        value != 64.0F && value != 128.0F && value != 256.0F) {
      return false;
    }
  }
  return true;
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
    if (modulo == OFF) modulo = 1;
    long steps = value - motorSteps;
    steps -= modulo*2L;
    for (int l = 0; l < modulo*4; l++) { if (steps % (modulo*4L) == 0) break; steps++; }
    indexSteps = steps;
  } else setInstrumentCoordinateSteps(value);
  VF("MSG:"); V(axisPrefix); VF("setInstrumentCoordinateParkSteps at "); V(indexSteps); VF(" (was "); V(value - motorSteps); VL(")");
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
    if (modulo == OFF) modulo = 1;
    long steps = value - indexSteps;
    steps -= modulo*2L;
    for (int l = 0; l < modulo*4; l++) { if (steps % (modulo*4L) == 0) break; steps++; }
    noInterrupts();
    targetSteps = steps;
    interrupts();
  } else setTargetCoordinateSteps(value);
  VF("MSG:"); V(axisPrefix); VF("setTargetCoordinateParkSteps at "); V(targetSteps); VF(" (was "); V(value - indexSteps); VL(")");
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
