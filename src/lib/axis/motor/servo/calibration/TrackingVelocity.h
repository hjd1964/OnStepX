// -----------------------------------------------------------------------------------
// calibrate servo tracking velocity
#pragma once

#include <Arduino.h>
#include "../../../../../Common.h"

#if defined(SERVO_MOTOR_PRESENT) && defined(CALIBRATE_SERVO_DC)

#include "../ServoDriver.h"

#define SERVO_CALIBRATION_TIME_PERIOD 10                // Seconds per test
#define SERVO_CALIBRATION_ERROR_THRESHOLD 0.5           // Max error percentage
#define SERVO_CALIBRATION_START_DUTY_CYCLE 0.001        // Starting duty cycle 0.001%
#define SERVO_CALIBRATION_IMBALANCE_ERROR_THRESHOLD 1.0 // Max imbalance percentage

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

  private:
    // get stiction break PWM value for specified direction
    float getStictionBreakPwm(bool forward);

    // set target velocity PWM value for specified direction
    float getTargetVelocityPwm(bool forward);

    // set experiment PWM value (bypasses PID)
    void setExperimentPwm(float pwm);

    // set experiment mode state
    void setExperimentMode(bool state);

    char axisPrefix[32];

    enum CalibrationState {
      CALIBRATION_IDLE,
      CALIBRATION_STICTION_SEARCH_FWD,
      CALIBRATION_VELOCITY_SEARCH_FWD,
      CALIBRATION_STICTION_SEARCH_REV,
      CALIBRATION_VELOCITY_SEARCH_REV,
      CALIBRATION_CHECK_IMBALANCE
    };

    CalibrationState calibrationState;

    float calibrationPwm;
    float stictionBreakPwmFwd;
    float stictionBreakPwmRev;
    float targetVelocityPwmFwd;
    float targetVelocityPwmRev;
    unsigned long calibrationStartTime;
    long calibrationStartTicks;
    float calibrationMinPwm;
    float calibrationMaxPwm;
    float targetVelocity;
    int calibrationPhaseCount;
};

#endif