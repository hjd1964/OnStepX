// -----------------------------------------------------------------------------------
// Axis motion control

#include "Axis.h"

#include "../tasks/OnTask.h"
#include "../nv/Nv.h"

#ifdef MOTOR_PRESENT

Axis *axisWrapper[9];
IRAM_ATTR void axisWrapper1() { axisWrapper[0]->poll(); }
IRAM_ATTR void axisWrapper2() { axisWrapper[1]->poll(); }
IRAM_ATTR void axisWrapper3() { axisWrapper[2]->poll(); }
IRAM_ATTR void axisWrapper4() { axisWrapper[3]->poll(); }
IRAM_ATTR void axisWrapper5() { axisWrapper[4]->poll(); }
IRAM_ATTR void axisWrapper6() { axisWrapper[5]->poll(); }
IRAM_ATTR void axisWrapper7() { axisWrapper[6]->poll(); }
IRAM_ATTR void axisWrapper8() { axisWrapper[7]->poll(); }
IRAM_ATTR void axisWrapper9() { axisWrapper[8]->poll(); }

Axis::Axis(uint8_t axisNumber, const AxisPins *pins, const AxisSettings *settings, const AxisMeasure axisMeasure, float targetTolerance) {
  axisPrefix[5] = '0' + axisNumber;
  this->axisNumber = axisNumber;

  this->pins = pins;
  this->settings.stepsPerMeasure = settings->stepsPerMeasure;
  this->settings.reverse = settings->reverse;
  this->settings.limits = settings->limits;
  backlashFreq = settings->backlashFreq;

  // attach the function pointers to the callbacks
  axisWrapper[axisNumber - 1] = this;
  switch (axisNumber) {
    case 1: callback = axisWrapper1; break;
    case 2: callback = axisWrapper2; break;
    case 3: callback = axisWrapper3; break;
    case 4: callback = axisWrapper4; break;
    case 5: callback = axisWrapper5; break;
    case 6: callback = axisWrapper6; break;
    case 7: callback = axisWrapper7; break;
    case 8: callback = axisWrapper8; break;
    case 9: callback = axisWrapper9; break;
  }

  switch (axisMeasure) {
    case AXIS_MEASURE_UNKNOWN: strcpy(unitsStr, "?"); unitsRadians = false; break;
    case AXIS_MEASURE_MICRONS: strcpy(unitsStr, "um"); unitsRadians = false; break;
    case AXIS_MEASURE_DEGREES: strcpy(unitsStr, " deg"); unitsRadians = false; break;
    case AXIS_MEASURE_RADIANS: strcpy(unitsStr, " deg"); unitsRadians = true;  break;
  }

  this->targetTolerance = targetTolerance;
}

