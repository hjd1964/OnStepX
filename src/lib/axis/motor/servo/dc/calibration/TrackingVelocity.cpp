// ============================== TrackingVelocity.cpp ===============================
// -----------------------------------------------------------------------------------
// Servo Tracking Velocity Calibration — Overview
//
// Goal
//   Calibrate, per direction, the minimum PWM duty (%) that:
//     • breaks stiction from rest (u_break)
//     • sustains motion after a short kick (u_hold)
//
// Mode (Open-loop)
//   While calibration is active (experimentMode=true) the axis is run open-loop:
//     • Tracking/PID/hysteresis/nonlinear helpers are disabled in this TU
//     • The driver receives a fixed % command (experimentVelocity)
//     • “Velocity” below means encoder-derived steps/s (not a PID setpoint)
//
// Per-direction phases
//   1) Stiction Break (u_break)
//      - Staircase upward from SERVO_CALIBRATION_START_VELOCITY_PERCENT.
//      - Between steps, STOP and wait SERVO_CALIBRATION_MOTOR_SETTLE_TIME ms to kill carry-over.
//      - Acceptance rule: must meet the SAME consecutive-moving requirement as u_hold
//        (SERVO_CALIBRATION_REQUIRED_MOVING_SAMPLES). UB additionally enforces a hard
//        per-candidate sample cap (SERVO_CAL_UB_MAX_SAMPLES) to avoid indefinite waiting.
//   2) Velocity Hold (u_hold)
//      - From u_break, staircase downward to find the minimum % that keeps moving.
//      - Each candidate is kick-started at u_break, then held at the candidate %.
//      - Acceptance rule: SAME consecutive-moving requirement. Any failed sample
//        immediately stalls the candidate.
//
// Reporting/imbalance
//   After both directions finish, an imbalance warning is logged if FWD/REV u_hold
//   differ beyond SERVO_CALIBRATION_IMBALANCE_ERROR_THRESHOLD.
//
// ------------------------------ How the state machine works -------------------------
// There are TWO interleaved state machines:
//
//  (A) High-level PHASE machine (per axis) [CalibrationState]:
//      IDLE → U_BREAK → U_HOLD → (flip direction) → U_BREAK → U_HOLD → CHECK_IMBALANCE
//                                               ↘ (if both axes enabled) WAIT_PEER
//      If both axes are enabled, we serialize execution RA first, DEC second via WAIT_TURN.
//
//  (B) Motor SUB-STATE machine (per axis) [MotorState]:
//      STOPPED ↔ SETTLING → (start test) → KICKING → HOLDING → RUNNING_STEADY
//
//      - SETTLING: enforced pause after STOP to let motion decay.
//      - KICKING:  briefly command u_break to overcome stiction for a u_hold test.
//      - HOLDING:  hold candidate % and sample velocity at fixed cadence.
//      - RUNNING_STEADY: means “accepted as moving” (name kept for API compatibility).
//
// ------------------------------ “Motor moved?” decision -----------------------------
// At each HOLDING sample (every SERVO_CALIBRATION_VELOCITY_SETTLE_CHECK_INTERVAL ms):
//   - Compute instantaneous velocity from encoder/step deltas.
//   - Accept a single sample as MOVING if BOTH are true:
//       • |velocity| > SERVO_CALIBRATION_MIN_DETECTABLE_VELOCITY  (noise floor guard)
//       • |deltaCounts| ≥ SERVO_CALIBRATION_MIN_COUNTS_PER_SAMPLE (minimum progress)
//
// Acceptance rule (shared by UB & UH):
//   - Require SERVO_CALIBRATION_REQUIRED_MOVING_SAMPLES consecutive MOVING samples
//     to accept the candidate as RUNNING_STEADY.
//   - For u_hold (UH): any non-moving sample immediately fails the candidate.
//   - For u_break (UB): keep sampling until a small cap (SERVO_CAL_UB_MAX_SAMPLES);
//     if we still haven’t reached the consecutive requirement, fail the candidate.
//
// ------------------------------ Two-axis coordination ------------------------------
// - Selection mask: CALIBRATE_SERVO_AXIS_SELECT (bit0=Axis1, bit1=Axis2). In this .cpp,
//   0 expands to “both” via SERVO_CAL_ALL_AXES_MASK (0x03).
// - Turn-taking: If both axes are enabled, Axis 1 runs first while Axis 2 waits
//   in WAIT_TURN. When Axis 1 ends, it grants the turn to Axis 2.
// - Joint report: First finisher waits (WAIT_PEER) for the other; then a single
//   combined report is printed.
// -----------------------------------------------------------------------------------

