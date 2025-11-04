// -----------------------------------------------------------------------------------
// Servo Tracking Velocity Calibration - Overview
//
// This routine performs a 3-phase, bidirectional calibration to determine the
// velocity % needed to reliably track at sidereal velocity with a DC servo motor.
// The process measures the minimum and maximum duty cycles needed to overcome
// static friction (stiction) and identifies the lowest sustained velocity values
// required for continuous motion.
//
// Calibration logic (per direction) proceeds in 3 main steps:
//
// 1) Stiction Ceiling (stictionCeiling):
//    - Use exponential search (doubling the velocity value starting from
//      SERVO_CALIBRATION_START_VELOCITY_PERCENT) to find a velocity that initiates
//      any movement.
//    - This gives the upper bound for the motor's stiction threshold.
//    - Result is stored in stictionCeilingFwd or stictionCeilingRev.
//
// 2) Stiction Floor (stictionFloor):
//    - Refine the result using a binary search between
//      SERVO_CALIBRATION_VELOCITY_SEARCH_MIN_FACTOR * stictionCeiling
//      and stictionCeiling.
//    - This identifies the lowest velocity percentage that reliably causes movement.
//    - Between each test step, the motor is stopped for
//      SERVO_CALIBRATION_MOTOR_SETTLE_TIME to ensure clean restarts
//      and avoid carry-over motion.
//    - The final result is stored in stictionFloorFwd or stictionFloorRev.
//    - This velocity will also serve as a kickstarter during the next phase.
//
// 3) Velocity Tracking Calibration:
//    - This was an experiment and might be removed in the future. The idea was to check
//      whether if we kick-start the motor with the min breaking stiction velocity if we can lower gradually
//      the needed velocity to keep the motor going. If this was the case we could have the motor
//      rotating closer to the sidereal rotation.
//    - Starting from stictionFloor, a binary search is used to find the lowest
//      velocity that maintains continuous rotation after kickstarting the motor.
//    - The motor is kicked using stictionFloor before attempting a lower velocity.
//    - If the motor stalls or fails to rotate, velocity is increased again.
//    - The result is the trackingVelFwd or trackingVelRev, representing the duty
//      cycle needed to maintain sidereal tracking velocity with minimal power.
//
//    At least in my mesu-200 maxon 353611 motors this completely fails.
//
// After both directions are calibrated, an imbalance check is performed.
// If the forward/reverse duty cycles differ significantly, a warning is logged.
// This can indicate mount imbalance or frictional asymmetry.
//
// The result is a robust and safe calibration of stiction breaking and min velocity
// customized per motor and mechanical setup and telescope load.

#include "TrackingVelocity.h"

#if defined(SERVO_MOTOR_PRESENT) && defined(CALIBRATE_SERVO_DC)

// Make sure we are dealing with just the motors and no algorithms (plain linear mapping to PWM)
#ifdef SERVO_HYSTERESIS_ENABLE
  #undef SERVO_HYSTERESIS_ENABLE
#endif

#ifdef SERVO_STICTION_KICK
  #undef SERVO_STICTION_KICK
#endif

#ifdef SERVO_NONLINEAR_ENABLE
  #undef SERVO_NONLINEAR_ENABLE
#endif

ServoCalibrateTrackingVelocity::ServoCalibrateTrackingVelocity(uint8_t axisNumber) {
  this->axisNumber = axisNumber;
  snprintf(axisPrefix, sizeof(axisPrefix), "Axis%d ServoCalibration", axisNumber);
}

void ServoCalibrateTrackingVelocity::init() {
  experimentVelocity = 0.0f;
  experimentMode = false;
  calibrationState = CALIBRATION_IDLE;
  motorState = MOTOR_STOPPED;

  // Reset calibration values
  resetCalibrationValues();

  lastStateChangeTime = 0;
  calibrationStepStartTime = 0;
  calibrationStepStartTicks = 0;
  lastVelocityMeasurement = 0;
  lastVelocityTime = 0;

  lastTicks = 0;
  lastCheckTime = 0;
  lastDeltaCounts = 0;

  steadySinceMs = 0;
  steadyStartTicks = 0;

  enabled = false;
  everMovedFwd = false;
  everMovedRev = false;

  velIters = 0;
  bestLowVelocitySearchAbs = 0.0f;
  bestVelSearchErr = 1e9f; // sentinel "worst" error
  velSearchBestAvgVel = 0.0f;
  velSearchBestCounts = 0;

  refineIters = 0;

  // setup the queue
  hasQueuedTest = false;
  queuedState = CALIBRATION_IDLE;
  queuedVelocity = 0.0f;

  // stop the motor
  setVelocity(0);
  startSettling();
}