bool Axis::init(Motor *motor) {
  this->motor = motor;
  motor->getDefaultParameters(&settings.param1, &settings.param2, &settings.param3, &settings.param4, &settings.param5, &settings.param6);
  AxisStoredSettings defaultSettings = settings;

  // check for reverting axis settings in NV
  if (!nv.hasValidKey()) {
    VF("MSG:"); V(axisPrefix); VLF("writing defaults to NV");
    uint16_t axesToRevert = nv.readUI(NV_AXIS_SETTINGS_REVERT);
    bitSet(axesToRevert, axisNumber);
    nv.write(NV_AXIS_SETTINGS_REVERT, axesToRevert);
  }

  // write axis settings to NV
  // NV_AXIS_SETTINGS_REVERT bit 0 = settings at compile (0) or run time (1), bits 1 to 9 = reset axis n on next boot
  if (AxisStoredSettingsSize < sizeof(AxisStoredSettings)) { nv.initError = true; DF("ERR:"); D(axisPrefix); DLF("AxisStoredSettingsSize error"); return false; }
  uint16_t axesToRevert = nv.readUI(NV_AXIS_SETTINGS_REVERT);
  if (!(axesToRevert & 1)) bitSet(axesToRevert, axisNumber);
  uint16_t nvAxisSettingsBase = NV_AXIS_SETTINGS_BASE + (axisNumber - 1)*AxisStoredSettingsSize;
  if (bitRead(axesToRevert, axisNumber) || nv.isNull(nvAxisSettingsBase, sizeof(AxisStoredSettings))) {
    VF("MSG:"); V(axisPrefix); VLF("reverting settings to Config.h defaults");
    nv.updateBytes(nvAxisSettingsBase, &settings, sizeof(AxisStoredSettings));
  }
  bitClear(axesToRevert, axisNumber);
  nv.write(NV_AXIS_SETTINGS_REVERT, axesToRevert);

  // read axis settings from NV
  nv.readBytes(nvAxisSettingsBase, &settings, sizeof(AxisStoredSettings));

  // special ODrive case, a way to pass the stepsPerMeasure to it
  if (motor->getParameterTypeCode() == 'O') settings.param6 = settings.stepsPerMeasure;

  // set parameters
  if (!motor->setParameters(settings.param1, settings.param2, settings.param3, settings.param4, settings.param5, settings.param6)) {
    DF("ERR:"); D(axisPrefix); DLF("setting parameters failed!"); return false;
  }

  // check parameters
  if (!validateAxisSettings(axisNumber, settings)) {
    VF("MSG:"); V(axisPrefix); VLF("settings validation failed reverting settings to Config.h defaults");
    settings = defaultSettings;
    nv.updateBytes(nvAxisSettingsBase, &settings, sizeof(AxisStoredSettings));
    if (!validateAxisSettings(axisNumber, settings)) {
      DF("ERR:"); D(axisPrefix); DLF("settings validation still failed!");
      return false;
    }
  }

  #if DEBUG == VERBOSE
    VF("MSG:"); V(axisPrefix); VF("stepsPerMeasure="); V(settings.stepsPerMeasure);
    V(", reverse="); if (settings.reverse == OFF) VLF("OFF"); else if (settings.reverse == ON) VLF("ON"); else VLF("?");
    VF("MSG:"); V(axisPrefix); VF("backlash takeup frequency set to ");
    if (unitsRadians) V(radToDegF(backlashFreq)); else V(backlashFreq);
    V(unitsStr); VLF("/s");
  #endif

  // activate home and limit sense
  VF("MSG:"); V(axisPrefix); VLF("adding any home and/or limit senses");
  homeSenseHandle = sense.add(pins->home, pins->axisSense.homeInit, pins->axisSense.homeTrigger);
  minSenseHandle = sense.add(pins->min, pins->axisSense.minMaxInit, pins->axisSense.minTrigger);
  maxSenseHandle = sense.add(pins->max, pins->axisSense.minMaxInit, pins->axisSense.maxTrigger);
  #if LIMIT_SENSE_STRICT != ON
    commonMinMaxSense = pins->min != OFF && pins->min == pins->max;
  #endif

  // setup motor
  if (!motor->init()) { DF("ERR:"); D(axisPrefix); DLF("no motor driver!"); return false; }

  motor->setReverse(settings.reverse);
  motor->setBacklashFrequencySteps(backlashFreq*settings.stepsPerMeasure);

  // start monitor
  VF("MSG:"); V(axisPrefix); VF("start motion controller task (rate "); V(FRACTIONAL_SEC_US); VF("us priority 1)... ");
  uint8_t taskHandle = 0;
  char taskName[] = "Ax_Motn";
  taskName[2] = axisNumber + '0';
  taskHandle = tasks.add(0, 0, true, 1, callback, taskName);
  tasks.setPeriodMicros(taskHandle, FRACTIONAL_SEC_US);
  if (taskHandle) { VLF("success"); } else {
    VLF("FAILED!");
    DF("ERR:"); D(axisPrefix); DLF("no motion controller task!"); return false; 
  }
  motor->monitorHandle = taskHandle;

  return true;
}

void Axis::enable(bool state) {
  enabled = state;
  motor->enable(enabled & !poweredDown);
}

void Axis::setPowerDownTime(int time) {
  if (time == 0) powerDownStandstill = false; else { powerDownStandstill = true; powerDownDelay = time; }
}

