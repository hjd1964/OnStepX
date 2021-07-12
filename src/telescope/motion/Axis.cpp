// -----------------------------------------------------------------------------------
// Axis motion control

#include "../../Common.h"
#include "../../tasks/OnTask.h"
extern Tasks tasks;
#include "../Telescope.h"
#include "Axis.h"
#include "../../lib/sense/Sense.h"

extern unsigned long periodSubMicros;

void Axis::enable(bool value) {
  if (pins.enable != OFF) {
    if (value) { digitalWriteEx(pins.enable, invertEnable?HIGH:LOW); } else { digitalWriteEx(pins.enable, invertEnable?LOW:HIGH); }
  }
  enabled = value;
}

bool Axis::isEnabled() {
  return enabled;
}

double Axis::getStepsPerMeasure() {
  return settings.stepsPerMeasure;
}

int Axis::getStepsPerStepSlewing() {
  return stepsPerStepSlewing;
}

void Axis::setMotorCoordinate(double value) {
  long steps = lround(value*settings.stepsPerMeasure);
  setMotorCoordinateSteps(steps);
}

double Axis::getMotorCoordinate() {
  noInterrupts();
  long steps = motorSteps + backlashSteps;
  interrupts();
  return steps/settings.stepsPerMeasure;
}

void Axis::setMotorCoordinateSteps(long value) {
  indexSteps    = 0;
  noInterrupts();
  motorSteps    = value;
  targetSteps   = value;
  backlashSteps = 0;
  interrupts();
}

long Axis::getMotorCoordinateSteps() {
  noInterrupts();
  long steps = motorSteps + backlashSteps;
  interrupts();
  return steps;
}

void Axis::setInstrumentCoordinate(double value) {
  long steps = lround(value*settings.stepsPerMeasure);
  noInterrupts();
  indexSteps = steps - motorSteps;
  interrupts();
}

double Axis::getInstrumentCoordinate() {
  noInterrupts();
  long steps = motorSteps + indexSteps;
  interrupts();
  return steps/settings.stepsPerMeasure;
}

long Axis::getInstrumentCoordinateSteps() {
  noInterrupts();
  long steps = motorSteps + indexSteps;
  interrupts();
  return steps;
}

void Axis::setInstrumentCoordinatePark(double value) {
  long steps = lround(value*settings.stepsPerMeasure);
  steps -= settings.microsteps*2L;
  for (int l = 0; l < settings.microsteps*4; l++) { if (steps % settings.microsteps*4L == 0) break; steps++; }
  noInterrupts();
  indexSteps = steps - motorSteps;
  interrupts();
}

void Axis::setTargetCoordinatePark(double value) {
  target = value;
  long steps = lround(value*settings.stepsPerMeasure);
  long targetParkSteps = steps - indexSteps;
  targetParkSteps -= settings.microsteps*2L;
  for (int l = 0; l < settings.microsteps*4; l++) { if (targetParkSteps % settings.microsteps*4L == 0) break; targetParkSteps++; }
  noInterrupts();
  targetSteps = targetParkSteps;
  interrupts();
}

void Axis::setTargetCoordinate(double value) {
  target = value;
  long steps = lround(value*settings.stepsPerMeasure);
  noInterrupts();
  targetSteps = steps - indexSteps;
  interrupts();
}

void Axis::setTargetCoordinateSteps(long value) {
  target = value/settings.stepsPerMeasure;
  noInterrupts();
  targetSteps = value - indexSteps;
  interrupts();
}

double Axis::getTargetCoordinate() {
  noInterrupts();
  long steps = targetSteps + indexSteps;
  interrupts();
  return steps/settings.stepsPerMeasure;
}

long Axis::getTargetCoordinateSteps() {
  noInterrupts();
  long steps = targetSteps + indexSteps;
  interrupts();
  return steps;
}

// mark the start coordinate of a autoSlewRateByDistance()
void Axis::markOriginCoordinate() {
  noInterrupts();
  originSteps = motorSteps;
  interrupts();
}

// check if we're near the target coordinate during an auto slew
bool Axis::nearTarget() {
  return labs(motorSteps - targetSteps) <= step * 2;
}

// distance to origin or target, whichever is closer, in "measures" (degrees, microns, etc.)
double Axis::getOriginOrTargetDistance() {
  noInterrupts();
  long steps = motorSteps;
  interrupts();
  long distanceOrigin = labs(originSteps - steps);
  long distanceTarget = labs(targetSteps - steps);
  if (distanceOrigin < distanceTarget) return distanceOrigin/settings.stepsPerMeasure; else return distanceTarget/settings.stepsPerMeasure;
}