void ServoCalibrateTrackingVelocity::start(float trackingFrequency, long instrumentCoordinateSteps) {
  if (!(CALIBRATE_SERVO_AXIS_SELECT & (1 << (axisNumber - 1)))) {
    VF("MSG: "); V(axisPrefix); VLF(" Calibration skipped for this axis");
    return;
  }

  VF("MSG: "); V(axisPrefix); VL(" Starting 3-phase bidirectional calibration");

  // Initialize state machine
  enabled = true;
  experimentMode = true;
  calibrationDirectionIsForward = true;
  calibrationState = CALIBRATION_STICTION_CEILING;
  motorState = MOTOR_STOPPED;
  calibrationVelocity = SERVO_CALIBRATION_START_VELOCITY_PERCENT;
  targetVelocity = trackingFrequency;
  lastStateChangeTime = millis();

  // Start by stopping motor and waiting for settle
  setVelocity(0);
  startSettling();
}

void ServoCalibrateTrackingVelocity::updateState(long instrumentCoordinateSteps) {
  if(!enabled || !experimentMode) return;

  currentTime = millis();
  currentTicks = instrumentCoordinateSteps;

  // Handle motor state transitions
  handleMotorState();

  // State timeout check (safety feature)
  if (currentTime - lastStateChangeTime > SERVO_CALIBRATION_TIMEOUT ) {
    VF("WARN: "); V(axisPrefix); VLF(" State timeout, resetting calibration");
    handleCalibrationFailure();
    return;
  }

  // Only process state machine when motor is in steady state or stopped
  if (motorState != MOTOR_RUNNING_STEADY && motorState != MOTOR_STOPPED) return;

  // Main state machine processing
  switch (calibrationState) {
    case CALIBRATION_IDLE:
      break;

    case CALIBRATION_STICTION_CEILING:
      processStictionCeiling();
      break;

    case CALIBRATION_STICTION_FLOOR:
      processStictionFloor();
      break;

    case CALIBRATION_VELOCITY_SEARCH:
      processVelocitySearch();
      break;

    case CALIBRATION_CHECK_IMBALANCE:
      processImbalanceCheck();
      break;

    default:
      break;
  }
}

// Private helper methods //////////////////////////////////////////////////////

void ServoCalibrateTrackingVelocity::handleMotorState() {
  float currentVelocity = 0.0;
  switch (motorState) {
    case MOTOR_SETTLING:
      // check also the velocity
      currentVelocity = calculateInstantaneousVelocity();
      if (currentTime - settleStartTime >= SERVO_CALIBRATION_MOTOR_SETTLE_TIME) {
        motorState = MOTOR_STOPPED;
        V(axisPrefix); VF(": Motor settled to STOPPED_STATE"); VF(" Vel="); V(currentVelocity); VL(" steps/s");

        // NOW safe to start the next test, if any
        if (hasQueuedTest) {
          // if we were kicking the motor
          kickToFloorVelocity = false;
          calibrationState = queuedState;
          hasQueuedTest = false;
          startTest(queuedVelocity);
        }
      }
      lastVelocityMeasurement = currentVelocity;
      lastVelocityTime = currentTime;

      break;

    case MOTOR_ACCELERATING:
      // Check if motor has reached steady state
      if (currentTime - lastVelocityTime >= SERVO_CALIBRATION_VELOCITY_SETTLE_CHECK_INTERVAL) {
        currentVelocity = calculateInstantaneousVelocity();
        float velocityChange = fabsf(currentVelocity - lastVelocityMeasurement);

        if (fabsf(currentVelocity) > SERVO_CALIBRATION_MIN_DETECTABLE_VELOCITY &&
            fabsf(velocityChange) < SERVO_CALIBRATION_VELOCITY_STABILITY_THRESHOLD) {

          motorState = MOTOR_RUNNING_STEADY;
          calibrationStepStartTime = currentTime;
          calibrationStepStartTicks = currentTicks;

          // NEW: start steady window anchors for sustained stiction check
          steadySinceMs = currentTime;
          steadyStartTicks = currentTicks;

          V(axisPrefix);
          if (calibrationDirectionIsForward) { VF(" FWD"); } else { VF(" REV"); }
          VF(": STEADY_STATE  @ VelocityPercent="); V(calibrationVelocity); VF("%");
          VF(" Vel="); V(currentVelocity); VL(" steps/s");

          // NOW safe to start the next test, if any
          if (hasQueuedTest) {
            // if we were kicking the motor
            kickToFloorVelocity = false;
            calibrationState = queuedState;
            hasQueuedTest = false;
            startTest(queuedVelocity);
          }

        } else if (fabsf(currentVelocity) <= SERVO_CALIBRATION_MIN_DETECTABLE_VELOCITY) {
          // The motor never managed to
          motorState = MOTOR_STOPPED;
          calibrationStepStartTime = currentTime;
          calibrationStepStartTicks = currentTicks;

          V(axisPrefix);
          if (calibrationDirectionIsForward) { VF(" FWD"); } else { VF(" REV"); }
          VF(": STOPPED_STATE  @ Velocity (%)="); V(calibrationVelocity); VF("%");
          VF(" Vel="); V(currentVelocity); VL(" steps/s");
        }

        lastVelocityMeasurement = currentVelocity;
        lastVelocityTime = currentTime;
      }
      break;

    case MOTOR_RUNNING_STEADY:
      // Motor is running steady, nothing to do (ceil/floor/velocity code will read window)
      break;

    case MOTOR_STOPPED:
      // Nothing to do here, waiting for state machine to start next test
      break;
  }
}

