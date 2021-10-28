// -----------------------------------------------------------------------------------
// Axis motion control

#include "Axis.h"

#include "../tasks/OnTask.h"
#include "../sense/Sense.h"

#if defined(SERVO_DRIVER_PRESENT) || defined(SD_DRIVER_PRESENT)

// constructor
Axis::Axis(uint8_t axisNumber, const AxisPins *pins, const AxisSettings *settings) {
  axisPrefix[9] = '0' + axisNumber;
  this->axisNumber = axisNumber;

  this->pins = pins;
  this->settings = *settings;
}

// sets up the driver step/dir/enable pins and any associated driver mode control
void Axis::init(Motor *motor, void (*callback)()) {
  this->motor = motor;
  this->callback = callback;

  // start monitor
  V(axisPrefix); VF("start monitor task (rate "); V(FRACTIONAL_SEC_US); VF("us priority 1)... ");
  uint8_t taskHandle = 0;
  char taskName[] = "Ax_Mtr";
  taskName[2] = axisNumber + '0';
  taskHandle = tasks.add(0, 0, true, 1, callback, taskName);
  tasks.setPeriodMicros(taskHandle, FRACTIONAL_SEC_US);
  if (taskHandle) { VLF("success"); } else { VLF("FAILED!"); }

  // check for reverting axis settings in NV
  if (!nv.isKeyValid()) {
    V(axisPrefix); VLF("writing defaults to NV");
    uint16_t axesToRevert = nv.readUI(NV_AXIS_SETTINGS_REVERT);
    bitSet(axesToRevert, axisNumber);
    nv.write(NV_AXIS_SETTINGS_REVERT, axesToRevert);
  }

  // write axis settings to NV
  // NV_AXIS_SETTINGS_REVERT bit 0 = settings at compile (0) or run time (1), bits 1 to 9 = reset axis n on next boot
  if (AxisSettingsSize < sizeof(AxisSettings)) { nv.readOnly(true); DL("ERR: Axis::init(); AxisSettingsSize error NV subsystem writes disabled"); }
  uint16_t axesToRevert = nv.readUI(NV_AXIS_SETTINGS_REVERT);
  if (!(axesToRevert & 1)) bitSet(axesToRevert, axisNumber);
  if (bitRead(axesToRevert, axisNumber)) {
    V(axisPrefix); VLF("reverting settings to Config.h defaults");
    nv.updateBytes(NV_AXIS_SETTINGS_BASE + (axisNumber - 1)*AxisSettingsSize, &settings, sizeof(AxisSettings));
  }
  bitClear(axesToRevert, axisNumber);
  nv.write(NV_AXIS_SETTINGS_REVERT, axesToRevert);

  // read axis settings from NV
  nv.readBytes(NV_AXIS_SETTINGS_BASE + (axisNumber - 1)*AxisSettingsSize, &settings, sizeof(AxisSettings));
  if (!validateAxisSettings(axisNumber, settings)) nv.readOnly(true);

  #if DEBUG == VERBOSE
    V(axisPrefix); VF("stepsPerMeasure="); V(settings.stepsPerMeasure);
    V(", reverse="); if (settings.reverse == OFF) VL("OFF"); else if (settings.reverse == ON) VL("ON"); else VL("?");
    V(axisPrefix); VF("backlash takeup frequency set to ");
    if (axisNumber <= 3) { V(radToDegF(settings.backlashFreq)); VL(" deg/sec."); } else { V(settings.backlashFreq); VL(" microns/sec."); }
  #endif

  // setup motor
  motor->setReverse(settings.reverse);
  motor->setParam(settings.param1, settings.param2);
  motor->setBacklashFrequencySteps(settings.backlashFreq*settings.stepsPerMeasure);

  // activate home and limit sense
  V(axisPrefix); VLF("adding any home and/or limit senses");
  homeSenseHandle = sense.add(pins->home, pins->axisSense.homeInit, pins->axisSense.homeTrigger);
  minSenseHandle = sense.add(pins->min, pins->axisSense.minMaxInit, pins->axisSense.minTrigger);
  maxSenseHandle = sense.add(pins->max, pins->axisSense.minMaxInit, pins->axisSense.maxTrigger);
}