void Axis::setPowerDownOverrideTime(int time) {
  if (time == 0) powerDownOverride = false; else {
    if (poweredDown) {
      poweredDown = false;
      motor->enable(true);
    }
    powerDownOverride = true;
    powerDownOverrideEnds = millis() + time;
  }
}

void Axis::setBacklash(float value) {
  if (autoRate == AR_NONE) motor->setBacklashSteps(round(value*settings.stepsPerMeasure));
}

float Axis::getBacklash() {
  return motor->getBacklashSteps()/settings.stepsPerMeasure;
}

CommandError Axis::resetPosition(double value) {
  return resetPositionSteps(lround(value*settings.stepsPerMeasure));
}

CommandError Axis::resetPositionSteps(long value) {
  if (autoRate != AR_NONE) return CE_SLEW_IN_MOTION;
  if (motor->getFrequencySteps() != 0) return CE_SLEW_IN_MOTION;
  motor->resetPositionSteps(value);
  return CE_NONE;
}

double Axis::getMotorPosition() {
  return motor->getMotorPositionSteps()/settings.stepsPerMeasure;
}

double Axis::getIndexPosition() {
  return motor->getIndexPositionSteps()/settings.stepsPerMeasure;
}

double distance(double c1, double c2) {
  double d1 = fabs(c1 - c2);
  double d2 = fabs(c2 - c1);
  if (d1 <= d2) return d1; else return d2;
}

double Axis::wrap(double value) {
  if (wrapEnabled) {
    while (value > settings.limits.max) value -= wrapAmount;
    while (value < settings.limits.min) value += wrapAmount;
  }
  return value;
}

double Axis::unwrap(double value) {
  if (wrapEnabled) {
    double position = motor->getInstrumentCoordinateSteps()/settings.stepsPerMeasure;
    while (value > position + wrapAmount/2.0L) value -= wrapAmount;
    while (value < position - wrapAmount/2.0L) value += wrapAmount;
  }
  return value;
}

double Axis::unwrapNearest(double value) {
  if (wrapEnabled) {
    value = unwrap(value);
    double instr = motor->getInstrumentCoordinateSteps()/settings.stepsPerMeasure;
//    VF("MSG:"); V(axisPrefix);
//    VF("unwrapNearest instr "); V(radToDeg(instr));
//    VF(", before "); V(radToDeg(value));
    double dist = distance(value, instr);
    if (distance(value + wrapAmount, instr) < dist) value += wrapAmount; else
    if (distance(value - wrapAmount, instr) < dist) value -= wrapAmount;
//    VF(", after "); VL(radToDeg(value));
  }
  return value;
}

void Axis::setInstrumentCoordinate(double value) {
  setInstrumentCoordinateSteps(lround(unwrap(value)*settings.stepsPerMeasure));
}

double Axis::getInstrumentCoordinate() {
  return wrap(motor->getInstrumentCoordinateSteps()/settings.stepsPerMeasure);
}

void Axis::setInstrumentCoordinatePark(double value) {
  motor->setInstrumentCoordinateParkSteps(lround(unwrapNearest(value)*settings.stepsPerMeasure), settings.subdivisions);
}

void Axis::setTargetCoordinatePark(double value) {
  motor->setFrequencySteps(0);
  motor->setTargetCoordinateParkSteps(lround(unwrapNearest(value)*settings.stepsPerMeasure), settings.subdivisions);
}

void Axis::setTargetCoordinate(double value) {
  setTargetCoordinateSteps(lround(unwrapNearest(value)*settings.stepsPerMeasure));
}

double Axis::getTargetCoordinate() {
  return wrap(motor->getTargetCoordinateSteps()/settings.stepsPerMeasure);
}

bool Axis::atTarget() {
  return labs(motor->getTargetDistanceSteps()) <= targetTolerance*settings.stepsPerMeasure;
}

bool Axis::nearTarget() {
  return labs(motor->getTargetDistanceSteps()) < backlashFreq*settings.stepsPerMeasure;
}

double Axis::getTargetDistance() {
  return labs(motor->getTargetDistanceSteps())/settings.stepsPerMeasure;
}

double Axis::getOriginOrTargetDistance() {
  return motor->getOriginOrTargetDistanceSteps()/settings.stepsPerMeasure;
}