// compute current speed
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
  float velocity = dCounts / elapsed; // steps/sec

#ifdef SERVO_CAL_DEBUG
  VF("DBG: "); V(axisPrefix);VF(" Velocity (%) ="); V(calibrationVelocity);
  VF(" dTicks="); V(dCounts);
  VF(" dt(ms)=");V(currentTime - lastCheckTime);
  VF(" Vel="); V(velocity); VL(" steps/s");
#endif

  lastTicks = currentTicks;
  lastCheckTime = currentTime;
  lastDeltaCounts = dCounts; // cache raw counts used for this instant velocity

  return velocity;
}

// This function checks whether stiction is broken. Else it does an exponential search over the velocity percentage space
// clamped to SERVO_CALIBRATION_STOP_VELOCITY_PERCENT
void ServoCalibrateTrackingVelocity::processStictionCeiling() {
  // NEW: Use sustained average while RUNNING_STEADY to decide "broken"
  if (motorState == MOTOR_RUNNING_STEADY) {
    unsigned long steadyElapsedMs = currentTime - steadySinceMs;
    if (steadyElapsedMs < SERVO_CALIBRATION_STICTION_SAMPLE_INTERVAL_MS) return;

    float elapsedSec = steadyElapsedMs / 1000.0f;
    long  dCounts    = currentTicks - steadyStartTicks;
    float avgVel     = dCounts / elapsedSec;

    if (fabsf(avgVel) > SERVO_CALIBRATION_MIN_DETECTABLE_VELOCITY) {
      // motion detected and sustained at this velocity
      float absVelocity = fabsf(calibrationVelocity);
      float& stictionMax = calibrationDirectionIsForward ? stictionCeilingFwd : stictionCeilingRev;
      if (calibrationDirectionIsForward) {
        everMovedFwd = true;
        stictionCeilingVelFwd = avgVel;
        stictionCeilingCountsFwd = dCounts;
      } else {
        everMovedRev = true;
        stictionCeilingVelRev = avgVel;
        stictionCeilingCountsRev = dCounts;
      }
      stictionMax = absVelocity;

      V(axisPrefix);
      if (calibrationDirectionIsForward) { VF(" FWD"); } else { VF(" REV"); }
      VF(" break: BROKEN STICTION @ Velocity % ="); V(calibrationVelocity);
      VF(" Vel="); V(avgVel); VL(" steps/s");

      setVelocity(0);
      // non-blocking settle:
      settleStartTime = currentTime;
      motorState = MOTOR_SETTLING;

      transitionToRefine();
      return;
    } else {
      // Treat as no sustained motion → exponential step up (with clamp)
      V(axisPrefix);
      if (calibrationDirectionIsForward) { VF(" FWD"); } else { VF(" REV"); }
      VF(" break: NO SUSTAINED MOVE @ Velocity % ="); V(calibrationVelocity);
      VF(" avgVel="); V(avgVel); VL(" steps/s");

      calibrationVelocity = copysignf(
        fminf(fabsf(calibrationVelocity) * 2.0f, SERVO_CALIBRATION_STOP_VELOCITY_PERCENT),
        calibrationVelocity
      );

      if (fabsf(calibrationVelocity) >= SERVO_CALIBRATION_STOP_VELOCITY_PERCENT) {
        float& stictionMax = calibrationDirectionIsForward ? stictionCeilingFwd : stictionCeilingRev;
        stictionMax = SERVO_CALIBRATION_STOP_VELOCITY_PERCENT;
        bool ever = calibrationDirectionIsForward ? everMovedFwd : everMovedRev;
        if (!ever) {
          V(axisPrefix); VF(calibrationDirectionIsForward ? " FWD" : " REV");
          VLF(" break: COULD NOT BREAK STICTION up to SERVO_CALIBRATION_STOP_VELOCITY_PERCENT – skipping refine");
          setVelocity(0);
          startSettling();
          if (calibrationDirectionIsForward) {
            calibrationDirectionIsForward = false;
            queueNextTest(CALIBRATION_STICTION_CEILING, -SERVO_CALIBRATION_START_VELOCITY_PERCENT);
          } else {
            calibrationState = CALIBRATION_CHECK_IMBALANCE;
            processImbalanceCheck();
          }
          return;
        }
        transitionToRefine();
      } else {
        // Re-apply increased velocity and let state machine re-enter steady
        startTest(calibrationVelocity);
      }
      return;
    }
  }

  // If not steady yet, wait for RUNNING_STEADY (ACCELERATING or STOPPED are handled elsewhere)
  if (motorState == MOTOR_STOPPED) {
    // We can still issue a NO MOVE step up if we're stopped and the sample interval elapsed
    if (currentTime - lastVelocityTime < SERVO_CALIBRATION_STICTION_SAMPLE_INTERVAL_MS) return;

    float velocity = calculateInstantaneousVelocity();
    lastVelocityTime = currentTime;

    V(axisPrefix);
    if (calibrationDirectionIsForward) { VF(" FWD"); } else { VF(" REV"); }
    VF(" break: NO MOVE @ Velocity % ="); V(calibrationVelocity);
    VF(" Vel="); V(velocity); VL(" steps/s");

    calibrationVelocity = copysignf(
      fminf(fabsf(calibrationVelocity) * 2.0f, SERVO_CALIBRATION_STOP_VELOCITY_PERCENT),
      calibrationVelocity
    );

    if (fabsf(calibrationVelocity) >= SERVO_CALIBRATION_STOP_VELOCITY_PERCENT) {
      float& stictionMax = calibrationDirectionIsForward ? stictionCeilingFwd : stictionCeilingRev;
      stictionMax = SERVO_CALIBRATION_STOP_VELOCITY_PERCENT;
      bool ever = calibrationDirectionIsForward ? everMovedFwd : everMovedRev;
      if (!ever) {
        V(axisPrefix); VF(calibrationDirectionIsForward ? " FWD" : " REV");
        VLF(" break: COULD NOT BREAK STICTION up to SERVO_CALIBRATION_STOP_VELOCITY_PERCENT – skipping refine");
        setVelocity(0);
        startSettling();
        if (calibrationDirectionIsForward) {
          calibrationDirectionIsForward = false;
          queueNextTest(CALIBRATION_STICTION_CEILING, -SERVO_CALIBRATION_START_VELOCITY_PERCENT);
        } else {
          calibrationState = CALIBRATION_CHECK_IMBALANCE;
          processImbalanceCheck();
        }
        return;
      }
      transitionToRefine();
    } else {
      startTest(calibrationVelocity);
    }
  }
}

