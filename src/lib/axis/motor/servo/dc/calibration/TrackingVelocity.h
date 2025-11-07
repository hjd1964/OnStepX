// -----------------------------------------------------------------------------------
// Servo Tracking Velocity Calibration - Overview
// [Header comments remain the same...]

#pragma once

#include <Arduino.h>
#include "../../../../../../Common.h"

#if defined(SERVO_MOTOR_PRESENT) && defined(CALIBRATE_SERVO_DC)

#include "../DcServoDriver.h"

#ifndef CALIBRATE_SERVO_AXIS_SELECT
  #define CALIBRATE_SERVO_AXIS_SELECT 3
#endif

// Configuration constants
#define SERVO_CALIBRATION_START_VELOCITY_PERCENT 2.5f
#define SERVO_CALIBRATION_STOP_VELOCITY_PERCENT 12.0f
#define SERVO_CALIBRATION_STAIRCASE_STEP_PERCENT 0.05f
#define SERVO_CALIBRATION_MOTOR_SETTLE_TIME 1000
#define SERVO_CALIBRATION_VELOCITY_SETTLE_CHECK_INTERVAL 1000
#define SERVO_CALIBRATION_TIMEOUT 10000000
#define SERVO_CALIBRATION_REFINE_MAX_ITERATIONS 200
#define SERVO_CALIBRATION_VELOCITY_STABILITY_THRESHOLD 50.0f
#define SERVO_CALIBRATION_ERROR_THRESHOLD 5.0f
#define SERVO_CALIBRATION_IMBALANCE_ERROR_THRESHOLD 2.0f
#define SERVO_CALIBRATION_VELOCITY_MEASURE_WINDOW_MS 1500
#define SERVO_CALIBRATION_MIN_DETECTABLE_VELOCITY 0.01f
#define SERVO_CALIBRATION_STICTION_SAMPLE_INTERVAL_MS 1000
#define SERVO_CALIBRATION_KICK_DURATION_MS 500

enum CalibrationState {
  CALIBRATION_IDLE,
  CALIBRATION_U_BREAK,
  CALIBRATION_U_HOLD,
  CALIBRATION_CHECK_IMBALANCE
};

enum MotorState {
  MOTOR_STOPPED,
  MOTOR_SETTLING,
  MOTOR_KICKING,
  MOTOR_HOLDING,
  MOTOR_RUNNING_STEADY
};

class ServoCalibrateTrackingVelocity {
public:
  ServoCalibrateTrackingVelocity(uint8_t axisNumber);
  void init();
  void start(float trackingFrequency, long instrumentCoordinateSteps);
  void updateState(long instrumentCoordinateSteps);

  float getU_break(bool forward);
  float getU_hold(bool forward);

  bool experimentMode;
  float experimentVelocity;
  bool enabled;

  void printReport();

private:
  // Consolidated direction data structure
  struct DirectionData {
    float u_break = 0.0f;
    float u_hold = 0.0f;
    float breakVel = 0.0f;
    float holdVel = 0.0f;
    long breakCounts = 0;
    long holdCounts = 0;
    bool everMoved = false;
  };

  void handleMotorState();
  void processUBreak();
  void processUHold();
  void processImbalanceCheck();
  void handleCalibrationFailure();
  float calculateInstantaneousVelocity();
  void startSettling();
  void startTest(float velocityPercent);
  void setVelocity(float velocityPercent);
  void resetCalibrationValues();
  void proceedToUHold();
  void finishUHold();
  void switchDirectionOrComplete();

  // Helper methods for repeated patterns
  DirectionData& currentDirectionData();
  const DirectionData& currentDirectionData() const;
  void recordMovementData(float velocity, long counts);
  void logTestResult(const char* phase, bool moved);

  ServoDcDriver* driver = nullptr;
  uint8_t axisNumber;
  char axisPrefix[16];

  CalibrationState calibrationState;
  MotorState motorState;
  bool calibrationDirectionIsForward;

  // Timing and measurements
  unsigned long currentTime;
  unsigned long lastStateChangeTime;
  unsigned long settleStartTime;
  unsigned long calibrationStepStartTime;
  unsigned long lastVelocityTime;
  unsigned long kickStartTime;
  long currentTicks;
  long calibrationStepStartTicks;
  float lastVelocityMeasurement;

  long lastDeltaCounts;
  unsigned long steadySinceMs;
  long steadyStartTicks;

  float calibrationVelocity;
  float targetVelocity;

  // Consolidated direction data
  DirectionData fwdData;
  DirectionData revData;

  int staircaseIters;
  float currentSearchVelocity;
  bool foundValidMotion;

  long lastTicks;
  unsigned long lastCheckTime;

  bool hasQueuedTest = false;
  CalibrationState queuedState = CALIBRATION_IDLE;
  float queuedVelocity = 0.0f;

  void queueNextTest(CalibrationState st, float velocity) {
    queuedState = st;
    queuedVelocity = velocity;
    hasQueuedTest = true;
  }
};

#endif