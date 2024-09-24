// -----------------------------------------------------------------------------------
// Axis motion control
#pragma once

#include "../../Common.h"
#include "../sense/Sense.h"

#ifdef MOTOR_PRESENT

// default start of axis class hardware timers
#ifndef AXIS_HARDWARE_TIMER_BASE
#define AXIS_HARDWARE_TIMER_BASE    1
#endif

// default location of settings in NV
#ifndef NV_AXIS_SETTINGS_REVERT
#define NV_AXIS_SETTINGS_REVERT     100    // bytes: 2   , addr:  100.. 101
#endif
#ifndef NV_AXIS_SETTINGS_BASE
#define NV_AXIS_SETTINGS_BASE       102    // bytes: 25  , addr:  102.. 224 (for 9 axes)
#endif

// polling frequency for monitoring axis motion (default 100X/second) 
#ifndef FRACTIONAL_SEC
#define FRACTIONAL_SEC              100.0F
#endif
#define FRACTIONAL_SEC_US           (lround(1000000.0F/FRACTIONAL_SEC))

// time limit in seconds for slew home phases
#ifndef SLEW_HOME_REFINE_TIME_LIMIT
#define SLEW_HOME_REFINE_TIME_LIMIT 120
#endif

// ON blocks all motion when min/max are on the same pin, applies to all axes (mount/rotator/focusers)
#ifndef LIMIT_SENSE_STRICT
#define LIMIT_SENSE_STRICT          OFF
#endif

#include "../../libApp/commands/ProcessCmds.h"
#include "motor/Motor.h"
#include "motor/stepDir/StepDir.h"
#include "motor/servo/Servo.h"
#include "motor/oDrive/ODrive.h"

// helpers for step/dir and servo parameters
#define subdivisions param1
#define subdivisionsGoto param2
#define currentHold param3
#define currentRun param4
#define currentGoto param5
#define integral param1
#define porportional param2
#define derivative param3
#define integralGoto param4
#define porportionalGoto param5
#define derivativeGoto param6

#pragma pack(1)
typedef struct AxisLimits {
  float min;
  float max;
} AxisLimits;

typedef struct AxisSettings {
  double     stepsPerMeasure;
  int8_t     reverse;
  AxisLimits limits;
  float      backlashFreq;
} AxisSettings;

#define AxisStoredSettingsSize 41
typedef struct AxisStoredSettings {
  double     stepsPerMeasure;
  int8_t     reverse;
  float      param1, param2, param3, param4, param5, param6;
  AxisLimits limits;
} AxisStoredSettings;
#pragma pack()

typedef struct AxisSense {
  int32_t    homeTrigger;
  int8_t     homeInit;
  float      homeDistLimit;
  int32_t    minTrigger;
  int32_t    maxTrigger;
  int8_t     minMaxInit;
} AxisSense;

typedef struct AxisPins {
  int16_t    min;
  int16_t    home;
  int16_t    max;
  AxisSense  axisSense;
} AxisPins;

typedef struct AxisErrors {
  uint8_t minLimitSensed:1;
  uint8_t maxLimitSensed:1;
} AxisErrors;

enum AutoRate: uint8_t {AR_NONE, AR_RATE_BY_TIME_ABORT, AR_RATE_BY_TIME_END, AR_RATE_BY_DISTANCE, AR_RATE_BY_TIME_FORWARD, AR_RATE_BY_TIME_REVERSE};
enum HomingStage: uint8_t {HOME_NONE, HOME_FINE, HOME_SLOW, HOME_FAST};
enum AxisMeasure: uint8_t {AXIS_MEASURE_UNKNOWN, AXIS_MEASURE_MICRONS, AXIS_MEASURE_DEGREES, AXIS_MEASURE_RADIANS};

class Axis {
  public:
    // constructor
    Axis(uint8_t axisNumber, const AxisPins *pins, const AxisSettings *settings, const AxisMeasure axisMeasure, float targetTolerance = 0.0F);

    // process axis commands
    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // sets up the driver step/dir/enable pins and any associated driver mode control
    bool init(Motor *motor);

    // enables or disables the associated step/dir driver
    void enable(bool value);

    // get the enabled state
    inline bool isEnabled() { return enabled && !motor->calibrating; }

    // time (in ms) before automatic power down at standstill, use 0 to disable
    void setPowerDownTime(int value);

    // time (in ms) to disable automatic power down at standstill, use 0 to disable
    void setPowerDownOverrideTime(int value);

    // get steps per measure
    inline double getStepsPerMeasure() { return settings.stepsPerMeasure; }

    // get tracking mode steps per slewing mode step
    inline int getStepsPerStepSlewing() { return motor->getStepsPerStepSlewing(); }

    // reset motor and target angular position, in "measure" units
    CommandError resetPosition(double value);

    // reset motor and target angular position, in steps
    CommandError resetPositionSteps(long value);

