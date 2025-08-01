// -----------------------------------------------------------------------------------
// calibrate servo tracking velocity

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
// 1) Stiction Break Max (stictionBreakMax):
//    - Use exponential search (doubling the PWM value starting from
//      SERVO_CALIBRATION_START_DUTY_CYCLE) to find a PWM that initiates
//      any movement.
//    - This gives the upper bound for the motor's stiction threshold.
//    - Result is stored in stictionBreakMaxFwd or stictionBreakMaxRev.
//
// 2) Stiction Break Min (stictionBreakMin):
//    - Refine the result using a binary search between
//      SERVO_CALIBRATION_VELOCITY_SEARCH_MIN_FACTOR * stictionBreakMax
//      and stictionBreakMax.
//    - This identifies the lowest PWM that reliably causes movement.
//    - Between each test step, the motor is stopped for
//      SERVO_CALIBRATION_STICTION_SETTLE_TIME to ensure clean restarts
//      and avoid carry-over motion.
//    - The final result is stored in stictionBreakMinFwd or stictionBreakMinRev.
//    - This PWM will also serve as a kickstarter during the next phase.
//
// 3) Velocity Tracking Calibration:
//    - Starting from stictionBreakMin, a binary search is used to find the lowest
//      PWM that maintains continuous rotation after kickstarting the motor.
//    - The motor is kicked using stictionBreakMin before attempting a lower PWM.
//    - If the motor stalls or fails to rotate, PWM is increased again.
//    - The result is the trackingPwmFwd or trackingPwmRev, representing the duty
//      cycle needed to maintain sidereal tracking velocity with minimal power.
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
  strcpy(axisPrefix, " Axis_VelocityCalibrate, ");
  axisPrefix[5] = '0' + axisNumber;
}

void ServoCalibrateTrackingVelocity::init() {
  experimentPwm = 0.0F;
  experimentMode = false;

  calibrationState = CALIBRATION_IDLE;
  calibrationPwm = 0;
  stictionBreakMaxFwd = 0;
  stictionBreakMinFwd = 0;
  trackingPwmFwd = 0;
  stictionBreakMaxRev = 0;
  stictionBreakMinRev = 0;
  trackingPwmRev = 0;
  calibrationStartTime = 0;
  calibrationStartTicks = 0;
  calibrationMinPwm = 0;
  calibrationMaxPwm = 0;
  targetVelocity = 0;
  calibrationPhaseCount = 0;
  settleStartTime = 0;
  waitingForSettle = false;
  stictionTestPwm = 0;
}

void ServoCalibrateTrackingVelocity::start(float trackingFrequency, long instrumentCoordinateSteps) {
  // Check if calibration is enabled for this axis
  if (!(CALIBRATE_SERVO_AXIS_SELECT & (1 << (axisNumber - 1)))) {
    VF("MSG:"); V(axisPrefix); VLF("Calibration skipped for this axis");
    return;
  }

  VF("MSG:"); V(axisPrefix); VL("Starting 3-phase bidirectional calibration");

  calibrationState = CALIBRATION_STICTION_BREAK_MAX_FWD;
  calibrationPwm = SERVO_CALIBRATION_START_DUTY_CYCLE;
  calibrationStartTicks = instrumentCoordinateSteps;
  calibrationStartTime = millis();
  targetVelocity = trackingFrequency;

  experimentMode = true;
  setExperimentPwm(calibrationPwm);

  VF("MSG:"); V(axisPrefix); VF("Testing FWD PWM=");
  V(calibrationPwm); VLF("%");
}