void ServoCalibrateTrackingVelocity::transitionToRefine() {
  // Enter the stiction floor (refine) phase
  calibrationState = CALIBRATION_STICTION_FLOOR;
  lastStateChangeTime = currentTime;

  const float stictionMax = calibrationDirectionIsForward
                              ? stictionCeilingFwd
                              : stictionCeilingRev;

  // Initial bounds for binary search
  calibrationMinVelocity = SERVO_CALIBRATION_VELOCITY_SEARCH_MIN_FACTOR * stictionMax;
  if (calibrationMinVelocity < 0.0f) calibrationMinVelocity = 0.0f;   // safety clamp
  calibrationMaxVelocity = stictionMax;

  float mid = 0.5f * (calibrationMinVelocity + calibrationMaxVelocity);
  calibrationVelocity = calibrationDirectionIsForward ? mid : -mid;

  refineIters = 0;
  refineSawMove = false;

  // Start the first refine test at the midpoint velocity
  startTest(calibrationVelocity);
}

void ServoCalibrateTrackingVelocity::processStictionFloor() {
  // Up to REFINE_MAX_ITERATIONS
  if (++refineIters > SERVO_CALIBRATION_REFINE_MAX_ITERATIONS) {
    VLF("WARN: refine iteration cap reached; using current bounds");
    calibrationMaxVelocity = fmaxf(calibrationMaxVelocity, calibrationMinVelocity);
     // fall through to convergence
  }

  // We only evaluate after a test has resolved to a steady state or stop.
  if (motorState != MOTOR_RUNNING_STEADY && motorState != MOTOR_STOPPED) return;

  const bool moved = (motorState == MOTOR_RUNNING_STEADY);

  if (!moved) {
    // No movement → raise the lower bound
    calibrationMinVelocity = fabsf(calibrationVelocity);

    V(axisPrefix);
    if (calibrationDirectionIsForward) { VF(" FWD"); } else { VF(" REV"); }
    VF(" refine: NO MOVE @ VELOCITY="); V(calibrationVelocity); VLF("%");
  } else {
    // Movement → lower the upper bound
    calibrationMaxVelocity = fabsf(calibrationVelocity);
    refineSawMove = true;

    // Record measured velocity and counts for the last MOVE
    if (calibrationDirectionIsForward) {
      stictionFloorVelFwd    = lastVelocityMeasurement;
      stictionFloorCountsFwd = lastDeltaCounts;
    } else {
      stictionFloorVelRev    = lastVelocityMeasurement;
      stictionFloorCountsRev = lastDeltaCounts;
    }

    V(axisPrefix);
    if (calibrationDirectionIsForward) { VF(" FWD"); } else { VF(" REV"); }
    VF(" refine: MOVE  @ VELOCITY="); V(calibrationVelocity);
    VF("%, v≈"); V(lastVelocityMeasurement); VL(" steps/s");
  }

  // Convergence check (need at least one MOVE, and span within abs/rel tol)
  float span = calibrationMaxVelocity - calibrationMinVelocity;
  float tolAbs = SERVO_CALIBRATION_STICTION_REFINE_ABS;
  float tolRel = SERVO_CALIBRATION_STICTION_REFINE_REL * calibrationMaxVelocity; // ~ vs ceiling
  float tol = fmaxf(tolAbs, tolRel);
  if (refineSawMove && span <= tol) {
    // Conservative pick: the smallest velocity percent known to MOVE
    const float resultMin = calibrationMaxVelocity;
    const float lastNoMoveAbs = calibrationMinVelocity;      // <-- highest known stall from refine

    // Store floor for reporting
    float& stictionMin = calibrationDirectionIsForward ? stictionFloorFwd : stictionFloorRev;
    stictionMin = resultMin;

    // If there’s no meaningful headroom below floor, accept floor and move on
    if (resultMin - lastNoMoveAbs <= tol) {
      float& trackingVelocity = calibrationDirectionIsForward ? trackingVelocityFwd : trackingVelocityRev;
      trackingVelocity = resultMin;

      // If tracking = floor, reuse floor's measured values
      if (calibrationDirectionIsForward) {
        trackingVelocityMeasuredFwd = stictionFloorVelFwd;
        trackingCountsFwd = stictionFloorCountsFwd;
      } else {
        trackingVelocityMeasuredRev = stictionFloorVelRev;
        trackingCountsRev = stictionFloorCountsRev;
      }

      V(axisPrefix); VF(calibrationDirectionIsForward ? " FWD" : " REV");
      VF(" floor used as minimum after stiction broken VELOCITY="); V(trackingVelocity); VLF("% (no headroom)");
      setVelocity(0); startSettling();
      if (calibrationDirectionIsForward) {
        calibrationDirectionIsForward = false;
        queueNextTest(CALIBRATION_STICTION_CEILING, -SERVO_CALIBRATION_START_VELOCITY_PERCENT);
      } else {
        calibrationState = CALIBRATION_CHECK_IMBALANCE;
        processImbalanceCheck();
      }
      return;
    }

    // ---- Enter velocity search (always below the floor) ----
    velIters = 0;
    bestVelSearchErr = 1e9f;
    bestLowVelocitySearchAbs = resultMin;  // worst-case is the floor
    velSearchBestAvgVel = 0.0f;
    velSearchBestCounts = 0;
    calibrationState = CALIBRATION_VELOCITY_SEARCH;

    const float floorAbs = resultMin;
    calibrationMinVelocity = lastNoMoveAbs;  // start from highest known stall
    calibrationMaxVelocity = floorAbs;       // up to the floor

    // first probe strictly below floor
    float mid = 0.5f * (calibrationMinVelocity + calibrationMaxVelocity);
    calibrationVelocity = (calibrationDirectionIsForward ? +mid : -mid);

    // prime a kick at floor, then run the queued test at "mid"
    const float sign = calibrationDirectionIsForward ? 1.0f : -1.0f;
    queueNextTest(CALIBRATION_VELOCITY_SEARCH, calibrationVelocity);

    setVelocity(sign * floorAbs); // kick at floor
    motorState = MOTOR_ACCELERATING;
    lastStateChangeTime = currentTime;
    lastVelocityTime = currentTime;
    lastVelocityMeasurement = 0;
    V(axisPrefix); VF(calibrationDirectionIsForward ? " FWD" : " REV");
    VF(" kick: floor Velocity="); V(floorAbs); VL("%");
    return;
  }

  // Continue binary search: midpoint of [min, max]
  float mid = 0.5f * (calibrationMinVelocity + calibrationMaxVelocity);
  calibrationVelocity = calibrationDirectionIsForward ? mid : -mid;

  // Start the next test; handleMotorState() will advance to RUNNING_STEADY or STOPPED
  startTest(calibrationVelocity);
}