void Axis::setSlewAccelerationRate(float mpsps) {
  if (autoRate == AR_NONE) {
    slewAccelRateFs = mpsps/FRACTIONAL_SEC;
    if (slewAccelRateFs > backlashFreq) slewAccelRateFs = backlashFreq;
    slewAccelTime = NAN;
  }
}

void Axis::setSlewAccelerationTime(float seconds) {
  if (autoRate == AR_NONE) {
    if (seconds < 0.1F) seconds = 0.1F;
    slewAccelTime = seconds;
  }
}

void Axis::setSlewAccelerationRateAbort(float mpsps) {
  if (autoRate == AR_NONE) {
    abortAccelRateFs = mpsps/FRACTIONAL_SEC;
    if (abortAccelRateFs > backlashFreq) abortAccelRateFs = backlashFreq;
    abortAccelTime = NAN;
  }
}

void Axis::setSlewAccelerationTimeAbort(float seconds) {
  if (autoRate == AR_NONE) abortAccelTime = seconds;
}

CommandError Axis::autoGoto(float frequency) {
  if (!enabled) return CE_SLEW_ERR_IN_STANDBY;
  if (autoRate != AR_NONE) return CE_SLEW_IN_SLEW;
  if (motionError(DIR_BOTH)) return CE_SLEW_ERR_OUTSIDE_LIMITS;
  if (motorFault()) return CE_SLEW_ERR_HARDWARE_FAULT;

  if (!isnan(frequency)) setFrequencySlew(frequency);

  VF("MSG:"); V(axisPrefix);
  VF("autoGoto start ");

  motor->markOriginCoordinateSteps();
  motor->setSynchronized(false);
  motor->setSlewing(true);
  autoRate = AR_RATE_BY_DISTANCE;

  #if DEBUG == VERBOSE
    if (unitsRadians) V(radToDeg(slewFreq)); else V(slewFreq);
    V(unitsStr); VF("/s, accel ");
    if (unitsRadians) SERIAL_DEBUG.print(radToDeg(slewAccelRateFs)*FRACTIONAL_SEC, 3); else SERIAL_DEBUG.print(slewAccelRateFs*FRACTIONAL_SEC, 3);
    V(unitsStr); VLF("/s/s");
  #endif

  return CE_NONE;
}

CommandError Axis::autoSlew(Direction direction, float frequency) {
  if (!enabled) return CE_SLEW_ERR_IN_STANDBY;
  if (autoRate == AR_RATE_BY_DISTANCE) return CE_SLEW_IN_SLEW;
  if (direction != DIR_FORWARD && direction != DIR_REVERSE) return CE_SLEW_ERR_UNSPECIFIED;
  if (motionError(direction)) return CE_SLEW_ERR_OUTSIDE_LIMITS;
  if (motorFault()) return CE_SLEW_ERR_HARDWARE_FAULT;

  if (!isnan(frequency)) setFrequencySlew(frequency);

  VF("MSG:"); V(axisPrefix);
  if (autoRate == AR_NONE) {
    motor->setSynchronized(true);
    motor->setSlewing(true);
    VF("MSG:"); V(axisPrefix); VF("autoSlew start ");
  } else { VF("autoSlew resum "); }

  if (direction == DIR_FORWARD) {
    autoRate = AR_RATE_BY_TIME_FORWARD;
    VF("fwd@ ");
  } else {
    autoRate = AR_RATE_BY_TIME_REVERSE;
    VF("rev@ ");
  }

  #if DEBUG == VERBOSE
    if (unitsRadians) {
      if (radToDeg(slewFreq) >= 0.01F ) {
        V(radToDeg(slewFreq)); V(unitsStr);
      } else {
        V(radToDeg(slewFreq)*3600.0F); V(" arc-sec");
      } 
    } else { V(slewFreq); V(unitsStr); }

    VF("/s, accel ");
    if (unitsRadians) SERIAL_DEBUG.print(radToDeg(slewAccelRateFs)*FRACTIONAL_SEC, 3); else SERIAL_DEBUG.print(slewAccelRateFs*FRACTIONAL_SEC, 3);
    V(unitsStr); VLF("/s/s");
  #endif

  return CE_NONE;
}