#include "TrackingVelocity.h"

#if defined(SERVO_MOTOR_PRESENT) && defined(CALIBRATE_SERVO_DC)

// ---------- Compact logging helpers (one-line logs) ----------
static inline void LOG_HDR(const char* axisPrefix) { V(axisPrefix); VF(" "); }
static inline void LOG_DIR(bool fwd){ VF(fwd ? "F " : "R "); }  // F=forward, R=reverse
static inline const char* PHASE_UB(){ return "UB"; }             // u_break
static inline const char* PHASE_UH(){ return "UH"; }             // u_hold

// Disable servo helpers during calibration (we want raw behavior)
#ifdef SERVO_HYSTERESIS_ENABLE
  #undef SERVO_HYSTERESIS_ENABLE
#endif
#ifdef SERVO_STICTION_KICK
  #undef SERVO_STICTION_KICK
#endif
#ifdef SERVO_NONLINEAR_ENABLE
  #undef SERVO_NONLINEAR_ENABLE
#endif

#ifdef TRACK_AUTOSTART
  #undef TRACK_AUTOSTART
#endif

// ------------------------ UB sampling cap (avoid infinite wait) ---------------------
// UB uses the same consecutive requirement as UH, but we also enforce a hard cap on the
// total number of samples we will take for a single UB candidate before declaring STALLED.
#ifndef SERVO_CAL_UB_MAX_SAMPLES
  #define SERVO_CAL_UB_MAX_SAMPLES 6
#endif

// -------------------- Joint calibration coordination (for 2 axes) -------------------
struct AxisCalibSnapshot {
  bool valid = false;
  ServoCalibrateTrackingVelocity::DirectionData fwd;
  ServoCalibrateTrackingVelocity::DirectionData rev;
  char axisPrefix[32] = {0};
};

// 1-based index: [1] and [2] used; [0] unused
static volatile bool g_axisEnabledForCal[3] = {false,false,false};
static volatile bool g_axisDone[3]          = {false,false,false};
static AxisCalibSnapshot g_snapshot[3];
static volatile bool g_jointReportPrinted   = false;

// Turn-taking: 0 = none yet, 1 = Axis 1's turn, 2 = Axis 2's turn
static volatile uint8_t g_currentAxisTurn   = 0;

static inline uint8_t otherAxis(uint8_t axis) { return axis == 1 ? 2 : 1; }

// Save final per-axis results for the joint report
static void snapshotAxisData(uint8_t axis,
                             const char* axisPrefix,
                             const ServoCalibrateTrackingVelocity::DirectionData& fwd,
                             const ServoCalibrateTrackingVelocity::DirectionData& rev) {
  AxisCalibSnapshot &s = g_snapshot[axis];
  s.valid = true;
  s.fwd   = fwd;
  s.rev   = rev;
  snprintf(s.axisPrefix, sizeof(s.axisPrefix), "%s", axisPrefix ? axisPrefix : "");
}

// Print one axis table
static void printOneAxisTable(const AxisCalibSnapshot& s) {
  ServoCalibrateTrackingVelocity::printReport(s.axisPrefix, s.fwd, s.rev);
}

// Print the combined report for both axes (if present)
static void printJointReport() {
  VLF("\n===== Joint Servo Calibration =====");
  if (g_snapshot[1].valid) printOneAxisTable(g_snapshot[1]);
  if (g_snapshot[2].valid) printOneAxisTable(g_snapshot[2]);

  if (g_snapshot[1].valid && g_snapshot[2].valid) {
    float a1 = (g_snapshot[1].fwd.u_hold + g_snapshot[1].rev.u_hold) / 2.0f;
    float a2 = (g_snapshot[2].fwd.u_hold + g_snapshot[2].rev.u_hold) / 2.0f;
    float delta = fabsf(a1 - a2);
    VF("Cross-axis avg u_hold delta: "); V(delta); VLF("%");
    (void)delta; // silence if logs trimmed
  }
  VLF("==================================\n");
}