void ServoCalibrateTrackingVelocity::updateState(long instrumentCoordinateSteps) {
  if (calibrationState == CALIBRATION_IDLE) return;

  unsigned long currentTime = millis();
  long currentTicks = instrumentCoordinateSteps;

  // Handle settling period
  if (waitingForSettle) {
    if (currentTime - settleStartTime >= SERVO_CALIBRATION_STICTION_SETTLE_TIME) {
      waitingForSettle = false;
      calibrationStartTime = currentTime;
      calibrationStartTicks = currentTicks;
      setExperimentPwm(stictionTestPwm);
    }
    return;
  }

  // Skip if calibration period not completed
  if (currentTime - calibrationStartTime < (SERVO_CALIBRATION_TIME_PERIOD)) {
    return;
  }

  float elapsedSec = (currentTime - calibrationStartTime) / 1000.0f;
  float actualVelocity = (currentTicks - calibrationStartTicks) / elapsedSec;
  float unsignedVelocity = fabs(actualVelocity);

  switch (calibrationState) {
    case CALIBRATION_STICTION_BREAK_MAX_FWD:
      if (unsignedVelocity < 0.1f) {
        calibrationPwm *= 2.0f;

        if (calibrationPwm > 100.0f) {
          VF("MSG:"); V(axisPrefix); VL("FWD Max Stiction not found");
          calibrationState = CALIBRATION_IDLE;
          experimentMode = false;
          setExperimentPwm(0);
        } else {
          setExperimentPwm(calibrationPwm);
          calibrationStartTime = currentTime;
          calibrationStartTicks = currentTicks;
          calibrationPhaseCount++;

          VF("MSG:"); V(axisPrefix); VF("FWD Max Stiction Phase ");
          V(calibrationPhaseCount); VF(": PWM=");
          V(calibrationPwm); VLF("% - No movement");
        }
      } else {
        stictionBreakMaxFwd = calibrationPwm;
        calibrationState = CALIBRATION_STICTION_REFINE_MIN_FWD;
        calibrationPwm = stictionBreakMaxFwd * 0.5f;
        calibrationPhaseCount = 0;

        // wait the motor to settle after previous pwm - set it to 0
        settleStartTime = currentTime;
        waitingForSettle = true;
        stictionTestPwm = calibrationPwm;

        // Set binary search bounds for min stiction
        calibrationMinPwm = SERVO_CALIBRATION_VELOCITY_SEARCH_MIN_FACTOR * stictionBreakMaxFwd;
        calibrationMaxPwm = stictionBreakMaxFwd;
        calibrationPwm = (calibrationMinPwm + calibrationMaxPwm) / 2.0f;

        setExperimentPwm(0);

        VF("MSG:"); V(axisPrefix); VF("FWD Max Stiction at ");
        V(stictionBreakMaxFwd); V("% PWM"); V("% - Movement: ");
        V(unsignedVelocity); VLF(" steps/sec");
      }
      break;

    case CALIBRATION_STICTION_REFINE_MIN_FWD:
      if (unsignedVelocity < 0.1f) {
        // No movement: too weak → increase lower bound
        calibrationMinPwm = calibrationPwm;
      } else {
        // Movement detected → PWM is sufficient → tighten upper bound
        calibrationMaxPwm = calibrationPwm;
      }

      // Stop condition: bounds close enough
      if (fabs(calibrationMaxPwm - calibrationMinPwm) <= SERVO_CALIBRATION_STICTION_REFINE_STEP) {
        stictionBreakMinFwd = calibrationMaxPwm;
        calibrationState = CALIBRATION_VELOCITY_SEARCH_FWD;
        calibrationPwm = stictionBreakMinFwd * SERVO_CALIBRATION_KICKSTART_DROP_FACTOR;
        calibrationPhaseCount = 0;

        VF("MSG:"); V(axisPrefix); VF("FWD Min Stiction found at ");
        V(stictionBreakMinFwd); VLF("% PWM");

        setExperimentPwm(calibrationPwm);
        calibrationStartTime = currentTime;
        calibrationStartTicks = currentTicks;
        break;
      }

      // Continue binary search
      calibrationPwm = (calibrationMinPwm + calibrationMaxPwm) / 2.0f;

      // wait the motor to settle after previous pwm - set it to 0
      settleStartTime = currentTime;
      waitingForSettle = true;
      stictionTestPwm = calibrationPwm;
      setExperimentPwm(0);

      VF("MSG:"); V(axisPrefix); VF("FWD Min Stiction Phase ");
      V(calibrationPhaseCount); VF(": PWM=");
      V(calibrationPwm); V("% - Movement: ");
      V(unsignedVelocity); VLF(" steps/sec");

      calibrationPhaseCount++;
      break;

    case CALIBRATION_VELOCITY_SEARCH_FWD: {
        // If motor isn't moving, kickstart with stictionBreakMin and try again
        if (fabs(actualVelocity) < 0.1f) {
            VF("MSG:"); V(axisPrefix); VF("FWD Motor stopped at PWM=");
            V(calibrationPwm); VL("% - Applying stictionBreakMin");

            // Try to kickstart with stictionBreakMin
            setExperimentPwm(stictionBreakMinFwd);
            delay(SERVO_CALIBRATION_TIME_PERIOD);

            // Check if motor moves with stictionBreakMin
            long checkStartTicks = instrumentCoordinateSteps;
            unsigned long checkStartTime = millis();
            delay(SERVO_CALIBRATION_TIME_PERIOD);
            float checkVelocity = (instrumentCoordinateSteps - checkStartTicks) / ((millis() - checkStartTime) / 1000.0f);

            if (fabs(checkVelocity) < 0.1f) {
                // Motor didn't move even with stictionBreakMin - abort calibration
                VF("MSG:"); V(axisPrefix); VL("FWD Motor won't move with stictionBreakMin");
                VF("MSG:"); V(axisPrefix); VL("Skipping FWD velocity search — switching to REV");

                // Instead of jumping directly to REV, insert settling first:
                trackingPwmFwd = calibrationPwm;
                calibrationState = CALIBRATION_PREP_REV;
                settleStartTime = millis();
                waitingForSettle = true;
                setExperimentPwm(0);  // Actively stop motor
                break;
            }

            // Motor moved with stictionBreakMin - continue binary search with higher PWM
            calibrationMinPwm = stictionBreakMinFwd;
            calibrationPwm = (calibrationMinPwm + calibrationMaxPwm) / 2.0f;

            setExperimentPwm(calibrationPwm);
            calibrationStartTime = millis();
            calibrationStartTicks = instrumentCoordinateSteps;
            calibrationPhaseCount++;
            break;
        }

        float velocityError = actualVelocity - targetVelocity;
        float relError = fabs(velocityError / targetVelocity) * 100.0f;

        VF("MSG:"); V(axisPrefix); VF("FWD Velocity Phase ");
        V(calibrationPhaseCount); VF(": PWM=");
        V(calibrationPwm); VF("% → ");
        V(actualVelocity); VF(" steps/sec (err=");
        V(relError); VLF("%)");

        if (relError <= SERVO_CALIBRATION_ERROR_THRESHOLD) {
            trackingPwmFwd = calibrationPwm;
            VF("MSG:"); V(axisPrefix); VF("FWD Tracking at ");
            V(trackingPwmFwd); VLF("% PWM");

            calibrationState = CALIBRATION_STICTION_BREAK_MAX_REV;
            calibrationPwm = -SERVO_CALIBRATION_START_DUTY_CYCLE;
            setExperimentPwm(calibrationPwm);
            calibrationStartTime = millis();
            calibrationStartTicks = instrumentCoordinateSteps;
            calibrationPhaseCount = 0;
        } else {
            // Binary search update
            if (velocityError < 0) {
                // Too slow - need higher PWM
                calibrationMinPwm = calibrationPwm;
            } else {
                // Too fast - need lower PWM
                calibrationMaxPwm = calibrationPwm;
            }

            // Calculate new PWM value
            calibrationPwm = (calibrationMinPwm + calibrationMaxPwm) / 2.0f;

            // Ensure we stay within reasonable bounds
            calibrationPwm = constrain(calibrationPwm,
                SERVO_CALIBRATION_START_DUTY_CYCLE,
                stictionBreakMaxFwd);

            setExperimentPwm(calibrationPwm);
            calibrationStartTime = millis();
            calibrationStartTicks = instrumentCoordinateSteps;
            calibrationPhaseCount++;
        }
        break;
    }

    case CALIBRATION_PREP_REV:
      if (millis() - settleStartTime >= SERVO_CALIBRATION_STICTION_SETTLE_TIME) {
        calibrationState = CALIBRATION_STICTION_BREAK_MAX_REV;
        calibrationPwm = -SERVO_CALIBRATION_START_DUTY_CYCLE;
        setExperimentPwm(calibrationPwm);
        calibrationStartTime = millis();
        calibrationStartTicks = instrumentCoordinateSteps;
        calibrationPhaseCount = 0;
      }
      break;

    case CALIBRATION_STICTION_BREAK_MAX_REV:
    if (unsignedVelocity < 0.1f) {
      calibrationPwm *= 2.0f;

      if (calibrationPwm < -SERVO_CALIBRATION_PWM_MAX) {
        VF("MSG:"); V(axisPrefix); VL("REV Max Stiction not found");
        stictionBreakMaxRev = 0.0f;
        stictionBreakMinRev = 0.0f;

        // Skip to balance check (continue calibration)
        calibrationState = CALIBRATION_CHECK_IMBALANCE;
        experimentMode = false;
        setExperimentPwm(0);
      } else {
        setExperimentPwm(calibrationPwm);
        calibrationStartTime = currentTime;
        calibrationStartTicks = currentTicks;
        calibrationPhaseCount++;

        VF("MSG:"); V(axisPrefix); VF("REV Max Stiction Phase ");
        V(calibrationPhaseCount); VF(": PWM=");
        V(calibrationPwm); VLF("% - No movement");
      }
    } else {
      stictionBreakMaxRev = fabs(calibrationPwm);
      calibrationState = CALIBRATION_STICTION_REFINE_MIN_REV;
      calibrationPhaseCount = 0;

      // Set binary search bounds for min stiction
      calibrationMinPwm = SERVO_CALIBRATION_VELOCITY_SEARCH_MIN_FACTOR * stictionBreakMaxRev;
      calibrationMaxPwm = stictionBreakMaxRev;
      calibrationPwm = (calibrationMinPwm + calibrationMaxPwm) / 2.0f;

      // Wait motor to settle before applying new PWM
      settleStartTime = currentTime;
      waitingForSettle = true;
      stictionTestPwm = -calibrationPwm;  // Make sure it's negative

      setExperimentPwm(0);

      VF("MSG:"); V(axisPrefix); VF("REV Max Stiction at ");
      V(stictionBreakMaxRev); VF("% PWM - Movement: ");
      V(unsignedVelocity); VLF(" steps/sec");
    }
    break;

    case CALIBRATION_STICTION_REFINE_MIN_REV:
      if (unsignedVelocity < 0.1f) {
        // No movement: update min bound (motor too weak)
        calibrationMinPwm = calibrationPwm;
      } else {
        // Movement: update max bound (PWM sufficient)
        calibrationMaxPwm = calibrationPwm;
      }

      // Stop condition: bounds are close enough
      if (fabs(calibrationMaxPwm - calibrationMinPwm) <= SERVO_CALIBRATION_STICTION_REFINE_STEP) {
        stictionBreakMinRev = fabs(calibrationMaxPwm);
        calibrationState = CALIBRATION_VELOCITY_SEARCH_REV;
        calibrationPwm = -stictionBreakMinRev * SERVO_CALIBRATION_KICKSTART_DROP_FACTOR;
        calibrationPhaseCount = 0;

        VF("MSG:"); V(axisPrefix); V("REV Min Stiction found at ");
        V(stictionBreakMinRev); VLF("% PWM");

        setExperimentPwm(calibrationPwm);
        calibrationStartTime = currentTime;
        calibrationStartTicks = currentTicks;
        break;
      }

      // Continue binary search
      calibrationPwm = (calibrationMinPwm + calibrationMaxPwm) / 2.0f;

      // wait the motor to settle after previous pwm - set it to 0
      settleStartTime = currentTime;
      waitingForSettle = true;
      stictionTestPwm = calibrationPwm;
      setExperimentPwm(0);

      VF("MSG:"); V(axisPrefix); VF("REV Min Stiction Phase ");
      V(calibrationPhaseCount); VF(": PWM=");
      V(calibrationPwm); VF("% - Movement: ");
      V(unsignedVelocity); VLF(" steps/sec");

      calibrationPhaseCount++;
      break;

    case CALIBRATION_VELOCITY_SEARCH_REV: {
        // If motor isn't moving, kickstart with stictionBreakMin and try again
        if (fabs(actualVelocity) < 0.1f) {
            VF("MSG:"); V(axisPrefix); VF("REV Motor stopped at PWM=");
            V(calibrationPwm); VL("% - Applying stictionBreakMin");

            // Try to kickstart with stictionBreakMin (negative value for reverse)
            setExperimentPwm(-stictionBreakMinRev);
            delay(SERVO_CALIBRATION_TIME_PERIOD);

            // Check if motor moves with stictionBreakMin
            long checkStartTicks = instrumentCoordinateSteps;
            unsigned long checkStartTime = millis();
            delay(SERVO_CALIBRATION_TIME_PERIOD);
            float checkVelocity = (instrumentCoordinateSteps - checkStartTicks) / ((millis() - checkStartTime) / 1000.0f);

            if (fabs(checkVelocity) < 0.1f) {
                // Motor didn't move even with stictionBreakMin - abort calibration
                VF("MSG:"); V(axisPrefix); VL("REV Motor won't move with stictionBreakMin - aborting calibration");
                VF("MSG:"); V(axisPrefix); VL("Skipping REV velocity search — switching to BALANCE");

                trackingPwmFwd = 0.0f;
                calibrationState = CALIBRATION_CHECK_IMBALANCE;
                calibrationPwm = -SERVO_CALIBRATION_START_DUTY_CYCLE;
                setExperimentPwm(calibrationPwm);
                calibrationStartTime = millis();
                calibrationStartTicks = instrumentCoordinateSteps;
                calibrationPhaseCount = 0;
                break;
            }

            // Motor moved with stictionBreakMin - continue binary search with higher PWM (more negative)
            calibrationMaxPwm = -stictionBreakMinRev;
            calibrationPwm = (calibrationMinPwm + calibrationMaxPwm) / 2.0f;

            setExperimentPwm(calibrationPwm);
            calibrationStartTime = millis();
            calibrationStartTicks = instrumentCoordinateSteps;
            calibrationPhaseCount++;
            break;
        }

        float velocityError = actualVelocity - (-targetVelocity); // Note negative target for reverse
        float relError = fabs(velocityError / targetVelocity) * 100.0f;

        VF("MSG:"); V(axisPrefix); VF("REV Velocity Phase ");
        V(calibrationPhaseCount); VF(": PWM=");
        V(calibrationPwm); VF("% → ");
        V(actualVelocity); VF(" steps/sec (err=");
        V(relError); VLF("%)");

        if (relError <= SERVO_CALIBRATION_ERROR_THRESHOLD) {
            trackingPwmRev = fabs(calibrationPwm);
            VF("MSG:"); V(axisPrefix); VF("REV Tracking at ");
            V(trackingPwmRev); VLF("% PWM");

            calibrationState = CALIBRATION_CHECK_IMBALANCE;
            setExperimentPwm(0);
        } else {
            // Binary search update
            if (velocityError < 0) {
                // Too slow (not enough reverse) - need more negative PWM
                calibrationMaxPwm = calibrationPwm;
            } else {
                // Too fast (too much reverse) - need less negative PWM
                calibrationMinPwm = calibrationPwm;
            }

            // Calculate new PWM value
            calibrationPwm = (calibrationMinPwm + calibrationMaxPwm) / 2.0f;

            // Ensure we stay within reasonable bounds
            calibrationPwm = constrain(calibrationPwm,
                -SERVO_CALIBRATION_START_DUTY_CYCLE,
                -stictionBreakMinRev);

            setExperimentPwm(calibrationPwm);
            calibrationStartTime = millis();
            calibrationStartTicks = instrumentCoordinateSteps;
            calibrationPhaseCount++;
        }
        break;
    }

    case CALIBRATION_CHECK_IMBALANCE:
      VF("MSG:"); V(axisPrefix); VF("FWD: StictionMax=");
      V(stictionBreakMaxFwd); VF("%, StictionMin=");
      V(stictionBreakMinFwd); VF("%, Tracking=");
      V(trackingPwmFwd); VLF("%");

      VF("MSG:"); V(axisPrefix); VF("REV: StictionMax=");
      V(stictionBreakMaxRev); VF("%, StictionMin=");
      V(stictionBreakMinRev); VF("%, Tracking=");
      V(trackingPwmRev); VLF("%");

      // Check for significant imbalance between directions
      float imbalance = fabs(trackingPwmFwd - trackingPwmRev) / ((trackingPwmFwd + trackingPwmRev)/2.0f) * 100.0f;
      if (imbalance > SERVO_CALIBRATION_IMBALANCE_ERROR_THRESHOLD) {
          VF("MSG:"); V(axisPrefix); VF("WARNING: Significant imbalance between directions: ");
          V(imbalance); VLF("%");
      }

      experimentMode = false;
      setExperimentPwm(0);
      printReport();  // print report
      calibrationState = CALIBRATION_IDLE;
      break;
  }
}