CommandError Axis::autoSlewHome(unsigned long timeout) {
  if (!enabled) return CE_SLEW_ERR_IN_STANDBY;
  if (autoRate != AR_NONE) return CE_SLEW_IN_SLEW;
  if (motionError(DIR_BOTH)) return CE_SLEW_ERR_OUTSIDE_LIMITS;
  if (motorFault()) return CE_SLEW_ERR_HARDWARE_FAULT;

  if (pins->axisSense.homeTrigger != OFF) {
    motor->setSynchronized(true);
    if (homingStage == HOME_NONE) homingStage = HOME_FAST;
    if (autoRate == AR_NONE) {
      motor->setSlewing(true);
      VF("MSG:"); V(axisPrefix); VF("autoSlewHome ");
      switch (homingStage) {
        case HOME_FAST: VF("fast "); break;
        case HOME_SLOW: VF("slow "); break;
        case HOME_FINE: VF("fine "); break;
        default: break;
      }
    }
    if (sense.isOn(homeSenseHandle)) {
      VF("fwd@ ");
      autoRate = AR_RATE_BY_TIME_FORWARD;
    } else {
      VF("rev@ ");
      autoRate = AR_RATE_BY_TIME_REVERSE;
    }

    // automatically set timeout if not specified
    if (timeout == 0) timeout = (pins->axisSense.homeDistLimit/slewFreq)*1.2F*1000.0F;

    #if DEBUG == VERBOSE
      if (unitsRadians) V(radToDeg(slewFreq)); else V(slewFreq);
      V(unitsStr); VF("/s, accel ");
      if (unitsRadians) SERIAL_DEBUG.print(radToDeg(slewAccelRateFs)*FRACTIONAL_SEC, 3); else SERIAL_DEBUG.print(slewAccelRateFs*FRACTIONAL_SEC, 3);
      V(unitsStr); VF("/s/s, timeout ");
    #endif
    VL(timeout);
    homeTimeoutTime = millis() + timeout;
  }
  return CE_NONE;
}

void Axis::autoSlewStop() {
  if (autoRate <= AR_RATE_BY_TIME_END) return;

  motor->setSynchronized(true);

  VF("MSG:"); V(axisPrefix); VLF("slew stopping");
  autoRate = AR_RATE_BY_TIME_END;
  poll();
}

void Axis::autoSlewAbort() {
  if (autoRate <= AR_RATE_BY_TIME_ABORT) return;

  motor->setSynchronized(true);

  VF("MSG:"); V(axisPrefix); VLF("slew aborting");
  autoRate = AR_RATE_BY_TIME_ABORT;
  homingStage = HOME_NONE;
  poll();
}

bool Axis::isSlewing() {
  return autoRate != AR_NONE;  
}

