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

// Configuration constants

#define SERVO_CALIBRATION_START_DUTY_CYCLE 0.1f               // Initial PWM percentage for stiction search
#define SERVO_CALIBRATION_STOP_DUTY_CYCLE 12.0f               // Maximum allowed PWM percentage

#define SERVO_CALIBRATION_MOTOR_SETTLE_TIME 1000              // ms to wait after stopping motor
#define SERVO_CALIBRATION_VELOCITY_SETTLE_CHECK_INTERVAL 300  // ms between velocity checks
#define SERVO_CALIBRATION_TIMEOUT 1000000                     // ms before calibration fails

#define SERVO_CALIBRATION_STICTION_REFINE_ABS 0.05f           // %PWM
#define SERVO_CALIBRATION_STICTION_REFINE_REL 0.10f           // 10% of stiction ceiling
#define SERVO_CALIBRATION_REFINE_MAX_ITERATIONS 40            // Iterations in PWM floor exploration and tracking velocity search
#define SERVO_CALIBRATION_VELOCITY_SEARCH_MIN_FACTOR 0.5f     // Min stiction as % of max stiction
#define SERVO_CALIBRATION_VELOCITY_STABILITY_THRESHOLD 50.0f  // steps/sec^2 for steady state

#define SERVO_CALIBRATION_KICKSTART_DROP_FACTOR 0.9f          // Tracking PWM as % of min stiction
#define SERVO_CALIBRATION_ERROR_THRESHOLD 5.0f                // Velocity error % for success

#define SERVO_CALIBRATION_IMBALANCE_ERROR_THRESHOLD 2.0f      // Fwd/Rev imbalance warning threshold

#define SERVO_CALIBRATION_VELOCITY_MEASURE_WINDOW_MS 1500     // Measure tracking velocity on a bigger window

#define SERVO_CALIBRATION_MIN_DETECTABLE_VELOCITY 0.1f        // steps/sec minimum movement threshold
#define SERVO_CALIBRATION_STICTION_SAMPLE_INTERVAL_MS  80     // short debounce to avoid a single noisy read


// Calibration states
enum CalibrationState {
  CALIBRATION_IDLE,
  CALIBRATION_STICTION_CEILING,
  CALIBRATION_STICTION_FLOOR,
  CALIBRATION_VELOCITY_SEARCH,
  CALIBRATION_CHECK_IMBALANCE
};

class ServoCalibrateTrackingVelocity {
public:
  ServoCalibrateTrackingVelocity(uint8_t axisNumber);
  void init();
  void start(float trackingFrequency, long instrumentCoordinateSteps);
  void updateState(long instrumentCoordinateSteps);

  // Getters for calibration results
  float getStictionCeiling(bool forward);
  float getStictionFloor(bool forward);
  float getTrackingPwm(bool forward);

  bool experimentMode;
  float experimentPwm;
  bool enabled;

  // Debug/status
  void printReport();

private:
  // Motor control states
  enum MotorState {
    MOTOR_STOPPED,
    MOTOR_SETTLING,
    MOTOR_ACCELERATING,
    MOTOR_RUNNING_STEADY
  };

  // Core methods
  void handleMotorState();
  void processStictionCeiling();
  void processStictionFloor();
  void processVelocitySearch();
  void processImbalanceCheck();
  void handleCalibrationFailure();

  // Helper methods
  float calculateInstantaneousVelocity();
  void startSettling();
  void startTest(float pwm);
  void setPwm(float pwm);
  void transitionToRefine();
  void resetCalibrationValues();

  // Configuration
  uint8_t axisNumber;
  char axisPrefix[16]; // For logging

  // State tracking
  CalibrationState calibrationState;
  MotorState motorState;
  bool calibrationDirectionIsForward;

  // Timing and measurements
  unsigned long currentTime;
  unsigned long lastStateChangeTime;
  unsigned long settleStartTime;
  unsigned long calibrationStepStartTime;
  unsigned long lastVelocityTime;
  long currentTicks;
  long calibrationStepStartTicks;
  float lastVelocityMeasurement;

  // Calibration parameters
  float calibrationPwm;
  float calibrationMinPwm;
  float calibrationMaxPwm;
  float targetVelocity;

  // Results storage
  float stictionCeilingFwd;
  float stictionFloorFwd;
  float trackingPwmFwd;
  float stictionCeilingRev;
  float stictionFloorRev;
  float trackingPwmRev;

  long lastTicks;
  unsigned long lastCheckTime;

  bool everMovedFwd;
  bool everMovedRev;
  int refineIters;  // guard against infinite loops

  // Velocity search bookkeeping
  int   velIters;
  float bestVelSearchPwmAbs;
  float bestVelSearchErr;
  bool kickToFloorPWM;

  bool refineSawMove;

  // A tiny queue for kickstarting / settling the motor before a test
  bool hasQueuedTest = false;
  CalibrationState queuedState = CALIBRATION_IDLE;
  float queuedPwm = 0.0f;

  inline void queueNextTest(CalibrationState st, float pwm) {
  queuedState = st;
  queuedPwm = pwm;
  hasQueuedTest = true;
}

};

#endif