// ------------------------------------------------------------------------------------
// Class methods
// ------------------------------------------------------------------------------------

ServoCalibrateTrackingVelocity::ServoCalibrateTrackingVelocity(uint8_t axisNumber) {
  this->axisNumber = axisNumber;
  snprintf(axisPrefix, sizeof(axisPrefix), "Axis%d ServoCalibrate", axisNumber);
}

void ServoCalibrateTrackingVelocity::init() {
  experimentVelocity = 0.0f;
  experimentMode = false;
  calibrationState = CALIBRATION_IDLE;
  motorState = MOTOR_STOPPED;

  resetCalibrationValues();

  lastStateChangeTime = lastVelocityTime = settleStartTime =
    calibrationStepStartTime = kickStartTime = 0;
  currentTicks = calibrationStepStartTicks = lastTicks = 0;
  lastVelocityMeasurement = 0.0f;
  lastDeltaCounts = 0;
  steadySinceMs = 0;
  steadyStartTicks = 0;
  lastCheckTime = 0;

  staircaseIters = 0;
  currentSearchVelocity = 0.0f;
  foundValidMotion = false;

  hasQueuedTest = false;
  queuedState = CALIBRATION_IDLE;
  queuedVelocity = 0.0f;

  consecutiveMovingSamples = 0;
  candidateSampleCount = 0;

  if (axisNumber == 1) {
    g_axisDone[1] = g_axisDone[2] = false;
    g_axisEnabledForCal[1] = g_axisEnabledForCal[2] = false;  // set in start()
    g_snapshot[1] = AxisCalibSnapshot();
    g_snapshot[2] = AxisCalibSnapshot();
    g_jointReportPrinted = false;
    g_currentAxisTurn = 0;
  }

  setVelocity(0);
  startSettling();
}

void ServoCalibrateTrackingVelocity::start(float trackingFrequency, long /*instrumentCoordinateSteps*/) {
  // Expand 0 → all-axes mask, otherwise use the provided mask
  const uint8_t effectiveMask =
      (CALIBRATE_SERVO_AXIS_SELECT == 0) ? SERVO_CAL_ALL_AXES_MASK
                                         : CALIBRATE_SERVO_AXIS_SELECT;

  // Respect axis selection bitmask (skip if not requested)
  if (!(effectiveMask & (1U << (axisNumber - 1)))) {
    LOG_HDR(axisPrefix); VLF("skip");
    g_axisEnabledForCal[axisNumber] = false;
    return;
  }

  g_axisEnabledForCal[axisNumber] = true;

  LOG_HDR(axisPrefix); VLF("start");

  enabled = true;
  experimentMode = true;
  calibrationDirectionIsForward = true;
  calibrationState = CALIBRATION_U_BREAK;   // may be overridden to WAIT_TURN
  motorState = MOTOR_STOPPED;
  calibrationVelocity = SERVO_CALIBRATION_START_VELOCITY_PERCENT;
  targetVelocity = trackingFrequency;
  lastStateChangeTime = millis();

  staircaseIters = 0;
  currentSearchVelocity = calibrationVelocity;
  foundValidMotion = false;

  const bool bothEnabled = (effectiveMask == SERVO_CAL_ALL_AXES_MASK);
  if (bothEnabled) {
    if (g_currentAxisTurn == 0) {
      g_currentAxisTurn = 1; // Axis 1 starts
    }
    if (axisNumber != g_currentAxisTurn) {
      LOG_HDR(axisPrefix); VLF("wait-turn");
      calibrationState = CALIBRATION_WAIT_TURN;
      setVelocity(0);
      startSettling();
      return;
    }
  }

  setVelocity(0);
  startSettling();
}

