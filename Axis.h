// -----------------------------------------------------------------------------------
// Axis motion control
#pragma once

#include "src/lib/StepDrivers.h"

typedef struct AxisSettings {
  double  stepsPerMeasure;
  int8_t  reverse;
  int16_t min;
  int16_t max;
} AxisSettings;

enum MicrostepModeControl {MMC_TRACKING,MMC_SLEWING_READY,MMC_SLEWING,MMC_TRACKING_READY};

class Axis {
  public:
    Axis(int8_t stepPin, int8_t dirPin, int8_t enabledPin, DriverPins ModePins, DriverSettings ModeSettings) :
      stepPin{ stepPin }, dirPin{ dirPin }, enabledPin{ enabledPin },
      ModePins{ ModePins }, ModeSettings{ ModeSettings } {}

    void init(bool invertStep, bool invertDir, bool invertEnabled, uint8_t task_handle) {
      this->invertStep = invertStep;
      pinModeInitEx(stepPin, OUTPUT, !invertStep?LOW:HIGH);

      this->invertDir = invertDir;
      pinModeInitEx(dirPin, OUTPUT, !invertDir?LOW:HIGH);

      this->invertEnabled = invertEnabled;
      pinModeEx(enabledPin, OUTPUT); enable(false);

      this->task_handle = task_handle;

      stepDriver.init();
    }

    void enable(bool value) {
      enabled = value;
      if (enabledPin != OFF) {
        if (enabled) digitalWrite(enabledPin, invertEnabled?LOW:HIGH); else digitalWrite(enabledPin, invertEnabled?HIGH:LOW);
      }
    }

    bool isEnabled() {
      return enabled;
    }

    // set motor coordinate, in "measure" units
    void setMotorCoordinate(double value) {
      long steps = lround(value*spm);
      setMotorCoordinateSteps(steps);
    }

    // set motor coordinate, in steps
    void setMotorCoordinateSteps(long value) {
      indexSteps    = 0;
      noInterrupts();
      motorSteps    = value;
      targetSteps   = value;
      backlashSteps = 0;
      interrupts();
    }

    // get motor coordinate, in steps
    long getMotorCoordinateSteps() {
      noInterrupts();
      long steps = motorSteps + backlashSteps;
      interrupts();
      return steps;
    }

    // set instrument coordinate, in "measure" units
    void setInstrumentCoordinate(double value) {
      long steps = value*spm;
      noInterrupts();
      indexSteps = steps - motorSteps;
      interrupts();
    }

    // get instrument coordinate, in "measure" units
    double getInstrumentCoordinate() {
      noInterrupts();
      long steps = motorSteps + indexSteps;
      interrupts();
      return steps / spm;
    }

    // get instrument coordinate, in steps
    long getInstrumentCoordinateSteps() {
      noInterrupts();
      long steps = motorSteps + indexSteps;
      interrupts();
      return steps;
    }

    // set origin coordinate as current location
    void markOriginCoordinate() {
      originSteps = getInstrumentCoordinateSteps();
    }

    // set target coordinate, in "measures" (degrees, microns, etc.)
    void setTargetCoordinate(double value) {
      long steps = lround(value*spm);
      noInterrupts();
      targetSteps = steps;
      interrupts();
    }

    // get target coordinate, in "measures" (degrees, microns, etc.)
    double getTargetCoordinate() {
      noInterrupts();
      long steps = targetSteps;
      interrupts();
      return steps/spm;
    }