void Axis::poll() {
  // make sure we're ready
  if (axisNumber == 0) return;

  // check physical limit switches
  errors.minLimitSensed = sense.isOn(minSenseHandle);
  errors.maxLimitSensed = sense.isOn(maxSenseHandle);
  bool commonMinMaxSensed = commonMinMaxSense && (errors.minLimitSensed || errors.maxLimitSensed);

  // let the user know if the associated senses change state
  #if DEBUG == VERBOSE
    bool senseMin = sense.isOn(minSenseHandle);
    bool senseMax = sense.isOn(maxSenseHandle);
    if (lastSenseMin != senseMin || lastSenseMax != senseMax) {
      VF("MSG:"); V(axisPrefix); VF("limit sense state: ");
      V("A"); V(axisNumber); V("S-");
      if (senseMin) VF("< "); else VF("  ");
      V("A"); V(axisNumber); V("S+");
      if (senseMax) VLF("< "); else VLF("  ");
      lastSenseMin = senseMin;
      lastSenseMax = senseMax;
    }
    bool senseHome = sense.isOn(homeSenseHandle);
    if (lastSenseHome != senseHome) {
      VF("MSG:"); V(axisPrefix); VF("home sense state: ");
      if (senseHome) VLF("ON"); else VLF("OFF");
      lastSenseHome = senseHome;
    }
  #endif

  // stop homing as we pass by the switch or times out
  if (homingStage != HOME_NONE && (autoRate == AR_RATE_BY_TIME_FORWARD || autoRate == AR_RATE_BY_TIME_REVERSE)) {
    if (autoRate == AR_RATE_BY_TIME_FORWARD && !sense.isOn(homeSenseHandle)) autoSlewStop();
    if (autoRate == AR_RATE_BY_TIME_REVERSE && sense.isOn(homeSenseHandle)) autoSlewStop();
    if ((long)(millis() - homeTimeoutTime) > 0) {
      VF("MSG:"); V(axisPrefix); VLF("autoSlewHome timed out");
      autoSlewAbort();
    }
  }
  Y;

  // slewing
  if (autoRate != AR_NONE && !motor->inBacklash) {

    if (autoRate != AR_RATE_BY_TIME_ABORT) {
      if (motionError(motor->getDirection())) {
        VF("MSG:"); V(axisPrefix); VLF("motion error");
        autoSlewAbort();
        return;
      }
      if (motorFault()) {
        VF("MSG:"); V(axisPrefix); VLF("motor fault");
        autoSlewAbort();
        return;
      }
    }
    if (autoRate == AR_RATE_BY_DISTANCE) {
      if (commonMinMaxSensed) {
        VF("MSG:"); V(axisPrefix); VLF("commonMinMaxSensed");
        autoSlewAbort();
        return;
      }
      if (atTarget()) {
        motor->setSlewing(false);
        autoRate = AR_NONE;
        freq = 0.0F;
        motor->setSynchronized(true);
        VF("MSG:"); V(axisPrefix); VLF("slew stopped");
      } else {
        freq = sqrtf(2.0F*(slewAccelRateFs*FRACTIONAL_SEC)*getOriginOrTargetDistance());
        if (freq < backlashFreq) freq = backlashFreq;
        if (freq > slewFreq) freq = slewFreq;
        if (motor->getTargetDistanceSteps() < 0) freq = -freq;
      }
    } else
    if (autoRate == AR_RATE_BY_TIME_FORWARD) {
      freq += slewAccelRateFs;
      if (freq > slewFreq) freq = slewFreq;
    } else
    if (autoRate == AR_RATE_BY_TIME_REVERSE) {
      freq -= slewAccelRateFs;
      if (freq < -slewFreq) freq = -slewFreq;
    } else
    if (autoRate == AR_RATE_BY_TIME_END) {
      if (commonMinMaxSensed) {
        VF("MSG:"); V(axisPrefix); VLF("commonMinMaxSensed");
        autoSlewAbort();
        return;
      }

      if (freq > slewAccelRateFs) freq -= slewAccelRateFs; else if (freq < -slewAccelRateFs) freq += slewAccelRateFs; else freq = 0.0F;
      if (fabs(freq) <= slewAccelRateFs) {
        motor->setSlewing(false);
        autoRate = AR_NONE;
        freq = 0.0F;
        if (homingStage == HOME_FAST) homingStage = HOME_SLOW; else 
        if (homingStage == HOME_SLOW) {
          if (!sense.isOn(homeSenseHandle)) homingStage = HOME_FINE; else {
            slewFreq *= 6.0F;
            VF("MSG:"); V(axisPrefix); VLF("autoSlewHome approach correction");
          }
        } else
        if (homingStage == HOME_FINE) homingStage = HOME_NONE;
        if (homingStage != HOME_NONE) {
          float f = fabs(slewFreq)/6.0F;
          if (f < 0.0003F) f = 0.0003F;
          setFrequencySlew(f);
          autoSlewHome(SLEW_HOME_REFINE_TIME_LIMIT * 1000);
        } else {
          VF("MSG:"); V(axisPrefix); VLF("slew stopped");
        }
      }
    } else
    if (autoRate == AR_RATE_BY_TIME_ABORT) {
      if (freq > abortAccelRateFs) freq -= abortAccelRateFs; else if (freq < -abortAccelRateFs) freq += abortAccelRateFs; else freq = 0.0F;
      if (fabs(freq) <= abortAccelRateFs) {
        motor->setSlewing(false);
        autoRate = AR_NONE;
        freq = 0.0F;
        VF("MSG:"); V(axisPrefix); VLF("slew aborted");
      }
    } else freq = 0.0F;
  } else {
    freq = 0.0F;
    if (commonMinMaxSensed || motionError(DIR_BOTH) || motorFault()) baseFreq = 0.0F;
  }
  Y;

  setFrequency(freq);

  // keep associated motor updated
  motor->poll();

  // respond to the motor disabling itself
  if (autoRate != AR_NONE && !motor->enabled) {
    autoRate = AR_NONE;
    freq = 0.0F;
    VF("MSG:"); V(axisPrefix); VLF("motion stopped, motor disabled!");
  }
}