void ServoCalibrateTrackingVelocity::updateState(long instrumentCoordinateSteps) {
  if(!enabled || !experimentMode) return;

  currentTime = millis();
  currentTicks = instrumentCoordinateSteps;

  handleMotorState();

  if (currentTime - lastStateChangeTime > SERVO_CALIBRATION_TIMEOUT) {
    LOG_HDR(axisPrefix); VLF("timeout → reset");
    handleCalibrationFailure();
    return;
  }

  if (calibrationState == CALIBRATION_WAIT_TURN) {
    if (g_currentAxisTurn == axisNumber) {
      calibrationState = CALIBRATION_U_BREAK;
      hasQueuedTest = false;
      queueNextTest(CALIBRATION_U_BREAK, calibrationDirectionIsForward
                                       ? SERVO_CALIBRATION_START_VELOCITY_PERCENT
                                       : -SERVO_CALIBRATION_START_VELOCITY_PERCENT);
      if (motorState != MOTOR_SETTLING) {
        setVelocity(0);
        startSettling();
      }
    }
    return;
  }

  if (calibrationState == CALIBRATION_WAIT_PEER) {
    uint8_t other = otherAxis(axisNumber);
    if (!g_axisEnabledForCal[other] || g_axisDone[other]) {
      if (!g_jointReportPrinted) {
        g_jointReportPrinted = true;
        printJointReport();
      }
      experimentMode = false;
      calibrationState = CALIBRATION_IDLE;
      setVelocity(0);
      LOG_HDR(axisPrefix); VLF("done");
    }
    return;
  }

  // Only run phase logic when a candidate outcome is resolvable
  if (motorState != MOTOR_RUNNING_STEADY && motorState != MOTOR_STOPPED) return;

  switch (calibrationState) {
    case CALIBRATION_U_BREAK:          processUBreak(); break;
    case CALIBRATION_U_HOLD:           processUHold();  break;
    case CALIBRATION_CHECK_IMBALANCE:  processImbalanceCheck(); break;
    default: break;
  }
}

// -------------------------------- Motor sub-state machine ---------------------------
void ServoCalibrateTrackingVelocity::handleMotorState() {
  switch (motorState) {
    case MOTOR_SETTLING: {
      float currentVelocity = calculateInstantaneousVelocity();
      if (currentTime - settleStartTime >= SERVO_CALIBRATION_MOTOR_SETTLE_TIME) {
        motorState = MOTOR_STOPPED;
        LOG_HDR(axisPrefix); VF("settled v="); V(currentVelocity); VLF(" sps");
        if (hasQueuedTest) {
          calibrationState = queuedState;
          hasQueuedTest = false;
          startTest(queuedVelocity);
        }
      }
      lastVelocityMeasurement = currentVelocity;
      lastVelocityTime = currentTime;
      break;
    }

    case MOTOR_KICKING:
      if (currentTime - kickStartTime >= SERVO_CALIBRATION_KICK_DURATION_MS) {
        setVelocity(calibrationVelocity);
        motorState = MOTOR_HOLDING;
        calibrationStepStartTime = currentTime;
        calibrationStepStartTicks = currentTicks;
        lastVelocityTime = currentTime;
        lastVelocityMeasurement = 0;
        consecutiveMovingSamples = 0;
        candidateSampleCount = 0;

        LOG_HDR(axisPrefix); LOG_DIR(calibrationDirectionIsForward);
        VF("kick→hold @"); V(calibrationVelocity); VLF("%");
      }
      break;

    case MOTOR_HOLDING:
      // Both UB and UH require N consecutive moving samples.
      // UB additionally has a hard sample cap to avoid endless waiting.
      if (currentTime - lastVelocityTime >= SERVO_CALIBRATION_VELOCITY_SETTLE_CHECK_INTERVAL) {
        float v_now = calculateInstantaneousVelocity();
        long  dCounts = lastDeltaCounts;

        const bool isUB = (calibrationState == CALIBRATION_U_BREAK);
        const int  requiredConsec = SERVO_CALIBRATION_REQUIRED_MOVING_SAMPLES;

        const bool movingNow =
          (fabsf(v_now) > SERVO_CALIBRATION_MIN_DETECTABLE_VELOCITY) &&
          (labs(dCounts) >= SERVO_CALIBRATION_MIN_COUNTS_PER_SAMPLE);

        // count total samples in this candidate (UB and UH)
        candidateSampleCount++;

        if (movingNow) {
          consecutiveMovingSamples++;
          LOG_HDR(axisPrefix); LOG_DIR(calibrationDirectionIsForward);
          VF(isUB ? PHASE_UB() : PHASE_UH());
          VF(" hold@"); V(calibrationVelocity); VF("% sps="); V(v_now);
          VF(" cnt="); V(dCounts); VF(" ok "); V(consecutiveMovingSamples);
          VF("/"); V(requiredConsec);
          if (consecutiveMovingSamples >= requiredConsec) VF(" → MOVE");
          VL("");

          if (consecutiveMovingSamples >= requiredConsec) {
            motorState = MOTOR_RUNNING_STEADY;     // accept candidate
            steadySinceMs = currentTime;
            steadyStartTicks = currentTicks;
          }
        } else {
          // failed sample → reset the consecutive counter for BOTH UB and UH
          consecutiveMovingSamples = 0;

          if (isUB) {
            // UB: keep sampling until cap, then fail if we never reached requiredConsec
            if (candidateSampleCount >= SERVO_CAL_UB_MAX_SAMPLES) {
              motorState = MOTOR_STOPPED;
              LOG_HDR(axisPrefix); VLF("HOLD_STALLED (UB: no consecutive move within cap)");
            }
            // else: keep waiting for next sample
          } else {
            // UH: strict — any failure cancels this candidate immediately
            motorState = MOTOR_STOPPED;
            LOG_HDR(axisPrefix); VLF("HOLD_STALLED");
          }
        }

        lastVelocityMeasurement = v_now;
        lastVelocityTime = currentTime;
      }
      break;

    case MOTOR_RUNNING_STEADY:
      // Nothing to do; phase logic will treat as "moved"
      break;

    case MOTOR_STOPPED:
      // Idle; phase logic will interpret as "not moved"
      break;
  }
}

