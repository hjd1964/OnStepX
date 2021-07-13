// -----------------------------------------------------------------------------------
// Axis motion control
#pragma once

#include <Arduino.h>
#include "../../Common.h"
#include "../../commands/ProcessCmds.h"
#include "StepDrivers.h"

#if AXIS1_DRIVER_STEP == AXIS2_DRIVER_STEP == AXIS3_DRIVER_STEP == AXIS1_DRIVER_STEP == AXIS1_DRIVER_STEP == AXIS1_DRIVER_STEP == AXIS1_DRIVER_STEP == AXIS1_DRIVER_STEP == AXIS1_DRIVER_STEP == HIGH
  #define DRIVER_STEP_DEFAULTS
#endif

#pragma pack(1)
typedef struct AxisLimits {
  float min;
  float max;
} AxisLimits;

#define AxisSettingsSize 21
typedef struct AxisSettings {
  double     stepsPerMeasure;
  int8_t     reverse;
  int16_t    microsteps;
  int16_t    currentRun;
  AxisLimits limits;
} AxisSettings;
#pragma pack()

typedef struct AxisSense {
  int32_t home;
  int8_t  homeInit;
  int32_t min;
  int32_t max;
  int8_t  minMaxInit;
} AxisSense;

typedef struct AxisPins {
  int16_t   step;
  uint8_t   stepState;
  int16_t   dir;
  int16_t   enable;
  uint8_t   enabledState;
  int16_t   min;
  int16_t   home;
  int16_t   max;
  AxisSense sense;
} AxisPins;

typedef struct AxisErrors {
  uint8_t minLimitSensed:1;
  uint8_t maxLimitSensed:1;
} AxisErrors;

enum MicrostepModeControl: uint8_t {MMC_TRACKING, MMC_SLEWING_REQUEST, MMC_SLEWING_READY, MMC_SLEWING};
enum Direction: uint8_t {DIR_NONE, DIR_FORWARD, DIR_REVERSE};
enum AutoRate: uint8_t {AR_NONE, AR_RATE_BY_DISTANCE, AR_RATE_BY_TIME_FORWARD, AR_RATE_BY_TIME_REVERSE, AR_RATE_BY_TIME_END, AR_RATE_BY_TIME_ABORT};
enum HomingStage: uint8_t {HOME_NONE, HOME_FINE, HOME_SLOW, HOME_FAST};

class Axis {
  public:
    // sets up the driver step/dir/enable pins and any associated driver mode control
    void init(uint8_t axisNumber, bool alternateLimits, bool validKey);

    // process commands for this axis
    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // enables or disables the associated step/dir driver
    void enable(bool value);

    // get the enabled state
    bool isEnabled();

    // get steps per measure
    double getStepsPerMeasure();

    // get tracking mode steps per slewing mode step
    int getStepsPerStepSlewing();

    // set motor coordinate, in "measure" units
    void setMotorCoordinate(double value);
    // get motor coordinate, in "measure" units
    double getMotorCoordinate();

    // sets motor and target coordinates in steps, also zeros backlash and index 
    void setMotorCoordinateSteps(long value);
    // get motor coordinate in steps
    long getMotorCoordinateSteps();

    // get instrument coordinate, in steps
    long getInstrumentCoordinateSteps();

    // set instrument coordinate, in "measures" (radians, microns, etc.)
    // with backlash disabled this indexes to the nearest position where the motor wouldn't cog
    void setInstrumentCoordinatePark(double value);
    // set instrument coordinate, in "measures" (radians, microns, etc.)
    void setInstrumentCoordinate(double value);
    // get instrument coordinate in steps
    double getInstrumentCoordinate();

    // mark origin coordinate as current location
    void markOriginCoordinate();

    // set target coordinate, in "measures" (degrees, microns, etc.)
    // with backlash disabled this moves to the nearest position where the motor doesn't cog
    void setTargetCoordinatePark(double value);
    // set target coordinate, in "measures" (degrees, microns, etc.)
    void setTargetCoordinate(double value);
    // set target coordinate, in steps
    void setTargetCoordinateSteps(long value);
    // get target coordinate, in "measures" (degrees, microns, etc.)
    double getTargetCoordinate();
    // get target coordinate, in steps
    long getTargetCoordinateSteps();
    // returns true if within 2 steps of target
    bool nearTarget();

    // set frequency in "measures" (degrees, microns, etc.) per second (0 stops motion)
    void setFrequency(float frequency);
    // get frequency in "measures" (degrees, microns, etc.) per second
    float getFrequency();
    // get frequency in steps per second
    float getFrequencySteps();
    // set base movement frequency in "measures" (radians, microns, etc.) per second
    void setFrequencyBase(float frequency);
    // set slew frequency in "measures" (radians, microns, etc.) per second
    void setFrequencySlew(float frequency);
    // set minimum slew frequency in "measures" (radians, microns, etc.) per second
    void setFrequencyMin(float frequency);
    // set maximum frequency in "measures" (radians, microns, etc.) per second
    void setFrequencyMax(float frequency);

