// -----------------------------------------------------------------------------------
// Servo Tracking Velocity Calibration - Overview
//
// This routine performs a 3-phase, bidirectional calibration to determine the
// PWM duty cycles needed to reliably track at sidereal velocity with a DC servo motor.
// The process measures the minimum and maximum duty cycles needed to overcome
// static friction (stiction) and identifies the lowest sustained PWM values
// required for continuous motion.
//
// Calibration logic (per direction) proceeds in 3 main steps:
//
// 1) Stiction Ceiling (stictionCeiling):
//    - Use exponential search (doubling the PWM value starting from
//      SERVO_CALIBRATION_START_DUTY_CYCLE) to find a PWM that initiates
//      any movement.
//    - This gives the upper bound for the motor's stiction threshold.
//    - Result is stored in stictionCeilingFwd or stictionCeilingRev.
//
// 2) Stiction Floor (stictionFloor):
//    - Refine the result using a binary search between
//      SERVO_CALIBRATION_VELOCITY_SEARCH_MIN_FACTOR * stictionCeiling
//      and stictionCeiling.
//    - This identifies the lowest PWM that reliably causes movement.
//    - Between each test step, the motor is stopped for
//      SERVO_CALIBRATION_MOTOR_SETTLE_TIME to ensure clean restarts
//      and avoid carry-over motion.
//    - The final result is stored in stictionFloorFwd or stictionFloorRev.
//    - This PWM will also serve as a kickstarter during the next phase.
//
// 3) Velocity Tracking Calibration:
//    - This was an experiment and might be removed in the future. The idea was to check
//      whether if we kick-start the motor with the min breaking stiction PWM if we can lower gradually
//      the needed PWM duty cycle to keep the motor going. If this was the case we could have the motor
//      rotating closer to the sidereal rotation.
//    - Starting from stictionFloor, a binary search is used to find the lowest
//      PWM that maintains continuous rotation after kickstarting the motor.
//    - The motor is kicked using stictionFloor before attempting a lower PWM.
//    - If the motor stalls or fails to rotate, PWM is increased again.
//    - The result is the trackingPwmFwd or trackingPwmRev, representing the duty
//      cycle needed to maintain sidereal tracking velocity with minimal power.
//
//    At least in my mesu-200 maxon 353611 motors this completely fails.
//
// After both directions are calibrated, an imbalance check is performed.
// If the forward/reverse duty cycles differ significantly, a warning is logged.
// This can indicate mount imbalance or frictional asymmetry.
//
// The result is a robust and safe calibration of tracking PWM duty cycles
// customized per motor and mechanical setup and telescope load.

#include "TrackingVelocity.h"

#if defined(SERVO_MOTOR_PRESENT) && defined(CALIBRATE_SERVO_DC)

ServoCalibrateTrackingVelocity::ServoCalibrateTrackingVelocity(uint8_t axisNumber) {
  this->axisNumber = axisNumber;
  snprintf(axisPrefix, sizeof(axisPrefix), "Axis%d ServoCalibration", axisNumber);
}

