// -----------------------------------------------------------------------------------
// Axis motion control
#pragma once
#include <Arduino.h>

#include "../StepDrivers/StepDrivers.h"

typedef struct AxisSettings {
  double  stepsPerMeasure;
  int8_t  reverse;
  double  min;
  double  max;
} AxisSettings;

typedef struct AxisPins {
  int8_t step;
  int8_t dir;
  int8_t enable;
  bool   invertStep;
  bool   invertDir;
  bool   invertEnable;
} AxisPins;

enum MicrostepModeControl {MMC_TRACKING,MMC_SLEWING_READY,MMC_SLEWING,MMC_TRACKING_READY};

class Axis {
  public:
    // creation and basic initialization
    Axis(AxisPins Pins, DriverPins ModePins, DriverSettings ModeSettings) :
      Pins{ Pins }, ModePins{ ModePins }, ModeSettings{ ModeSettings } {};

    // sets up the driver step/dir/enable pins and any associated driver mode control
    void init(uint8_t axisNumber, AxisSettings axisSettings);

    // enables or disables the associated step/dir driver
    void enable(bool value);

    // get the enabled state
    bool isEnabled();

    // get steps per measure
    double getStepsPerMeasure();

    // set motor coordinate, in "measure" units
    void setMotorCoordinate(double value);

    // set and get motor coordinate, in steps
    void setMotorCoordinateSteps(long value);
    long getMotorCoordinateSteps();

    // get instrument coordinate, in steps
    long getInstrumentCoordinateSteps();

    // set and get instrument coordinate, in "measures" (radians, microns, etc.)
    void setInstrumentCoordinate(double value);
    double getInstrumentCoordinate();

    // set origin coordinate as current location
    void markOriginCoordinate();

    // target coordinate, in "measures" (degrees, microns, etc.)
    void moveTargetCoordinate(double value);
    void setTargetCoordinate(double value);
    double getTargetCoordinate();
    bool nearTarget();

    // sets maximum frequency in "measures" (radians, microns, etc.) per second
    void setFrequencyMax(double frequency);

    // causes a movement at frequency "measures" (degrees, microns, etc.) per second (0 stops motion)
    void setFrequency(double frequency);

    // set and get tracking state (movement of motor to target)
    void setTracking(bool tracking);
    bool getTracking();

    // set and get backlash in "measures" (radians, microns, etc.)
    void   setBacklash(double value);
    double getBacklash();

    // get minimum and maximum position in "measures" (radians, microns, etc.)
    double getMinCoordinate();
    double getMaxCoordinate();

    // for TMC drivers, etc. report status
    inline bool fault() { return false; };

    // set dir as required and move coord toward the target and take a step; requires two calls to take a step
    void move(const int8_t stepPin, const int8_t dirPin);

  private:
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

    const AxisPins       Pins         = {OFF, OFF, OFF, false, false, false};
    const DriverPins     ModePins     = {OFF, OFF, OFF, OFF, OFF};
    const DriverSettings ModeSettings = {OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF};
    StepDriver stepDriver{ModePins, ModeSettings};
};
