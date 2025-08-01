// -----------------------------------------------------------------------------------
// calibrate servo tracking velocity

#include "TrackingVelocity.h"

#if defined(SERVO_MOTOR_PRESENT) && defined(CALIBRATE_SERVO_DC)

ServoCalibrateTrackingVelocity::ServoCalibrateTrackingVelocity(uint8_t axisNumber) {
  strcpy(axisPrefix, " Axis_VelocityCalibrate, ");
  axisPrefix[5] = '0' + axisNumber;
}

void ServoCalibrateTrackingVelocity::init() {
  // initialize experiment mode variables
  experimentPwm = 0.0F;
  experimentMode = false;

  calibrationState = CALIBRATION_IDLE;
  calibrationPwm = 0;
  stictionBreakPwmFwd = 0;
  stictionBreakPwmRev = 0;
  targetVelocityPwmFwd = 0;
  targetVelocityPwmRev = 0;
  calibrationStartTime = 0;
  calibrationStartTicks = 0;
  calibrationMinPwm = 0;
  calibrationMaxPwm = 0;
  targetVelocity = 0;
  calibrationPhaseCount = 0;
}

// start calibration (check if enabled before calling this)
void ServoCalibrateTrackingVelocity::start(float trackingFrequency, long instrumentCoordinateSteps) {

  // record starting position and time
  calibrationStartTicks = instrumentCoordinateSteps;
  calibrationStartTime = millis();
  
  VF("MSG:"); V(axisPrefix); VL("Starting bidirectional calibration");

  calibrationState = CALIBRATION_STICTION_SEARCH_FWD;
  calibrationPwm = SERVO_CALIBRATION_START_DUTY_CYCLE;  // Start with forward direction
  stictionBreakPwmFwd = 0;
  stictionBreakPwmRev = 0;
  targetVelocityPwmFwd = 0;
  targetVelocityPwmRev = 0;
  calibrationMinPwm = 0;
  calibrationMaxPwm = 0;
  calibrationPhaseCount = 0;

  // set target velocity based on tracking frequency
  targetVelocity = trackingFrequency;

  // enable experiment mode to bypass PID
  experimentMode = true;
  setExperimentPwm(calibrationPwm);

  VF("MSG:"); V(axisPrefix); VF("Testing FWD PWM=");
  V(calibrationPwm); VLF("%");
}