// distance to target in "measures" (degrees, microns, etc.)
double Axis::getTargetDistance() {
  noInterrupts();
  long steps = motorSteps;
  interrupts();
  return labs(targetSteps - steps)/settings.stepsPerMeasure;
}

// distance to target in steps
long Axis::getTargetDistanceSteps() {
  noInterrupts();
  long steps = motorSteps;
  interrupts();
  return labs(targetSteps - steps);
}

// set base movement frequency in "measures" (radians, microns, etc.) per second
void Axis::setFrequencyBase(float frequency) {
  if (minFreq != 0.0F && frequency < minFreq) frequency = minFreq;
  if (maxFreq != 0.0F && frequency > maxFreq) frequency = maxFreq;
  baseFreq = frequency;
}

// set slew frequency in "measures" (radians, microns, etc.) per second
void Axis::setFrequencySlew(float frequency) {
  if (minFreq != 0.0F && frequency < minFreq) frequency = minFreq;
  if (maxFreq != 0.0F && frequency > maxFreq) frequency = maxFreq;
  slewFreq = frequency;
  if (frequency != 0.0F) minPeriodMicros = 1000000.0F/((slewFreq + baseFreq)*settings.stepsPerMeasure); else minPeriodMicros = 0.0F;
}

// set maximum slew frequency in "measures" (radians, microns, etc.) per second
void Axis::setFrequencyMax(float frequency) {
  maxFreq = frequency;
}

// set minimum slew frequency in "measures" (radians, microns, etc.) per second
void Axis::setFrequencyMin(float frequency) {
  minFreq = frequency;
}

// set acceleration rate in "measures" per second per second
void Axis::setSlewAccelerationRate(float mpsps) {
  slewMpspcs = mpsps/100.0F;
}

// set time to emergency stop movement, with acceleration in "measures" per second per second
void Axis::setSlewAccelerationRateAbort(float mpsps) {
  abortMpspcs = mpsps/100.0F;
}

// slew, with acceleration distance (in "measures" to FrequencySlew)
void Axis::autoSlewRateByDistance(float distance) {
  markOriginCoordinate();
  autoRate = AR_RATE_BY_DISTANCE;
  slewAccelerationDistance = distance;
  tracking = false;
  driver.modeDecaySlewing();
  V(axisPrefix); VLF("autoSlewRateByDistance(); slew started");
}

// stops, with deacceleration by distance
void Axis::autoSlewRateByDistanceStop() {
  driver.modeDecayTracking();
  tracking = true;
  autoRate = AR_NONE;
}

// slew, with acceleration in "measures" per second per second
void Axis::autoSlew(Direction direction) {
  if (direction == DIR_NONE) return;
  if (autoRate == AR_NONE) {
    tracking = true;
    driver.modeDecaySlewing();
    V(axisPrefix); VLF("autoSlew(); slew started");
  }
  if (direction == DIR_FORWARD) autoRate = AR_RATE_BY_TIME_FORWARD; else autoRate = AR_RATE_BY_TIME_REVERSE;
}

// slew to home, with acceleration in "measures" per second per second
void Axis::autoSlewHome() {
  if (pins.sense.home != OFF) {
    tracking = true;
    if (homingStage == HOME_NONE) homingStage = HOME_FAST;
    if (autoRate == AR_NONE) {
      driver.modeDecaySlewing();
      V(axisPrefix); VF("autoSlewHome(); slew home ");
      switch (homingStage) {
        case HOME_FAST: VL("fast"); break;
        case HOME_SLOW: VL("slow"); break;
        case HOME_FINE: VL("fine"); break;
        case HOME_NONE: VL("????"); break;
      }
    }
    if (senses.read(homeSenseHandle)) {
      V(axisPrefix); VLF("autoSlewHome(); moving forward");
      autoRate = AR_RATE_BY_TIME_FORWARD;
    } else {
      V(axisPrefix); VLF("autoSlewHome(); moving reverse");
      autoRate = AR_RATE_BY_TIME_REVERSE;
    }
  }
}

// stops, with deacceleration by time
void Axis::autoSlewStop() {
  if (autoRate != AR_NONE && autoRate != AR_RATE_BY_TIME_ABORT) {
    V(axisPrefix); VLF("autoSlewStop(); slew stopping");
    autoRate = AR_RATE_BY_TIME_END;
    poll();
  }
}

// emergency stops, with deacceleration by time
void Axis::autoSlewAbort() {
  if (autoRate != AR_NONE) {
    V(axisPrefix); VLF("autoSlewAbort(); slew aborting");
    autoRate = AR_RATE_BY_TIME_ABORT;
    homingStage = HOME_NONE;
    tracking = true;
    poll();
  }
}

// checks if slew is active on this axis
bool Axis::autoSlewActive() {
  return autoRate != AR_NONE;  
}