// Compute instantaneous velocity in steps/s using elapsed time.
float ServoCalibrateTrackingVelocity::calculateInstantaneousVelocity() {
  if (lastCheckTime == 0) {
    lastCheckTime = currentTime;
    lastTicks = currentTicks;
    lastDeltaCounts = 0;
    return 0.0f;
  }

  float elapsed = (currentTime - lastCheckTime) / 1000.0f;
  if (elapsed <= 0) return 0.0f;

  long dCounts = (currentTicks - lastTicks);
  float velocity = dCounts / elapsed;

#ifdef SERVO_CAL_DEBUG
  VF("DBG: "); V(axisPrefix); VF(" vel%="); V(calibrationVelocity);
  VF(", dTicks="); V(dCounts); VF(", Vel="); V(velocity); VL(" steps/s");
#endif

  lastTicks = currentTicks;
  lastCheckTime = currentTime;
  lastDeltaCounts = dCounts;

  return velocity;
}

// ------------------------------- Helpers & Phase handlers ---------------------------

ServoCalibrateTrackingVelocity::DirectionData&
ServoCalibrateTrackingVelocity::currentDirectionData() {
  return calibrationDirectionIsForward ? fwdData : revData;
}

const ServoCalibrateTrackingVelocity::DirectionData&
ServoCalibrateTrackingVelocity::currentDirectionData() const {
  return calibrationDirectionIsForward ? fwdData : revData;
}

void ServoCalibrateTrackingVelocity::recordMovementData(float velocity, long counts) {
  DirectionData& data = currentDirectionData();
  if (calibrationState == CALIBRATION_U_BREAK) {
    data.breakVel    = velocity;
    data.breakCounts = counts;
  } else {
    data.holdVel     = velocity;
    data.holdCounts  = counts;
  }
}

void ServoCalibrateTrackingVelocity::logTestResult(const char* phase, bool moved) {
  LOG_HDR(axisPrefix); LOG_DIR(calibrationDirectionIsForward);
  V(phase); VF(": "); VF(moved ? "MOVE" : "STALL");
  VF(" @"); V(calibrationVelocity); VF("%");
  if (moved) { VF(" v≈"); V(lastVelocityMeasurement); VF(" sps"); }
  VL("");
}

