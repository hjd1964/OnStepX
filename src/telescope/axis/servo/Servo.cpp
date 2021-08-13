// -----------------------------------------------------------------------------------
// Axis servo driver motion

#include "Servo.h"

#ifdef SERVO_DRIVER_PRESENT

#include "../../../tasks/OnTask.h"
extern Tasks tasks;
#include "../../Telescope.h"
#include "../../mount/Mount.h"
#include "../../rotator/Rotator.h"
#include "../../focuser/Focuser.h"
#include "../../../lib/sense/Sense.h"

#ifdef MOUNT_PRESENT
  extern unsigned long periodSubMicros;
#else
  #define periodSubMicros 1.0
#endif

#ifdef AXIS1_DRIVER_DC
  Encoder axis1Enc(AXIS1_ENC_A_PIN, AXIS1_ENC_B_PIN);
  IRAM_ATTR void moveAxis1() { mount.axis1.servo.move(); }
#endif
#ifdef AXIS2_DRIVER_SERVO
  Encoder axis2Enc(AXIS2_ENC_A_PIN, AXIS2_ENC_B_PIN);
  IRAM_ATTR void moveAxis2() { mount.axis2.servo.move(); }
#endif
#ifdef AXIS3_DRIVER_SERVO
  Encoder axis3Enc(AXIS3_ENC_A_PIN, AXIS3_ENC_B_PIN);
  IRAM_ATTR void moveAxis3() { rotator.axis.servo.move(); }
#endif
#ifdef AXIS4_DRIVER_SERVO
  Encoder axis4Enc(AXIS4_ENC_A_PIN, AXIS4_ENC_B_PIN);
  IRAM_ATTR void moveAxis4() { focuser.axis[0]->servo.move(); }
#endif
#ifdef AXIS5_DRIVER_SERVO
  Encoder axis5Enc(AXIS5_ENC_A_PIN, AXIS5_ENC_B_PIN);
  IRAM_ATTR void moveAxis5() { focuser.axis[1]->servo.move(); }
#endif
#ifdef AXIS6_DRIVER_SERVO
  Encoder axis6Enc(AXIS6_ENC_A_PIN, AXIS6_ENC_B_PIN);
  IRAM_ATTR void moveAxis6() { focuser.axis[2]->servo.move(); }
#endif
#ifdef AXIS7_DRIVER_SERVO
  Encoder axis7Enc(AXIS7_ENC_A_PIN, AXIS7_ENC_B_PIN);
  IRAM_ATTR void moveAxis7() { focuser.axis[3]->servo.move(); }
#endif
#ifdef AXIS8_DRIVER_SERVO
  Encoder axis8Enc(AXIS8_ENC_A_PIN, AXIS8_ENC_B_PIN);
  IRAM_ATTR void moveAxis8() { focuser.axis[4]->servo.move(); }
#endif
#ifdef AXIS9_DRIVER_SERVO
  Encoder axis9Enc(AXIS9_ENC_A_PIN, AXIS9_ENC_B_PIN);
  IRAM_ATTR void moveAxis9() { focuser.axis[5]->servo.move(); }
#endif