void Axis::poll() {
  // make sure we're ready
  if (axisNumber == 0) return;

  // check limits
  error.minLimitSensed = senses.read(minSenseHandle);
  error.maxLimitSensed = senses.read(maxSenseHandle);

  // stop homing as we pass by the sensor
  if (homingStage != HOME_NONE) {
    if (autoRate == AR_RATE_BY_TIME_FORWARD && !senses.read(homeSenseHandle)) autoSlewStop();
    if (autoRate == AR_RATE_BY_TIME_REVERSE && senses.read(homeSenseHandle)) autoSlewStop();
  }

  // automatically abort slew and stop tracking
  if (autoRate != AR_RATE_BY_TIME_ABORT && lastPeriod != 0) {
    if (trackingStep < 0 && motionReverseError()) {
      V(axisPrefix); VLF("poll(); motion reverse limit or error");
      autoSlewAbort();
      return;
    }
    if (trackingStep > 0 && motionForwardError()) {
      V(axisPrefix); VLF("poll(); motion forward limit or error");
      autoSlewAbort();
      return;
    }
  }

  // slewing
  if (autoRate != AR_NONE) {
    if (autoRate == AR_RATE_BY_DISTANCE) {
      if (getTargetDistanceSteps() == 0) {
        V(axisPrefix); VLF("poll(); slew automatically stopped");
        freq = 0.0F;
        setFrequency(freq);
        autoRate = AR_NONE;
        tracking = true;
      } else {
        freq = (getOriginOrTargetDistance()/slewAccelerationDistance)*slewFreq + backlashFreq;
        if (freq < backlashFreq) freq = backlashFreq;
        if (freq > slewFreq) freq = slewFreq;
        if (direction == DIR_REVERSE) freq = -freq;
      }
    } else
    if (autoRate == AR_RATE_BY_TIME_FORWARD) {
      freq += slewMpspcs;
      if (freq > slewFreq) freq = slewFreq;
    } else
    if (autoRate == AR_RATE_BY_TIME_REVERSE) {
      freq -= slewMpspcs;
      if (freq < -slewFreq) freq = -slewFreq;
    } else
    if (autoRate == AR_RATE_BY_TIME_END) {
      if (freq > slewMpspcs) freq -= slewMpspcs; else if (freq < -slewMpspcs) freq += slewMpspcs; else freq = 0.0F;
      if (abs(freq) <= slewMpspcs) {
        driver.modeDecayTracking();
        autoRate = AR_NONE;
        freq = 0.0F;
        if (homingStage == HOME_FAST) homingStage = HOME_SLOW; else 
        if (homingStage == HOME_SLOW) homingStage = HOME_FINE; else
        if (homingStage == HOME_FINE) homingStage = HOME_NONE;
        if (homingStage != HOME_NONE) {
          float f = slewFreq/4.0F;
          if (abs(f) < 0.0003) { if (f < 0) f = -0.0003; else f = 0.0003; }
          setFrequencySlew(abs(f));
          autoSlewHome();
        } else {
          V(axisPrefix); VLF("poll(); slew stopped");
        }
      }
    } else
    if (autoRate == AR_RATE_BY_TIME_ABORT) {
      if (freq > abortMpspcs) freq -= abortMpspcs; else if (freq < -abortMpspcs) freq += abortMpspcs; else freq = 0.0F;
      if (fabs(freq) <= abortMpspcs) {
        driver.modeDecayTracking();
        autoRate = AR_NONE;
        freq = 0.0F;
        V(axisPrefix); VLF("poll(); slew aborted");
      }
    } else freq = 0.0F;

    // ISR swap and microstep mode switching
    if (microstepModeControl == MMC_SLEWING) {
      if (fabs(freq) <= backlashFreq*1.2F || autoRate == AR_NONE) {
        if (driver.modeSwitchAllowed()) {
          V(axisPrefix); VLF("poll(); mode switch tracking set");
          driver.modeMicrostepTracking();
        }
        microstepModeControl = MMC_TRACKING;
        if (enableMoveFast(false)) {
          #if DEBUG_MODE == VERBOSE
            V(axisPrefix); VF("poll(); high speed ISR swapped out at ");
            if (axisNumber <= 3) { V(radToDeg(freq)); VL(" deg/sec."); } else { V(freq); VL(" microns/sec."); }
          #endif
        }
      }
    } else {
      if (fabs(freq) > backlashFreq*1.2F) {
        if (driver.modeSwitchAllowed()) {
          if (microstepModeControl == MMC_TRACKING) {
            microstepModeControl = MMC_SLEWING_REQUEST;
            V(axisPrefix); VLF("poll(); mode switch slewing requested");
            return;
          } else
          if (microstepModeControl != MMC_SLEWING_READY) return;
          V(axisPrefix); VLF("poll(); mode switch slewing set");
          slewStep = driver.modeMicrostepSlewing();
        }
        microstepModeControl = MMC_SLEWING;
        if (enableMoveFast(true)) {
          #if DEBUG_MODE == VERBOSE
            V(axisPrefix); VF("poll(); high speed ISR swapped in at ");
            if (axisNumber <= 3) { V(radToDeg(freq)); VL(" deg/sec."); } else { V(freq); VL(" microns/sec."); }
          #endif
        }
      }
    }
  } else freq = 0.0F;

  // apply the composite or backlash frequency as required
  float f = freq;
  if (tracking) f += baseFreq;
  if (microstepModeControl == MMC_SLEWING) {
    setFrequency(f/slewStep);
  } else {
    if (inBacklash()) { if (f >= 0.0) f = backlashFreq; else f = -backlashFreq; }
    setFrequency(f);
  }

  // refresh the driver status
  driver.updateStatus();
}

