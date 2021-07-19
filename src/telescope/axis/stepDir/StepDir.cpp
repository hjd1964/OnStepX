// -----------------------------------------------------------------------------------
// Axis step/dir driver motion

#include "StepDir.h"
#include "../../../tasks/OnTask.h"
extern Tasks tasks;
#include "../../Telescope.h"
#include "../../../lib/sense/Sense.h"

#ifdef MOUNT_PRESENT
  extern unsigned long periodSubMicros;
#else
  #define periodSubMicros 1.0
#endif

#if AXIS1_DRIVER_MODEL != OFF
  const StepDirPins Axis1Pins = { AXIS1_STEP_PIN, AXIS1_DRIVER_STEP, AXIS1_DIR_PIN, AXIS1_ENABLE_PIN, AXIS1_DRIVER_ENABLE };
  IRAM_ATTR void moveAxis1() { telescope.mount.axis1.motor.move(AXIS1_STEP_PIN, AXIS1_DIR_PIN); }
  IRAM_ATTR void moveFFAxis1() { telescope.mount.axis1.motor.moveFF(AXIS1_STEP_PIN); }
  IRAM_ATTR void moveFRAxis1() { telescope.mount.axis1.motor.moveFR(AXIS1_STEP_PIN); }
#endif
#if AXIS2_DRIVER_MODEL != OFF
  const StepDirPins Axis2Pins = { AXIS2_STEP_PIN, AXIS2_DRIVER_STEP, AXIS2_DIR_PIN, AXIS2_ENABLE_PIN, AXIS2_DRIVER_ENABLE };
  IRAM_ATTR void moveAxis2() { telescope.mount.axis2.motor.move(AXIS2_STEP_PIN, AXIS2_DIR_PIN); }
  IRAM_ATTR void moveFFAxis2() { telescope.mount.axis2.motor.moveFF(AXIS2_STEP_PIN); }
  IRAM_ATTR void moveFRAxis2() { telescope.mount.axis2.motor.moveFR(AXIS2_STEP_PIN); }
#endif
#if AXIS3_DRIVER_MODEL != OFF
  const StepDirPins Axis3Pins = { AXIS3_STEP_PIN, AXIS3_DRIVER_STEP, AXIS3_DIR_PIN, AXIS3_ENABLE_PIN, AXIS3_DRIVER_ENABLE };
  inline void moveAxis3() { telescope.rotator.axis.motor.move(AXIS3_STEP_PIN, AXIS3_DIR_PIN); }
#endif
#if AXIS4_DRIVER_MODEL != OFF
  const StepDirPins Axis4Pins = { AXIS4_STEP_PIN, AXIS4_DRIVER_STEP, AXIS4_DIR_PIN, AXIS4_ENABLE_PIN, AXIS4_DRIVER_ENABLE };
  inline void moveAxis4() { telescope.focuser.axis[0]->motor.move(AXIS4_STEP_PIN, AXIS4_DIR_PIN); }
#endif
#if AXIS5_DRIVER_MODEL != OFF
  const StepDirPins Axis5Pins = { AXIS5_STEP_PIN, AXIS5_DRIVER_STEP, AXIS5_DIR_PIN, AXIS5_ENABLE_PIN, AXIS5_DRIVER_ENABLE };
  inline void moveAxis5() { telescope.focuser.axis[1]->motor.move(AXIS5_STEP_PIN, AXIS5_DIR_PIN); }
#endif
#if AXIS6_DRIVER_MODEL != OFF
  const StepDirPins Axis6Pins = { AXIS6_STEP_PIN, AXIS6_DRIVER_STEP, AXIS6_DIR_PIN, AXIS6_ENABLE_PIN, AXIS6_DRIVER_ENABLE };
  inline void moveAxis6() { telescope.focuser.axis[2]->motor.move(AXIS6_STEP_PIN, AXIS6_DIR_PIN); }