// u_break: staircase upward from rest until we get a "moved" or hit limits
void ServoCalibrateTrackingVelocity::processUBreak() {
  if (++staircaseIters > SERVO_CALIBRATION_REFINE_MAX_ITERATIONS) {
    LOG_HDR(axisPrefix); VLF("UB iter-limit");
    if (!foundValidMotion) {
      currentDirectionData().u_break = SERVO_CALIBRATION_STOP_VELOCITY_PERCENT;
      LOG_HDR(axisPrefix); VF("UB none→max="); V(currentDirectionData().u_break); VLF("%");
    }
    proceedToUHold();
    return;
  }

  if (motorState != MOTOR_RUNNING_STEADY && motorState != MOTOR_STOPPED) return;

  const bool moved = (motorState == MOTOR_RUNNING_STEADY);
  const float currentAbsVelocity = fabsf(calibrationVelocity);

  LOG_HDR(axisPrefix); LOG_DIR(calibrationDirectionIsForward);
  VF("UB "); VF(moved ? "OK" : "NO"); VF(" @"); V(currentAbsVelocity); VLF("%");

  if (moved) {
    foundValidMotion = true;
    recordMovementData(lastVelocityMeasurement, lastDeltaCounts);
    currentDirectionData().u_break = currentAbsVelocity;
    currentDirectionData().everMoved = true;

    LOG_HDR(axisPrefix); LOG_DIR(calibrationDirectionIsForward);
    VF("UB set="); V(currentAbsVelocity); VF("% v≈"); V(lastVelocityMeasurement); VLF(" sps");
    proceedToUHold();
  } else {
    const float nextAbsVelocity = currentAbsVelocity + SERVO_CALIBRATION_STAIRCASE_STEP_PERCENT;

    if (nextAbsVelocity > SERVO_CALIBRATION_STOP_VELOCITY_PERCENT) {
      if (!foundValidMotion) {
        currentDirectionData().u_break = SERVO_CALIBRATION_STOP_VELOCITY_PERCENT;
        LOG_HDR(axisPrefix); VF("UB none→max="); V(currentDirectionData().u_break); VLF("%");
      }
      proceedToUHold();
    } else {
      calibrationVelocity = calibrationDirectionIsForward ? nextAbsVelocity : -nextAbsVelocity;
      startTest(calibrationVelocity);
    }
  }
}

// u_hold: staircase downward; each candidate must meet the N-consecutive-moving rule
void ServoCalibrateTrackingVelocity::processUHold() {
  if (++staircaseIters > SERVO_CALIBRATION_REFINE_MAX_ITERATIONS) {
    LOG_HDR(axisPrefix); VLF("UH iter-limit");
    finishUHold();
    return;
  }

  if (motorState != MOTOR_RUNNING_STEADY && motorState != MOTOR_STOPPED) return;

  const bool moved = (motorState == MOTOR_RUNNING_STEADY);
  const float currentAbsVelocity = fabsf(calibrationVelocity);
  const float u_break = currentDirectionData().u_break;

  if (moved) {
    recordMovementData(lastVelocityMeasurement, lastDeltaCounts);
    currentDirectionData().u_hold = currentAbsVelocity;

    logTestResult("u_hold", true);

    const float nextAbsVelocity = currentAbsVelocity - SERVO_CALIBRATION_STAIRCASE_STEP_PERCENT;

    if (nextAbsVelocity < SERVO_CALIBRATION_START_VELOCITY_PERCENT) {
      LOG_HDR(axisPrefix); VF("UH min="); V(currentDirectionData().u_hold); VLF("%");
      finishUHold();
    } else {
      // Kick at u_break to overcome stiction, then test the next-lower candidate
      calibrationVelocity = calibrationDirectionIsForward ? nextAbsVelocity : -nextAbsVelocity;
      setVelocity(calibrationDirectionIsForward ? u_break : -u_break);
      motorState = MOTOR_KICKING;
      kickStartTime = currentTime;
      lastStateChangeTime = currentTime;
    }
  } else {
    logTestResult("u_hold", false);

    if (currentDirectionData().u_hold == 0.0f) {
      currentDirectionData().u_hold = u_break; // fall back
      LOG_HDR(axisPrefix); VF("UH use UB="); V(currentDirectionData().u_hold); VLF("%");
    }
    finishUHold();
  }
}