void ServoCalibrateTrackingVelocity::processVelocitySearch() {
  // Only act when either the motor is steady (to evaluate) or stopped (treat as too low).
  if (motorState != MOTOR_RUNNING_STEADY && motorState != MOTOR_STOPPED) return;

  const float sign     = calibrationDirectionIsForward ? 1.0f : -1.0f;
  const float floorAbs = calibrationDirectionIsForward ? stictionFloorFwd : stictionFloorRev;
  const float velocityAbs   = fabsf(calibrationVelocity);

  // tolerances used for convergence
  const float tolAbs = SERVO_CALIBRATION_STICTION_REFINE_ABS;
  const float tolRel = SERVO_CALIBRATION_STICTION_REFINE_REL * floorAbs;
  const float tol    = fmaxf(tolAbs, tolRel);

  // If our lower bound is already at (or within tol of) the floor, use floor
  if (calibrationMinVelocity >= floorAbs - tol) {
    float& trackingVelocity = calibrationDirectionIsForward ? trackingVelocityFwd : trackingVelocityRev;
    trackingVelocity = floorAbs;

    // measured velocity & counts: prefer best search sample, fallback to floor sample
    if (calibrationDirectionIsForward) {
      trackingVelocityMeasuredFwd = (velSearchBestAvgVel != 0.0f) ? velSearchBestAvgVel : stictionFloorVelFwd;
      trackingCountsFwd           = (velSearchBestCounts   != 0  ) ? velSearchBestCounts   : stictionFloorCountsFwd;
    } else {
      trackingVelocityMeasuredRev = (velSearchBestAvgVel != 0.0f) ? velSearchBestAvgVel : stictionFloorVelRev;
      trackingCountsRev           = (velSearchBestCounts   != 0  ) ? velSearchBestCounts   : stictionFloorCountsRev;
    }

    VF("MSG: "); V(axisPrefix);
    VF(calibrationDirectionIsForward ? " FWD" : " REV");
    VF(" velocity search: pinned at floor, using VELOCITY="); V(trackingVelocity); VLF("%");
    goto velocity_search_done;
  }

  if (motorState == MOTOR_STOPPED) {
    // too low -> raise lower bound, keep within [0,floor]
    calibrationMinVelocity = fminf(floorAbs, fmaxf(calibrationMinVelocity, velocityAbs));
  } else {
    // steady -> measure and adjust bounds (still within [0,floor])
    const unsigned long dt_ms = currentTime - calibrationStepStartTime;
    if (dt_ms < SERVO_CALIBRATION_VELOCITY_MEASURE_WINDOW_MS) return;

    const float elapsedSec = dt_ms / 1000.0f;
    const long  dCounts    = currentTicks - calibrationStepStartTicks;
    const float avgVel     = (float)dCounts / elapsedSec; // steps/sec (signed)

    // Compare by magnitude (direction set by 'sign')
    const float targetMag  = fabsf(targetVelocity);
    const float actualMag  = fabsf(avgVel);

    // Degenerate: no valid target; accept current velocity and exit this phase
    if (targetMag <= 0.0f) {
      float& trackingVelocity = calibrationDirectionIsForward ? trackingVelocityFwd : trackingVelocityRev;
      trackingVelocity = velocityAbs; // degenerate case
      if (calibrationDirectionIsForward) {
        trackingVelocityMeasuredFwd = avgVel;
        trackingCountsFwd = dCounts;
      } else {
        trackingVelocityMeasuredRev = avgVel;
        trackingCountsRev = dCounts;
      }
      V(axisPrefix); VLF(": velocity search skipped (target=0)");
      goto velocity_search_done;
    }

    // Track best-so-far for graceful bailout
    const float errPct = fabsf(actualMag - targetMag) / targetMag * 100.0f;
    if (errPct < bestVelSearchErr) {
      bestVelSearchErr       = errPct;
      bestLowVelocitySearchAbs = velocityAbs;
      velSearchBestAvgVel    = avgVel;
      velSearchBestCounts    = dCounts;
    }
    // Decide next bounds from whether we're too slow/fast
    if (actualMag < targetMag) {
      calibrationMinVelocity = fminf(floorAbs, fmaxf(calibrationMinVelocity, velocityAbs));
    } else {
      calibrationMaxVelocity = fminf(floorAbs, fmaxf(0.0f, fminf(calibrationMaxVelocity, velocityAbs)));
    }

    // Success?
    if (errPct <= SERVO_CALIBRATION_ERROR_THRESHOLD) {
      float& trackingVelocity = calibrationDirectionIsForward ? trackingVelocityFwd : trackingVelocityRev;
      trackingVelocity = velocityAbs;

      if (calibrationDirectionIsForward) {
        trackingVelocityMeasuredFwd = avgVel;
        trackingCountsFwd = dCounts;
      } else {
        trackingVelocityMeasuredRev = avgVel;
        trackingCountsRev = dCounts;
      }

      VF("MSG: "); V(axisPrefix);
      VF(calibrationDirectionIsForward ? " FWD" : " REV");
      VF(" velocity OK: Velocity ="); V(trackingVelocity);VLF("%");
      VF("%, v="); V(avgVel); VF(" steps/s, err="); V(errPct); VLF("%");
      goto velocity_search_done;
    }
  }

  // Convergence / iteration cap: worst case is the floor
  if ((calibrationMaxVelocity - calibrationMinVelocity) <= tol || ++velIters > SERVO_CALIBRATION_REFINE_MAX_ITERATIONS) {
    float& trackingVelocity = calibrationDirectionIsForward ? trackingVelocityFwd : trackingVelocityRev;
    trackingVelocity = calibrationMaxVelocity;                  // <= floorAbs

    // measured velocity: prefer best measured avg, otherwise fallback to floor measurement
    if (calibrationDirectionIsForward) {
      trackingVelocityMeasuredFwd = (velSearchBestAvgVel != 0.0f) ? velSearchBestAvgVel : stictionFloorVelFwd;
      trackingCountsFwd           = (velSearchBestCounts   != 0  ) ? velSearchBestCounts   : stictionFloorCountsFwd;
    } else {
      trackingVelocityMeasuredRev = (velSearchBestAvgVel != 0.0f) ? velSearchBestAvgVel : stictionFloorVelRev;
      trackingCountsRev           = (velSearchBestCounts   != 0  ) ? velSearchBestCounts   : stictionFloorCountsRev;
    }

    if (trackingVelocity < floorAbs - tol) {
      VF("MSG: "); V(axisPrefix);
      VF(calibrationDirectionIsForward ? " FWD" : " REV");
      VF(" velocity search: below-floor VELOCITY="); V(trackingVelocity); VLF("%");
    } else {
      // worst-case: use the floor
      trackingVelocity = floorAbs;
      VF("MSG: "); V(axisPrefix);
      VF(calibrationDirectionIsForward ? " FWD" : " REV");
      VF(" velocity search: using floor VELOCITY="); V(trackingVelocity); VLF("%");
    }
    goto velocity_search_done;
  }

  {
    // Next midpoint strictly below (or equal to) the floor.
    float midAbs = 0.5f * (calibrationMinVelocity + calibrationMaxVelocity);
    // kick at floor, then queue the test at midAbs
    setVelocity(sign * floorAbs);
    motorState = MOTOR_ACCELERATING;
    lastStateChangeTime = currentTime;
    lastVelocityTime = currentTime;
    lastVelocityMeasurement = 0;

    queueNextTest(CALIBRATION_VELOCITY_SEARCH, sign * midAbs);
    V(axisPrefix); VF(calibrationDirectionIsForward ? " FWD" : " REV");
    VF(" kick: floor velocity ="); V(floorAbs); VF("% -> next test at "); V(midAbs); VLF("%");
    return;
  }

velocity_search_done:
  setVelocity(0);
  startSettling();
  if (calibrationDirectionIsForward) {
    // Switch to reverse direction starting again from stiction ceiling
    calibrationDirectionIsForward = false;
    calibrationState = CALIBRATION_STICTION_CEILING;
    // Queue the next test, since we first have to make sure the motor are stopped
    queueNextTest(CALIBRATION_STICTION_CEILING, -SERVO_CALIBRATION_START_VELOCITY_PERCENT);
  } else {
    calibrationState = CALIBRATION_CHECK_IMBALANCE;
    processImbalanceCheck();
  }
}