void Axis::setFrequencyMin(float frequency) {
  minFreq = frequency;
}

void Axis::setFrequencyMax(float frequency) {
  maxFreq = frequency;
}

void Axis::setFrequencySlew(float frequency) {
  if (minFreq != 0.0F && frequency < minFreq) frequency = minFreq;
  if (maxFreq != 0.0F && frequency > maxFreq) frequency = maxFreq;
  slewFreq = frequency;

  // adjust acceleration rates if they depend on slewFreq
  if (!isnan(slewAccelTime)) slewAccelRateFs = (slewFreq/slewAccelTime)/FRACTIONAL_SEC;
  if (!isnan(abortAccelTime)) abortAccelRateFs = (slewFreq/abortAccelTime)/FRACTIONAL_SEC;
}

void Axis::setFrequency(float frequency) {
  frequency *= scaleFreq;
  if (powerDownStandstill && frequency == 0.0F && baseFreq == 0.0F) {
    if (!poweredDown) {
      if (!powerDownOverride || (long)(millis() - powerDownOverrideEnds) > 0) {
        powerDownOverride = false;
        if ((long)(millis() - powerDownTime) > 0) {
          poweredDown = true;
          motor->enable(false);
        }
      }
    }
  } else {
    if (poweredDown) {
      poweredDown = false;
      motor->enable(true);
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

float Axis::getFrequency() {
  return motor->getFrequencySteps()/settings.stepsPerMeasure;
}

float Axis::getBacklashFrequency() {
  return backlashFreq;
}

DriverStatus Axis::getStatus() {
  return motor->getDriverStatus();
}

void Axis::setMotionLimitsCheck(bool state) {
  limitsCheck = state;
}

bool Axis::motionError(Direction direction) {
  bool result = false;

  if (direction == DIR_FORWARD || direction == DIR_BOTH) {
    result = getInstrumentCoordinateSteps() > lroundf(0.9F*INT32_MAX) ||
             (limitsCheck && homingStage == HOME_NONE && getInstrumentCoordinate() > settings.limits.max + 1.0F/this->settings.stepsPerMeasure) ||
             (!commonMinMaxSense && errors.maxLimitSensed);
    if (result == true && result != lastErrorResult) { VF("MSG:"); V(axisPrefix); VLF("motion error forward limit"); }
  } else

  if (direction == DIR_REVERSE || direction == DIR_BOTH) {
    result = getInstrumentCoordinateSteps() < lroundf(0.9F*INT32_MIN) ||
             (limitsCheck && homingStage == HOME_NONE && getInstrumentCoordinate() < settings.limits.min - 1.0F/this->settings.stepsPerMeasure) ||
             (!commonMinMaxSense && errors.minLimitSensed);
    if (result == true && result != lastErrorResult) { VF("MSG:"); V(axisPrefix); VLF("motion error reverse limit"); }
  }

  lastErrorResult = result;

  return lastErrorResult;
}

bool Axis::motionErrorSensed(Direction direction) {
  if ((direction == DIR_REVERSE || direction == DIR_BOTH) && errors.minLimitSensed) return true; else
  if ((direction == DIR_FORWARD || direction == DIR_BOTH) && errors.maxLimitSensed) return true; else return false;
}

#endif
