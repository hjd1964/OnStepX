// -----------------------------------------------------------------------------------
// Axis motion control
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;

#include "../telescope/Telescope.h"
extern Telescope telescope;
#include "Axis.h"

extern unsigned long periodSubMicros;

#if AXIS1_DRIVER_MODEL != OFF
  const AxisPins     Axis1StepPins = {AXIS1_STEP_PIN, AXIS1_DIR_PIN, AXIS1_ENABLE_PIN, false, false, true};
  const AxisSettings Axis1Settings = {AXIS1_STEPS_PER_DEGREE*RAD_DEG_RATIO, AXIS1_DRIVER_REVERSE};
  IRAM_ATTR void moveAxis1() { telescope.mount.axis1.move(AXIS1_STEP_PIN, AXIS1_DIR_PIN); }
  inline void moveForwardFastAxis1() { telescope.mount.axis1.moveForwardFast(AXIS1_STEP_PIN, AXIS1_DIR_PIN); }
  inline void moveReverseFastAxis1() { telescope.mount.axis1.moveReverseFast(AXIS1_STEP_PIN, AXIS1_DIR_PIN); }
#endif
#if AXIS2_DRIVER_MODEL != OFF
  const AxisPins     Axis2StepPins = {AXIS2_STEP_PIN, AXIS2_DIR_PIN, AXIS2_ENABLE_PIN, false, false, true};
  const AxisSettings Axis2Settings = {AXIS2_STEPS_PER_DEGREE*RAD_DEG_RATIO, AXIS2_DRIVER_REVERSE};
  IRAM_ATTR void moveAxis2() { telescope.mount.axis2.move(AXIS2_STEP_PIN, AXIS2_DIR_PIN); }
  inline void moveForwardFastAxis2() { telescope.mount.axis2.moveForwardFast(AXIS2_STEP_PIN, AXIS2_DIR_PIN); }
  inline void moveReverseFastAxis2() { telescope.mount.axis2.moveReverseFast(AXIS2_STEP_PIN, AXIS2_DIR_PIN); }
#endif
#if AXIS3_DRIVER_MODEL != OFF
  const AxisPins     Axis3StepPins = {AXIS3_STEP_PIN, AXIS3_DIR_PIN, AXIS3_ENABLE_PIN, false, false, true};
  const AxisSettings Axis3Settings = {AXIS3_STEPS_PER_DEGREE*RAD_DEG_RATIO, AXIS3_DRIVER_REVERSE};
  inline void moveAxis3() { telescope.rotator.axis.move(AXIS3_STEP_PIN, AXIS3_DIR_PIN); }
#endif
#if AXIS4_DRIVER_MODEL != OFF
  const AxisPins     Axis4StepPins = {AXIS4_STEP_PIN, AXIS4_DIR_PIN, AXIS4_ENABLE_PIN, false, false, true};
  const AxisSettings Axis4Settings = {AXIS4_STEPS_PER_DEGREE*RAD_DEG_RATIO, AXIS4_DRIVER_REVERSE};
  inline void moveAxis4() { telescope.focuser1.axis.move(AXIS4_STEP_PIN, AXIS4_DIR_PIN); }
#endif
#if AXIS5_DRIVER_MODEL != OFF
  const AxisPins     Axis5StepPins = {AXIS5_STEP_PIN, AXIS5_DIR_PIN, AXIS5_ENABLE_PIN, false, false, true};
  const AxisSettings Axis5Settings = {AXIS5_STEPS_PER_DEGREE*RAD_DEG_RATIO, AXIS5_DRIVER_REVERSE};
  inline void moveAxis5() { telescope.focuser2.axis.move(AXIS2_STEP_PIN, AXIS5_DIR_PIN); }
#endif
#if AXIS6_DRIVER_MODEL != OFF
  const AxisPins     Axis6StepPins = {AXIS6_STEP_PIN, AXIS6_DIR_PIN, AXIS6_ENABLE_PIN, false, false, true};
  const AxisSettings Axis6Settings = {AXIS6_STEPS_PER_DEGREE*RAD_DEG_RATIO, AXIS6_DRIVER_REVERSE};
  inline void moveAxis6() { telescope.focuser3.axis.move(AXIS6_STEP_PIN, AXIS6_DIR_PIN); }