// enables or disables the associated step/dir driver
void Axis::enable(bool state) {
  enabled = state;
  motor->power(state & !poweredDown);
}

// time (in ms) before automatic power down at standstill, use 0 to disable
void Axis::setPowerDownTime(int value) {
  if (value == 0) powerDownStandstill = false; else { powerDownStandstill = true; powerDownDelay = value; }
}

// time (in ms) to disable automatic power down at standstill, use 0 to disable
void Axis::setPowerDownOverrideTime(int value) {
  if (value == 0) powerDownOverride = false; else { powerDownOverride = true; powerDownOverrideEnds = millis() + value; }
}

// set backlash amount in "measures" (radians, microns, etc.)
void Axis::setBacklash(float value) {
  if (autoRate == AR_NONE) motor->setBacklashSteps(round(value*settings.stepsPerMeasure));
}

// get backlash amount in "measures" (radians, microns, etc.)
float Axis::getBacklash() {
  return motor->getBacklashSteps()/settings.stepsPerMeasure;
}

// reset motor and target angular position, in "measure" units
CommandError Axis::resetPosition(double value) {
  return resetPositionSteps(lround(value*settings.stepsPerMeasure));
}

// reset motor and target angular position, in steps
CommandError Axis::resetPositionSteps(long value) {
  if (autoRate != AR_NONE) return CE_SLEW_IN_MOTION;
  if (motor->getFrequencySteps() != 0) return CE_SLEW_IN_MOTION;
  motor->resetPositionSteps(value);
  return CE_NONE;
}

// get motor position, in "measure" units
double Axis::getMotorPosition() {
  return motor->getMotorPositionSteps()/settings.stepsPerMeasure;
}

// get index position, in "measure" units
double Axis::getIndexPosition() {
  return motor->getIndexPositionSteps()/settings.stepsPerMeasure;
}

double distance(double c1, double c2) {
  double d1 = abs(c1 - c2);
  double d2 = abs(c2 - c1);
  if (d1 <= d2) return d1; else return d2;
}

// convert from unwrapped (full range) to normal (+/- wrapAmount) coordinate
double Axis::wrap(double value) {
  if (wrapEnabled) {
    while (value > settings.limits.max) value -= wrapAmount;
    while (value < settings.limits.min) value += wrapAmount;
  }
  return value;
}

// convert from normal (+/- wrapAmount) to an unwrapped (full range) coordinate
double Axis::unwrap(double value) {
  if (wrapEnabled) {
    double position = motor->getInstrumentCoordinateSteps()/settings.stepsPerMeasure;
    while (value > position + wrapAmount/2.0L) value -= wrapAmount;
    while (value < position - wrapAmount/2.0L) value += wrapAmount;
  }
  return value;
}

// convert from normal (+/- wrapAmount) to an unwrapped (full range) coordinate
// nearest the instrument coordinate
double Axis::unwrapNearest(double value) {
  if (wrapEnabled) {
    value = unwrap(value);
    double instr = motor->getInstrumentCoordinateSteps()/settings.stepsPerMeasure;
//    V(axisPrefix);
//    VF("unwrapNearest instr "); V(radToDeg(instr));
//    VF(", before "); V(radToDeg(value));
    double dist = distance(value, instr);
    if (distance(value + wrapAmount, instr) < dist) value += wrapAmount; else
    if (distance(value - wrapAmount, instr) < dist) value -= wrapAmount;
//    VF(", after "); VL(radToDeg(value));
  }
  return value;
}

// set instrument coordinate, in "measures" (radians, microns, etc.)
void Axis::setInstrumentCoordinate(double value) {
  setInstrumentCoordinateSteps(lround(unwrap(value)*settings.stepsPerMeasure));
}

// get instrument coordinate
double Axis::getInstrumentCoordinate() {
  return wrap(motor->getInstrumentCoordinateSteps()/settings.stepsPerMeasure);
}