#endif
#if AXIS7_DRIVER_MODEL != OFF
  const StepDirPins Axis7Pins = { AXIS7_STEP_PIN, AXIS7_DRIVER_STEP, AXIS7_DIR_PIN, AXIS7_ENABLE_PIN, AXIS7_DRIVER_ENABLE };
  inline void moveAxis7() { telescope.focuser.axis[3]->motor.move(AXIS7_STEP_PIN, AXIS7_DIR_PIN); }
#endif
#if AXIS8_DRIVER_MODEL != OFF
  const StepDirPins Axis8Pins = { AXIS8_STEP_PIN, AXIS8_DRIVER_STEP, AXIS8_DIR_PIN, AXIS8_ENABLE_PIN, AXIS8_DRIVER_ENABLE };
  inline void moveAxis8() { telescope.focuser.axis[4]->motor.move(AXIS8_STEP_PIN, AXIS8_DIR_PIN); }
#endif
#if AXIS9_DRIVER_MODEL != OFF
  const StepDirPins Axis9Pins = { AXIS9_STEP_PIN, AXIS9_DRIVER_STEP, AXIS9_DIR_PIN, AXIS9_ENABLE_PIN, AXIS9_DRIVER_ENABLE };
  inline void moveAxis9() { telescope.focuser.axis[5]->motor.move(AXIS9_STEP_PIN, AXIS9_DIR_PIN); }
#endif

