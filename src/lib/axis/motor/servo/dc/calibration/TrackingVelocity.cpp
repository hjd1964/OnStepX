
// -----------------------------------------------------------------------------------
// Servo Tracking Velocity Calibration - Overview
//
// This routine performs a 2-phase, bidirectional calibration to determine the
// velocity % needed to reliably track at sidereal velocity with a DC servo motor.
// The process measures the minimum duty cycles needed to overcome static friction
// (stiction) and the minimum duty cycle to maintain continuous motion.
//
// Calibration logic (per direction) proceeds in 2 main steps:
//
// 1) Stiction Break (u_break):
//    - Use staircase search (incrementing the velocity value by fixed steps starting from
//      SERVO_CALIBRATION_START_VELOCITY_PERCENT) to find the minimum velocity that
//      reliably initiates movement.
//    - Between each test step, the motor is stopped for SERVO_CALIBRATION_MOTOR_SETTLE_TIME
//      to ensure clean restarts and avoid carry-over motion.
//    - The final result is stored in u_breakFwd or u_breakRev.
//
// 2) Velocity Hold (u_hold):
//    - Starting from u_break, use staircase search downwards to find the minimum
//      velocity that maintains continuous rotation after kickstarting the motor.
//    - The motor is first kicked using u_break to overcome stiction, then immediately
//      set to the candidate u_hold velocity.
//    - If the motor stalls or fails to rotate, velocity is increased again.
//    - The result is the u_holdFwd or u_holdRev, representing the duty cycle
//      needed to maintain sidereal tracking velocity with minimal power.
//
// After both directions are calibrated, an imbalance check is performed.
// If the forward/reverse duty cycles differ significantly, a warning is logged.
// This can indicate mount imbalance or frictional asymmetry.
//
// The result is a robust and safe calibration of stiction breaking and minimum
// velocity customized per motor and mechanical setup and telescope load.

#include "TrackingVelocity.h"

#if defined(SERVO_MOTOR_PRESENT) && defined(CALIBRATE_SERVO_DC)

// Disable servo algorithms for calibration
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

  resetCalibrationValues();

  lastStateChangeTime = lastVelocityTime = settleStartTime =
    calibrationStepStartTime = kickStartTime = 0;
  currentTicks = calibrationStepStartTicks = lastTicks = 0;
  lastVelocityMeasurement = 0.0f;
  lastDeltaCounts = 0;
  steadySinceMs = steadyStartTicks = 0;
  lastCheckTime = 0;

  staircaseIters = 0;
  currentSearchVelocity = 0.0f;
  foundValidMotion = false;

  hasQueuedTest = false;
  queuedState = CALIBRATION_IDLE;
  queuedVelocity = 0.0f;

  setVelocity(0);
  startSettling();
}

void ServoCalibrateTrackingVelocity::start(float trackingFrequency, long instrumentCoordinateSteps) {
  if (!(CALIBRATE_SERVO_AXIS_SELECT & (1 << (axisNumber - 1)))) {
    VF("MSG: "); V(axisPrefix); VLF(" Calibration skipped for this axis");
    return;
  }

  VF("MSG: "); V(axisPrefix); VL(" Starting 2-phase bidirectional calibration");

  enabled = true;
  experimentMode = true;
  calibrationDirectionIsForward = true;
  calibrationState = CALIBRATION_U_BREAK;
  motorState = MOTOR_STOPPED;
  calibrationVelocity = SERVO_CALIBRATION_START_VELOCITY_PERCENT;
  targetVelocity = trackingFrequency;
  lastStateChangeTime = millis();

  staircaseIters = 0;
  currentSearchVelocity = calibrationVelocity;
  foundValidMotion = false;

  setVelocity(0);
  startSettling();
}

void ServoCalibrateTrackingVelocity::updateState(long instrumentCoordinateSteps) {
  if(!enabled || !experimentMode) return;

  currentTime = millis();
  currentTicks = instrumentCoordinateSteps;

  handleMotorState();

  if (currentTime - lastStateChangeTime > SERVO_CALIBRATION_TIMEOUT) {
    VF("WARN: "); V(axisPrefix); VLF(" State timeout, resetting calibration");
    handleCalibrationFailure();
    return;
  }

  if (motorState != MOTOR_RUNNING_STEADY && motorState != MOTOR_STOPPED) return;

  switch (calibrationState) {
    case CALIBRATION_U_BREAK: processUBreak(); break;
    case CALIBRATION_U_HOLD: processUHold(); break;
    case CALIBRATION_CHECK_IMBALANCE: processImbalanceCheck(); break;
    default: break;
  }
}

// Private helper methods //////////////////////////////////////////////////////

