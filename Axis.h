// -----------------------------------------------------------------------------------
// Axis motion control
#pragma once

typedef struct AxisSettings {
  double  stepsPerMeasure;
  int8_t  reverse;
  int16_t min;
  int16_t max;
} AxisSettings;

class Axis {
  public:
    Axis(int8_t stepPin, int8_t dirPin, int8_t enabledPin) : stepPin{ stepPin }, dirPin{ dirPin }, enabledPin{ enabledPin } {}
    
    void init(bool invertStep, bool invertDir, bool invertEnabled, uint8_t event_handle) {
      this->invertStep  = invertStep;
      pinModeInit(stepPin,OUTPUT,!invertStep?LOW:HIGH);

      this->invertDir  = invertDir;
      pinModeInit(dirPin,OUTPUT,!invertDir?LOW:HIGH);

      this->invertEnabled = invertEnabled;
      pinModeEx(enabledPin,OUTPUT); enable(false);

      this->event_handle=event_handle;
    }

    void enable(bool value) {
      enabled = value;
      if (enabledPin != OFF) {
        if (enabled) digitalWrite(enabledPin,invertEnabled?LOW:HIGH); else digitalWrite(enabledPin,invertEnabled?HIGH:LOW);
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
          digitalWriteF(stepPin,invertStep?LOW:HIGH);
        } else {
          if (motorSteps + backlashSteps < targetSteps) {
            if (backlashSteps < backlashAmountSteps) backlashSteps += step; else motorSteps += step;
            digitalWriteF(stepPin,invertStep?LOW:HIGH);
          }
        }
      } else {
        if (motorSteps + backlashSteps > targetSteps) {
          if (!dirFwd) {
            dirFwd = !dirFwd;
            digitalWriteF(dirPin,invertDir?LOW:HIGH);
          }
        } else if (motorSteps + backlashSteps < targetSteps) {
          if (dirFwd) {
            dirFwd = !dirFwd;
            digitalWriteF(dirPin,invertDir?HIGH:LOW);
          }
        }
        if (microstepModeControl == MMC_SLEWING_READY) microstepModeControl = MMC_SLEWING;
        digitalWriteF(stepPin,invertStep?HIGH:LOW);
      }
      takeStep = !takeStep;
    }

    // low overhead forward only movement (invoked after backlash is at backlashAmountSteps)
    void moveFastForward(const int8_t stepPin, const int8_t dirPin) {
      if (takeStep) {
        if (motorSteps < targetSteps) { motorSteps += step; digitalWriteF(stepPin,HIGH); }
      } else {
        if (microstepModeControl == MMC_TRACKING_READY) microstepModeControl = MMC_TRACKING;
        digitalWriteF(stepPin,LOW);
      }
      takeStep = !takeStep;
    }
    
    // low overhead backward only movement (invoked after backlash is at 0)
    void moveFastBackward(const int8_t stepPin, const int8_t dirPin) {
      if (takeStep) {
        if (motorSteps > targetSteps) { motorSteps -= step; digitalWriteF(stepPin,HIGH); }
      } else {
        if (microstepModeControl == MMC_TRACKING_READY) microstepModeControl = MMC_TRACKING;
        digitalWriteF(stepPin,LOW);
      }
      takeStep = !takeStep;
    }

    // low overhead forward only movement (invoked after backlash is at backlashAmountSteps)
    void pulseFastForward(const int8_t stepPin, const int8_t dirPin) {
      digitalWriteF(stepPin,LOW);
      if (microstepModeControl == MMC_TRACKING_READY) microstepModeControl = MMC_TRACKING;
      if (motorSteps < targetSteps) { motorSteps += step; digitalWriteF(stepPin,HIGH); }
    }

    // low overhead backward only movement (invoked after backlash is at 0)
    void pulseFastBackward(const int8_t stepPin, const int8_t dirPin) {
      digitalWriteF(stepPin,LOW);
      if (microstepModeControl == MMC_TRACKING_READY) microstepModeControl = MMC_TRACKING;
      if (motorSteps > targetSteps) { motorSteps -= step; digitalWriteF(stepPin,HIGH); }
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
      if (isnan(d) || fabs(d) > 134000000) { tasks.setPeriod(event_handle,0); return; }
      next_period_in_microseconds = lround(d);
      if (next_period_in_microseconds < minPeriodMicrosHalf) next_period_in_microseconds=minPeriodMicrosHalf;
      // handle the case where the move() method isn't called to set the new period
      if (last_period_in_microseconds == 0) tasks.setPeriodMicros(event_handle,next_period_in_microseconds);
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

    uint8_t event_handle       = 0;

    bool invertStep, invertDir, invertEnabled;

    bool   enabled             = false;
    bool   dirFwd              = true;
    bool   takeStep            = false;
    
    bool   tracking            = false;
    int    trackingStep        = 1;
    int    step                = 1;

    double origin              = 0.0;
    long   originSteps         = 0;
    double target              = 0.0;
    volatile long targetSteps  = 0;
    double motor               = 0.0;
    volatile long motorSteps   = 0;
    long   indexSteps          = 0;

    long   backlashSteps       = 0;
    long   backlashAmountSteps = 0;

    long   minSteps            = 0;
    long   maxSteps            = 0;

    double spm                 = 1.0;

    double trackingFreq                       = 0.0;
    double trackingPeriodMicros               = 0.0;
    long   trackingPeriodMicrosHalf           = 0;
    unsigned long last_period_in_microseconds = 0;
    unsigned long next_period_in_microseconds = 0;

    double maxFreq             = 0.0;
    double minPeriodMicros     = 0.0;
    long   minPeriodMicrosHalf = 0;

    MicrostepModeControl microstepModeControl = MMC_TRACKING;
};