// set instrument coordinate park, in "measures" (radians, microns, etc.)
// with backlash disabled this indexes to the nearest position where the motor wouldn't cog
void Axis::setInstrumentCoordinatePark(double value) {
  motor->setInstrumentCoordinateParkSteps(lround(unwrapNearest(value)*settings.stepsPerMeasure), settings.subdivisions);
}

// set target coordinate park, in "measures" (degrees, microns, etc.)
// with backlash disabled this moves to the nearest position where the motor doesn't cog
void Axis::setTargetCoordinatePark(double value) {
  motor->setFrequencySteps(0);
  motor->setTargetCoordinateParkSteps(lround(unwrapNearest(value)*settings.stepsPerMeasure), settings.subdivisions);
}

// set target coordinate, in "measures" (degrees, microns, etc.)
void Axis::setTargetCoordinate(double value) {
  setTargetCoordinateSteps(lround(unwrapNearest(value)*settings.stepsPerMeasure));
}

// get target coordinate, in "measures" (degrees, microns, etc.)
double Axis::getTargetCoordinate() {
  return wrap(motor->getTargetCoordinateSteps()/settings.stepsPerMeasure);
}

// check if we're at the target coordinate during an auto slew
bool Axis::atTarget() {
  return labs(motor->getTargetDistanceSteps()) < 1;
}

// distance to target in "measures" (degrees, microns, etc.)
double Axis::getTargetDistance() {
  return labs(motor->getTargetDistanceSteps())/settings.stepsPerMeasure;
}

// distance to origin or target, whichever is closer, in "measures" (degrees, microns, etc.)
double Axis::getOriginOrTargetDistance() {
  return motor->getOriginOrTargetDistanceSteps()/settings.stepsPerMeasure;
}

// set acceleration rate in "measures" per second per second (for autoSlew)
void Axis::setSlewAccelerationRate(float mpsps) {
  if (autoRate == AR_NONE) {
    slewMpspfs = mpsps/FRACTIONAL_SEC;
    slewAccelTime = NAN;
  }
}

// set acceleration rate in seconds (for autoSlew)
void Axis::setSlewAccelerationTime(float seconds) {
  if (autoRate == AR_NONE) slewAccelTime = seconds;
}

// set acceleration for emergency stop movement in "measures" per second per second
void Axis::setSlewAccelerationRateAbort(float mpsps) {
  if (autoRate == AR_NONE) {
    abortMpspfs = mpsps/FRACTIONAL_SEC;
    abortAccelTime = NAN;
  }
}

// set acceleration for emergency stop movement in seconds (for autoSlewStop)
void Axis::setSlewAccelerationTimeAbort(float seconds) {
  if (autoRate == AR_NONE) abortAccelTime = seconds;
}

// slew with rate by distance
// \param distance: acceleration distance in measures (to frequency)
// \param frequency: optional frequency of slew in "measures" (radians, microns, etc.) per second
CommandError Axis::autoSlewRateByDistance(float distance, float frequency) {
  if (!enabled) return CE_SLEW_ERR_IN_STANDBY;
  if (autoRate != AR_NONE) return CE_SLEW_IN_SLEW;
  if (motionError(DIR_BOTH)) return CE_SLEW_ERR_OUTSIDE_LIMITS;

  if (!isnan(frequency)) setFrequencySlew(frequency);

  motor->markOriginCoordinateSteps();
  slewAccelerationDistance = distance;
  motor->setSynchronized(false);
  motor->setSlewing(true);
  autoRate = AR_RATE_BY_DISTANCE;
  rampFreq = 0.0F;
  return CE_NONE;
}

// stops, with deacceleration by distance
bool Axis::autoSlewRateByDistanceStop() {
  if (autoRate != AR_RATE_BY_DISTANCE) return false;
  autoRate = AR_NONE;
  motor->setSlewing(false);
  motor->setSynchronized(true);
  return true;
}