    // set dir as required to move coord toward the target and take a step; requires two calls to take a step
    void move(const int8_t stepPin, const int8_t dirPin) {
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

    // sets maximum frequency in "measures" (radians, microns, etc.) per second
    void setFrequencyMax(double frequency) {
      maxFreq = frequency*spm;
      if (frequency != 0.0) minPeriodMicros = 1000000.0/maxFreq; else minPeriodMicros = 0.0;
      minPeriodMicrosHalf = lround(minPeriodMicros/2.0);
    }

    // causes a movement at frequency "measures" (degrees, microns, etc.) per second (0 stops motion)
    void setFrequency(double frequency) {
      double d=500000.0/(frequency*spm);
      if (isnan(d) || fabs(d) > 134000000) { tasks.setPeriod(task_handle, 0); return; }
      unsigned long periodMicroseconds = lround(d);
      if (periodMicroseconds < minPeriodMicrosHalf) periodMicroseconds = minPeriodMicrosHalf;
      // handle the case where the move() method isn't called to set the new period
      if (lastFrequency == 0.0) tasks.setPeriodMicros(task_handle, periodMicroseconds);
      lastFrequency = frequency;
    }

    void setTracking(bool tracking) {
      this->tracking=tracking;
    }

    bool getTracking() {
      return tracking;
    }

    // set steps per measure
    // for conversion between steps and "measures" (radians, microns, etc.)
    void setStepsPerMeasure(double value) {
      spm = value;
    }

    void setBacklash(double value) {
      backlashAmountSteps = value * spm;
    }

    double getBacklash() {
      return backlashSteps / spm;
    }

    void setMinCoordinate(double value) {
      minSteps = value * spm;
    }

    double getMinCoordinate() {
      return minSteps / spm;
    }

    void setMaxCoordinate(double value) {
      maxSteps = value * spm;
    }

    double getMaxCoordinate() {
      return maxSteps / spm;
    }
        
  private:
    const int8_t stepPin, dirPin, enabledPin;

    uint8_t task_handle               = 0;

    bool   invertEnabled              = false;
    bool   enabled                    = false;
    bool   tracking                   = false;

    double origin                     = 0.0;
    double target                     = 0.0;
    double motor                      = 0.0;

    long   originSteps                = 0;

    volatile long targetSteps         = 0;
    volatile long motorSteps          = 0;
    volatile long indexSteps          = 0;
    volatile int  trackingStep        = 1;
    volatile int  step                = 1;
    volatile bool invertStep          = false;
    volatile bool takeStep            = false;
    volatile bool invertDir           = false;
    volatile bool dirFwd              = true;

    volatile long backlashSteps       = 0;
    long   backlashAmountSteps        = 0;

    long   minSteps                   = 0;
    long   maxSteps                   = 0;

    double spm                        = 1.0;

    double trackingFreq               = 0.0;
    double trackingPeriodMicros       = 0.0;
    long   trackingPeriodMicrosHalf   = 0;
    double lastFrequency              = 0.0;

    double maxFreq                    = 0.0;
    double minPeriodMicros            = 0.0;
    unsigned long minPeriodMicrosHalf = 0;

    MicrostepModeControl microstepModeControl = MMC_TRACKING;

    const DriverPins     ModePins     = {OFF, OFF, OFF, OFF, OFF};
    const DriverSettings ModeSettings = {OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF};
    StepDriver stepDriver{ModePins, ModeSettings};
};

// instantiate and callback wrappers
#if AXIS1_DRIVER_MODEL != OFF
  Axis axis1{AXIS1_STEP_PIN, AXIS1_DIR_PIN, AXIS1_ENABLE_PIN, Axis1DriverModePins, Axis1DriverModeSettings};
  void moveAxis1() { axis1.move(AXIS1_STEP_PIN, AXIS1_DIR_PIN); }
#endif
#if AXIS2_DRIVER_MODEL != OFF
  Axis axis2{AXIS2_STEP_PIN, AXIS2_DIR_PIN, AXIS2_ENABLE_PIN, Axis2DriverModePins, Axis2DriverModeSettings};
  void moveAxis2() { axis2.move(AXIS2_STEP_PIN, AXIS2_DIR_PIN); }
#endif
#if AXIS3_DRIVER_MODEL != OFF
  Axis axis3{AXIS3_STEP_PIN, AXIS3_DIR_PIN, AXIS3_ENABLE_PIN, Axis3DriverModePins, Axis3DriverModeSettings};
  void moveAxis3() { axis3.move(AXIS3_STEP_PIN, AXIS3_DIR_PIN); }
#endif
#if AXIS4_DRIVER_MODEL != OFF
  Axis axis4{AXIS4_STEP_PIN, AXIS4_DIR_PIN, AXIS4_ENABLE_PIN, Axis4DriverModePins, Axis4DriverModeSettings};
  void moveAxis4() { axis4.move(AXIS4_STEP_PIN, AXIS4_DIR_PIN); }
#endif
#if AXIS5_DRIVER_MODEL != OFF
  Axis axis5{AXIS5_STEP_PIN, AXIS5_DIR_PIN, AXIS5_ENABLE_PIN, Axis5DriverModePins, Axis5DriverModeSettings};
  void moveAxis5() { axis5.move(AXIS5_STEP_PIN, AXIS5_DIR_PIN); }
#endif
#if AXIS6_DRIVER_MODEL != OFF
  Axis axis6{AXIS6_STEP_PIN, AXIS6_DIR_PIN, AXIS6_ENABLE_PIN, Axis6DriverModePins, Axis6DriverModeSettings};
  void moveAxis6() { axis6.move(AXIS6_STEP_PIN, AXIS6_DIR_PIN); }
#endif
