// -----------------------------------------------------------------------------------
// Axis motion control
#pragma once
#include <Arduino.h>

#include "StepDrivers.h"

typedef struct AxisLimits {
  float min;
  float max;
} AxisLimits;

typedef struct AxisSettings {
  double     stepsPerMeasure;
  uint16_t   backlashAmountSteps;
  int8_t     reverse;
  AxisLimits limits;
} AxisSettings;

typedef struct AxisPins {
  int8_t step;
  int8_t dir;
  int8_t enable;
  bool   invertStep;
  bool   invertDir;
  bool   invertEnable;
} AxisPins;

typedef struct AxisErrors {
  uint8_t driverFault:1;
  uint8_t motorFault:1;
  uint8_t minExceeded:1;
  uint8_t maxExceeded:1;
  uint8_t minLimitSensed:1;
  uint8_t maxLimitSensed:1;
} AxisErrors;

enum MicrostepModeControl {MMC_TRACKING,MMC_SLEWING_READY,MMC_SLEWING,MMC_TRACKING_READY};
enum Direction {DIR_NONE, DIR_FORWARD, DIR_REVERSE};
enum AutoRate {AR_NONE, AR_RATE_BY_DISTANCE, AR_RATE_BY_START_TIME, AR_RATE_BY_END_TIME};

class Axis {
  public:
    // sets up the driver step/dir/enable pins and any associated driver mode control
    void init(uint8_t axisNumber);

    // enables or disables the associated step/dir driver
    void enable(bool value);

    // get the enabled state
    bool isEnabled();

    // get steps per measure
    double getStepsPerMeasure();

    // get tracking mode steps per goto mode step
    int getStepsPerStepGoto();

    // set motor coordinate, in "measure" units
    void setMotorCoordinate(double value);
    double getMotorCoordinate();

    // set and get motor coordinate, in steps
    void setMotorCoordinateSteps(long value);
    long getMotorCoordinateSteps();

    // get instrument coordinate, in steps
    long getInstrumentCoordinateSteps();

    // set and get instrument coordinate, in "measures" (radians, microns, etc.)
    void setInstrumentCoordinate(double value);
    double getInstrumentCoordinate();

    // mark origin coordinate as current location
    void markOriginCoordinate();

    // target coordinate, in "measures" (degrees, microns, etc.)
    void moveTargetCoordinate(double value);
    void setTargetCoordinate(double value);
    double getTargetCoordinate();
    bool nearTarget();

    // distance to origin or target, whichever is closer, in "measures" (degrees, microns, etc.)
    double getOriginOrTargetDistance();

    // sets movement frequency in "measures" (degrees, microns, etc.) per second (0 stops motion)
    void setFrequency(double frequency);
    // gets movement frequency in "measures" (degrees, microns, etc.) per second
    double getFrequency();
    // gets movement frequency in steps per second
    double getFrequencySteps();
    // sets maximum frequency in "measures" (radians, microns, etc.) per second
    void setFrequencyMax(double frequency);

    // automatically adjusts movement frequency by distance (in radians to FrequencyMax)
    void autoSlewRateByDistance(double distance);
    // automatically adjusts movement frequency by time (in seconds to FrequencyMax)
    void autoSlewRateByTime(double time);
    // stops automatic movement
    void autoSlewStop(double time);
    // time to stop movement (emergency stop)
    void setAutoSlewAbortTime(double time);
    // stops automatic movement (emergency stop)
    void autoSlewAbort();
    // monitor movement
    void poll();

    // set tracking state (automatic movement of target)
    void setTracking(bool state);
    // get tracking state (automatic movement of target)
    bool getTracking();

    // set backlash in "measures" (radians, microns, etc.)
    void setBacklash(double value);
    // get backlash in "measures" (radians, microns, etc.)
    double getBacklash();
    // returns true if traveling through backlash
    bool inBacklash();
    // disable backlash compensation, to work properly there must be an enable call to match
    void disableBacklash();
    // enable backlash compensation, to work properly this must be proceeded by a disable call
    void enableBacklash();

    // for TMC drivers, etc. report status
    inline bool fault() { return false; };

    // swaps fast unidirectional movement ISR for slewing in/out
    void enableMoveFast(bool state);

    // sets dir as required and moves coord toward target; requires two calls to take a step
    void move(const int8_t stepPin, const int8_t dirPin);
    // fast axis movement forward only, no backlash, no mode switching; requires one or two calls to take a step depending on mode
    void moveForwardFast(const int8_t stepPin, const int8_t dirPin);
    // fast axis movement reverse only, no backlash, no mode switching; requires one or two calls to take a step depending on mode
    void moveReverseFast(const int8_t stepPin, const int8_t dirPin);

    // checks for an error that would disallow forward motion
    bool motionForwardError();
    // checks for an error that would disallow reverse motion
    bool motionReverseError();
    // checks for an error that would disallow motion
    bool motionError();

    AxisSettings settings;
    AxisErrors error;

  private:
    StepDriver driver;

    AxisPins pins;

    uint8_t taskHandle                = 0;
    uint8_t axisNumber                = 0;

    bool   invertEnabled              = false;
    bool   enabled                    = false;
    bool   tracking                   = false;
    bool   moveFast                   = false;

    double origin                     = 0.0;
    double target                     = 0.0;
    double motor                      = 0.0;

    long   originSteps                = 0;

    volatile long targetSteps         = 0;
    volatile long motorSteps          = 0;
    volatile long indexSteps          = 0;
    volatile int  trackingStep        = 1;
    volatile int  step                = 1;
    volatile int  stepGoto            = 1;
    volatile bool invertStep          = false;
    volatile bool takeStep            = false;
    volatile bool invertDir           = false;
    Direction direction               = DIR_NONE;

    volatile long backlashSteps       = 0;
    volatile long backlashAmountSteps = 0;
    unsigned long backlashStepsStore  = 0;
    double backlashFreq               = siderealToRad(TRACK_BACKLASH_RATE);

    double spm                        = 1.0;

    double trackingFreq               = 0.0;
    double trackingPeriodMicros       = 0.0;
    long   trackingPeriodMicrosHalf   = 0;
    unsigned long lastPeriod          = 0;

    double maxFreq                    = 0.0;
    double lastFreq                   = 0.0;
    double minPeriodMicros            = 0.0;
    unsigned long minPeriodMicrosHalf = 0;
    AutoRate autoRate                 = AR_NONE;
    unsigned long autoRateStartTime   = 0;
    unsigned long autoRateEndTime     = 0;
    unsigned long autoRateAbortTime   = 0;
    double autoRateTimeToMax          = 0;
    double slewAccelerationDistance   = 0;

    MicrostepModeControl microstepModeControl = MMC_TRACKING;
};
