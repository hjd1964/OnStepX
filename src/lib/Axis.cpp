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
#include "Clock.h"
extern Clock clock;
#include "Axis.h"

#if AXIS1_DRIVER_MODEL != OFF
  const AxisPins       Axis1DriverStepPins     = {AXIS1_STEP_PIN,AXIS1_DIR_PIN,AXIS1_ENABLE_PIN,false,false,true};
  const DriverPins     Axis1DriverModePins     = {AXIS1_M0_PIN,AXIS1_M1_PIN,AXIS1_M2_PIN,AXIS1_M3_PIN,AXIS1_DECAY_PIN};
  const DriverSettings Axis1DriverModeSettings = {AXIS1_DRIVER_MODEL,AXIS1_DRIVER_MICROSTEPS,AXIS1_DRIVER_MICROSTEPS_GOTO,AXIS1_DRIVER_IHOLD,AXIS1_DRIVER_IRUN,AXIS1_DRIVER_IGOTO,AXIS1_DRIVER_DECAY,AXIS1_DRIVER_DECAY_GOTO};
  StepDriver axis1Driver{Axis1DriverModePins, Axis1DriverModeSettings};
  Axis       axis1{Axis1DriverStepPins, Axis1DriverModePins, Axis1DriverModeSettings};
  void moveAxis1() { axis1.move(AXIS1_STEP_PIN, AXIS1_DIR_PIN); }
#endif

#if AXIS2_DRIVER_MODEL != OFF
  const AxisPins       Axis2DriverStepPins     = {AXIS2_STEP_PIN,AXIS2_DIR_PIN,AXIS2_ENABLE_PIN,false,false,true};
  const DriverPins     Axis2DriverModePins     = {AXIS2_M0_PIN,AXIS2_M1_PIN,AXIS2_M2_PIN,AXIS2_M3_PIN,AXIS2_DECAY_PIN};
  const DriverSettings Axis2DriverModeSettings = {AXIS2_DRIVER_MODEL,AXIS2_DRIVER_MICROSTEPS,AXIS2_DRIVER_MICROSTEPS_GOTO,AXIS2_DRIVER_IHOLD,AXIS2_DRIVER_IRUN,AXIS2_DRIVER_IGOTO,AXIS2_DRIVER_DECAY,AXIS2_DRIVER_DECAY_GOTO};
  StepDriver axis2Driver{Axis2DriverModePins, Axis2DriverModeSettings};
  Axis       axis2{Axis2DriverStepPins, Axis2DriverModePins, Axis2DriverModeSettings};
  void moveAxis2() { axis2.move(AXIS2_STEP_PIN, AXIS2_DIR_PIN); }
#endif

#if AXIS3_DRIVER_MODEL != OFF
  const AxisPins       Axis3DriverStepPins     = {AXIS3_STEP_PIN,AXIS3_DIR_PIN,AXIS3_ENABLE_PIN,false,false,true};
  const DriverPins     Axis3DriverModePins     = {AXIS3_M0_PIN,AXIS3_M1_PIN,AXIS3_M2_PIN,AXIS3_M3_PIN,AXIS3_DECAY_PIN};
  const DriverSettings Axis3DriverModeSettings = {AXIS3_DRIVER_MODEL,AXIS3_DRIVER_MICROSTEPS,AXIS3_DRIVER_MICROSTEPS_GOTO,AXIS3_DRIVER_IHOLD,AXIS3_DRIVER_IRUN,AXIS3_DRIVER_IGOTO,AXIS3_DRIVER_DECAY,AXIS3_DRIVER_DECAY_GOTO};
  StepDriver axis3Driver{Axis3DriverModePins, Axis3DriverModeSettings};
  Axis       axis3{Axis3DriverStepPins, Axis3DriverModePins, Axis3DriverModeSettings};
  void moveAxis3() { axis3.move(AXIS3_STEP_PIN, AXIS3_DIR_PIN); }
#endif