float ServoCalibrateTrackingVelocity::getStictionBreakMax(bool forward) {
  return forward ? stictionBreakMaxFwd : stictionBreakMaxRev;
}

float ServoCalibrateTrackingVelocity::getStictionBreakMin(bool forward) {
  return forward ? stictionBreakMinFwd : stictionBreakMinRev;
}

float ServoCalibrateTrackingVelocity::getTrackingPwm(bool forward) {
  return forward ? trackingPwmFwd : trackingPwmRev;
}

void ServoCalibrateTrackingVelocity::setExperimentPwm(float pwm) {
  experimentPwm = constrain(pwm, -SERVO_CALIBRATION_PWM_MAX, SERVO_CALIBRATION_PWM_MAX);
}

void ServoCalibrateTrackingVelocity::printReport() {
  VF("=== Calibration Report for"); V(axisPrefix); VLF(" ===");

  VF("Stiction Break (FWD): Max = "); V(stictionBreakMaxFwd); VF("%, Min = "); V(stictionBreakMinFwd); VLF("%");
  VF("Tracking PWM (FWD): "); V(trackingPwmFwd); VLF("%");

  VF("Stiction Break (REV): Max = "); V(stictionBreakMaxRev); VF("%, Min = "); V(stictionBreakMinRev); VLF("%");
  VF("Tracking PWM (REV): "); V(trackingPwmRev); VLF("%");

  float stictionImbalance = fabs(stictionBreakMinFwd - stictionBreakMinRev);
  float trackingImbalance = fabs(trackingPwmFwd - trackingPwmRev);

  VF("Stiction Min Imbalance: "); V(stictionImbalance); VLF("%");
  VF("Tracking PWM Imbalance: "); V(trackingImbalance); VLF("%");

  float pwmUsed = max(trackingPwmFwd, trackingPwmRev);
  if (pwmUsed >= SERVO_CALIBRATION_PWM_MAX - 0.1f) {
    VF("WARNING: PWM hit calibration limit ("); V(SERVO_CALIBRATION_PWM_MAX); VLF("%)");
  }

  VLF("=== End of Report ===");
}

#endif