// handle calibration state machine
void ServoCalibrateTrackingVelocity::updateState(long instrumentCoordinateSteps) {

  if (calibrationState == CALIBRATION_IDLE) return;

  long currentTicks = instrumentCoordinateSteps;
  unsigned long currentTime = millis();

  // skip if calibration period not completed
  if (currentTime - calibrationStartTime < (SERVO_CALIBRATION_TIME_PERIOD * 1000)) return;

  // calculate actual velocity with sign
  float elapsedSec = (currentTime - calibrationStartTime) / 1000.0f;
  float actualVelocity = (currentTicks - calibrationStartTicks) / elapsedSec;

  // determine direction and unsigned velocity
  bool forwardDirection = calibrationPwm > 0;
  float unsignedVelocity = fabs(actualVelocity);

  switch (calibrationState) {
    // forward Stiction Search
    case CALIBRATION_STICTION_SEARCH_FWD:
      // no movement
      if (unsignedVelocity < 0.1f) {
        calibrationPwm *= 2.0f; // double PWM

        if (calibrationPwm > 100.0f) {
          VF("MSG:"); V(axisPrefix);
          VL("Stiction break not found in FWD (max PWM reached)");
          calibrationState = CALIBRATION_IDLE;
          experimentMode = false;
          setExperimentPwm(0);
        } else {
          setExperimentPwm(calibrationPwm);
          calibrationStartTime = currentTime;
          calibrationStartTicks = currentTicks;
          calibrationPhaseCount++;

          VF("MSG:"); V(axisPrefix); VF("FWD Phase ");
          V(calibrationPhaseCount); VF(": PWM=");
          V(calibrationPwm); VLF("% - No movement");
        }
      } else
      // Movement detected
      {
        stictionBreakPwmFwd = calibrationPwm;
        calibrationState = CALIBRATION_VELOCITY_SEARCH_FWD;
        calibrationMinPwm = stictionBreakPwmFwd;
        calibrationMaxPwm = stictionBreakPwmFwd;
        calibrationPwm = calibrationMinPwm; // start from stiction value
        calibrationPhaseCount = 0;

        setExperimentPwm(calibrationPwm);
        calibrationStartTime = currentTime;
        calibrationStartTicks = currentTicks;

        VF("MSG:"); V(axisPrefix); VF("FWD Stiction break at ");
        V(stictionBreakPwmFwd); VLF("% PWM");
        VF("MSG:"); V(axisPrefix); VF("FWD Target velocity: ");
        V(targetVelocity); VLF(" steps/sec");
      }
      break;

    // forward Velocity Search
    case CALIBRATION_VELOCITY_SEARCH_FWD: {
      float velocityError = actualVelocity - targetVelocity;
      float absError = fabs(velocityError);
      float relError = (absError / targetVelocity) * 100.0f;

      VF("MSG:"); V(axisPrefix); VF("FWD Phase ");
      V(calibrationPhaseCount); VF(": PWM=");
      V(calibrationPwm); VF("% → ");
      V(actualVelocity); VF(" steps/sec (err=");
      V(relError); VLF("%)");

      if (relError <= SERVO_CALIBRATION_ERROR_THRESHOLD) {
        targetVelocityPwmFwd = calibrationPwm;
        VF("MSG:"); V(axisPrefix); VF("FWD SUCCESS at ");
        V(targetVelocityPwmFwd); VLF("% PWM");

        // Switch to reverse stiction search
        calibrationState = CALIBRATION_STICTION_SEARCH_REV;
        calibrationPwm = -SERVO_CALIBRATION_START_DUTY_CYCLE;
        setExperimentPwm(calibrationPwm);
        calibrationStartTime = currentTime;
        calibrationStartTicks = currentTicks;
        calibrationPhaseCount = 0;

        VF("MSG:"); V(axisPrefix); VLF("Starting REV calibration");
        VF("MSG:"); V(axisPrefix); VF("Testing REV PWM=");
        V(calibrationPwm); VLF("%");
      } else {
        // binary search adjustment
        if (velocityError < 0) {
          // too slow
          calibrationMinPwm = calibrationPwm;
        } else {
          // too fast
          calibrationMaxPwm = calibrationPwm;
        }

        // check convergence
        if (calibrationMaxPwm - calibrationMinPwm < 0.1f) {
          targetVelocityPwmFwd = calibrationPwm;
          VF("MSG:"); V(axisPrefix); VF("FWD CONVERGED at ");
          V(targetVelocityPwmFwd); VLF("% PWM");

          // switch to reverse stiction search
          calibrationState = CALIBRATION_STICTION_SEARCH_REV;
          calibrationPwm = -SERVO_CALIBRATION_START_DUTY_CYCLE;
          setExperimentPwm(calibrationPwm);
          calibrationStartTime = currentTime;
          calibrationStartTicks = currentTicks;
          calibrationPhaseCount = 0;

          VF("MSG:"); V(axisPrefix); VLF("Starting REV calibration");
          VF("MSG:"); V(axisPrefix); VF("Testing REV PWM=");
          V(calibrationPwm); VLF("%");
        } else {
          // new test point
          calibrationPwm = (calibrationMinPwm + calibrationMaxPwm) / 2.0f;
          setExperimentPwm(calibrationPwm);
          calibrationStartTime = currentTime;
          calibrationStartTicks = currentTicks;
          calibrationPhaseCount++;
        }
      }
      break;
    }

    // reverse Stiction Search
    case CALIBRATION_STICTION_SEARCH_REV:
      // no movement
      if (unsignedVelocity < 0.1f) {
        calibrationPwm *= 2.0f; // double PWM magnitude

        if (calibrationPwm < -100.0f) {
          VF("MSG:"); V(axisPrefix);
          VL("Stiction break not found in REV (max PWM reached)");
          calibrationState = CALIBRATION_CHECK_IMBALANCE;
        } else {
          setExperimentPwm(calibrationPwm);
          calibrationStartTime = currentTime;
          calibrationStartTicks = currentTicks;
          calibrationPhaseCount++;

          VF("MSG:"); V(axisPrefix); VF("REV Phase ");
          V(calibrationPhaseCount); VF(": PWM=");
          V(calibrationPwm); VLF("% - No movement");
        }
      } else
      // movement detected
      {
        stictionBreakPwmRev = fabs(calibrationPwm);
        calibrationState = CALIBRATION_VELOCITY_SEARCH_REV;
        calibrationMinPwm = -stictionBreakPwmRev; // negative value
        calibrationMaxPwm = -100.0f;
        calibrationPwm = calibrationMinPwm; // start from stiction value
        calibrationPhaseCount = 0;

        setExperimentPwm(calibrationPwm);
        calibrationStartTime = currentTime;
        calibrationStartTicks = currentTicks;

        VF("MSG:"); V(axisPrefix); VF("REV Stiction break at ");
        V(calibrationPwm); VLF("% PWM");
        VF("MSG:"); V(axisPrefix); VF("REV Target velocity: ");
        V(-targetVelocity); VLF(" steps/sec");
      }
      break;

    // reverse Velocity Search
    case CALIBRATION_VELOCITY_SEARCH_REV: {
      // use negative target velocity for reverse direction
      float velocityError = actualVelocity - (-targetVelocity);
      float absError = fabs(velocityError);
      float relError = (absError / targetVelocity) * 100.0f;

      VF("MSG:"); V(axisPrefix); VF("REV Phase ");
      V(calibrationPhaseCount); VF(": PWM=");
      V(calibrationPwm); VF("% → ");
      V(actualVelocity); VF(" steps/sec (err=");
      V(relError); VLF("%)");

      if (relError <= SERVO_CALIBRATION_ERROR_THRESHOLD) {
        targetVelocityPwmRev = fabs(calibrationPwm);
        VF("MSG:"); V(axisPrefix); VF("REV SUCCESS at ");
        V(targetVelocityPwmRev); VLF("% PWM");
        calibrationState = CALIBRATION_CHECK_IMBALANCE;
      } else {
        // binary search adjustment
        if (velocityError < 0) {
          // too slow (less negative than target)
          calibrationMinPwm = calibrationPwm;  // more negative
        } else {
          // too fast
          calibrationMaxPwm = calibrationPwm;  // less negative
        }

        // check convergence
        if (fabs(calibrationMaxPwm - calibrationMinPwm) < 0.1f) {
          targetVelocityPwmRev = fabs(calibrationPwm);
          VF("MSG:"); V(axisPrefix); VF("REV CONVERGED at ");
          V(targetVelocityPwmRev); VLF("% PWM");
          calibrationState = CALIBRATION_CHECK_IMBALANCE;
        } else {
          // new test point
          calibrationPwm = (calibrationMinPwm + calibrationMaxPwm) / 2.0f;
          setExperimentPwm(calibrationPwm);
          calibrationStartTime = currentTime;
          calibrationStartTicks = currentTicks;
          calibrationPhaseCount++;
        }
      }
      break;
    }

    // imbalance check
    case CALIBRATION_CHECK_IMBALANCE:
      VF("MSG:"); V(axisPrefix); VF("FWD: Stiction=");
      V(stictionBreakPwmFwd); VF("%, Velocity=");
      V(targetVelocityPwmFwd); VLF("%");

      VF("MSG:"); V(axisPrefix); VF("REV: Stiction=");
      V(stictionBreakPwmRev); VF("%, Velocity=");
      V(targetVelocityPwmRev); VLF("%");

      // calculate imbalance percentages
      float stictionImbalance = fabs(stictionBreakPwmFwd - stictionBreakPwmRev);
      float velocityImbalance = fabs(targetVelocityPwmFwd - targetVelocityPwmRev);

      VF("MSG:"); V(axisPrefix); VF("Stiction imbalance: ");
      V(stictionImbalance); VLF("%");
      VF("MSG:"); V(axisPrefix); VF("Velocity imbalance: ");
      V(velocityImbalance); VLF("%");

      // check against threshold
      if (stictionImbalance > SERVO_CALIBRATION_IMBALANCE_ERROR_THRESHOLD ||
          velocityImbalance > SERVO_CALIBRATION_IMBALANCE_ERROR_THRESHOLD) {
        VF("WRN:"); V(axisPrefix);
        VL("Significant imbalance detected! Check telescope balance.");
      }

      // finalize calibration
      experimentMode = false;
      setExperimentPwm(0);
      calibrationState = CALIBRATION_IDLE;
      break;
  }
}

// get stiction break PWM value
float ServoCalibrateTrackingVelocity::getStictionBreakPwm(bool forward) {
  return forward ? stictionBreakPwmFwd : stictionBreakPwmRev;
}

float ServoCalibrateTrackingVelocity::getTargetVelocityPwm(bool forward) {
  return forward ? targetVelocityPwmFwd : targetVelocityPwmRev;
}

// set experiment PWM value
void ServoCalibrateTrackingVelocity::setExperimentPwm(float pwm) {
  experimentPwm = constrain(pwm, -100.0, 100.0);
}

// set experiment mode state
void ServoCalibrateTrackingVelocity::setExperimentMode(bool state) {
  experimentMode = state;
}

#endif