void ServoCalibrateTrackingVelocity::proceedToUHold() {
  // Transition to u_hold: stop, settle, then queue first test below u_break
  setVelocity(0);
  startSettling();

  calibrationState = CALIBRATION_U_HOLD;
  staircaseIters = 0;

  const float u_break = currentDirectionData().u_break;
  currentSearchVelocity = max(u_break - SERVO_CALIBRATION_STAIRCASE_STEP_PERCENT,
                              SERVO_CALIBRATION_START_VELOCITY_PERCENT);

  LOG_HDR(axisPrefix); VF("UH first="); V(currentSearchVelocity); VLF("%");

  queueNextTest(CALIBRATION_U_HOLD, calibrationDirectionIsForward ? currentSearchVelocity : -currentSearchVelocity);
}

void ServoCalibrateTrackingVelocity::finishUHold() {
  setVelocity(0);
  startSettling();
  switchDirectionOrComplete();
}

void ServoCalibrateTrackingVelocity::switchDirectionOrComplete() {
  if (calibrationDirectionIsForward) {
    calibrationDirectionIsForward = false;
    calibrationState = CALIBRATION_U_BREAK;
    staircaseIters = 0;
    calibrationVelocity = -SERVO_CALIBRATION_START_VELOCITY_PERCENT;
    currentSearchVelocity = fabsf(calibrationVelocity);
    foundValidMotion = false;

    queueNextTest(CALIBRATION_U_BREAK, calibrationVelocity);
    LOG_HDR(axisPrefix); VLF("→ REVERSE");
  } else {
    calibrationState = CALIBRATION_CHECK_IMBALANCE;
    processImbalanceCheck();
  }
}

void ServoCalibrateTrackingVelocity::processImbalanceCheck() {
  snapshotAxisData(axisNumber, axisPrefix, fwdData, revData);

  if (fwdData.u_hold > 0 && revData.u_hold > 0) {
    float avgTracking = (fwdData.u_hold + revData.u_hold) / 2.0f;
    float imbalance = fabsf(fwdData.u_hold - revData.u_hold) / avgTracking * 100.0f;

    if (imbalance > SERVO_CALIBRATION_IMBALANCE_ERROR_THRESHOLD) {
      LOG_HDR(axisPrefix); VF("imbalance "); V(imbalance); VLF("%");
    }
  }

  g_axisDone[axisNumber] = true;

  uint8_t other = otherAxis(axisNumber);
  if (g_axisEnabledForCal[other] && !g_axisDone[other]) {
    g_currentAxisTurn = other; // pass the turn
  }

  const bool peerEnabled = g_axisEnabledForCal[other];
  const bool peerDone    = g_axisDone[other];

  if (peerEnabled && !peerDone) {
    calibrationState = CALIBRATION_WAIT_PEER;
    setVelocity(0);
    startSettling();
    return;
  }

  if (!g_jointReportPrinted) {
    g_jointReportPrinted = true;
    printJointReport();
  }

  experimentMode = false;
  calibrationState = CALIBRATION_IDLE;
  setVelocity(0);
  LOG_HDR(axisPrefix); VLF("done");
}

void ServoCalibrateTrackingVelocity::handleCalibrationFailure() {
  LOG_HDR(axisPrefix); VLF("ERR reset");
  experimentMode = false;
  calibrationState = CALIBRATION_IDLE;
  setVelocity(0);
  resetCalibrationValues();
  hasQueuedTest = false;
}

void ServoCalibrateTrackingVelocity::startSettling() {
  settleStartTime = currentTime;
  motorState = MOTOR_SETTLING;
}

