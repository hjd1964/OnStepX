// ============================== TrackingVelocity.h ==============================
#pragma once

#include <Arduino.h>
#include "../../../../../../Common.h"

#if defined(SERVO_MOTOR_PRESENT) && defined(CALIBRATE_SERVO_DC)

#include "../DcServoDriver.h"

// -----------------------------------------------------------------------------
// Axis selection mask:
//   - Bit 0 -> Axis 1, Bit 1 -> Axis 2 (i.e., 1=Axis1, 2=Axis2, 3=both).
//   - 0 also means all axes using SERVO_CAL_ALL_AXES_MASK
// -----------------------------------------------------------------------------
#ifndef CALIBRATE_SERVO_AXIS_SELECT
  #define CALIBRATE_SERVO_AXIS_SELECT 0
#endif
// When CALIBRATE_SERVO_AXIS_SELECT==0, use it as both axes
#define SERVO_CAL_ALL_AXES_MASK 0x03  // bit0=axis1, bit1=axis2

// -----------------------------------------------------------------------------
// Configuration constants (tuning and safety limits)
// All units are either percentages of PWM duty (for velocities) or milliseconds
// for timing, unless stated otherwise.
// -----------------------------------------------------------------------------
#define SERVO_CALIBRATION_START_VELOCITY_PERCENT         2.5f   // Initial staircase % for u_break (per direction)
#define SERVO_CALIBRATION_STOP_VELOCITY_PERCENT          12.0f  // Safety cap for commanded % during tests
#define SERVO_CALIBRATION_STAIRCASE_STEP_PERCENT         0.1f  // Step size for staircase up/down
#define SERVO_CALIBRATION_MOTOR_SETTLE_TIME              300    // ms to wait after STOP to remove transients
#define SERVO_CALIBRATION_VELOCITY_SETTLE_CHECK_INTERVAL 300    // ms between HOLD samples
#define SERVO_CALIBRATION_REQUIRED_MOVING_SAMPLES        1      // need this many consecutive moving samples to accept
#define SERVO_CALIBRATION_MIN_COUNTS_PER_SAMPLE          4L     // min raw encoder/step counts per sample to accept "moving" (tune for encoder)
#define SERVO_CALIBRATION_MIN_DETECTABLE_VELOCITY        1.0f   // steps/s—below this we treat as "not moving"
#define SERVO_CALIBRATION_REFINE_MAX_ITERATIONS          200    // Max steps per phase (u_break/u_hold) to avoid endless loops
#define SERVO_CALIBRATION_KICK_DURATION_MS               300    // ms to apply kick at u_break before HOLD
#define SERVO_CALIBRATION_IMBALANCE_ERROR_THRESHOLD      2.0f   // % threshold (vs. avg) to warn about FWD/REV imbalance
#define SERVO_CALIBRATION_TIMEOUT                        10000000 // ms guard for state timing (very generous)

// -----------------------------------------------------------------------------
// High-level calibration states (per-axis)
// -----------------------------------------------------------------------------
enum CalibrationState {
  CALIBRATION_IDLE,            // Not running
  CALIBRATION_U_BREAK,         // Staircase upward to overcome stiction from rest
  CALIBRATION_U_HOLD,          // Staircase downward to find minimum % that sustains motion
  CALIBRATION_CHECK_IMBALANCE, // Compute/report per-axis FWD/REV imbalance and snapshot data
  CALIBRATION_WAIT_PEER,       // Finished this axis; waiting for the other axis to finish (joint report)
  CALIBRATION_WAIT_TURN        // Both axes selected; wait here until it's this axis' turn to start
};

// -----------------------------------------------------------------------------
// Motor sub-states (lifecycle around each test step)
// -----------------------------------------------------------------------------
enum MotorState {
  MOTOR_STOPPED,         // At rest; used to delimit tests or indicate failure to move (after HOLD sample)
  MOTOR_SETTLING,        // Waiting for transients to decay after STOP/changes (enforced pause)
  MOTOR_KICKING,         // Brief kick at u_break to overcome stiction for u_hold tests
  MOTOR_HOLDING,         // Holding candidate % and sampling velocity periodically
  MOTOR_RUNNING_STEADY   // ACCEPTED MOVING (kept name for compatibility)
};

// -----------------------------------------------------------------------------
// Calibrator class (per axis instance)
// -----------------------------------------------------------------------------
class ServoCalibrateTrackingVelocity {
public:
  // ---------------------------------------------------------------------------
  // Per-direction compact record of results/telemetry for reporting
  // ---------------------------------------------------------------------------
  struct DirectionData {
    float u_break    = 0.0f; // % that first produced motion from rest (staircase up)
    float u_hold     = 0.0f; // % that minimally sustains motion (after kick; staircase down)
    float breakVel   = 0.0f; // measured steps/s at u_break
    float holdVel    = 0.0f; // measured steps/s at u_hold
    long  breakCounts= 0;    // raw delta counts measured at u_break (for diagnostics)
    long  holdCounts = 0;    // raw delta counts measured at u_hold  (for diagnostics)
    bool  everMoved  = false;// whether any motion was detected in this direction
  };

  // axisNumber is 1-based (1=RA, 2=DEC in common mounts)
  ServoCalibrateTrackingVelocity(uint8_t axisNumber);