#if AXIS4_DRIVER_MODEL != OFF
  const AxisPins       Axis4DriverStepPins     = {AXIS4_STEP_PIN,AXIS4_DIR_PIN,AXIS4_ENABLE_PIN,false,false,true};
  const DriverPins     Axis4DriverModePins     = {AXIS4_M0_PIN,AXIS4_M1_PIN,AXIS4_M2_PIN,AXIS4_M3_PIN,AXIS4_DECAY_PIN};
  const DriverSettings Axis4DriverModeSettings = {AXIS4_DRIVER_MODEL,AXIS4_DRIVER_MICROSTEPS,AXIS4_DRIVER_MICROSTEPS_GOTO,AXIS4_DRIVER_IHOLD,AXIS4_DRIVER_IRUN,AXIS4_DRIVER_IGOTO,AXIS4_DRIVER_DECAY,AXIS4_DRIVER_DECAY_GOTO};
  StepDriver axis3Driver{Axis4DriverModePins, Axis4DriverModeSettings};
  Axis       axis4{Axis4DriverStepPins, Axis4DriverModePins, Axis4DriverModeSettings};
  void moveAxis4() { axis4.move(AXIS4_STEP_PIN, AXIS4_DIR_PIN); }
#endif

#if AXIS5_DRIVER_MODEL != OFF
  const AxisPins       Axis5DriverStepPins     = {AXIS5_STEP_PIN,AXIS5_DIR_PIN,AXIS5_ENABLE_PIN,false,false,true};
  const DriverPins     Axis5DriverModePins     = {AXIS5_M0_PIN,AXIS5_M1_PIN,AXIS5_M2_PIN,AXIS5_M3_PIN,AXIS5_DECAY_PIN};
  const DriverSettings Axis5DriverModeSettings = {AXIS5_DRIVER_MODEL,AXIS5_DRIVER_MICROSTEPS,AXIS5_DRIVER_MICROSTEPS_GOTO,AXIS5_DRIVER_IHOLD,AXIS5_DRIVER_IRUN,AXIS5_DRIVER_IGOTO,AXIS5_DRIVER_DECAY,AXIS5_DRIVER_DECAY_GOTO};
  StepDriver axis3Driver{Axis5DriverModePins, Axis5DriverModeSettings};
  Axis       axis5{Axis5DriverStepPins, Axis5DriverModePins, Axis5DriverModeSettings};
  void moveAxis5() { axis5.move(AXIS5_STEP_PIN, AXIS5_DIR_PIN); }
#endif

#if AXIS6_DRIVER_MODEL != OFF
  const AxisPins       Axis6DriverStepPins     = {AXIS6_STEP_PIN,AXIS6_DIR_PIN,AXIS6_ENABLE_PIN,false,false,true};
  const DriverPins     Axis6DriverModePins     = {AXIS6_M0_PIN,AXIS6_M1_PIN,AXIS6_M2_PIN,AXIS6_M3_PIN,AXIS6_DECAY_PIN};
  const DriverSettings Axis6DriverModeSettings = {AXIS6_DRIVER_MODEL,AXIS6_DRIVER_MICROSTEPS,AXIS6_DRIVER_MICROSTEPS_GOTO,AXIS6_DRIVER_IHOLD,AXIS6_DRIVER_IRUN,AXIS6_DRIVER_IGOTO,AXIS6_DRIVER_DECAY,AXIS6_DRIVER_DECAY_GOTO};
  StepDriver axis3Driver{Axis6DriverModePins, Axis6DriverModeSettings};
  Axis       axis5{Axis6DriverStepPins, Axis6DriverModePins, Axis6DriverModeSettings};
  void moveAxis6() { axis6.move(AXIS6_STEP_PIN, AXIS6_DIR_PIN); }
#endif