void ServoCalibrateTrackingVelocity::startTest(float velocity) {
  calibrationVelocity = velocity;
  lastVelocityTime = currentTime;
  lastVelocityMeasurement = 0;
  lastStateChangeTime = currentTime;

  consecutiveMovingSamples = 0;
  candidateSampleCount = 0;

  if (calibrationState == CALIBRATION_U_BREAK) {
    setVelocity(calibrationVelocity);
    motorState = MOTOR_HOLDING;
    calibrationStepStartTime = currentTime;
    calibrationStepStartTicks = currentTicks;

    LOG_HDR(axisPrefix); LOG_DIR(calibrationDirectionIsForward);
    VF(PHASE_UB()); VF(" test @"); V(velocity); VLF("%");
  } else {
    float u_break = currentDirectionData().u_break;
    setVelocity(calibrationDirectionIsForward ? u_break : -u_break);
    motorState = MOTOR_KICKING;
    kickStartTime = currentTime;

    LOG_HDR(axisPrefix); LOG_DIR(calibrationDirectionIsForward);
    VF(PHASE_UH()); VF(" kick@"); V(u_break); VF("% → hold@"); V(velocity); VLF("%");
  }
}

void ServoCalibrateTrackingVelocity::setVelocity(float velocityPercent) {
  experimentVelocity = constrain(velocityPercent,
                                 -SERVO_CALIBRATION_STOP_VELOCITY_PERCENT,
                                 +SERVO_CALIBRATION_STOP_VELOCITY_PERCENT);
}

// -------- Getters -------------------------------------------------------------------
float ServoCalibrateTrackingVelocity::getU_break(bool forward) {
  return forward ? fwdData.u_break : revData.u_break;
}

float ServoCalibrateTrackingVelocity::getU_hold(bool forward) {
  return forward ? fwdData.u_hold : revData.u_hold;
}

// -------- Reports -------------------------------------------------------------------
void ServoCalibrateTrackingVelocity::printReport(const char* prefix,
                                                 const DirectionData& fwd,
                                                 const DirectionData& rev) {
  VF("\n=== Calibration Report: "); V(prefix); VLF(" ===");
  VLF("Dir  | u_break                         | u_hold");
  VLF("-----+---------------------------------+---------------------------------");

  auto printRow = [&](const char* dir, const DirectionData& data) {
    VF(dir); VF("  | ");
    VF("pct "); V(data.u_break); VF("%, v "); V(data.breakVel); VF(" steps/s, cnt "); V(data.breakCounts); VF("  | ");
    VF("pct "); V(data.u_hold);  VF("%, v "); V(data.holdVel);  VF(" steps/s, cnt "); V(data.holdCounts);  VL("");
  };

  printRow("FWD", fwd);
  printRow("REV", rev);

  float breakDeltaAbs = fabsf(fwd.u_break - rev.u_break);
  float holdDeltaAbs  = fabsf(rev.u_hold - fwd.u_hold);

  VF("Δ u_break: "); V(breakDeltaAbs); VF("%, ");
  VF("Δ u_hold: ");  V(holdDeltaAbs);  VLF("%");

  if (fwd.u_hold > 0.0f && rev.u_hold > 0.0f) {
    float avgTracking = (fwd.u_hold + rev.u_hold) / 2.0f;
    float holdImbalancePct = (avgTracking > 0.0f)
      ? (fabsf(fwd.u_hold - rev.u_hold) / avgTracking * 100.0f)
      : 0.0f;

    VF("u_hold imbalance vs avg: "); V(holdImbalancePct); VLF("%");

    if (holdImbalancePct > SERVO_CALIBRATION_IMBALANCE_ERROR_THRESHOLD) {
      VF("WARN: "); V(prefix); VF(" Imbalance: "); V(holdImbalancePct); VL("%");
    }
  }

  if (fwd.u_hold >= SERVO_CALIBRATION_STOP_VELOCITY_PERCENT - 0.1f) VLF("WARN: FWD velocity at calibration limit");
  if (rev.u_hold >= SERVO_CALIBRATION_STOP_VELOCITY_PERCENT - 0.1f) VLF("WARN: REV velocity at calibration limit");

  VF("=== End Report ===\n");
}

void ServoCalibrateTrackingVelocity::printReport() {
  printReport(axisPrefix, fwdData, revData);
}

void ServoCalibrateTrackingVelocity::resetCalibrationValues() {
  fwdData = DirectionData();
  revData = DirectionData();
}

#endif  // SERVO_MOTOR_PRESENT && CALIBRATE_SERVO_DC