bool StepDir::init(uint8_t axisNumber, int8_t reverse, int16_t microsteps, int16_t currentRun) {
  axisPrefix[10] = '0' + axisNumber;
  this->axisNumber = axisNumber;
  
  taskHandle = 0;
  #if AXIS1_DRIVER_MODEL != OFF
    if (axisNumber == 1) { pins = Axis1Pins; _move = moveAxis1; _moveFF = moveFFAxis1; _moveFR = moveFRAxis1; }
  #endif
  #if AXIS1_DRIVER_MODEL != OFF
    if (axisNumber == 2) { pins = Axis2Pins; _move = moveAxis2; _moveFF = moveFFAxis2; _moveFR = moveFRAxis2; }
  #endif
  #if AXIS3_DRIVER_MODEL != OFF
    if (axisNumber == 3) { pins = Axis3Pins; _move = moveAxis3; }
  #endif
  #if AXIS4_DRIVER_MODEL != OFF
    if (axisNumber == 4) { pins = Axis4Pins; _move = moveAxis4; }
  #endif
  #if AXIS5_DRIVER_MODEL != OFF
    if (axisNumber == 5) { pins = Axis5Pins; _move = moveAxis5; }
  #endif
  #if AXIS6_DRIVER_MODEL != OFF
    if (axisNumber == 6) { pins = Axis6Pins; _move = moveAxis6; }
  #endif
  #if AXIS7_DRIVER_MODEL != OFF
    if (axisNumber == 7) { pins = Axis7Pins; _move = moveAxis7; }
  #endif
  #if AXIS8_DRIVER_MODEL != OFF
    if (axisNumber == 8) { pins = Axis8Pins; _move = moveAxis8; }
  #endif
  #if AXIS9_DRIVER_MODEL != OFF
    if (axisNumber == 9) { pins = Axis9Pins; _move = moveAxis9; }
  #endif

  // make sure there is something to do
  if (_move == NULL) { V(axisPrefix); VF("nothing to do exiting!"); return false; }

  V(axisPrefix); D("init step="); D(pins.step); D(", dir="); D(pins.dir); D(", en="); DL(pins.enable);

  // init default driver enable state (disabled)
  pinModeEx(pins.enable, OUTPUT);
  power(false);

  // init default driver direction state (forward)
  if (!reverse) { dirFwd = LOW; dirRev = HIGH; } else { dirFwd = HIGH; dirRev = LOW; }
  pinMode(pins.dir, OUTPUT);
  digitalWriteF(pins.dir, dirFwd);

  // init default driver step state (clear)
  #ifndef DRIVER_STEP_DEFAULTS
    stepSet = pins.stepState;
    stepClr = !stepSet;
  #endif
  pinMode(pins.step, OUTPUT);
  digitalWriteF(pins.step, stepClr);

  // init driver advanced modes, etc.
  stepDriver.init(axisNumber);
  stepDriver.setCurrent(currentRun);

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

void StepDir::power(bool state) {
  if (pins.enable != OFF && pins.enable != SHARED_PIN) {
    digitalWriteEx(pins.enable, state?pins.enabledState:!pins.enabledState);
  } else {
    stepDriver.power(state);
  }
}

void StepDir::setMotorCoordinateSteps(long value) {
  indexSteps    = 0;
  noInterrupts();
  motorSteps    = value;
  targetSteps   = value;
  backlashSteps = 0;
  interrupts();
}

long StepDir::getMotorCoordinateSteps() {
  noInterrupts();
  long steps = motorSteps + backlashSteps;
  interrupts();
  return steps;
}

long StepDir::getInstrumentCoordinateSteps() {
  noInterrupts();
  long steps = motorSteps + indexSteps;
  interrupts();
  return steps;
}

void StepDir::setInstrumentCoordinateSteps(long value) {
  noInterrupts();
  indexSteps = value - motorSteps;
  interrupts();
}

// mark origin coordinate for autoSlewRateByDistance as current location
void StepDir::setOriginCoordinateSteps() {
  noInterrupts();
  originSteps = motorSteps;
  interrupts();
}

void StepDir::setTargetCoordinateSteps(long value) {
  noInterrupts();
  targetSteps = value - indexSteps;
  interrupts();
}

long StepDir::getTargetCoordinateSteps() {
  noInterrupts();
  long steps = targetSteps + indexSteps;
  interrupts();
  return steps;
}

// distance to target in steps (+/-)
long StepDir::getTargetDistanceSteps() {
  noInterrupts();
  long steps = motorSteps;
  interrupts();
  return targetSteps - steps;
}

// set target park coordinate, in steps (taking into account stepper motor cogging when powered off)
void StepDir::setTargetCoordinateParkSteps(long value, int modulo) {
  long steps = value - indexSteps;
  steps -= modulo*2L;
  for (int l = 0; l < modulo*4; l++) { if (steps % modulo*4L == 0) break; steps++; }
  noInterrupts();
  targetSteps = steps;
  interrupts();
}

// distance to origin or target, whichever is closer, in steps
long StepDir::getOriginOrTargetDistanceSteps() {
  noInterrupts();
  long steps = motorSteps;
  interrupts();
  long distanceOrigin = labs(originSteps - steps);
  long distanceTarget = labs(targetSteps - steps);
  if (distanceOrigin < distanceTarget) return distanceOrigin; else return distanceTarget;
}

// set backlash frequency in steps per second
void StepDir::setBacklashFrequencySteps(float frequency) {
  backlashFrequency = frequency;
}

void StepDir::setBacklashSteps(long value) {
  noInterrupts();
  backlashAmountSteps = value;
  interrupts();
}

long StepDir::getBacklashSteps() {
  noInterrupts();
  uint16_t backlash = backlashSteps;
  interrupts();
  return backlash;
}

void StepDir::disableBacklash() {
  noInterrupts();
  backlashStepsStore = backlashSteps;
  motorSteps += backlashSteps;
  backlashSteps = 0;
  interrupts();
}

void StepDir::enableBacklash() {
  noInterrupts();
  backlashSteps = backlashStepsStore;
  motorSteps -= backlashSteps;
  interrupts();
  backlashStepsStore = 0;
}

// returns true if within the backlash travel
bool StepDir::inBacklash() {
  noInterrupts();
  uint16_t backlash = backlashSteps;
  interrupts();
  if (backlash <= 0 || backlash >= backlashAmountSteps) return false; else return true;
}

// get the current direction of motion
Direction StepDir::getDirection() {
  if (lastPeriod == 0) return DIR_NONE;
  return direction;
}

// get the associated stepper drivers status
DriverStatus StepDir::getDriverStatus() {
  return stepDriver.getStatus();
}

// set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
void StepDir::setFrequencySteps(float frequency) {

  // if slewing has a larger step size adjust the frequency to account for it
  if (microstepModeControl == MMC_SLEWING) {
    frequency /= slewStep;
  } else {
    // if in backlash override the frequency to account for it
    if (inBacklash()) {
      if (frequency >= 0.0) frequency = backlashFrequency; else frequency = -backlashFrequency;
    }
  }

  // negative frequency, convert to positive and reverse the direction
  if (frequency < 0.0F) {
    frequency = -frequency;
    noInterrupts();
    trackingStep = -1;
    interrupts();
  } else {
    noInterrupts();
    trackingStep =  1;
    interrupts();
  }

  // frequency in steps per second to period in microsecond counts per step
  // also runs the timer twice as fast if using a square wave
  lastFrequency = frequency;
  #if STEP_WAVE_FORM == SQUARE
    float period = 500000.0F/frequency;
  #else
    float period = 1000000.0F/frequency;
  #endif
  if (period < minPeriodMicros) period = minPeriodMicros;

  if (!isnan(period) && fabs(period) <= 134000000.0F) {
    // convert microsecond counts to sub-microsecond counts
    period *= 16.0F;

    // adjust period for MCU clock inaccuracy
    period *= (SIDEREAL_PERIOD/periodSubMicros);

    // if this is the active period, just return
    if (lastPeriod == (unsigned long)lroundf(period)) return;
    lastPeriod = (unsigned long)lroundf(period);
  } else {
    lastPeriod = 0;
    noInterrupts();
    trackingStep = 0;
    interrupts();
  }
  tasks.setPeriodSubMicros(taskHandle, lastPeriod);
}

float StepDir::getFrequencySteps() {
  if (lastPeriod == 0) return 0;
  #if STEP_WAVE_FORM == SQUARE
    return 8000000.0F/lastPeriod;
  #else
    return 16000000.0F/lastPeriod;
  #endif
}

// set synchronized state (automatic movement of target at setFrequencySteps() rate)
void StepDir::setSynchronized(bool state) {
  tracking = state;
}

// get synchronized state (automatic movement of target at setFrequencySteps() rate)
bool StepDir::getSynchronized() {
  return tracking;
}

// set slewing state (hint that we are about to slew or are done slewing)
void StepDir::setSlewing(bool state) {
  if (state == true) stepDriver.modeDecaySlewing(); else stepDriver.modeDecayTracking();
}

// monitor movement
void StepDir::poll() {
  stepDriver.updateStatus();
  if (microstepModeControl == MMC_SLEWING) {
    if (lastFrequency <= backlashFrequency*1.2F) {
      if (stepDriver.modeSwitchAllowed()) {
        V(axisPrefix); VLF("mode switch tracking set");
        stepDriver.modeMicrostepTracking();
      }
      microstepModeControl = MMC_TRACKING;
      if (enableMoveFast(false)) {
        V(axisPrefix); VF("high speed ISR swapped out at "); V(radToDegF(lastFrequency)); VL(" steps/sec.");
      }
    }
  } else {
    if (lastFrequency > backlashFrequency*1.2F) {
      if (stepDriver.modeSwitchAllowed()) {
        if (microstepModeControl == MMC_TRACKING) {
          microstepModeControl = MMC_SLEWING_REQUEST;
          switchStep = stepDriver.getMicrostepRatio();
          V(axisPrefix); VLF("mode switch slewing requested");
          return;
        } else
        if (microstepModeControl != MMC_SLEWING_READY) return;
        V(axisPrefix); VLF("mode switch slewing set");
        slewStep = stepDriver.modeMicrostepSlewing();
      }
      microstepModeControl = MMC_SLEWING;
      if (enableMoveFast(true)) {
        V(axisPrefix); VF("high speed ISR swapped in at "); V(radToDegF(lastFrequency)); VL(" steps/sec.");
      }
    }
  }
}

// swaps in/out fast unidirectional ISR for slewing 
bool StepDir::enableMoveFast(const bool fast) {
  #ifdef MOUNT_PRESENT
    if (axisNumber > 2) return false;
    if (fast) {
      disableBacklash();
      if (direction == DIR_FORWARD) tasks.setCallback(taskHandle, _moveFF); else tasks.setCallback(taskHandle, _moveFR);
    } else {
      tasks.setCallback(taskHandle, _move);
      enableBacklash();
    }
    return true;
  #else
    return false;
  #endif
}

#if STEP_WAVE_FORM == SQUARE
  IRAM_ATTR void StepDir::move(const int8_t stepPin, const int8_t dirPin) {
    if (microstepModeControl == MMC_SLEWING_READY) return;
    if (takeStep) {
      if (direction == DIR_FORWARD) {
        if (backlashSteps < backlashAmountSteps) backlashSteps += step; else motorSteps += step;
        digitalWriteF(stepPin, stepSet);
      } else
      if (direction == DIR_REVERSE) {
        if (backlashSteps > 0) backlashSteps -= step; else motorSteps -= step;
        digitalWriteF(stepPin, stepSet);
      }
    } else {
      if (tracking) targetSteps += trackingStep;
      if (motorSteps + backlashSteps < targetSteps) {
        direction = DIR_FORWARD;
        digitalWriteF(dirPin, dirFwd);
      } else if (motorSteps + backlashSteps > targetSteps) {
        direction = DIR_REVERSE;
        digitalWriteF(dirPin, dirRev);
      } else direction = DIR_NONE;
      digitalWriteF(stepPin, stepClr);
      if (microstepModeControl == MMC_SLEWING_REQUEST && (motorSteps + backlashSteps)%switchStep == 0) microstepModeControl = MMC_SLEWING_READY;
    }
    takeStep = !takeStep;
  }
  IRAM_ATTR void StepDir::moveFF(const int8_t stepPin) {
    if (takeStep) {
      if (tracking) targetSteps += slewStep;
      if (motorSteps < targetSteps) { motorSteps += slewStep; digitalWriteF(stepPin, stepSet); }
    } else digitalWriteF(stepPin, stepClr);
    takeStep = !takeStep;
  }
  IRAM_ATTR void StepDir::moveFR(const int8_t stepPin) {
    if (takeStep) {
      if (tracking) targetSteps -= slewStep;
      if (motorSteps > targetSteps) { motorSteps -= slewStep; digitalWriteF(stepPin, stepSet); }
    } else digitalWriteF(stepPin, stepClr);
    takeStep = !takeStep;
  }
#else
  IRAM_ATTR void StepDir::move(const int8_t stepPin, const int8_t dirPin) {
    if (microstepModeControl == MMC_SLEWING_READY) return;
    digitalWriteF(stepPin, stepClr);
    if (tracking) targetSteps += trackingStep;
    if (motorSteps < targetSteps) {
      digitalWriteF(dirPin, dirFwd);
      direction = DIR_FORWARD;
      if (backlashSteps < backlashAmountSteps) backlashSteps += step; else motorSteps += step;
    } else
    if (motorSteps > targetSteps) {
      digitalWriteF(dirPin, dirFwd);
      direction = DIR_REVERSE;
      if (backlashSteps > 0) backlashSteps -= step; else motorSteps -= step;
    } else { direction = DIR_NONE; return; }
    digitalWriteF(stepPin, dirRev);
    if (microstepModeControl == MMC_SLEWING_REQUEST && (motorSteps + backlashSteps)%switchStep == 0) microstepModeControl = MMC_SLEWING_READY;
  }
  IRAM_ATTR void StepDir::moveFF(const int8_t stepPin) {
    digitalWriteF(stepPin, stepClr);
    if (tracking) targetSteps += slewStep;
    if (motorSteps < targetSteps) { motorSteps += slewStep; digitalWriteF(stepPin, stepSet); }
  }
  IRAM_ATTR void StepDir::moveFR(const int8_t stepPin) {
    digitalWriteF(stepPin, stepClr);
    if (tracking) targetSteps -= slewStep;
    if (motorSteps > targetSteps) { motorSteps -= slewStep; digitalWriteF(stepPin, stepSet); }
  }
#endif