void ServoCalibrateTrackingVelocity::init() {
  experimentPwm = 0.0f;
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
  enabled = false;
  everMovedFwd = false;
  everMovedRev = false;

  velIters = 0;
  bestVelSearchPwmAbs = 0.0f;
  bestVelSearchErr = 1e9f; // sentinel "worst" error

  refineIters = 0;

  // setup the queue
  hasQueuedTest = false;
  queuedState = CALIBRATION_IDLE;
  queuedPwm = 0.0f;

  // stop the motor
  setPwm(0);
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
  calibrationPwm = SERVO_CALIBRATION_START_DUTY_CYCLE;
  targetVelocity = trackingFrequency;
  lastStateChangeTime = millis();

  // Start by stopping motor and waiting for settle
  setPwm(0);
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
          kickToFloorPWM = false;
          calibrationState = queuedState;
          hasQueuedTest = false;
          startTest(queuedPwm);
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

        if (fabsf(currentVelocity) > SERVO_CALIBRATION_MIN_DETECTABLE_VELOCITY && fabsf(velocityChange) < SERVO_CALIBRATION_VELOCITY_STABILITY_THRESHOLD) {
          motorState = MOTOR_RUNNING_STEADY;
          calibrationStepStartTime = currentTime;
          calibrationStepStartTicks = currentTicks;

          V(axisPrefix);
          if (calibrationDirectionIsForward) { VF(" FWD"); } else { VF(" REV"); }
          VF(": STEADY_STATE  @ PWM="); V(calibrationPwm); VF("%");
          VF(" Vel="); V(currentVelocity); VL(" steps/s");

          // NOW safe to start the next test, if any
          if (hasQueuedTest) {
            // if we were kicking the motor
            kickToFloorPWM = false;
            calibrationState = queuedState;
            hasQueuedTest = false;
            startTest(queuedPwm);
          }

        } else if (fabsf(currentVelocity) <= SERVO_CALIBRATION_MIN_DETECTABLE_VELOCITY) {
          // The  motor never managed to
          motorState = MOTOR_STOPPED;
          calibrationStepStartTime = currentTime;
          calibrationStepStartTicks = currentTicks;

          V(axisPrefix);
          if (calibrationDirectionIsForward) { VF(" FWD"); } else { VF(" REV"); }
          VF(": STOPPED_STATE  @ PWM="); V(calibrationPwm); VF("%");
          VF(" Vel="); V(currentVelocity); VL(" steps/s");

        }

        lastVelocityMeasurement = currentVelocity;
        lastVelocityTime = currentTime;
      }
      break;

    case MOTOR_RUNNING_STEADY:
      // Motor is running steady, nothing to do
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
    return 0.0f;
  }


  float elapsed = (currentTime - lastCheckTime) / 1000.0f;
  if (elapsed <= 0) return 0.0f;
  float velocity = (currentTicks - lastTicks) / elapsed;

#ifdef SERVO_CAL_DEBUG
  VF("DBG: "); V(axisPrefix);VF(" PWM="); V(calibrationPwm);
  VF(" dTicks="); V(currentTicks - lastTicks);
  VF(" dt(ms)=");V(currentTime - lastCheckTime);
  VF(" Vel="); V(velocity); VL(" steps/s");
#endif

  lastTicks = currentTicks;
  lastCheckTime = currentTime;

  return velocity;
}