  // One-time per-run init: clears state, resets local accumulators
  void init();

  // Start the calibration sequence for this axis.
  //  - trackingFrequency: target sidereal steps/s reference (informational).
  //  - instrumentCoordinateSteps: current encoder/step count baseline (unused here, caller passes live counts in updateState).
  void start(float trackingFrequency, long instrumentCoordinateSteps);

  // Drive the state machine; call frequently from the main control loop.
  void updateState(long instrumentCoordinateSteps);

  // Readouts for the final tuned duty % values
  float getU_break(bool forward); // forward=true -> FWD, false -> REV
  float getU_hold(bool forward);  // forward=true -> FWD, false -> REV

  // Exposed experiment flags/telemetry (read-only from outside in normal use)
  bool  experimentMode = false;    // true while calibration is running
  float experimentVelocity = 0.0f; // last commanded velocity % (signed)
  bool  enabled = false;           // axis was selected and started

  // Reports
  static void printReport(const char* axisPrefix,
                          const struct DirectionData& fwd,
                          const struct DirectionData& rev);
  void printReport();

private:
  // --------------------------- Phase handlers --------------------------------
  void handleMotorState();                 // sub-state machine for settle/kick/hold
  void processUBreak();                    // staircase upward search
  void processUHold();                     // staircase downward refinement
  void processImbalanceCheck();            // per-axis FWD/REV imbalance + joint sync/report
  void handleCalibrationFailure();         // timeout/fault recovery

  // --------------------------- Utilities -------------------------------------
  float calculateInstantaneousVelocity();  // steps/s from delta counts/time
  void startSettling();                    // enter MOTOR_SETTLING and timestamp
  void startTest(float velocityPercent);   // begin a test step (kick/hold based on phase)
  void setVelocity(float velocityPercent); // clamp+store experimentVelocity (signed %)
  void resetCalibrationValues();           // clear DirectionData (FWD/REV)
  void proceedToUHold();                   // transition helper (after u_break success)
  void finishUHold();                      // transition helper (after u_hold completes)
  void switchDirectionOrComplete();        // flip FWD↔REV or exit to imbalance stage

  // --------------------------- Helpers ---------------------------------------
  DirectionData&       currentDirectionData();       // FWD or REV ref (mutable)
  const DirectionData& currentDirectionData() const; // FWD or REV ref (const)
  void recordMovementData(float velocity, long counts); // write last measurement
  void logTestResult(const char* phase, bool moved);    // compact progress log

  // Queue the next test to start right after settling
  void queueNextTest(CalibrationState st, float velocity) {
    queuedState = st;
    queuedVelocity = velocity;
    hasQueuedTest = true;
  }

  // --------------------------- Hardware/context ------------------------------
  ServoDcDriver* driver = nullptr; // optional driver hook if needed by platform
  uint8_t axisNumber;              // 1-based axis id
  char axisPrefix[32];             // used in logs (e.g., "Axis1 ServoCalibration")

  // --------------------------- State -----------------------------------------
  CalibrationState calibrationState = CALIBRATION_IDLE;
  MotorState       motorState       = MOTOR_STOPPED;
  bool             calibrationDirectionIsForward = true; // FWD=true, REV=false
  int candidateSampleCount = 0;  // samples taken in current HOLDING candidate (UB/UH)


  // --------------------------- Timing/telemetry -------------------------------
  unsigned long currentTime = 0;
  unsigned long lastStateChangeTime = 0;
  unsigned long settleStartTime = 0;
  unsigned long calibrationStepStartTime = 0;
  unsigned long lastVelocityTime = 0;
  unsigned long kickStartTime = 0;

  long currentTicks = 0;                // caller-provided instrument steps
  long calibrationStepStartTicks = 0;   // ticks at the start of a test step
  float lastVelocityMeasurement = 0.0f; // steps/s

  unsigned long steadySinceMs = 0;      // time we accepted the candidate
  long          steadyStartTicks = 0;   // ticks when we accepted the candidate

  long lastDeltaCounts = 0;             // raw delta counts from last sample

  float calibrationVelocity = 0.0f;     // signed % (positive FWD, negative REV)
  float targetVelocity = 0.0f;          // sidereal target (steps/s), informational

  // --------------------------- Results ----------------------------------------
  DirectionData fwdData; // accumulated forward-direction results
  DirectionData revData; // accumulated reverse-direction results

  // --------------------------- Search bookkeeping ----------------------------
  int   staircaseIters = 0;              // steps taken within current phase
  float currentSearchVelocity = 0.0f;    // helper for proposed % values
  bool  foundValidMotion = false;        // latched once motion is first observed

  long lastTicks = 0;                    // sample baseline
  unsigned long lastCheckTime = 0;       // sample timestamp
  int consecutiveMovingSamples = 0;      // counts consecutive moving samples for a candidate

  // --------------------------- Deferred start of next test --------------------
  bool            hasQueuedTest = false;               // true if next test is queued
  CalibrationState queuedState = CALIBRATION_IDLE;     // queued phase
  float           queuedVelocity = 0.0f;               // queued signed %
};

#endif  // SERVO_MOTOR_PRESENT && CALIBRATE_SERVO_DC