#endif

void Axis::init(uint8_t axisNumber) {
  this->axisNumber = axisNumber;
  VF("MSG: Axis::init, axis"); VL(axisNumber);
  driver.init(axisNumber);

  taskHandle = 0;
  #if AXIS1_DRIVER_MODEL != OFF
    if (axisNumber == 1) {
      pins = Axis1StepPins; settings = Axis1Settings;
      taskHandle = tasks.add(0, 0, true, 0, moveAxis1, "Axis1");
      if (!tasks.requestHardwareTimer(taskHandle, 1, 0)) VLF("MSG: Warning, didn't get h/w timer for Axis1 (using s/w timer)");
    }
  #endif
  #if AXIS1_DRIVER_MODEL != OFF
    if (axisNumber == 2) {
      pins = Axis2StepPins; settings = Axis2Settings;
      taskHandle = tasks.add(0, 0, true, 0, moveAxis2, "Axis2");
      if (!tasks.requestHardwareTimer(taskHandle, 2, 0)) VLF("MSG: Warning, didn't get h/w timer for Axis2 (using s/w timer)");
    }
  #endif
  #if AXIS3_DRIVER_MODEL != OFF
    pins = Axis3StepPins; settings = Axis3Settings;
    if (axisNumber == 3) handle = tasks.add(0, 0, true, 0, moveAxis3, "Axis3");
  #endif
  #if AXIS4_DRIVER_MODEL != OFF
    pins = Axis4StepPins; settings = Axis4Settings;
    if (axisNumber == 4) handle = tasks.add(0, 0, true, 0, moveAxis4, "Axis4");
  #endif
  #if AXIS5_DRIVER_MODEL != OFF
    pins = Axis5StepPins; settings = Axis5Settings;
    if (axisNumber == 5) handle = tasks.add(0, 0, true, 0, moveAxis5, "Axis5");
  #endif
  #if AXIS6_DRIVER_MODEL != OFF
    pins = Axis6StepPins; settings = Axis6Settings;
    if (axisNumber == 6) handle = tasks.add(0, 0, true, 0, moveAxis6, "Axis6");
  #endif

  spm = settings.stepsPerMeasure;
  if (settings.reverse) invertDir = !invertDir;

  pinModeInitEx(pins.step, OUTPUT, !invertStep?LOW:HIGH);
  pinModeInitEx(pins.dir, OUTPUT, !invertDir?LOW:HIGH);
  pinModeEx(pins.enable, OUTPUT); enable(false);
}

void Axis::enable(bool value) {
  if (pins.enable != OFF) {
    if (value) digitalWrite(pins.enable, invertEnabled?HIGH:LOW); else digitalWrite(pins.enable, invertEnabled?LOW:HIGH);
    enabled = value;
  }
}

bool Axis::isEnabled() {
  return enabled;
}

double Axis::getStepsPerMeasure() {
  return spm;
}

int Axis::getStepsPerStepGoto() {
  return stepGoto;
}

void Axis::setMotorCoordinate(double value) {
  long steps = lround(value*spm);
  setMotorCoordinateSteps(steps);
}

void Axis::setMotorCoordinateSteps(long value) {
  indexSteps    = 0;
  noInterrupts();
  motorSteps    = value;
  targetSteps   = value;
  backlashSteps = 0;
  interrupts();
}

long Axis::getMotorCoordinateSteps() {
  noInterrupts();
  long steps = motorSteps + backlashSteps;
  interrupts();
  return steps;
}

long Axis::getInstrumentCoordinateSteps() {
  noInterrupts();
  long steps = motorSteps + indexSteps;
  interrupts();
  return steps;
}

void Axis::setInstrumentCoordinate(double value) {
  long steps = value*spm;
  noInterrupts();
  indexSteps = steps - motorSteps;
  interrupts();
}

double Axis::getInstrumentCoordinate() {
  noInterrupts();
  long steps = motorSteps + indexSteps;
  interrupts();
  return steps / spm;
}

void Axis::setOriginCoordinate() {
  noInterrupts();
  originSteps = motorSteps;
  interrupts();
}