    // resets target position to the motor position
    inline void resetTargetToMotorPosition() { motor->resetTargetToMotorPosition(); }

    // get motor position, in "measure" units
    double getMotorPosition();

    // get motor position, in steps
    inline long getMotorPositionSteps() { return motor->getMotorPositionSteps(); }

    // get index position, in "measure" units
    double getIndexPosition();

    // get index position, in steps
    inline long getIndexPositionSteps() { return motor->getIndexPositionSteps(); }

    // coordinate wrap, in "measures" (radians, microns, etc.)
    // wrap occurs when the coordinate exceeds the min or max limits
    // default 0.0 (disabled)
    inline void coordinateWrap(double value) { wrapAmount = value; wrapEnabled = fabs(value) > 0.0F; }

    // set instrument coordinate, in "measures" (radians, microns, etc.)
    void setInstrumentCoordinate(double value);

    // set instrument coordinate, in steps
    inline void setInstrumentCoordinateSteps(long value) { motor->setInstrumentCoordinateSteps(value); }

    // get instrument coordinate
    double getInstrumentCoordinate();

    // get instrument coordinate, in steps
    inline long getInstrumentCoordinateSteps() { return motor->getInstrumentCoordinateSteps(); }

    // set instrument coordinate park, in "measures" (radians, microns, etc.)
    // with backlash disabled this indexes to the nearest position where the motor wouldn't cog
    void setInstrumentCoordinatePark(double value);

    // set target coordinate park, in "measures" (degrees, microns, etc.)
    // with backlash disabled this moves to the nearest position where the motor doesn't cog
    void setTargetCoordinatePark(double value);

    // set target coordinate, in "measures" (degrees, microns, etc.)
    void setTargetCoordinate(double value);

    // set target coordinate, in steps
    inline void setTargetCoordinateSteps(long value) { motor->setTargetCoordinateSteps(value); }

    // get target coordinate, in "measures" (degrees, microns, etc.)
    double getTargetCoordinate();

    // get target coordinate, in steps
    inline long getTargetCoordinateSteps() { return motor->getTargetCoordinateSteps(); }

    // distance to target in "measures" (degrees, microns, etc.)
    double getTargetDistance();

    // returns true if at target
    bool atTarget();

    // returns true if within one second of the target at the backlash takeup rate
    bool nearTarget();

    // set backlash amount in "measures" (radians, microns, etc.)
    void setBacklash(float value);

    // set backlash amount in steps
    inline void setBacklashSteps(long value) { if (autoRate == AR_NONE) motor->setBacklashSteps(value); }

    // get backlash amount in "measures" (radians, microns, etc.)
    float getBacklash();

    // get frequency in "measures" (degrees, microns, etc.) per second
    float getFrequency();

    // get frequency in steps per second
    float getFrequencySteps() { return motor->getFrequencySteps(); }

    // get direction
    Direction getDirection() { return motor->getDirection(); }

    // gets backlash frequency in "measures" (degrees, microns, etc.) per second
    float getBacklashFrequency();

    // reverse direction of motion
    inline void setReverse(bool reverse) {
      if (reverse) {
        if (settings.reverse == ON) motor->setReverse(OFF); else motor->setReverse(ON);
      } else {
        motor->setReverse(settings.reverse);
      }
    }

    // reverse homing direction
    inline void setHomeReverse(bool reverse) { sense.reverse(homeSenseHandle, reverse); }

    // set base movement frequency in "measures" (radians, microns, etc.) per second
    void setFrequencyBase(float frequency);

    // set slew frequency in "measures" (radians, microns, etc.) per second
    void setFrequencySlew(float frequency);

    // set minimum slew frequency in "measures" (radians, microns, etc.) per second
    void setFrequencyMin(float frequency);

    // set maximum frequency in "measures" (radians, microns, etc.) per second
    void setFrequencyMax(float frequency);

    // set frequency scaling factor (0.0 to 1.0)
    void setFrequencyScale(float frequency) { if (frequency >= 0.0F && frequency <= 1.0F) scaleFreq = frequency; }

    // set acceleration rate in "measures" per second per second (for autoSlew)
    void setSlewAccelerationRate(float mpsps);

    // set acceleration rate in seconds (for autoSlew)
    void setSlewAccelerationTime(float seconds);

    // set acceleration for emergency stop movement in "measures" per second per second
    void setSlewAccelerationRateAbort(float mpsps);

    // set acceleration for emergency stop movement in seconds (for autoSlewStop)
    void setSlewAccelerationTimeAbort(float seconds);

    // auto goto to destination target coordinate
    // \param frequency: optional frequency of slew in "measures" (radians, microns, etc.) per second
    CommandError autoGoto(float frequency = NAN);

    // auto slew
    // \param direction: direction of motion, DIR_FORWARD or DIR_REVERSE
    // \param frequency: optional frequency of slew in "measures" (radians, microns, etc.) per second
    CommandError autoSlew(Direction direction, float frequency = NAN);