void ServoCalibrateTrackingVelocity::processImbalanceCheck() {
  // Print calibration report
  printReport();

  // Check for significant imbalance
  if (trackingVelocityFwd > 0 && trackingVelocityRev > 0) {
    float avgTracking = (trackingVelocityFwd + trackingVelocityRev) / 2.0f;
    float imbalance = fabsf(trackingVelocityFwd - trackingVelocityRev) / avgTracking * 100.0f;

    if (imbalance > SERVO_CALIBRATION_IMBALANCE_ERROR_THRESHOLD) {
      VF("WARN: "); V(axisPrefix);
      VF(" Significant imbalance: "); V(imbalance); VL("%");
    }
  }

  // Calibration complete
  experimentMode = false;
  calibrationState = CALIBRATION_IDLE;
  setVelocity(0);
  VF("MSG: "); V(axisPrefix); VL(" complete");
}

void ServoCalibrateTrackingVelocity::handleCalibrationFailure() {
  VF("ERR: "); V(axisPrefix); VL("Calibration failed, resetting");

  // Reset to safe state
  experimentMode = false;
  calibrationState = CALIBRATION_IDLE;
  setVelocity(0);

  // Set default values
  resetCalibrationValues();

  hasQueuedTest = false; // cancel any queued action
}

void ServoCalibrateTrackingVelocity::startSettling() {
  settleStartTime = currentTime;
  motorState = MOTOR_SETTLING;
  VF("DBG: "); V(axisPrefix); VF(" Starting settling for ");
  V(SERVO_CALIBRATION_MOTOR_SETTLE_TIME); VL("ms");
}

