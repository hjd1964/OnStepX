// -----------------------------------------------------------------------------------
// calibrate servo tracking velocity
#pragma once

#include <Arduino.h>
#include "../../../../../Common.h"

#if defined(SERVO_MOTOR_PRESENT) && defined(CALIBRATE_SERVO_DC)

#include "../ServoDriver.h"

#ifndef CALIBRATE_SERVO_AXIS_SELECT
  #define CALIBRATE_SERVO_AXIS_SELECT 3  // 0 None, 1 for RA, 2 for DEC, 3 for all
#endif

#define SERVO_CALIBRATION_TIME_PERIOD 3000                // Seconds per test
#define SERVO_CALIBRATION_ERROR_THRESHOLD 0.5             // Max error percentage
#define SERVO_CALIBRATION_START_DUTY_CYCLE 0.001          // Starting duty cycle 0.001%
#define SERVO_CALIBRATION_IMBALANCE_ERROR_THRESHOLD 1.0   // Max imbalance percentage
#define SERVO_CALIBRATION_STICTION_REFINE_STEP 0.1        // 0.1% PWM refinement step
#define SERVO_CALIBRATION_STICTION_SETTLE_TIME 3000       // seconds to settle after movement
#define SERVO_CALIBRATION_VELOCITY_SEARCH_MIN_FACTOR 0.1 // Search between stiction * SERVO_CALIBRATION_VELOCITY_SEARCH_MIN_FACTOR and stiction
#define SERVO_CALIBRATION_PWM_MAX 10.0                    // max value for PWM [0.0 - 100.0]. BE CAUTIOUS!!!

// How much to drop the PWM from the stiction break minimum when starting velocity search.
// For example, 0.8 means start at 80% of the stictionBreakMin after kickstarting.
#define SERVO_CALIBRATION_KICKSTART_DROP_FACTOR 1.0f

class ServoCalibrateTrackingVelocity {
  public:
    ServoCalibrateTrackingVelocity(uint8_t axisNumber);

    void init();

    // start the calibration process
    void start(float trackingFrequency, long instrumentCoordinateSteps);

    // handle calibration state machine
    void updateState(long instrumentCoordinateSteps);

    bool experimentMode = false;
    float experimentPwm = 0.0F;

    // get calibration results
    float getStictionBreakMax(bool forward);
    float getStictionBreakMin(bool forward);
    float getTrackingPwm(bool forward);

    // print the report
    void printReport();

  private:
    // set experiment PWM value (bypasses PID)
    void setExperimentPwm(float pwm);

    char axisPrefix[32];

    enum CalibrationState {
      CALIBRATION_IDLE,
      CALIBRATION_STICTION_BREAK_MAX_FWD,
      CALIBRATION_STICTION_REFINE_MIN_FWD,
      CALIBRATION_VELOCITY_SEARCH_FWD,
      CALIBRATION_STICTION_BREAK_MAX_REV,
      CALIBRATION_STICTION_REFINE_MIN_REV,
      CALIBRATION_VELOCITY_SEARCH_REV,
      CALIBRATION_PREP_REV,
      CALIBRATION_CHECK_IMBALANCE
    };

    CalibrationState calibrationState;

    uint8_t axisNumber;

    // Calibration parameters
    float calibrationPwm;
    float stictionBreakMaxFwd;
    float stictionBreakMinFwd;
    float trackingPwmFwd;
    float stictionBreakMaxRev;
    float stictionBreakMinRev;
    float trackingPwmRev;

    unsigned long calibrationStartTime;
    long calibrationStartTicks;
    float calibrationMinPwm;
    float calibrationMaxPwm;
    float targetVelocity;
    int calibrationPhaseCount;

    // Stiction refinement variables
    unsigned long settleStartTime;
    bool waitingForSettle;
    float stictionTestPwm;
};

#endif