    // set acceleration rate in "measures" per second per second
    void setSlewAccelerationRate(float mpsps);
    // set time to emergency stop movement, with acceleration in "measures" per second per second
    void setSlewAccelerationRateAbort(float mpsps);

    // slew, with acceleration distance (in "measures" to FrequencySlew)
    void autoSlewRateByDistance(float distance);
    // stops, with deacceleration by distance
    void autoSlewRateByDistanceStop();
    // slew, with acceleration in "measures" per second per second
    void autoSlew(Direction direction);
    // slew to home, with acceleration in "measures" per second per second
    void autoSlewHome();
    // stops, with deacceleration by time
    void autoSlewStop();
    // emergency stops, with deacceleration by time
    void autoSlewAbort();
    // checks if slew is active on this axis
    bool autoSlewActive();

    // set tracking state (automatic movement of target)
    void setTracking(bool state);
    // get tracking state (automatic movement of target)
    bool getTracking();

    // set backlash in "measures" (radians, microns, etc.)
    void setBacklash(float value);
    // get backlash in "measures" (radians, microns, etc.)
    float getBacklash();

    // set backlash in steps
    void setBacklashSteps(long value);
    // get backlash in steps
    long getBacklashSteps();

    // for TMC drivers, etc. report status
    inline bool fault() { return false; };

    // refresh driver status information maximum frequency is 20ms
    void updateDriverStatus();

    // enable or disable checking of limits, default enabled
    void setMotionLimitsCheck(bool state);
    // checks for an error that would disallow forward motion
    bool motionForwardError();
    // checks for an error that would disallow reverse motion
    bool motionReverseError();
    // checks for an error that would disallow motion
    bool motionError();

    // callbacks

    // monitor movement
    void poll();
    // sets dir as required and moves coord toward target
    void move(const int8_t stepPin, const int8_t dirPin);
    // fast axis movement forward only, no backlash, no mode switching
    void slewForward(const int8_t stepPin);
    // fast axis movement reverse only, no backlash, no mode switching
    void slewReverse(const int8_t stepPin);

    AxisSettings settings;
    AxisErrors error;
    StepDriver driver;

  private:
    // distance to origin or target, whichever is closer, in "measures" (degrees, microns, etc.)
    double getOriginOrTargetDistance();
    // distance to target in "measures" (degrees, microns, etc.)
    double getTargetDistance();
    // distance to target in steps
    long getTargetDistanceSteps();
    // returns true if traveling through backlash
    bool inBacklash();
    // disable backlash compensation, to work properly there must be an enable call to match
    void disableBacklash();
    // enable backlash compensation, to work properly this must be proceeded by a disable call
    void enableBacklash();

    // swaps fast unidirectional movement ISR for slewing in/out
    bool enableMoveFast(const bool state);

    bool decodeAxisSettings(char *s, AxisSettings &a);
    bool validateAxisSettings(int axisNum, bool altAz, AxisSettings a);
    
    AxisPins pins;

    uint8_t taskHandle = 0;
    uint8_t axisNumber = 0;
    char axisPrefix[13] = "MSG: Axis_, ";

    bool enabled = false;
    bool tracking = true;
    bool limitsCheck = true;

    uint8_t homeSenseHandle = 0; // home sensor handle
    uint8_t minSenseHandle = 0;  // min sensor handle
    uint8_t maxSenseHandle = 0;  // max sensor handle

    long originSteps = 0;

    float backlashFreq = 0.0F;
    uint16_t backlashStepsStore;
    volatile uint16_t backlashSteps = 0;
    volatile uint16_t backlashAmountSteps = 0;

    #ifdef DRIVER_STEP_DEFAULTS
      #define stepClr LOW
      #define stepSet HIGH
    #else
      volatile uint8_t stepClr = LOW;
      volatile uint8_t stepSet = HIGH;
    #endif
    volatile uint8_t dirFwd = LOW;
    volatile uint8_t dirRev = HIGH;

    double target = 0.0;
    volatile long targetSteps = 0;
    volatile long motorSteps = 0;
    volatile long indexSteps = 0;
    volatile int  trackingStep = 1;
    volatile int  slewStep = 1;
    volatile int  step = 1;
    volatile int  stepsPerStepSlewing = 1;
    volatile bool takeStep = false;
    volatile Direction direction = DIR_NONE;

    unsigned long lastPeriod = 0;
    unsigned long lastPeriodSet = 0;

    float freq = 0.0F;
    float baseFreq = 0.0F;
    float slewFreq;
    float minFreq = 0.0F;
    float maxFreq = 0.0F;
    float lastFreq;
    float minPeriodMicros;
    AutoRate autoRate = AR_NONE;
    // auto slew rate distance in radians to max rate
    float slewAccelerationDistance;
    // auto slew rate in measures per second per centisecond
    float slewMpspcs;
    // abort slew rate in measures per second per centisecond
    float abortMpspcs;
    HomingStage homingStage = HOME_NONE;

    volatile MicrostepModeControl microstepModeControl = MMC_TRACKING;
};