// set frequency in "measures" (degrees, microns, etc.) per second (0 stops motion)
void Axis::setFrequency(float frequency) {
  if (frequency < 0.0F) {
    frequency = -frequency;
    noInterrupts(); trackingStep = -1; interrupts();
  } else {
    noInterrupts(); trackingStep =  1; interrupts();
  }

  lastFreq = frequency;
  // frequency in measures per second to period in microsecond counts per step
  float period = 1000000.0F/(frequency*settings.stepsPerMeasure);
  if (period < minPeriodMicros) period = minPeriodMicros;
  if (STEP_WAVE_FORM == SQUARE) period /= 2.0F;
  if (!isnan(period) && fabs(period) <= 134000000.0F) {
    // convert microsecond counts to sub-microsecond counts
    period *= 16.0F;
    lastPeriodSet = (unsigned long)lround(period);

    // adjust period for MCU clock inaccuracy
    period *= (SIDEREAL_PERIOD/periodSubMicros);
    // if this is the active period, just return
    if (lastPeriod == (unsigned long)lround(period)) return;
    lastPeriod = (unsigned long)lround(period);
  } else { period = 0.0; lastPeriodSet = 0; lastPeriod = 0; noInterrupts(); trackingStep = 0; interrupts(); }
  tasks.setPeriodSubMicros(taskHandle, lastPeriod);
}

float Axis::getFrequency() {
  return getFrequencySteps()/settings.stepsPerMeasure;
}

float Axis::getFrequencySteps() {
  if (lastPeriodSet == 0) return 0;
  #if STEP_WAVE_FORM == SQUARE
    return 16000000.0F/(lastPeriodSet*2.0F);
  #else
    return 16000000.0F/lastPeriodSet;
  #endif
}

void Axis::setTracking(bool state) {
  tracking = state;
}

bool Axis::getTracking() {
  return tracking;
}

void Axis::setBacklash(float value) {
  noInterrupts();
  backlashAmountSteps = round(value*settings.stepsPerMeasure);
  interrupts();
}

float Axis::getBacklash() {
  noInterrupts();
  uint16_t b = backlashSteps;
  interrupts();
  return b/settings.stepsPerMeasure;
}

void Axis::setBacklashSteps(long value) {
  noInterrupts();
  backlashAmountSteps = value;
  interrupts();
}

long Axis::getBacklashSteps() {
  noInterrupts();
  uint16_t b = backlashSteps;
  interrupts();
  return b;
}

bool Axis::inBacklash() {
  noInterrupts();
  uint16_t b = backlashSteps;
  interrupts();
  if (b <= 0 || b >= backlashAmountSteps) return false; else return true;
}

void Axis::disableBacklash() {
  noInterrupts();
  backlashStepsStore = backlashSteps;
  motorSteps += backlashSteps;
  backlashSteps = 0;
  interrupts();
}

void Axis::enableBacklash() {
  noInterrupts();
  backlashSteps = backlashStepsStore;
  motorSteps -= backlashSteps;
  interrupts();
  backlashStepsStore = 0;
}

void Axis::setMotionLimitsCheck(bool state) {
  limitsCheck = state;
}

bool Axis::motionForwardError() {
  return driver.getStatus().fault ||
         (limitsCheck && getInstrumentCoordinate() > settings.limits.max) ||
         error.maxLimitSensed;
}

bool Axis::motionReverseError() {
  return driver.getStatus().fault ||
         (limitsCheck && getInstrumentCoordinate() < settings.limits.min) ||
         error.minLimitSensed;
}

bool Axis::motionError() {
  return motionForwardError() || motionReverseError();
}