void ServoCalibrateTrackingVelocity::startTest(float velocity) {
  calibrationVelocity = velocity;
  setVelocity(velocity);
  lastVelocityTime = currentTime;
  lastVelocityMeasurement = 0;
  motorState = MOTOR_ACCELERATING;
  lastStateChangeTime = currentTime;

  V(axisPrefix);
  if (calibrationDirectionIsForward) {VF(" FWD");} else {VF(" REV");}
  VF(" Starting test at VELOCITY="); V(velocity); VL("%");
}

void ServoCalibrateTrackingVelocity::setVelocity(float velocityPercent) {
  // Clamp percent
  float clamped = constrain(velocityPercent,
                            -SERVO_CALIBRATION_STOP_VELOCITY_PERCENT,
                            +SERVO_CALIBRATION_STOP_VELOCITY_PERCENT);

  experimentVelocity = clamped; // telemetry in percent
}

// Getters ////////////////////////////////////////////////////////////////////
float ServoCalibrateTrackingVelocity::getStictionCeiling(bool forward) {
  return forward ? stictionCeilingFwd : stictionCeilingRev;
}

float ServoCalibrateTrackingVelocity::getStictionFloor(bool forward) {
  return forward ? stictionFloorFwd : stictionFloorRev;
}

float ServoCalibrateTrackingVelocity::getTrackingVelocity(bool forward) {
  return forward ? trackingVelocityFwd : trackingVelocityRev;
}