// auto slew with acceleration in "measures" per second per second
// \param direction: direction of motion, DIR_FORWARD or DIR_REVERSE
// \param frequency: optional frequency of slew in "measures" (radians, microns, etc.) per second
CommandError Axis::autoSlew(Direction direction, float frequency) {
  if (!enabled) return CE_SLEW_ERR_IN_STANDBY;
  if (autoRate == AR_RATE_BY_DISTANCE) return CE_SLEW_IN_SLEW;
  if (motionError(direction)) return CE_SLEW_ERR_OUTSIDE_LIMITS;
  if (direction != DIR_FORWARD && direction != DIR_REVERSE) return CE_SLEW_ERR_UNSPECIFIED;

  if (!isnan(frequency)) setFrequencySlew(frequency);

  V(axisPrefix);
  if (autoRate == AR_NONE) {
    motor->setSynchronized(true);
    motor->setSlewing(true);
    VF("autoSlew start ");
  } else { VF("autoSlew resum "); }
  if (direction == DIR_FORWARD) {
    autoRate = AR_RATE_BY_TIME_FORWARD;
    VF("fwd@ ");
  } else {
    autoRate = AR_RATE_BY_TIME_REVERSE;
    VF("rev@ ");
  }
  #if DEBUG == VERBOSE
    if (axisNumber <= 2) { V(radToDeg(slewFreq)); V("°/s, accel "); SERIAL_DEBUG.print(radToDeg(slewMpspfs)*FRACTIONAL_SEC, 3); VL("°/s/s"); }
    if (axisNumber == 3) { V(slewFreq); V("°/s, accel "); SERIAL_DEBUG.print(slewMpspfs*FRACTIONAL_SEC, 3); VL("°/s/s"); }
    if (axisNumber > 3) { V(slewFreq); V("um/s, accel "); SERIAL_DEBUG.print(slewMpspfs*FRACTIONAL_SEC, 3); VL("um/s/s"); }
  #endif

  return CE_NONE;
}

// slew to home, with acceleration in "measures" per second per second
CommandError Axis::autoSlewHome() {
  if (!enabled) return CE_SLEW_ERR_IN_STANDBY;
  if (autoRate != AR_NONE) return CE_SLEW_IN_SLEW;
  if (motionError(DIR_BOTH)) return CE_SLEW_ERR_OUTSIDE_LIMITS;

  if (pins->axisSense.homeTrigger != OFF) {
    motor->setSynchronized(true);
    if (homingStage == HOME_NONE) homingStage = HOME_FAST;
    if (autoRate == AR_NONE) {
      motor->setSlewing(true);
      V(axisPrefix); VF("autoSlewHome ");
      switch (homingStage) {
        case HOME_FAST: VL("fast started"); break;
        case HOME_SLOW: VL("slow started"); break;
        case HOME_FINE: VL("fine started"); break;
        case HOME_NONE: VL("?"); break;
      }
    }
    if (sense.isOn(homeSenseHandle)) {
      V(axisPrefix); VLF("move fwd");
      autoRate = AR_RATE_BY_TIME_FORWARD;
    } else {
      V(axisPrefix); VLF("move rev");
      autoRate = AR_RATE_BY_TIME_REVERSE;
    }
  }
  return CE_NONE;
}

// stops, with deacceleration by time
void Axis::autoSlewStop() {
  if (autoRate == AR_NONE || autoRate == AR_RATE_BY_TIME_ABORT) return;

  motor->setSynchronized(true);

  V(axisPrefix); VLF("slew stopping");
  autoRate = AR_RATE_BY_TIME_END;
  poll();
}

// emergency stops, with deacceleration by time
void Axis::autoSlewAbort() {
  if (autoRate == AR_NONE) return;

  motor->setSynchronized(true);

  V(axisPrefix); VLF("slew aborting");
  autoRate = AR_RATE_BY_TIME_ABORT;
  homingStage = HOME_NONE;
  poll();
}

// checks if slew is active on this axis
bool Axis::isSlewing() {
  return autoRate != AR_NONE;  
}