bool Servo::init(uint8_t axisNumber, int8_t reverse, int16_t microsteps, int16_t current) {
  axisPrefix[10] = '0' + axisNumber;
  this->axisNumber = axisNumber;
  
  taskHandle = 0;
  #ifdef AXIS1_DRIVER_DC
    if (axisNumber == 1) { enc = &axis1Enc; _move = moveAxis1; }
  #endif
  #ifdef AXIS2_DRIVER_SERVO
    if (axisNumber == 2) { enc = &axis2Enc; _move = moveAxis2; }
  #endif
  #ifdef AXIS3_DRIVER_SERVO
    if (axisNumber == 3) { enc = &axis3Enc; _move = moveAxis3; }
  #endif
  #ifdef AXIS4_DRIVER_SERVO
    if (axisNumber == 4) { enc = &axis4Enc; _move = moveAxis4; }
  #endif
  #ifdef AXIS5_DRIVER_SERVO
    if (axisNumber == 5) { enc = &axis5Enc; _move = moveAxis5; }
  #endif
  #ifdef AXIS6_DRIVER_SERVO
    if (axisNumber == 6) { enc = &axis6Enc; _move = moveAxis6; }
  #endif
  #ifdef AXIS7_DRIVER_SERVO
    if (axisNumber == 7) { enc = &axis7Enc; _move = moveAxis7; }
  #endif
  #ifdef AXIS8_DRIVER_SERVO
    if (axisNumber == 8) { enc = &axis8Enc; _move = moveAxis8; }
  #endif
  #ifdef AXIS9_DRIVER_SERVO
    if (axisNumber == 9) { enc = &axis9Enc; _move = moveAxis9; }
  #endif

  // make sure there is something to do
  if (_move == NULL) { V(axisPrefix); VF("nothing to do exiting!"); return false; }

  // init driver advanced modes, etc.
  driver.init(axisNumber);

  // now disable the driver
  power(false);

  // start the motor timer
  V(axisPrefix); VF("start task to move motor... ");
  char timerName[] = "Motor_";
  timerName[5] = '0' + axisNumber;
  taskHandle = tasks.add(0, 0, true, 0, _move, timerName);
  if (taskHandle) {
    V("success");
    if (axisNumber <= 2) { if (!tasks.requestHardwareTimer(taskHandle, axisNumber, 0)) { VF(" (no hardware timer!)"); } }
    VL("");
  } else { VL("FAILED!"); return false; }

  return true;
}

// sets motor power on/off (if possible)
void Servo::power(bool state) {
  driver.power(state);
}

// resets motor and target angular position in steps, also zeros backlash and index 
void Servo::resetPositionSteps(long value) {
  indexSteps    = 0;
  noInterrupts();
  motorSteps    = value;
  targetSteps   = value;
  backlashSteps = 0;
  interrupts();
}

// resets target position to the motor position
void Servo::resetTargetToMotorPosition() {
  noInterrupts();
  targetSteps = motorSteps;
  interrupts();
}

// get motor angular position in steps
long Servo::getMotorPositionSteps() {
  noInterrupts();
  long steps = motorSteps + backlashSteps;
  interrupts();
  return steps;
}

// get instrument coordinate, in steps
long Servo::getInstrumentCoordinateSteps() {
  noInterrupts();
  long steps = motorSteps + indexSteps;
  interrupts();
  return steps;
}

// set instrument coordinate, in steps
void Servo::setInstrumentCoordinateSteps(long value) {
  noInterrupts();
  indexSteps = value - motorSteps;
  interrupts();
}

// mark origin coordinate for autoSlewRateByDistance as current location
void Servo::setOriginCoordinateSteps() {
  noInterrupts();
  originSteps = motorSteps;
  interrupts();
}

// set target coordinate (with index), in steps
void Servo::setTargetCoordinateSteps(long value) {
  noInterrupts();
  targetSteps = value - indexSteps;
  interrupts();
}

// get target coordinate (with index), in steps
long Servo::getTargetCoordinateSteps() {
  noInterrupts();
  long steps = targetSteps + indexSteps;
  interrupts();
  return steps;
}

// distance to target in steps (+/-)
long Servo::getTargetDistanceSteps() {
  noInterrupts();
  long dist = targetSteps - motorSteps;
  interrupts();
  return dist;
}

// set target park coordinate, in steps (taking into account stepper motor cogging when powered off)
// should only be called when the axis is not moving
void Servo::setTargetCoordinateParkSteps(long value, int modulo) {
  long steps = value - indexSteps;
  steps -= modulo*2L;
  for (int l = 0; l < modulo*4; l++) { if (steps % (modulo*4L) == 0) break; steps++; }
  noInterrupts();
  targetSteps = steps;
  interrupts();
  V(axisPrefix); VF("setTargetCoordinateParkSteps at "); V(targetSteps); V(" (was "); V(value - indexSteps); VL(")");
}

// set instrument park coordinate, in steps
// should only be called when the axis is not moving
void Servo::setInstrumentCoordinateParkSteps(long value, int modulo) {
  long steps = value - motorSteps;
  steps -= modulo*2L;
  for (int l = 0; l < modulo*4; l++) { if (steps % (modulo*4L) == 0) break; steps++; }
  indexSteps = steps;
  V(axisPrefix); VF("setInstrumentCoordinateParkSteps at "); V(indexSteps); V(" (was "); V(value - motorSteps); VL(")");
}