void Axis::setTargetCoordinate(double value) {
  target = value*spm;
  long steps = lround(target);
  noInterrupts();
  targetSteps = steps - indexSteps;
  interrupts();
}

double Axis::getTargetCoordinate() {
  noInterrupts();
  long steps = targetSteps + indexSteps;
  interrupts();
  return steps/spm;
}

void Axis::moveTargetCoordinate(double value) {
  target += value*spm;
  long steps = lround(target);
  noInterrupts();
  targetSteps = steps - indexSteps;
  interrupts();
}

bool Axis::nearTarget() {
  return fabs(motorSteps - targetSteps) <= step * 2.0;
}

double Axis::getOriginOrTargetDistance() {
  noInterrupts();
  long steps = motorSteps;
  interrupts();
  long distanceOrigin = labs(originSteps - steps);
  long distanceTarget = labs(targetSteps - steps);
  if (distanceOrigin < distanceTarget) return distanceOrigin/spm; else return distanceTarget/spm;
}

void Axis::setFrequencyMax(double frequency) {
  maxFreq = frequency*spm;
  if (frequency != 0.0) minPeriodMicros = 1000000.0/maxFreq; else minPeriodMicros = 0.0;
}

void Axis::setFrequency(double frequency) {
  // frequency in measures per second to microsecond counts per step
  double d = 1000000.0/(frequency*spm);
  if (d < minPeriodMicros) d = minPeriodMicros;
  d /= 2.0;  // we need to run twice as fast to make a square wave
  if (!isnan(d) && fabs(d) <= 134000000) {
    d *= 16.0; // convert microsecond counts into sub-microsecond counts
    lastPeriod = (unsigned long)lround(d);
    d *= (SIDEREAL_PERIOD/periodSubMicros); // adjust period for MCU clock inaccuracy
  } else { d = 0; lastPeriod = 0; }
  tasks.setPeriodSubMicros(taskHandle, (unsigned long)lround(d));
}

double Axis::getFrequency() {
  return getFrequencySteps()/spm;
}

double Axis::getFrequencySteps() {
  if (lastPeriod == 0) return 0;
  return 16000000.0/(lastPeriod * 2UL);
}

void Axis::setTracking(bool state) {
  this->tracking = state;
}

bool Axis::getTracking() {
  return tracking;
}

void Axis::setBacklash(double value) {
  noInterrupts();
  backlashAmountSteps = value * spm;
  interrupts();
}

double Axis::getBacklash() {
  noInterrupts();
  long b = backlashSteps;
  interrupts();
  return b/spm;
}

bool Axis::inBacklash() {
  noInterrupts();
  long b = backlashSteps;
  interrupts();
  if (b == 0 || b == backlashAmountSteps) return false; else return true;
}

void Axis::disableBacklash() {
  noInterrupts();
  backlashStepsStore = backlashSteps;
  motorSteps += backlashSteps;
  backlashSteps = 0;
  interrupts();
}

void Axis::enableBacklash() {
  noInterrupts();
  backlashSteps = backlashStepsStore;
  motorSteps -= backlashSteps;
  interrupts();
  backlashStepsStore = 0;
}

void Axis::enableMoveFast(bool fast) {

  if (fast) disableBacklash(); else enableBacklash();

  // make sure the direction is set
  noInterrupts();
  if (motorSteps < targetSteps) {
    direction = DIR_FORWARD;
    digitalWriteF(pins.dir, invertDir?HIGH:LOW);
  } else if (motorSteps > targetSteps) {
    direction = DIR_REVERSE;
    digitalWriteF(pins.dir, invertDir?LOW:HIGH);
  } else direction = DIR_NONE;
  interrupts();
  
  if (direction == DIR_NONE) return;

  #if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF
    if (axisNumber == 1) {
      if (!fast) tasks.setCallback(taskHandle, moveAxis1); else {
        if (direction == DIR_FORWARD) tasks.setCallback(taskHandle, moveForwardFastAxis1); else tasks.setCallback(taskHandle, moveReverseFastAxis1);
      } 
    }
    if (axisNumber == 2) {
      if (!fast) tasks.setCallback(taskHandle, moveAxis2); else {
        if (direction == DIR_FORWARD) tasks.setCallback(taskHandle, moveForwardFastAxis2); else tasks.setCallback(taskHandle, moveReverseFastAxis2);
      }
    }
  #endif
}