ServoCalibrateTrackingVelocity::DirectionData& ServoCalibrateTrackingVelocity::currentDirectionData() {
  return calibrationDirectionIsForward ? fwdData : revData;
}

const ServoCalibrateTrackingVelocity::DirectionData& ServoCalibrateTrackingVelocity::currentDirectionData() const {
  return calibrationDirectionIsForward ? fwdData : revData;
}

void ServoCalibrateTrackingVelocity::recordMovementData(float velocity, long counts) {
  DirectionData& data = currentDirectionData();
  if (calibrationState == CALIBRATION_U_BREAK) {
    data.breakVel = velocity;
    data.breakCounts = counts;
  } else {
    data.holdVel = velocity;
    data.holdCounts = counts;
  }
}

void ServoCalibrateTrackingVelocity::logTestResult(const char* phase, bool moved) {
  V(axisPrefix);
  VF(calibrationDirectionIsForward ? " FWD" : " REV");
  VF(" "); V(phase); VF(": ");
  VF(moved ? "MOVING" : "STALLED");
  VF(" @ VELOCITY="); V(calibrationVelocity);
  if (moved) {
    VF("%, v≈"); V(lastVelocityMeasurement); VLF(" steps/s");
  } else {
    VLF("%");
  }
}

void ServoCalibrateTrackingVelocity::handleMotorState() {
  float currentVelocity = 0.0f;

  switch (motorState) {
    case MOTOR_SETTLING:
      currentVelocity = calculateInstantaneousVelocity();
      if (currentTime - settleStartTime >= SERVO_CALIBRATION_MOTOR_SETTLE_TIME) {
        motorState = MOTOR_STOPPED;
        V(axisPrefix); VF(": Settled to STOPPED"); VF(" Vel="); V(currentVelocity); VL(" steps/s");

        if (hasQueuedTest) {
          calibrationState = queuedState;
          hasQueuedTest = false;
          startTest(queuedVelocity);
        }
      }
      lastVelocityMeasurement = currentVelocity;
      lastVelocityTime = currentTime;
      break;

    case MOTOR_KICKING:
      if (currentTime - kickStartTime >= SERVO_CALIBRATION_KICK_DURATION_MS) {
        setVelocity(calibrationVelocity);
        motorState = MOTOR_HOLDING;
        calibrationStepStartTime = currentTime;
        calibrationStepStartTicks = currentTicks;
        lastVelocityTime = currentTime; // Reset velocity timing
        lastVelocityMeasurement = 0;    // Reset velocity measurement

        V(axisPrefix); VF(calibrationDirectionIsForward ? " FWD" : " REV");
        VF(": KICK→HOLD @ "); V(calibrationVelocity); VL("%");
      }
      break;

    case MOTOR_HOLDING:
      // Check velocity after minimum interval
      if (currentTime - lastVelocityTime >= SERVO_CALIBRATION_VELOCITY_SETTLE_CHECK_INTERVAL) {
        currentVelocity = calculateInstantaneousVelocity();

        V(axisPrefix); VF(calibrationDirectionIsForward ? " FWD" : " REV");
        VF(": HOLDING @"); V(calibrationVelocity); VF("%, currentVel="); V(currentVelocity); VL(" steps/s");

        if (fabsf(currentVelocity) > SERVO_CALIBRATION_MIN_DETECTABLE_VELOCITY) {
          // Motor is moving, check if it's stable
          float velocityChange = fabsf(currentVelocity - lastVelocityMeasurement);

          if (velocityChange < SERVO_CALIBRATION_VELOCITY_STABILITY_THRESHOLD) {
            motorState = MOTOR_RUNNING_STEADY;
            steadySinceMs = currentTime;
            steadyStartTicks = currentTicks;
            V(axisPrefix); VF(": REACHED STEADY STATE"); VL("");
          } else {
            // Still accelerating/decelerating, keep waiting
            V(axisPrefix); VF(": STILL SETTLING, velChange="); V(velocityChange); VL(" steps/s");
          }
        } else {
          // Motor is not moving enough
          motorState = MOTOR_STOPPED;
          V(axisPrefix); VF(": HOLD_STALLED - insufficient velocity"); VL("");
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
  VF("DBG: "); V(axisPrefix); VF(" vel="); V(calibrationVelocity);
  VF("%, dTicks="); V(dCounts); VF(", Vel="); V(velocity); VL(" steps/s");
#endif

  lastTicks = currentTicks;
  lastCheckTime = currentTime;
  lastDeltaCounts = dCounts;

  return velocity;
}

void ServoCalibrateTrackingVelocity::processUBreak() {
  if (++staircaseIters > SERVO_CALIBRATION_REFINE_MAX_ITERATIONS) {
    VF("WARN: "); V(axisPrefix); VLF(" u_break iteration limit reached");
    if (!foundValidMotion) {
      currentDirectionData().u_break = SERVO_CALIBRATION_STOP_VELOCITY_PERCENT;
      V(axisPrefix); VF(": No motion found up to "); V(currentDirectionData().u_break); VL("%");
    }
    proceedToUHold();
    return;
  }

  // We only evaluate after a test has resolved to a steady state or stop
  if (motorState != MOTOR_RUNNING_STEADY && motorState != MOTOR_STOPPED) return;

  bool moved = (motorState == MOTOR_RUNNING_STEADY);
  float currentAbsVelocity = fabsf(calibrationVelocity);

  V(axisPrefix); VF(" u_break: motorState=");
  switch(motorState) {
    case MOTOR_RUNNING_STEADY: VF("STEADY"); break;
    case MOTOR_STOPPED: VF("STOPPED"); break;
    default: VF("OTHER"); break;
  }
  VF(", moved="); V(moved); VF(", vel="); V(currentAbsVelocity); VL("%");

  if (moved) {
    foundValidMotion = true;
    recordMovementData(lastVelocityMeasurement, lastDeltaCounts);
    currentDirectionData().u_break = currentAbsVelocity;
    currentDirectionData().everMoved = true;

    V(axisPrefix); VF(" u_break: SUCCESS @ "); V(calibrationVelocity);
    VF("%, measuredVel="); V(lastVelocityMeasurement); VL(" steps/s");
    proceedToUHold();
  } else {
    V(axisPrefix); VF(" u_break: NO MOVE @ "); V(calibrationVelocity); VL("%");

    float nextAbsVelocity = currentAbsVelocity + SERVO_CALIBRATION_STAIRCASE_STEP_PERCENT;

    if (nextAbsVelocity > SERVO_CALIBRATION_STOP_VELOCITY_PERCENT) {
      if (!foundValidMotion) {
        currentDirectionData().u_break = SERVO_CALIBRATION_STOP_VELOCITY_PERCENT;
        V(axisPrefix); VF(": No motion found, using max="); V(currentDirectionData().u_break); VL("%");
      }
      proceedToUHold();
    } else {
      calibrationVelocity = calibrationDirectionIsForward ? nextAbsVelocity : -nextAbsVelocity;
      startTest(calibrationVelocity);
    }
  }
}

void ServoCalibrateTrackingVelocity::processUHold() {
  if (++staircaseIters > SERVO_CALIBRATION_REFINE_MAX_ITERATIONS) {
    VF("WARN: "); V(axisPrefix); VLF(" u_hold iteration limit reached");
    finishUHold();
    return;
  }

  if (motorState != MOTOR_RUNNING_STEADY && motorState != MOTOR_STOPPED) return;

  bool moved = (motorState == MOTOR_RUNNING_STEADY);
  float currentAbsVelocity = fabsf(calibrationVelocity);
  float u_break = currentDirectionData().u_break;

  if (moved) {
    recordMovementData(lastVelocityMeasurement, lastDeltaCounts);
    currentDirectionData().u_hold = currentAbsVelocity;

    logTestResult("u_hold", true);

    float nextAbsVelocity = currentAbsVelocity - SERVO_CALIBRATION_STAIRCASE_STEP_PERCENT;

    if (nextAbsVelocity < SERVO_CALIBRATION_START_VELOCITY_PERCENT) {
      V(axisPrefix); VF(" u_hold: Min velocity="); V(currentDirectionData().u_hold); VL("%");
      finishUHold();
    } else {
      calibrationVelocity = calibrationDirectionIsForward ? nextAbsVelocity : -nextAbsVelocity;
      setVelocity(calibrationDirectionIsForward ? u_break : -u_break);
      motorState = MOTOR_KICKING;
      kickStartTime = currentTime;
      lastStateChangeTime = currentTime;
    }
  } else {
    logTestResult("u_hold", false);

    if (currentDirectionData().u_hold == 0.0f) {
      currentDirectionData().u_hold = u_break;
      V(axisPrefix); VF(" u_hold: Using u_break="); V(currentDirectionData().u_hold); VL("%");
    }
    finishUHold();
  }
}

void ServoCalibrateTrackingVelocity::proceedToUHold() {
  setVelocity(0);
  startSettling();

  calibrationState = CALIBRATION_U_HOLD;
  staircaseIters = 0;

  float u_break = currentDirectionData().u_break;
  currentSearchVelocity = max(u_break - SERVO_CALIBRATION_STAIRCASE_STEP_PERCENT,
                             SERVO_CALIBRATION_START_VELOCITY_PERCENT);

  V(axisPrefix); VF(" u_hold: u_break="); V(u_break);
  VF("%, first test="); V(currentSearchVelocity); VL("%");

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
    V(axisPrefix); VL(" u_hold: Switching to REVERSE");
  } else {
    calibrationState = CALIBRATION_CHECK_IMBALANCE;
    processImbalanceCheck();
  }
}

void ServoCalibrateTrackingVelocity::processImbalanceCheck() {
  printReport();

  // Use the struct members instead of the old individual variables
  if (fwdData.u_hold > 0 && revData.u_hold > 0) {
    float avgTracking = (fwdData.u_hold + revData.u_hold) / 2.0f;
    float imbalance = fabsf(fwdData.u_hold - revData.u_hold) / avgTracking * 100.0f;

    if (imbalance > SERVO_CALIBRATION_IMBALANCE_ERROR_THRESHOLD) {
      VF("WARN: "); V(axisPrefix); VF(" Imbalance: "); V(imbalance); VL("%");
    }
  }

  experimentMode = false;
  calibrationState = CALIBRATION_IDLE;
  setVelocity(0);
  VF("MSG: "); V(axisPrefix); VL(" complete");
}

void ServoCalibrateTrackingVelocity::handleCalibrationFailure() {
  VF("ERR: "); V(axisPrefix); VL("Calibration failed, resetting");
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

  if (calibrationState == CALIBRATION_U_BREAK) {
    // For u_break, apply the test velocity directly
    setVelocity(calibrationVelocity);
    motorState = MOTOR_HOLDING;
    calibrationStepStartTime = currentTime;
    calibrationStepStartTicks = currentTicks;

    V(axisPrefix); VF(calibrationDirectionIsForward ? " FWD" : " REV");
    VF(" u_break Test @ vel="); V(velocity); VL("%");
  } else {
    // For u_hold, kick first then switch to test velocity
    float u_break = currentDirectionData().u_break;
    setVelocity(calibrationDirectionIsForward ? u_break : -u_break);
    motorState = MOTOR_KICKING;
    kickStartTime = currentTime;

    V(axisPrefix); VF(calibrationDirectionIsForward ? " FWD" : " REV");
    VF(" u_hold Test: KICK @"); V(u_break); VF("% → HOLD @"); V(velocity); VL("%");
  }
}

void ServoCalibrateTrackingVelocity::setVelocity(float velocityPercent) {
  experimentVelocity = constrain(velocityPercent, -SERVO_CALIBRATION_STOP_VELOCITY_PERCENT,
                                             +SERVO_CALIBRATION_STOP_VELOCITY_PERCENT);
}

// Getters ////////////////////////////////////////////////////////////////////
float ServoCalibrateTrackingVelocity::getU_break(bool forward) {
  return forward ? fwdData.u_break : revData.u_break;
}

float ServoCalibrateTrackingVelocity::getU_hold(bool forward) {
  return forward ? fwdData.u_hold : revData.u_hold;
}

void ServoCalibrateTrackingVelocity::printReport() {
  VF("\n=== Calibration Report: "); V(axisPrefix); VLF(" ===");
  VLF("Dir  | u_break                         | u_hold");
  VLF("-----+---------------------------------+---------------------------------");

  auto printRow = [this](const char* dir, const DirectionData& data) {
    VF(dir); VF("  | ");
    VF("pct "); V(data.u_break); VF("%, v "); V(data.breakVel); VF(" steps/s, cnt "); V(data.breakCounts); VF("  | ");
    VF("pct "); V(data.u_hold); VF("%, v "); V(data.holdVel); VF(" steps/s, cnt "); V(data.holdCounts); VL("");
  };

  printRow("FWD", fwdData);
  printRow("REV", revData);

  float breakImbalance = fabsf(fwdData.u_break - revData.u_break);
  float holdImbalance = fabsf(fwdData.u_hold - revData.u_hold);

  VF("Δ u_break: "); V(breakImbalance); VF("%, ");
  VF("Δ u_hold: "); V(holdImbalance); VLF("%");

  if (fwdData.u_hold >= SERVO_CALIBRATION_STOP_VELOCITY_PERCENT - 0.1f) VLF("WARN: FWD velocity at calibration limit");
  if (revData.u_hold >= SERVO_CALIBRATION_STOP_VELOCITY_PERCENT - 0.1f) VLF("WARN: REV velocity at calibration limit");

  VF("=== End Report ===\n");
}

void ServoCalibrateTrackingVelocity::resetCalibrationValues() {
  fwdData = DirectionData();
  revData = DirectionData();
}

#endif