// distance to origin or target, whichever is closer, in steps
long Servo::getOriginOrTargetDistanceSteps() {
  noInterrupts();
  long steps = motorSteps;
  interrupts();
  long distanceOrigin = labs(originSteps - steps);
  long distanceTarget = labs(targetSteps - steps);
  if (distanceOrigin < distanceTarget) return distanceOrigin; else return distanceTarget;
}

// set backlash frequency in steps per second
void Servo::setBacklashFrequencySteps(float frequency) {
  backlashFrequency = frequency;
}

// set backlash amount in steps
void Servo::setBacklashSteps(long value) {
  noInterrupts();
  backlashAmountSteps = value;
  interrupts();
}

// get backlash amount in steps
long Servo::getBacklashSteps() {
  noInterrupts();
  uint16_t backlash = backlashAmountSteps;
  interrupts();
  return backlash;
}

void Servo::disableBacklash() {
  noInterrupts();
  backlashAmountStepsStore = backlashAmountSteps;
  motorSteps += backlashSteps;
  backlashStepsStore = backlashSteps;
  backlashSteps = 0;
  interrupts();
}

void Servo::enableBacklash() {
  noInterrupts();
  backlashSteps = backlashStepsStore;
  motorSteps -= backlashSteps;
  backlashAmountSteps = backlashAmountStepsStore;
  interrupts();
  backlashStepsStore = 0;
  backlashAmountStepsStore = 0;
}

// get the current direction of motion
Direction Servo::getDirection() {
  if (lastPeriod != 0) {
    if (step == 1) return DIR_FORWARD;
    if (step == -1) return DIR_REVERSE;
  }
  return DIR_NONE;
}

// get the associated stepper drivers status
DriverStatus Servo::getDriverStatus() {
  driver.updateStatus();
  return driver.getStatus();
}

// set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
void Servo::setFrequencySteps(float frequency) {
  // negative frequency, convert to positive and reverse the direction
  int dir = 0;
  if (frequency > 0.0F) dir = 1; else if (frequency < 0.0F) { frequency = -frequency; dir = -1; }

  // if in backlash override the frequency
  if (inBacklash) frequency = backlashFrequency;

  if (frequency != currentFrequency) {
    lastFrequency = frequency;

    // frequency in steps per second to period in microsecond counts per step
    // also runs the timer twice as fast if using a square wave
    #if STEP_WAVE_FORM == SQUARE
      float period = 500000.0F/frequency;
    #else
      float period = 1000000.0F/frequency;
    #endif
   
    // range is 0 to 134 seconds/step
    if (!isnan(period) && period <= 130000000.0F) {
      // convert microsecond counts to sub-microsecond counts
      period *= 16.0F;

      // adjust period for MCU clock inaccuracy (signed 32bit numeric range covers about +/- 3% here)
      period *= (SIDEREAL_PERIOD/periodSubMicros);

      // remember the last active period
      lastPeriod = (unsigned long)lroundf(period);
    } else {
      lastPeriod = 0;
      frequency = 0.0F;
      dir = 0;
    }

    currentFrequency = frequency;

    // change the motor rate/direction
    if (step != dir) step = 0;
    tasks.setPeriodSubMicros(taskHandle, lastPeriod);
  }

  noInterrupts();
  step = dir;
  interrupts();
}

float Servo::getFrequencySteps() {
  if (lastPeriod == 0) return 0;
  return 16000000.0F/lastPeriod;
}

// set synchronized state (automatic movement of target at setFrequencySteps() rate)
void Servo::setSynchronized(bool state) {
  synchronized = state;
}

// get synchronized state (automatic movement of target at setFrequencySteps() rate)
bool Servo::getSynchronized() {
  return synchronized;
}

IRAM_ATTR void Servo::move() {
  if (synchronized && !inBacklash) targetSteps += step;
  if (motorSteps > targetSteps) {
    direction = DIR_REVERSE;
    if (backlashSteps > 0) { inBacklash = true; backlashSteps--; } else { inBacklash = false; motorSteps--; }
  } else 
  if (motorSteps < targetSteps || inBacklash) {
    direction = DIR_FORWARD;
    if (backlashSteps < backlashAmountSteps) { inBacklash = true; backlashSteps++; } else { inBacklash = false; motorSteps++; }
  } else { direction = DIR_NONE; return; }
}

#endif
