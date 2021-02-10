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

#include "Axis.h"

const DriverPins     Axis1DriverModePins     = {AXIS1_M0_PIN,AXIS1_M1_PIN,AXIS1_M2_PIN,AXIS1_M3_PIN,AXIS1_DECAY_PIN};
const DriverSettings Axis1DriverModeSettings = {AXIS1_DRIVER_MODEL,AXIS1_DRIVER_MICROSTEPS,AXIS1_DRIVER_MICROSTEPS_GOTO,AXIS1_DRIVER_IHOLD,AXIS1_DRIVER_IRUN,AXIS1_DRIVER_IGOTO,AXIS1_DRIVER_DECAY,AXIS1_DRIVER_DECAY_GOTO};
StepDriver axis1Driver{Axis1DriverModePins, Axis1DriverModeSettings};

const AxisPins Axis1Pins = {AXIS1_STEP_PIN, AXIS1_DIR_PIN, AXIS1_ENABLE_PIN, false, false, true};
Axis axis1{Axis1Pins, Axis1DriverModePins, Axis1DriverModeSettings};
void moveAxis1() { axis1.move(AXIS1_STEP_PIN, AXIS1_DIR_PIN); }

const DriverPins     Axis2DriverModePins     = {AXIS2_M0_PIN,AXIS2_M1_PIN,AXIS2_M2_PIN,AXIS2_M3_PIN,AXIS2_DECAY_PIN};
const DriverSettings Axis2DriverModeSettings = {AXIS2_DRIVER_MODEL,AXIS2_DRIVER_MICROSTEPS,AXIS2_DRIVER_MICROSTEPS_GOTO,AXIS2_DRIVER_IHOLD,AXIS2_DRIVER_IRUN,AXIS2_DRIVER_IGOTO,AXIS2_DRIVER_DECAY,AXIS2_DRIVER_DECAY_GOTO};
StepDriver axis2Driver{Axis2DriverModePins, Axis2DriverModeSettings};

const AxisPins Axis2Pins = {AXIS2_STEP_PIN, AXIS2_DIR_PIN, AXIS2_ENABLE_PIN, false, false, true};
Axis axis2{Axis2Pins, Axis2DriverModePins, Axis2DriverModeSettings};
void moveAxis2() { axis2.move(AXIS2_STEP_PIN, AXIS2_DIR_PIN); }

void Axis::init(uint8_t axisNumber) {
  pinModeInitEx(Pins.step, OUTPUT, !invertStep?LOW:HIGH);
  pinModeInitEx(Pins.dir, OUTPUT, !invertDir?LOW:HIGH);
  pinModeEx(Pins.enable, OUTPUT); enable(false);

  this->task_handle = task_handle;

  int handle;
  if (axisNumber == 1) {
    #if AXIS1_DRIVER_MODEL != OFF
      handle = tasks.add(0, 0, true, 0, moveAxis1, "MoveAx1"); tasks.requestHardwareTimer(handle,1,0);
    #endif
  }
  if (axisNumber == 2) {
    #if AXIS2_DRIVER_MODEL != OFF
      handle = tasks.add(0, 0, true, 0, moveAxis2, "MoveAx2"); tasks.requestHardwareTimer(handle,1,0);
    #endif
  }
  if (axisNumber == 3) {
    #if AXIS3_DRIVER_MODEL != OFF
      tasks.add(0, 0, true, 0, moveAxis3, "MoveAx3");
    #endif
  }
  if (axisNumber == 4) {
    #if AXIS4_DRIVER_MODEL != OFF
      tasks.add(0, 0, true, 0, moveAxis4, "MoveAx4");
    #endif
  }
  if (axisNumber == 5) {
    #if AXIS5_DRIVER_MODEL != OFF
      tasks.add(0, 0, true, 0, moveAxis5, "MoveAx5");
    #endif
  }
  if (axisNumber == 6) {
    #if AXIS6_DRIVER_MODEL != OFF
      tasks.add(0, 0, true, 0, moveAxis6, "MoveAx6");
    #endif
  }
  stepDriver.init();
}

void Axis::enable(bool value) {
  if (Pins.enable != OFF) {
    if (value) digitalWrite(Pins.enable, invertEnabled?HIGH:LOW); else digitalWrite(Pins.enable, invertEnabled?LOW:HIGH);
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
  long steps = lround(value*spm);
  noInterrupts();
  targetSteps = steps;
  interrupts();
}

double Axis::getTargetCoordinate() {
  noInterrupts();
  long steps = targetSteps;
  interrupts();
  return steps/spm;
}

void Axis::setFrequencyMax(double frequency) {
  maxFreq = frequency*spm;
  if (frequency != 0.0) minPeriodMicros = 1000000.0/maxFreq; else minPeriodMicros = 0.0;
  minPeriodMicrosHalf = lround(minPeriodMicros/2.0);
}

void Axis::setFrequency(double frequency) {
  double d=500000.0/(frequency*spm);
  if (isnan(d) || fabs(d) > 134000000) { tasks.setPeriod(task_handle, 0); return; }
  unsigned long periodMicroseconds = lround(d);
  if (periodMicroseconds < minPeriodMicrosHalf) periodMicroseconds = minPeriodMicrosHalf;
  // handle the case where the move() method isn't called to set the new period
  if (lastFrequency == 0.0) tasks.setPeriodMicros(task_handle, periodMicroseconds);
  lastFrequency = frequency;
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
