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
enum AutoRate {AR_NONE, AR_RATE_BY_DISTANCE, AR_RATE_BY_TIME_FORWARD, AR_RATE_BY_TIME_REVERSE, AR_RATE_BY_TIME_END, AR_RATE_BY_TIME_ABORT};

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
    void setTargetCoordinate(double value);
    double getTargetCoordinate();
    bool nearTarget();

    // distance to origin or target, whichever is closer, in "measures" (degrees, microns, etc.)
    double getOriginOrTargetDistance();

    // set frequency in "measures" (degrees, microns, etc.) per second (0 stops motion)
    void setFrequency(float frequency);
    // get frequency in "measures" (degrees, microns, etc.) per second
    float getFrequency();
    // get frequency in steps per second
    float getFrequencySteps();
    // set maximum frequency in "measures" (radians, microns, etc.) per second
    void setFrequencyMax(float frequency);

    // set time to emergency stop movement, with acceleration in "measures" per second per second
    void setSlewAccelerationRate(float mpsps);
    // set time to emergency stop movement, with acceleration in "measures" per second per second
    void setSlewAccelerationRateAbort(float mpsps);

    // slew, with acceleration in "measures" per second per second
    void autoSlew(Direction direction);
    // slew, with acceleration in distance (radians to FrequencyMax)
    void autoSlewRateByDistance(float distance);
    // stops automatic movement
    void autoSlewRateByDistanceStop();
    // stops automatic movement
    void autoSlewStop();
    // emergency stops automatic movement
    void autoSlewAbort();
    // checks if slew is active on this axis
    bool autoSlewActive();
    // monitor movement
    void poll();

    // set tracking state (automatic movement of target)
    void setTracking(bool state);
    // get tracking state (automatic movement of target)
    bool getTracking();

    // set backlash in "measures" (radians, microns, etc.)
    void setBacklash(float value);
    // get backlash in "measures" (radians, microns, etc.)
    float getBacklash();
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

    uint8_t taskHandle = 0;
    uint8_t axisNumber = 0;

    bool invertEnable = false;
    bool enabled = false;
    bool tracking = false;
    bool moveFast = false;

    long originSteps = 0;

    float backlashFreq = siderealToRad(TRACK_BACKLASH_RATE);
    unsigned long backlashStepsStore;
    volatile long backlashSteps = 0;
    volatile long backlashAmountSteps = 0;

    volatile bool invertStep = false;
    volatile bool invertDir = false;
    volatile long targetSteps = 0;
    volatile long motorSteps = 0;
    volatile long indexSteps = 0;
    volatile int  trackingStep = 1;
    volatile int  step = 1;
    volatile int  stepGoto = 1;
    volatile bool takeStep = false;
    volatile Direction direction = DIR_NONE;

    unsigned long lastPeriod;

    float freq = 0.0F;
    float maxFreq;
    float lastFreq;
    float minPeriodMicros;
    AutoRate autoRate = AR_NONE;
    // auto slew rate distance in radians to max rate
    float slewAccelerationDistance;
    // auto slew rate in measures per second per centisecond
    float slewMpspcs;
    // abort slew rate in measures per second per centisecond
    float abortMpspcs;

    MicrostepModeControl microstepModeControl = MMC_TRACKING;
};