// monitor movement
void Axis::poll() {
  // make sure we're ready
  if (axisNumber == 0) return;

  // check physical limit switches
  errors.minLimitSensed = sense.isOn(minSenseHandle);
  errors.maxLimitSensed = sense.isOn(maxSenseHandle);

  // stop homing as we pass by the switch
  if (homingStage != HOME_NONE) {
    if (autoRate == AR_RATE_BY_TIME_FORWARD && !sense.isOn(homeSenseHandle)) autoSlewStop();
    if (autoRate == AR_RATE_BY_TIME_REVERSE && sense.isOn(homeSenseHandle)) autoSlewStop();
  }
  Y;

  // slewing
  if (autoRate != AR_NONE && !motor->inBacklash) {
    if (autoRate != AR_RATE_BY_TIME_ABORT) {
      if (motionError(motor->getDirection())) { autoSlewAbort(); return; }
    }
    if (autoRate == AR_RATE_BY_DISTANCE) {
      if (motor->getTargetDistanceSteps() == 0) {
        motor->setSlewing(false);
        autoRate = AR_NONE;
        freq = 0.0F;
        motor->setSynchronized(true);
        V(axisPrefix); VLF("slew stopped");
      } else {
        if (fabs(freq) > settings.backlashFreq) {
          if (motor->getTargetDistanceSteps() < 0) rampFreq -= getRampDirection()*slewMpspfs; else rampFreq += getRampDirection()*slewMpspfs;
          freq = rampFreq;
          if (freq < -slewFreq) freq = -slewFreq;
          if (freq > slewFreq) freq = slewFreq;
        } else {
          freq = (getOriginOrTargetDistance()/slewAccelerationDistance)*slewFreq;
          if (freq < settings.backlashFreq/2.0F) freq = settings.backlashFreq/2.0F;
          if (freq > settings.backlashFreq*1.05F) freq = settings.backlashFreq*1.05F;
          if (motor->getTargetDistanceSteps() < 0) freq = -freq;
          rampFreq = freq;
        }
      }
    } else
    if (autoRate == AR_RATE_BY_TIME_FORWARD) {
      freq += slewMpspfs;
      if (freq > slewFreq) freq = slewFreq;
    } else
    if (autoRate == AR_RATE_BY_TIME_REVERSE) {
      freq -= slewMpspfs;
      if (freq < -slewFreq) freq = -slewFreq;
    } else
    if (autoRate == AR_RATE_BY_TIME_END) {
      if (freq > slewMpspfs) freq -= slewMpspfs; else if (freq < -slewMpspfs) freq += slewMpspfs; else freq = 0.0F;
      if (abs(freq) <= slewMpspfs) {
        motor->setSlewing(false);
        autoRate = AR_NONE;
        freq = 0.0F;
        if (homingStage == HOME_FAST) homingStage = HOME_SLOW; else 
        if (homingStage == HOME_SLOW) homingStage = HOME_FINE; else
        if (homingStage == HOME_FINE) homingStage = HOME_NONE;
        if (homingStage != HOME_NONE) {
          float f = slewFreq/4.0F;
          if (abs(f) < 0.0003F) { if (f < 0.0F) f = -0.0003F; else f = 0.0003F; }
          setFrequencySlew(abs(f));
          autoSlewHome();
        } else {
          V(axisPrefix); VLF("slew stopped");
        }
      }
    } else
    if (autoRate == AR_RATE_BY_TIME_ABORT) {
      if (freq > abortMpspfs) freq -= abortMpspfs; else if (freq < -abortMpspfs) freq += abortMpspfs; else freq = 0.0F;
      if (fabs(freq) <= abortMpspfs) {
        motor->setSlewing(false);
        autoRate = AR_NONE;
        freq = 0.0F;
        V(axisPrefix); VLF("slew aborted");
      }
    } else freq = 0.0F;
  } else {
    freq = 0.0F;
    if (motionError(DIR_BOTH)) baseFreq = 0.0F;
  }
  Y;

  setFrequency(freq);

  // keep associated motor updated
  motor->poll();
}

// set minimum slew frequency in "measures" (radians, microns, etc.) per second
void Axis::setFrequencyMin(float frequency) {
  minFreq = frequency;
}