IRAM_ATTR void Axis::move(const int8_t stepPin, const int8_t dirPin) {
  if (takeStep) {
    if (direction == DIR_FORWARD) {
      if (backlashSteps < backlashAmountSteps) backlashSteps += step; else motorSteps += step;
      digitalWriteF(stepPin, HIGH);
    } else
    if (direction == DIR_REVERSE) {
      if (backlashSteps > 0) backlashSteps -= step; else motorSteps -= step;
      digitalWriteF(stepPin, HIGH);
    }
  } else {
    if (tracking) targetSteps += trackingStep;
    if (motorSteps + backlashSteps < targetSteps) {
      direction = DIR_FORWARD;
      digitalWriteF(dirPin, LOW);
    } else if (motorSteps + backlashSteps > targetSteps) {
      direction = DIR_REVERSE;
      digitalWriteF(dirPin, HIGH);
    } else direction = DIR_NONE;
    if (microstepModeControl == MMC_SLEWING_READY) microstepModeControl = MMC_SLEWING;
    digitalWriteF(stepPin, LOW);
  }
  takeStep = !takeStep;
}

#if MODE_SWITCH == ON
  #if STEP_WAVE_FORM == SQUARE
  IRAM_ATTR void Axis::moveForwardFast(const int8_t stepPin, const int8_t dirPin) {
    if (takeStep) {
      if (motorSteps < targetSteps) { motorSteps += step; digitalWriteF(stepPin, HIGH); }
    } else digitalWriteF(stepPin, LOW);
    takeStep = !takeStep;
  }
  IRAM_ATTR void Axis::moveReverseFast(const int8_t stepPin, const int8_t dirPin) {
    if (takeStep) {
      if (motorSteps > targetSteps) { motorSteps -= step; digitalWriteF(stepPin, HIGH); }
    } else digitalWriteF(stepPin, LOW);
    takeStep = !takeStep;
  }
  #endif
  #if STEP_WAVE_FORM == PULSE
  IRAM_ATTR void Axis::moveForwardFast(const int8_t stepPin, const int8_t dirPin) {
    digitalWriteF(stepPin, LOW);
    if (motorSteps < targetSteps) { motorSteps += step; digitalWriteF(stepPin, HIGH); }
  }
  IRAM_ATTR void Axis::moveReverseFast(const int8_t stepPin, const int8_t dirPin) {
    digitalWriteF(stepPin, LOW);
    if (motorSteps > targetSteps) { motorSteps -= step; digitalWriteF(stepPin, HIGH); }
  }
  #endif
#else
  #if STEP_WAVE_FORM == SQUARE
    IRAM_ATTR void Axis::moveForwardFast(const int8_t stepPin, const int8_t dirPin) {
      if (takeStep) {
        if (motorSteps < targetSteps) { motorSteps++; digitalWriteF(stepPin, HIGH); }
      } else digitalWriteF(stepPin, LOW);
      takeStep = !takeStep;
    }
    IRAM_ATTR void Axis::moveReverseFast(const int8_t stepPin, const int8_t dirPin) {
      if (takeStep) {
        if (motorSteps > targetSteps) { motorSteps--; digitalWriteF(stepPin, HIGH); }
      } else digitalWriteF(stepPin, LOW);
      takeStep = !takeStep;
    }
  #endif
  #if STEP_WAVE_FORM == PULSE
    IRAM_ATTR void Axis::moveForwardFast(const int8_t stepPin, const int8_t dirPin) {
      digitalWriteF(stepPin, LOW);
      if (motorSteps < targetSteps) { motorSteps++; digitalWriteF(stepPin, HIGH); }
    }
    IRAM_ATTR void Axis::moveReverseFast(const int8_t stepPin, const int8_t dirPin) {
      digitalWriteF(stepPin, LOW);
      if (motorSteps > targetSteps) { motorSteps--; digitalWriteF(stepPin, HIGH); }
    }
  #endif
#endif