// This function checks whether stiction is broken. Else it does an exponential search over the PWM space
// clamped to SERVO_CALIBRATION_STOP_DUTY_CYCLE
void ServoCalibrateTrackingVelocity::processStictionCeiling() {
  // sample at a short cadence
  if (currentTime - lastVelocityTime < SERVO_CALIBRATION_STICTION_SAMPLE_INTERVAL_MS) return;

  float velocity = calculateInstantaneousVelocity();
  lastVelocityTime = currentTime;

  if (fabsf(velocity) > SERVO_CALIBRATION_MIN_DETECTABLE_VELOCITY) {
    // motion detected at this PWM
    float absPwm = fabsf(calibrationPwm);
    float& stictionMax = calibrationDirectionIsForward ? stictionCeilingFwd : stictionCeilingRev;
    if (calibrationDirectionIsForward) everMovedFwd = true; else everMovedRev = true;
    stictionMax = absPwm;

    V(axisPrefix);
    if (calibrationDirectionIsForward) { VF(" FWD"); } else { VF(" REV"); }
    VF(" break: BROKEN STICTION @ PWM="); V(calibrationPwm); VF("%");
    VF(" Vel="); V(velocity); VL(" steps/s");

    setPwm(0);
    // non-blocking settle:
    settleStartTime = currentTime;
    motorState = MOTOR_SETTLING;  // your existing settling handler will time this out

    transitionToRefine();
    return;
  }

  // no motion → exponential step up (with clamp)
  V(axisPrefix);
  if (calibrationDirectionIsForward) { VF(" FWD"); } else { VF(" REV"); }
  VF(" break: NO MOVE @ PWM="); V(calibrationPwm); VF("%");
  VF(" Vel="); V(velocity); VL(" steps/s");

  calibrationPwm = copysignf(
      fminf(fabsf(calibrationPwm) * 2.0f, SERVO_CALIBRATION_STOP_DUTY_CYCLE),
      calibrationPwm
  );

  // We explored the space and found no movement up to max
  if (fabsf(calibrationPwm) >= SERVO_CALIBRATION_STOP_DUTY_CYCLE) {
    // give up for this direction, record max
    float& stictionMax = calibrationDirectionIsForward ? stictionCeilingFwd : stictionCeilingRev;
    stictionMax = SERVO_CALIBRATION_STOP_DUTY_CYCLE;
    bool ever = calibrationDirectionIsForward ? everMovedFwd : everMovedRev;
    if (!ever) {
      // Direction failed: report + switch/finish
      V(axisPrefix); VF(calibrationDirectionIsForward ? " FWD" : " REV");
      VLF(" break: COULD NOT BREAK STICTION up to PWM_MAX – skipping refine");
      setPwm(0);
      startSettling();
      if (calibrationDirectionIsForward) {
        calibrationDirectionIsForward = false;
        queueNextTest(CALIBRATION_STICTION_CEILING, -SERVO_CALIBRATION_START_DUTY_CYCLE);
      } else {
        calibrationState = CALIBRATION_CHECK_IMBALANCE;
        processImbalanceCheck();
      }
      return;
    }
    transitionToRefine();
  } else {
    startTest(calibrationPwm);  // re-apply PWM and continue probing
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
  calibrationMinPwm = SERVO_CALIBRATION_VELOCITY_SEARCH_MIN_FACTOR * stictionMax;
  if (calibrationMinPwm < 0.0f) calibrationMinPwm = 0.0f;   // safety clamp
  calibrationMaxPwm = stictionMax;

  float mid = 0.5f * (calibrationMinPwm + calibrationMaxPwm);
  calibrationPwm = calibrationDirectionIsForward ? mid : -mid;

  refineIters = 0;
  refineSawMove = false;

  // Start the first refine test at the midpoint PWM
  startTest(calibrationPwm);
}

void ServoCalibrateTrackingVelocity::processStictionFloor() {
  // Up to REFINE_MAX_ITERATIONS
  if (++refineIters > SERVO_CALIBRATION_REFINE_MAX_ITERATIONS) {
    VLF("WARN: refine iteration cap reached; using current bounds");
    calibrationMaxPwm = fmaxf(calibrationMaxPwm, calibrationMinPwm);
     // fall through to convergence
  }

  // We only evaluate after a test has resolved to a steady state or stop.
  // (updateState() already guards for this; keeping a safety check here too.)
  if (motorState != MOTOR_RUNNING_STEADY && motorState != MOTOR_STOPPED) return;

  const bool moved = (motorState == MOTOR_RUNNING_STEADY);

  if (!moved) {
    // No movement → raise the lower bound
    calibrationMinPwm = fabsf(calibrationPwm);

    V(axisPrefix);
    if (calibrationDirectionIsForward) { VF(" FWD"); } else { VF(" REV"); }
    VF(" refine: NO MOVE @ PWM="); V(calibrationPwm); VLF("%");
  } else {
    // Movement → lower the upper bound
    calibrationMaxPwm = fabsf(calibrationPwm);
    refineSawMove = true;

    V(axisPrefix);
    if (calibrationDirectionIsForward) { VF(" FWD"); } else { VF(" REV"); }
    VF(" refine: MOVE  @ PWM="); V(calibrationPwm);
    VF("%, v≈"); V(lastVelocityMeasurement); VL(" steps/s");
  }

  // Convergence check (need at least one MOVE, and span within abs/rel tol)
  float span = calibrationMaxPwm - calibrationMinPwm;
  float tolAbs = SERVO_CALIBRATION_STICTION_REFINE_ABS;
  float tolRel = SERVO_CALIBRATION_STICTION_REFINE_REL * calibrationMaxPwm; // ~ vs ceiling
  float tol = fmaxf(tolAbs, tolRel);
  if (refineSawMove && span <= tol) {
    // Conservative pick: the smallest PWM known to MOVE
    const float resultMin = calibrationMaxPwm;
    const float lastNoMoveAbs = calibrationMinPwm;      // <-- highest known stall from refine

    // Store floor for reporting
    float& stictionMin = calibrationDirectionIsForward ? stictionFloorFwd : stictionFloorRev;
    stictionMin = resultMin;

    // If there’s no meaningful headroom below floor, accept floor and move on
    if (resultMin - lastNoMoveAbs <= tol) {
      float& trackingPwm = calibrationDirectionIsForward ? trackingPwmFwd : trackingPwmRev;
      trackingPwm = resultMin;
      V(axisPrefix); VF(calibrationDirectionIsForward ? " FWD" : " REV");
      VF(" floor used as tracking PWM="); V(trackingPwm); VLF("% (no headroom)");
      setPwm(0); startSettling();
      if (calibrationDirectionIsForward) {
        calibrationDirectionIsForward = false;
        queueNextTest(CALIBRATION_STICTION_CEILING, -SERVO_CALIBRATION_START_DUTY_CYCLE);
      } else {
        calibrationState = CALIBRATION_CHECK_IMBALANCE;
        processImbalanceCheck();
      }
      return;
    }

    // ---- Enter velocity search (always below the floor) ----
    velIters = 0;
    bestVelSearchErr = 1e9f;
    bestVelSearchPwmAbs = resultMin;                 // worst-case is the floor
    calibrationState = CALIBRATION_VELOCITY_SEARCH;

    const float floorAbs = resultMin;
    calibrationMinPwm = lastNoMoveAbs;          // <-- start from the highest known stall
    calibrationMaxPwm = floorAbs;               // <-- up to the floor

    // first probe strictly below floor
    float mid = 0.5f * (calibrationMinPwm + calibrationMaxPwm);
    calibrationPwm = (calibrationDirectionIsForward ? +mid : -mid);

    // prime a kick at floor, then run the queued test at "mid"
    // (apply kick and mark as accelerating so state machine advances)
    const float sign = calibrationDirectionIsForward ? 1.0f : -1.0f;
    queueNextTest(CALIBRATION_VELOCITY_SEARCH, calibrationPwm);

    setPwm(sign * floorAbs);                         // kick at floor
    motorState = MOTOR_ACCELERATING;                 // important for state handling
    lastStateChangeTime = currentTime;
    lastVelocityTime = currentTime;
    lastVelocityMeasurement = 0;
    V(axisPrefix); VF(calibrationDirectionIsForward ? " FWD" : " REV");
    VF(" kick: floor PWM="); V(floorAbs); VL("%");
    return;
  }

  // Continue binary search: midpoint of [min, max]
  float mid = 0.5f * (calibrationMinPwm + calibrationMaxPwm);
  calibrationPwm = calibrationDirectionIsForward ? mid : -mid;

  // Start the next test; handleMotorState() will advance to RUNNING_STEADY or STOPPED
  startTest(calibrationPwm);
}

void ServoCalibrateTrackingVelocity::processVelocitySearch() {
  // Only act when either the motor is steady (to evaluate) or stopped (treat as too low).
  if (motorState != MOTOR_RUNNING_STEADY && motorState != MOTOR_STOPPED) return;

  const float sign     = calibrationDirectionIsForward ? 1.0f : -1.0f;
  const float floorAbs = calibrationDirectionIsForward ? stictionFloorFwd : stictionFloorRev;
  const float pwmAbs   = fabsf(calibrationPwm);

  // tolerances used for convergence
  const float tolAbs = SERVO_CALIBRATION_STICTION_REFINE_ABS;
  const float tolRel = SERVO_CALIBRATION_STICTION_REFINE_REL * floorAbs;
  const float tol    = fmaxf(tolAbs, tolRel);

  // If our lower bound is already at (or within tol of) the floor,
  // the best we can do is the floor; accept and exit.
  if (calibrationMinPwm >= floorAbs - tol) {
    float& trackingPwm = calibrationDirectionIsForward ? trackingPwmFwd : trackingPwmRev;
    trackingPwm = floorAbs;
    VF("MSG: "); V(axisPrefix);
    VF(calibrationDirectionIsForward ? " FWD" : " REV");
    VF(" velocity search: pinned at floor, using PWM="); V(trackingPwm); VLF("%");
    goto velocity_search_done;
  }

  if (motorState == MOTOR_STOPPED) {
    // too low -> raise lower bound, keep within [0,floor]
    calibrationMinPwm = fminf(floorAbs, fmaxf(calibrationMinPwm, pwmAbs));
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

    // Degenerate: no valid target; accept current PWM and exit this phase
    if (targetMag <= 0.0f) {
      float& trackingPwm = calibrationDirectionIsForward ? trackingPwmFwd : trackingPwmRev;
      trackingPwm = pwmAbs; // degenerate case
      V(axisPrefix); VLF(": velocity search skipped (target=0)");
      goto velocity_search_done;
    }

    // Track best-so-far for graceful bailout
    const float errPct = fabsf(actualMag - targetMag) / targetMag * 100.0f;
    if (errPct < bestVelSearchErr) {
      bestVelSearchErr    = errPct;
      bestVelSearchPwmAbs = pwmAbs;
    }
    // Decide next bounds from whether we're too slow/fast
    if (actualMag < targetMag) {
      // too slow -> we need more PWM (move upward but never above floor)
      calibrationMinPwm = fminf(floorAbs, fmaxf(calibrationMinPwm, pwmAbs));
    } else {
      // too fast -> we can try lower PWM
      calibrationMaxPwm = fminf(floorAbs, fmaxf(0.0f, fminf(calibrationMaxPwm, pwmAbs)));
    }

    //Success??
    if (errPct <= SERVO_CALIBRATION_ERROR_THRESHOLD) {
      float& trackingPwm = calibrationDirectionIsForward ? trackingPwmFwd : trackingPwmRev;
      trackingPwm = pwmAbs;
      VF("MSG: "); V(axisPrefix);
      VF(calibrationDirectionIsForward ? " FWD" : " REV");
      VF(" velocity OK: PWM="); V(trackingPwm);
      VF("%, v="); V(avgVel); VF(" steps/s, err="); V(errPct); VLF("%");
      goto velocity_search_done;
    }
  }

  // Convergence / iteration cap: worst case is the floor
  if ((calibrationMaxPwm - calibrationMinPwm) <= tol || ++velIters > SERVO_CALIBRATION_REFINE_MAX_ITERATIONS) {
    float& trackingPwm = calibrationDirectionIsForward ? trackingPwmFwd : trackingPwmRev;
    trackingPwm = calibrationMaxPwm;                  // <= floorAbs
    if (trackingPwm < floorAbs - tol) {
      // found sustainable below-floor
      VF("MSG: "); V(axisPrefix);
      VF(calibrationDirectionIsForward ? " FWD" : " REV");
      VF(" velocity search: below-floor PWM="); V(trackingPwm); VLF("%");
    } else {
      // worst-case: use the floor
      trackingPwm = floorAbs;
      VF("MSG: "); V(axisPrefix);
      VF(calibrationDirectionIsForward ? " FWD" : " REV");
      VF(" velocity search: using floor PWM="); V(trackingPwm); VLF("%");
    }
    goto velocity_search_done;
  }

  {
    // Next midpoint strictly below (or equal to) the floor.
    float midAbs = 0.5f * (calibrationMinPwm + calibrationMaxPwm);
    // kick at floor, then queue the test at midAbs
    setPwm(sign * floorAbs);
    motorState = MOTOR_ACCELERATING;
    lastStateChangeTime = currentTime;
    lastVelocityTime = currentTime;
    lastVelocityMeasurement = 0;

    queueNextTest(CALIBRATION_VELOCITY_SEARCH, sign * midAbs);
    V(axisPrefix); VF(calibrationDirectionIsForward ? " FWD" : " REV");
    VF(" kick: floor PWM="); V(floorAbs); VF("% -> next test at "); V(midAbs); VLF("%");
    return;
  }

velocity_search_done:
  setPwm(0);
  startSettling();
  if (calibrationDirectionIsForward) {
    // Switch to reverse direction starting again from stiction ceiling
    calibrationDirectionIsForward = false;
    calibrationState = CALIBRATION_STICTION_CEILING;
    // Queue the next test, since we first have to make sure the motor are stopped
    queueNextTest(CALIBRATION_STICTION_CEILING, -SERVO_CALIBRATION_START_DUTY_CYCLE);
  } else {
    calibrationState = CALIBRATION_CHECK_IMBALANCE;
    processImbalanceCheck();
  }
}

void ServoCalibrateTrackingVelocity::processImbalanceCheck() {
  // Print calibration report
  printReport();

  // Check for significant imbalance
  if (trackingPwmFwd > 0 && trackingPwmRev > 0) {
    float avgTracking = (trackingPwmFwd + trackingPwmRev) / 2.0f;
    float imbalance = fabsf(trackingPwmFwd - trackingPwmRev) / avgTracking * 100.0f;

    if (imbalance > SERVO_CALIBRATION_IMBALANCE_ERROR_THRESHOLD) {
      VF("WARN: "); V(axisPrefix);
      VF(" Significant imbalance: "); V(imbalance); VL("%");
    }
  }

  // Calibration complete
  experimentMode = false;
  calibrationState = CALIBRATION_IDLE;
  setPwm(0);
  VF("MSG: "); V(axisPrefix); VL(" complete");
}

void ServoCalibrateTrackingVelocity::handleCalibrationFailure() {
  VF("ERR: "); V(axisPrefix); VL("Calibration failed, resetting");

  // Reset to safe state
  experimentMode = false;
  calibrationState = CALIBRATION_IDLE;
  setPwm(0);

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

void ServoCalibrateTrackingVelocity::startTest(float pwm) {
  calibrationPwm = pwm;
  setPwm(pwm);
  lastVelocityTime = currentTime;
  lastVelocityMeasurement = 0;
  motorState = MOTOR_ACCELERATING;
  lastStateChangeTime = currentTime;

  V(axisPrefix);
  if (calibrationDirectionIsForward) {VF(" FWD");} else {VF(" REV");}
  VF(" Starting test at PWM="); V(pwm); VL("%");
}

void ServoCalibrateTrackingVelocity::setPwm(float pwm) {
  // Apply PWM with constraints
  experimentPwm = constrain(pwm, -SERVO_CALIBRATION_STOP_DUTY_CYCLE, SERVO_CALIBRATION_STOP_DUTY_CYCLE);
}

// Getters ////////////////////////////////////////////////////////////////////
float ServoCalibrateTrackingVelocity::getStictionCeiling(bool forward) {
  return forward ? stictionCeilingFwd : stictionCeilingRev;
}

float ServoCalibrateTrackingVelocity::getStictionFloor(bool forward) {
  return forward ? stictionFloorFwd : stictionFloorRev;
}

float ServoCalibrateTrackingVelocity::getTrackingPwm(bool forward) {
  return forward ? trackingPwmFwd : trackingPwmRev;
}

void ServoCalibrateTrackingVelocity::printReport() {
  VF("\n=== Calibration Report: "); V(axisPrefix); VLF(" ===");

  VF("Stiction Ceiling at FWD: Max="); V(stictionCeilingFwd);
  VF("%, Floor="); V(stictionFloorFwd); VL("%");

  VF("Tracking PWM FWD: "); V(trackingPwmFwd); VL("%\n");

  VF("Stiction Ceiling at REV: Max="); V(stictionCeilingRev);
  VF("%, Floor="); V(stictionFloorRev); VL("%");

  VF("Tracking PWM REV: "); V(trackingPwmRev); VL("%\n");

  float stictionImbalance = fabsf(stictionFloorFwd - stictionFloorRev);
  float trackingImbalance = fabsf(trackingPwmFwd - trackingPwmRev);

  VF("Stiction Min Imbalance: "); V(stictionImbalance); VL("%");
  VF("Tracking PWM Imbalance: "); V(trackingImbalance); VL("%\n");

  if (trackingPwmFwd >= SERVO_CALIBRATION_STOP_DUTY_CYCLE - 0.1f) {
    VLF("WARN: FWD PWM at calibration limit");
  }
  if (trackingPwmRev >= SERVO_CALIBRATION_STOP_DUTY_CYCLE - 0.1f) {
    VLF("WARN: REV PWM at calibration limit");
  }

  VF("=== End Report ===\n");
}

void ServoCalibrateTrackingVelocity::resetCalibrationValues(void) {
  stictionCeilingFwd = SERVO_CALIBRATION_STOP_DUTY_CYCLE;
  stictionFloorFwd = 0.0f;
  trackingPwmFwd = 0.0f;
  stictionCeilingRev = SERVO_CALIBRATION_STOP_DUTY_CYCLE;
  stictionFloorRev = 0.0f;
  trackingPwmRev = 0.0f;
}

#endif