void Axis::init(uint8_t axisNumber) {
  VF("MSG: Axis::init, axis"); VL(axisNumber);
  pinModeInitEx(Pins.step, OUTPUT, !invertStep?LOW:HIGH);
  pinModeInitEx(Pins.dir, OUTPUT, !invertDir?LOW:HIGH);
  pinModeEx(Pins.enable, OUTPUT); enable(false);

  int handle = 0;
  if (axisNumber == 1) {
    #if AXIS1_DRIVER_MODEL != OFF
      handle = tasks.add(0, 0, true, 0, moveAxis1, "MoveAx1");
      if (!tasks.requestHardwareTimer(handle, 1, 0)) VLF("MSG: Warning, didn't get h/w timer for Axis1 (using s/w timer)");
    #endif
  }
  if (axisNumber == 2) {
    #if AXIS2_DRIVER_MODEL != OFF
      handle = tasks.add(0, 0, true, 0, moveAxis2, "MoveAx2");
      if (!tasks.requestHardwareTimer(handle, 2, 0)) VLF("MSG: Warning, didn't get h/w timer for Axis2 (using s/w timer)");
    #endif
  }
  if (axisNumber == 3) {
    #if AXIS3_DRIVER_MODEL != OFF
      tasks.add(0, 0, true, 0, moveAxis3, "MoveAx3");
    #endif
  }
  if (axisNumber == 4) {
    #if AXIS4_DRIVER_MODEL != OFF
      handle = tasks.add(0, 0, true, 0, moveAxis4, "MoveAx4");
    #endif
  }
  if (axisNumber == 5) {
    #if AXIS5_DRIVER_MODEL != OFF
      handle = tasks.add(0, 0, true, 0, moveAxis5, "MoveAx5");
    #endif
  }
  if (axisNumber == 6) {
    #if AXIS6_DRIVER_MODEL != OFF
      handle = tasks.add(0, 0, true, 0, moveAxis6, "MoveAx6");
    #endif
  }
  task_handle = handle;
  stepDriver.init();
}

void Axis::enable(bool value) {
  if (Pins.enable != OFF) {
    if (value) digitalWrite(Pins.enable, invertEnabled?HIGH:LOW); else digitalWrite(Pins.enable, invertEnabled?LOW:HIGH);
    enabled = value;
  }
}

bool Axis::isEnabled() {
  return enabled;
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

void Axis::markOriginCoordinate() {
  originSteps = getInstrumentCoordinateSteps();
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
  return fabs((motorSteps + backlashSteps) - targetSteps) <= step * 2.0;
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
  if (isnan(d) || fabs(d) > 134000000) { tasks.setPeriod(task_handle, 0); return; }
  d *= 16.0; // convert microsecond counts into sub-microsecond counts
  d *= (16000000.0/clock.getPeriodSubMicros()); // adjust period for MCU clock inaccuracy
  tasks.setPeriodSubMicros(task_handle, (unsigned long)lround(d));
}

void Axis::setTracking(bool tracking) {
  this->tracking = tracking;
}

bool Axis::getTracking() {
  return tracking;
}

void Axis::setStepsPerMeasure(double value) {
  spm = value;
}

void Axis::setBacklash(double value) {
  backlashAmountSteps = value * spm;
}

double Axis::getBacklash() {
  return backlashSteps/spm;
}

void Axis::setMinCoordinate(double value) {
  minSteps = value*spm;
}

double Axis::getMinCoordinate() {
  return minSteps/spm;
}

void Axis::setMaxCoordinate(double value) {
  maxSteps = value*spm;
}

double Axis::getMaxCoordinate() {
  return maxSteps/spm;
}

void Axis::move(const int8_t stepPin, const int8_t dirPin) {
  if (takeStep) {
    if (tracking) targetSteps += trackingStep;
    if (motorSteps + backlashSteps > targetSteps) {
      if (backlashSteps > 0) backlashSteps -= step; else motorSteps -= step;
      digitalWriteF(stepPin, invertStep?LOW:HIGH);
    } else {
      if (motorSteps + backlashSteps < targetSteps) {
        if (backlashSteps < backlashAmountSteps) backlashSteps += step; else motorSteps += step;
        digitalWriteF(stepPin, invertStep?LOW:HIGH);
      }
    }
  } else {
    if (motorSteps + backlashSteps > targetSteps) {
      if (!dirFwd) {
        dirFwd = !dirFwd;
        digitalWriteF(dirPin, invertDir?LOW:HIGH);
      }
    } else if (motorSteps + backlashSteps < targetSteps) {
      if (dirFwd) {
        dirFwd = !dirFwd;
        digitalWriteF(dirPin, invertDir?HIGH:LOW);
      }
    }
    if (microstepModeControl == MMC_SLEWING_READY) microstepModeControl = MMC_SLEWING;
    digitalWriteF(stepPin,invertStep?HIGH:LOW);
  }
  takeStep = !takeStep;
}