// --- Compact report helpers
static inline void prDirHeader(const char* dir) {
  VF(dir); VF("  | ");
}
static inline void prPctVelCnt(float pct, float vel, long cnt) {
  VF("pct "); V(pct); VF("%, v "); V(vel); VF(" steps/s, cnt "); V(cnt);
}

void ServoCalibrateTrackingVelocity::printReport() {
  VF("\n=== Calibration Report: "); V(axisPrefix); VLF(" ===");

  // Header
  VLF("Dir  | Ceiling                         | Floor                           | Tracking");
  VLF("-----+---------------------------------+---------------------------------+---------------------------------");

  // FWD row
  prDirHeader("FWD");
  prPctVelCnt(stictionCeilingFwd, stictionCeilingVelFwd, stictionCeilingCountsFwd); VF("  | ");
  prPctVelCnt(stictionFloorFwd,   stictionFloorVelFwd,   stictionFloorCountsFwd);   VF("  | ");
  prPctVelCnt(trackingVelocityFwd, trackingVelocityMeasuredFwd, trackingCountsFwd); VL("");

  // REV row
  prDirHeader("REV");
  prPctVelCnt(stictionCeilingRev, stictionCeilingVelRev, stictionCeilingCountsRev); VF("  | ");
  prPctVelCnt(stictionFloorRev,   stictionFloorVelRev,   stictionFloorCountsRev);   VF("  | ");
  prPctVelCnt(trackingVelocityRev, trackingVelocityMeasuredRev, trackingCountsRev); VL("");

  // Imbalance (use magnitudes of % tracking/floor)
  float stictionImbalance = fabsf(stictionFloorFwd - stictionFloorRev);
  float trackingImbalance = fabsf(trackingVelocityFwd - trackingVelocityRev);

  VF("Δ Stiction Floor: "); V(stictionImbalance); VF("%, ");
  VF("Δ Tracking: "); V(trackingImbalance); VLF("%");

  if (trackingVelocityFwd >= SERVO_CALIBRATION_STOP_VELOCITY_PERCENT - 0.1f) {
    VLF("WARN: FWD velocity at calibration limit");
  }
  if (trackingVelocityRev >= SERVO_CALIBRATION_STOP_VELOCITY_PERCENT - 0.1f) {
    VLF("WARN: REV velocity at calibration limit");
  }

  VF("=== End Report ===\n");
}

void ServoCalibrateTrackingVelocity::resetCalibrationValues(void) {
  stictionCeilingFwd = SERVO_CALIBRATION_STOP_VELOCITY_PERCENT;
  stictionFloorFwd = 0.0f;
  trackingVelocityFwd = 0.0f;
  stictionCeilingRev = SERVO_CALIBRATION_STOP_VELOCITY_PERCENT;
  stictionFloorRev = 0.0f;
  trackingVelocityRev = 0.0f;

  // Measured velocities (steps/s) & counts
  stictionCeilingVelFwd = 0.0f;
  stictionFloorVelFwd   = 0.0f;
  trackingVelocityMeasuredFwd = 0.0f;
  stictionCeilingCountsFwd = 0;
  stictionFloorCountsFwd   = 0;
  trackingCountsFwd        = 0;

  stictionCeilingVelRev = 0.0f;
  stictionFloorVelRev   = 0.0f;
  trackingVelocityMeasuredRev = 0.0f;
  stictionCeilingCountsRev = 0;
  stictionFloorCountsRev   = 0;
  trackingCountsRev        = 0;
}

#endif