// set maximum slew frequency in "measures" (radians, microns, etc.) per second
void Axis::setFrequencyMax(float frequency) {
  maxFreq = frequency;
}

// frequency for base movement in "measures" (radians, microns, etc.) per second
void Axis::setFrequencyBase(float frequency) {
  baseFreq = frequency;
}

// frequency for slews in "measures" (radians, microns, etc.) per second
void Axis::setFrequencySlew(float frequency) {
  if (minFreq != 0.0F && frequency < minFreq) frequency = minFreq;
  if (maxFreq != 0.0F && frequency > maxFreq) frequency = maxFreq;
  slewFreq = frequency;

  // adjust acceleration rates if they depend on slewFreq
  if (!isnan(slewAccelTime)) slewMpspfs = (slewFreq/slewAccelTime)/FRACTIONAL_SEC;
  if (!isnan(abortAccelTime)) abortMpspfs = (slewFreq/abortAccelTime)/FRACTIONAL_SEC;
}

// set frequency in "measures" (degrees, microns, etc.) per second (0 stops motion)
void Axis::setFrequency(float frequency) {
  if (powerDownStandstill && frequency == 0.0F) {
    if (!poweredDown) {
      if (!powerDownOverride || (long)(millis() - powerDownOverrideEnds) > 0) {
        powerDownOverride = false;
        if ((long)(millis() - powerDownTime) > 0) {
          poweredDown = true;
          motor->power(false);
          V(axisPrefix); VLF("driver powered down");
        }
      }
    }
  } else {
    if (poweredDown) {
      poweredDown = false;
      motor->power(true);
      V(axisPrefix); VLF("driver powered up");
    }
    powerDownTime = millis() + powerDownDelay;
  }

  if (frequency != 0.0F) {
    if (frequency < 0.0F) {
      if (maxFreq != 0.0F && frequency < -maxFreq) frequency = -maxFreq;
      if (minFreq != 0.0F && frequency > -minFreq) frequency = -minFreq;
    } else {
      if (minFreq != 0.0F && frequency < minFreq) frequency = minFreq;
      if (maxFreq != 0.0F && frequency > maxFreq) frequency = maxFreq;
    }
  }

  // apply base frequency as required
  if (enabled) {
    if (autoRate == AR_NONE) {
      motor->setFrequencySteps((frequency + baseFreq)*settings.stepsPerMeasure);
    } else {
      motor->setFrequencySteps(frequency*settings.stepsPerMeasure);
    }
  } else {
    motor->setFrequencySteps(0.0F);
  }
}

// get frequency in "measures" (degrees, microns, etc.) per second
float Axis::getFrequency() {
  return motor->getFrequencySteps()/settings.stepsPerMeasure;
}

// gets backlash frequency in "measures" (degrees, microns, etc.) per second
float Axis::getBacklashFrequency() {
  return settings.backlashFreq;
}

// get associated motor driver status
DriverStatus Axis::getStatus() {
  return motor->getDriverStatus();
}

void Axis::setMotionLimitsCheck(bool state) {
  limitsCheck = state;
}

// checks for an error that would disallow motion in a given direction or DIR_BOTH for any motion
bool Axis::motionError(Direction direction) {
  if (motor->getDriverStatus().fault) { V(axisPrefix); VLF("motion error driver fault"); return true; }

  if (direction == DIR_FORWARD || direction == DIR_BOTH) {
    bool result = getInstrumentCoordinateSteps() > lroundf(0.9F*INT32_MAX) ||
                  (limitsCheck && getInstrumentCoordinate() > settings.limits.max) ||
                  errors.maxLimitSensed;
    if (result == true) { V(axisPrefix); VLF("motion error forward limit"); }
    return result;
  } else

  if (direction == DIR_REVERSE || direction == DIR_BOTH) {
    bool result = getInstrumentCoordinateSteps() < lroundf(0.9F*INT32_MIN) ||
                  (limitsCheck && getInstrumentCoordinate() < settings.limits.min) ||
                  errors.minLimitSensed;
    if (result == true) { V(axisPrefix); VLF("motion error reverse limit"); }
    return result;
  }

  return false;
}

#endif