    // slew to home using home sensor, with acceleration in "measures" per second per second
    CommandError autoSlewHome(unsigned long timeout = 0);

    // check if homing is in progress
    bool isHoming() { return homingStage != HOME_NONE; }

    // check if a home sensor is available
    inline bool hasHomeSense() { return pins->axisSense.homeTrigger != OFF; }

    // stops, with deacceleration by time
    void autoSlewStop();

    // emergency stops, with deacceleration by time
    void autoSlewAbort();

    // checks if slew is active on this axis
    bool isSlewing();

    // returns 1 if departing origin or -1 if approaching target
    inline int getRampDirection() { return motor->getRampDirection(); }

    // set synchronized state (automatic movement of target at setFrequencySteps() rate)
    inline void setSynchronized(bool state) { motor->setSynchronized(state); }

    // get synchronized state (automatic movement of target at setFrequencySteps() rate)
    inline bool getSynchronized() { return motor->getSynchronized(); }

    // report fault status of motor driver, if available
    inline bool motorFault() { return motor->getDriverStatus().fault; };

    // get associated motor driver status
    DriverStatus getStatus();

    // enable/disable numeric position range limits (doesn't apply to limit switches)
    void setMotionLimitsCheck(bool state);

    // checks for an error that would disallow motion in a given direction or DIR_BOTH for any motion
    bool motionError(Direction direction);

    // checks for an sense error that would disallow motion in a given direction or DIR_BOTH for any motion
    bool motionErrorSensed(Direction direction);

    // calibrate the motor if required
    void calibrate(float value) { motor->calibrate(value); }

    // calibrate the motor driver if required
    void calibrateDriver() { motor->calibrateDriver(); }

    // monitor movement
    void poll();

    // associated motor
    Motor *motor;

    AxisStoredSettings settings;

    bool commonMinMaxSense = false;

  private:
    // set frequency in "measures" (degrees, microns, etc.) per second (0 stops motion)
    void setFrequency(float frequency);

    // sets driver power on/off
    void powered(bool value);

    // distance to origin or target, whichever is closer, in "measures" (degrees, microns, etc.)
    double getOriginOrTargetDistance();

    // returns true if traveling through backlash
    bool inBacklash();

    // convert from unwrapped (full range) to normal (+/- wrapAmount) coordinate
    double wrap(double value);

    // convert from normal (+/- wrapAmount) to an unwrapped (full range) coordinate
    double unwrap(double value);

    // convert from normal (+/- wrapAmount) to an unwrapped (full range) coordinate
    // nearest the instrument coordinate
    double unwrapNearest(double value);

    bool decodeAxisSettings(char *s, AxisStoredSettings &a);

    bool validateAxisSettings(int axisNum, AxisStoredSettings a);
    
    AxisErrors errors;
    bool lastErrorResult = false;

    uint8_t axisNumber = 0;
    char axisPrefix[13] = "MSG: Axis_, ";
    char unitsStr[5] = "?";
    bool unitsRadians = false;

    bool enabled = false;        // enable/disable logical state (disabled is powered down)
    bool limitsCheck = true;     // enable/disable numeric position range limits (doesn't apply to limit switches)

    uint8_t homeSenseHandle = 0; // home sensor handle
    uint8_t minSenseHandle = 0;  // min sensor handle
    uint8_t maxSenseHandle = 0;  // max sensor handle

    uint16_t backlashStepsStore;
    volatile uint16_t backlashSteps = 0;
    volatile uint16_t backlashAmountSteps = 0;

    volatile bool takeStep = false;

    // coordinate handling
    double wrapAmount = 0.0L;
    bool wrapEnabled = false;

    // power down standstill control
    bool powerDownStandstill = false;
    bool powerDownOverride = false;
    unsigned long powerDownDelay;
    unsigned long powerDownOverrideEnds;
    bool poweredDown = false;
    unsigned long powerDownTime = 0;

    // timeout for home switch detection
    unsigned long homeTimeoutTime = 0;

    // rates (in measures per second) to control motor movement
    float freq = 0.0F;
    float rampFreq = 0.0F;
    float baseFreq = 0.0F;
    float minFreq = 0.0F;
    float slewFreq = 0.0F;
    float maxFreq = 0.0F;
    float scaleFreq = 1.0F;
    float backlashFreq = 0.0F;

    float targetTolerance = 0.0F;

    AutoRate autoRate = AR_NONE;       // auto slew mode
    float slewAccelRateFs;             // auto slew rate in measures per second per frac-sec
    float abortAccelRateFs;            // abort slew rate in measures per second per frac-sec
    float slewAccelTime = NAN;         // auto slew acceleration time in seconds
    float abortAccelTime = NAN;        // abort slew acceleration time in seconds

    HomingStage homingStage = HOME_NONE;

    const AxisPins *pins;

    void (*volatile callback)() = NULL;
};

#endif
