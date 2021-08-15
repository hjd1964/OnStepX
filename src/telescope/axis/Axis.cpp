// -----------------------------------------------------------------------------------
// Axis motion control

#include "Axis.h"

#ifdef AXIS_PRESENT

#include "../../tasks/OnTask.h"
extern Tasks tasks;

#include "../Telescope.h"
#include "../../lib/sense/Sense.h"

const AxisPins Pins[] = {
#if AXIS1_DRIVER_MODEL != OFF
  { 1, AXIS1_SENSE_LIMIT_MIN_PIN, AXIS1_SENSE_HOME_PIN, AXIS1_SENSE_LIMIT_MAX_PIN, { AXIS1_SENSE_HOME, AXIS1_SENSE_HOME_INIT, AXIS1_SENSE_LIMIT_MIN, AXIS1_SENSE_LIMIT_MAX, AXIS1_SENSE_LIMIT_INIT } },
#endif
#if AXIS2_DRIVER_MODEL != OFF
  { 2, AXIS2_SENSE_LIMIT_MIN_PIN, AXIS2_SENSE_HOME_PIN, AXIS2_SENSE_LIMIT_MAX_PIN, { AXIS2_SENSE_HOME, AXIS2_SENSE_HOME_INIT, AXIS2_SENSE_LIMIT_MIN, AXIS2_SENSE_LIMIT_MAX, AXIS2_SENSE_LIMIT_INIT } },
#endif
#if AXIS3_DRIVER_MODEL != OFF
  { 3, AXIS3_SENSE_LIMIT_MIN_PIN, AXIS3_SENSE_HOME_PIN, AXIS3_SENSE_LIMIT_MAX_PIN, { AXIS3_SENSE_HOME, AXIS3_SENSE_HOME_INIT, AXIS3_SENSE_LIMIT_MIN, AXIS3_SENSE_LIMIT_MAX, AXIS3_SENSE_LIMIT_INIT } },
#endif
#if AXIS4_DRIVER_MODEL != OFF
  { 4, AXIS4_SENSE_LIMIT_MIN_PIN, AXIS4_SENSE_HOME_PIN, AXIS4_SENSE_LIMIT_MAX_PIN, { AXIS4_SENSE_HOME, AXIS4_SENSE_HOME_INIT, AXIS4_SENSE_LIMIT_MIN, AXIS4_SENSE_LIMIT_MAX, AXIS4_SENSE_LIMIT_INIT } },
#endif
#if AXIS5_DRIVER_MODEL != OFF
  { 5, AXIS5_SENSE_LIMIT_MIN_PIN, AXIS5_SENSE_HOME_PIN, AXIS5_SENSE_LIMIT_MAX_PIN, { AXIS5_SENSE_HOME, AXIS5_SENSE_HOME_INIT, AXIS5_SENSE_LIMIT_MIN, AXIS5_SENSE_LIMIT_MAX, AXIS5_SENSE_LIMIT_INIT } },
#endif
#if AXIS6_DRIVER_MODEL != OFF
  { 6, AXIS6_SENSE_LIMIT_MIN_PIN, AXIS6_SENSE_HOME_PIN, AXIS6_SENSE_LIMIT_MAX_PIN, { AXIS6_SENSE_HOME, AXIS6_SENSE_HOME_INIT, AXIS6_SENSE_LIMIT_MIN, AXIS6_SENSE_LIMIT_MAX, AXIS6_SENSE_LIMIT_INIT } },
#endif
#if AXIS7_DRIVER_MODEL != OFF
  { 7, AXIS7_SENSE_LIMIT_MIN_PIN, AXIS7_SENSE_HOME_PIN, AXIS7_SENSE_LIMIT_MAX_PIN, { AXIS7_SENSE_HOME, AXIS7_SENSE_HOME_INIT, AXIS7_SENSE_LIMIT_MIN, AXIS7_SENSE_LIMIT_MAX, AXIS7_SENSE_LIMIT_INIT } },
#endif
#if AXIS8_DRIVER_MODEL != OFF
  { 8, AXIS8_SENSE_LIMIT_MIN_PIN, AXIS8_SENSE_HOME_PIN, AXIS8_SENSE_LIMIT_MAX_PIN, { AXIS8_SENSE_HOME, AXIS8_SENSE_HOME_INIT, AXIS8_SENSE_LIMIT_MIN, AXIS8_SENSE_LIMIT_MAX, AXIS8_SENSE_LIMIT_INIT } },
#endif
#if AXIS9_DRIVER_MODEL != OFF
  { 9, AXIS9_SENSE_LIMIT_MIN_PIN, AXIS9_SENSE_HOME_PIN, AXIS9_SENSE_LIMIT_MAX_PIN, { AXIS9_SENSE_HOME, AXIS9_SENSE_HOME_INIT, AXIS9_SENSE_LIMIT_MIN, AXIS9_SENSE_LIMIT_MAX, AXIS9_SENSE_LIMIT_INIT } },
#endif
};

const AxisSettings DefaultSettings[] = {
#if AXIS1_DRIVER_MODEL != OFF
  { AXIS1_STEPS_PER_DEGREE*RAD_DEG_RATIO, AXIS1_REVERSE, AXIS1_PARAMETER1, AXIS1_PARAMETER2, { degToRadF(AXIS1_LIMIT_MIN), degToRadF(AXIS1_LIMIT_MAX) }, siderealToRad(TRACK_BACKLASH_RATE) },
#endif
#if AXIS2_DRIVER_MODEL != OFF
  { AXIS2_STEPS_PER_DEGREE*RAD_DEG_RATIO, AXIS2_REVERSE, AXIS2_PARAMETER1, AXIS2_PARAMETER2, { degToRadF(AXIS2_LIMIT_MIN), degToRadF(AXIS2_LIMIT_MAX) }, siderealToRad(TRACK_BACKLASH_RATE) },
#endif
#if AXIS3_DRIVER_MODEL != OFF
  { AXIS3_STEPS_PER_DEGREE, AXIS3_REVERSE, AXIS3_PARAMETER1, AXIS3_PARAMETER2, { AXIS3_LIMIT_MIN, AXIS3_LIMIT_MAX }, AXIS3_BACKLASH_RATE },
#endif
#if AXIS4_DRIVER_MODEL != OFF
  { AXIS4_STEPS_PER_MICRON, AXIS4_REVERSE, AXIS4_PARAMETER1, AXIS4_PARAMETER2, { AXIS4_LIMIT_MIN*1000.0F, AXIS4_LIMIT_MAX*1000.0F }, AXIS4_BACKLASH_RATE },
#endif
#if AXIS5_DRIVER_MODEL != OFF
  { AXIS5_STEPS_PER_MICRON, AXIS5_REVERSE, AXIS5_PARAMETER1, AXIS5_PARAMETER2, { AXIS5_LIMIT_MIN*1000.0F, AXIS5_LIMIT_MAX*1000.0F }, AXIS5_BACKLASH_RATE },
#endif
#if AXIS6_DRIVER_MODEL != OFF
  { AXIS6_STEPS_PER_MICRON, AXIS6_REVERSE, AXIS6_PARAMETER1, AXIS6_PARAMETER2, { AXIS6_LIMIT_MIN*1000.0F, AXIS6_LIMIT_MAX*1000.0F }, AXIS6_BACKLASH_RATE },
#endif
#if AXIS7_DRIVER_MODEL != OFF
  { AXIS7_STEPS_PER_MICRON, AXIS7_REVERSE, AXIS7_PARAMETER1, AXIS7_PARAMETER2, { AXIS7_LIMIT_MIN*1000.0F, AXIS7_LIMIT_MAX*1000.0F }, AXIS7_BACKLASH_RATE },
#endif
#if AXIS8_DRIVER_MODEL != OFF
  { AXIS8_STEPS_PER_MICRON, AXIS8_REVERSE, AXIS8_PARAMETER1, AXIS8_PARAMETER2, { AXIS8_LIMIT_MIN*1000.0F, AXIS8_LIMIT_MAX*1000.0F }, AXIS8_BACKLASH_RATE },
#endif
#if AXIS9_DRIVER_MODEL != OFF
  { AXIS9_STEPS_PER_MICRON, AXIS9_REVERSE, AXIS9_PARAMETER1, AXIS9_PARAMETER2, { AXIS9_LIMIT_MIN*1000.0F, AXIS9_LIMIT_MAX*1000.0F }, AXIS9_BACKLASH_RATE },
#endif
};

#if AXIS1_DRIVER_MODEL != OFF
  Axis axis1;
#endif
#if AXIS2_DRIVER_MODEL != OFF
  Axis axis2;
#endif
#if AXIS3_DRIVER_MODEL != OFF
  Axis axis3;
#endif
#if AXIS4_DRIVER_MODEL != OFF
  Axis axis4;
#endif
#if AXIS5_DRIVER_MODEL != OFF
  Axis axis5;
#endif
#if AXIS6_DRIVER_MODEL != OFF
  Axis axis6;
#endif
#if AXIS7_DRIVER_MODEL != OFF
  Axis axis7;
#endif
#if AXIS8_DRIVER_MODEL != OFF
  Axis axis8;
#endif
#if AXIS9_DRIVER_MODEL != OFF
  Axis axis9;
#endif

Axis *axes[9] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static uint8_t pollingTaskHandle  = 0;

inline void pollAxes() {
  #if AXIS1_DRIVER_MODEL != OFF
    axis1.poll(); Y;
  #endif
  #if AXIS2_DRIVER_MODEL != OFF
    axis2.poll(); Y;
  #endif
  #if AXIS3_DRIVER_MODEL != OFF
    axis3.poll(); Y;
  #endif
  #if AXIS4_DRIVER_MODEL != OFF
    axis4.poll(); Y;
  #endif
  #if AXIS5_DRIVER_MODEL != OFF
    axis5.poll(); Y;
  #endif
  #if AXIS6_DRIVER_MODEL != OFF
    axis6.poll(); Y;
  #endif
  #if AXIS7_DRIVER_MODEL != OFF
    axis7.poll(); Y;
  #endif
  #if AXIS8_DRIVER_MODEL != OFF
    axis8.poll(); Y;
  #endif
  #if AXIS9_DRIVER_MODEL != OFF
    axis9.poll(); Y;
  #endif
}

// setup array of axes for easy access
Axis::Axis() {
  #if AXIS1_DRIVER_MODEL != OFF
    axes[0] = &axis1;
  #endif
  #if AXIS2_DRIVER_MODEL != OFF
    axes[1] = &axis2;
  #endif
  #if AXIS3_DRIVER_MODEL != OFF
    axes[2] = &axis3;
  #endif
  #if AXIS4_DRIVER_MODEL != OFF
    axes[3] = &axis4;
  #endif
  #if AXIS5_DRIVER_MODEL != OFF
    axes[4] = &axis5;
  #endif
  #if AXIS6_DRIVER_MODEL != OFF
    axes[5] = &axis6;
  #endif
  #if AXIS7_DRIVER_MODEL != OFF
    axes[6] = &axis7;
  #endif
  #if AXIS8_DRIVER_MODEL != OFF
    axes[7] = &axis8;
  #endif
  #if AXIS9_DRIVER_MODEL != OFF
    axes[8] = &axis9;
  #endif
}

// sets up the driver step/dir/enable pins and any associated driver mode control
void Axis::init(uint8_t axisNumber, bool alternateLimits) {
  axisPrefix[9] = '0' + axisNumber;
  this->axisNumber = axisNumber;

  // start monitor
  if (pollingTaskHandle == 0) {
    VF("MSG: Axis, start monitor task (rate 20ms priority 1)... ");
    pollingTaskHandle = tasks.add(20, 0, true, 1, pollAxes, "AxsPoll");
    if (pollingTaskHandle) { VL("success"); } else { VL("FAILED!"); }
  }

  // load constants for this axis
  for (uint8_t i = 0; i < 10; i++) {
    if (Pins[i].axis == axisNumber) {
      index = i;
      settings = DefaultSettings[i];
      break;
    }
    if (i == 9) { DLF("ERR: Axis::init(); indexing failed!"); return; }
  }

  // assign axis to array and set motor
  switch (axisNumber) {
    #if AXIS1_DRIVER_MODEL != OFF
      case 1: motor = &motor1; break;
    #endif
    #if AXIS2_DRIVER_MODEL != OFF
      case 2: motor = &motor2; break;
    #endif
    #if AXIS3_DRIVER_MODEL != OFF
      case 3: motor = &motor3; break;
    #endif
    #if AXIS4_DRIVER_MODEL != OFF
      case 4: motor = &motor4; break;
    #endif
    #if AXIS5_DRIVER_MODEL != OFF
      case 5: motor = &motor5; break;
    #endif
    #if AXIS6_DRIVER_MODEL != OFF
      case 6: motor = &motor6; break;
    #endif
    #if AXIS7_DRIVER_MODEL != OFF
      case 7:  motor = &motor7; break;
    #endif
    #if AXIS8_DRIVER_MODEL != OFF
      case 8: motor = &motor8; break;
    #endif
    #if AXIS9_DRIVER_MODEL != OFF
      case 9: motor = &motor9; break;
    #endif
    default:
      DLF("ERR: Axis::init(); couldn't find a motor!");
      return;
  }

  // check for reverting axis settings in NV
  if (!validKey) {
    V(axisPrefix); VLF("writing default settings to NV");
    uint16_t axesToRevert = nv.readUI(NV_AXIS_SETTINGS_REVERT);
    bitSet(axesToRevert, axisNumber);
    nv.write(NV_AXIS_SETTINGS_REVERT, axesToRevert);
  }

  // write axis settings to NV
  // NV_AXIS_SETTINGS_REVERT bit 0 = settings at compile (0) or run time (1), bits 1 to 9 = reset axis n on next boot
  if (AxisSettingsSize < sizeof(AxisSettings)) { initError.nv = true; DL("ERR: Axis::init(); AxisSettingsSize error NV subsystem writes disabled"); nv.readOnly(true); }
  uint16_t axesToRevert = nv.readUI(NV_AXIS_SETTINGS_REVERT);
  if (!(axesToRevert & 1)) bitSet(axesToRevert, axisNumber);
  if (bitRead(axesToRevert, axisNumber)) nv.updateBytes(NV_AXIS_SETTINGS_BASE + (axisNumber - 1)*AxisSettingsSize, &settings, sizeof(AxisSettings));
  bitClear(axesToRevert, axisNumber);
  nv.write(NV_AXIS_SETTINGS_REVERT, axesToRevert);

  // read axis settings from NV
  nv.readBytes(NV_AXIS_SETTINGS_BASE + (axisNumber - 1)*AxisSettingsSize, &settings, sizeof(AxisSettings));
  if (!validateAxisSettings(axisNumber, alternateLimits, settings)) initError.value = true;

  #if DEBUG == VERBOSE
    V(axisPrefix); VF("stepsPerMeasure="); V(settings.stepsPerMeasure);
    V(", reverse="); if (settings.reverse == OFF) VL("OFF"); else if (settings.reverse == ON) VL("OFF"); else VL("?");
    V(axisPrefix); VF("backlash takeup frequency set to ");
    if (axisNumber <= 3) { V(radToDegF(settings.backlashFreq)); VL(" deg/sec."); } else { V(settings.backlashFreq); VL(" microns/sec."); }
  #endif

  // get the motor ready
  if (!motor->init(axisNumber, settings.reverse, settings.param1, settings.param2)) {
    VF("MSG: Axis, nothing to do exiting!");
    return;
  }
  motor->setBacklashFrequencySteps(settings.backlashFreq*settings.stepsPerMeasure);

  // activate home and limit sense
  V(axisPrefix); VLF("adding any home and/or limit senses");
  homeSenseHandle = senses.add(Pins[index].home, Pins[index].sense.homeInit, Pins[index].sense.homeTrigger);
  minSenseHandle = senses.add(Pins[index].min, Pins[index].sense.minMaxInit, Pins[index].sense.minTrigger);
  maxSenseHandle = senses.add(Pins[index].max, Pins[index].sense.minMaxInit, Pins[index].sense.maxTrigger);
}

// enables or disables the associated step/dir driver
void Axis::enable(bool state) {
  enabled = state;
  motor->power(state & !poweredDown);
}

bool Axis::isEnabled() {
  return enabled;
}

// time (in ms) before automatic power down at standstill, use 0 to disable
void Axis::setPowerDownTime(int value) {
  if (value == 0) powerDownStandstill = false; else { powerDownStandstill = true; powerDownDelay = value; }
}

// time (in ms) to disable automatic power down at standstill, use 0 to disable
void Axis::setPowerDownOverrideTime(int value) {
  if (value == 0) powerDownOverride = false; else { powerDownOverride = true; powerDownOverrideEnds = millis() + value; }
}

double Axis::getStepsPerMeasure() {
  return settings.stepsPerMeasure;
}

int Axis::getStepsPerStepSlewing() {
  return stepsPerStepSlewing;
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

double Axis::getMotorPosition() {
  return motor->getMotorPositionSteps()/settings.stepsPerMeasure;
}

void Axis::setInstrumentCoordinate(double value) {
  setInstrumentCoordinateSteps(lround(value*settings.stepsPerMeasure));
}

void Axis::setInstrumentCoordinateSteps(long value) {
  motor->setInstrumentCoordinateSteps(value);
}

double Axis::getInstrumentCoordinate() {
  return motor->getInstrumentCoordinateSteps()/settings.stepsPerMeasure;
}

void Axis::setInstrumentCoordinatePark(double value) {
  motor->setInstrumentCoordinateParkSteps(lround(value*settings.stepsPerMeasure), settings.subdivisions);
}

void Axis::setTargetCoordinatePark(double value) {
  motor->setFrequencySteps(0);
  motor->setTargetCoordinateParkSteps(lround(value*settings.stepsPerMeasure), settings.subdivisions);
}

void Axis::setTargetCoordinate(double value) {
  setTargetCoordinateSteps(lround(value*settings.stepsPerMeasure));
}

// set target coordinate, in steps
void Axis::setTargetCoordinateSteps(long value) {
  motor->setTargetCoordinateSteps(value);
}

double Axis::getTargetCoordinate() {
  return motor->getTargetCoordinateSteps()/settings.stepsPerMeasure;
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
    slewMpspcs = mpsps/100.0F;
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
    abortMpspcs = mpsps/100.0F;
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
    if (axisNumber <= 2) { V(radToDeg(slewFreq)); V("°/s, accel "); SERIAL_DEBUG.print(radToDeg(slewMpspcs)*100.0F, 3); VL("°/s/s"); }
    if (axisNumber == 3) { V(slewFreq); V("°/s, accel "); SERIAL_DEBUG.print(slewMpspcs*100.0F, 3); VL("°/s/s"); }
    if (axisNumber > 3) { V(slewFreq); V("um/s, accel "); SERIAL_DEBUG.print(slewMpspcs*100.0F, 3); VL("um/s/s"); }
  #endif

  return CE_NONE;
}

// slew to home, with acceleration in "measures" per second per second
CommandError Axis::autoSlewHome() {
  if (!enabled) return CE_SLEW_ERR_IN_STANDBY;
  if (autoRate != AR_NONE) return CE_SLEW_IN_SLEW;
  if (motionError(DIR_BOTH)) return CE_SLEW_ERR_OUTSIDE_LIMITS;

  if (Pins[index].sense.homeTrigger != OFF) {
    motor->setSynchronized(true);
    if (homingStage == HOME_NONE) homingStage = HOME_FAST;
    if (autoRate == AR_NONE) {
      motor->setSlewing(true);
      V(axisPrefix); VF("autoSlewHome started");
      switch (homingStage) {
        case HOME_FAST: VL("fast"); break;
        case HOME_SLOW: VL("slow"); break;
        case HOME_FINE: VL("fine"); break;
        case HOME_NONE: VL("?"); break;
      }
    }
    if (senses.read(homeSenseHandle)) {
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
  V(axisPrefix); VLF("slew stopping");
  autoRate = AR_RATE_BY_TIME_END;
  poll();
}

// emergency stops, with deacceleration by time
void Axis::autoSlewAbort() {
  if (autoRate == AR_NONE) return;
  V(axisPrefix); VLF("slew aborting");
  autoRate = AR_RATE_BY_TIME_ABORT;
  homingStage = HOME_NONE;
  motor->setSynchronized(true);
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
  errors.minLimitSensed = senses.read(minSenseHandle);
  errors.maxLimitSensed = senses.read(maxSenseHandle);

  // stop homing as we pass by the switch
  if (homingStage != HOME_NONE) {
    if (autoRate == AR_RATE_BY_TIME_FORWARD && !senses.read(homeSenseHandle)) autoSlewStop();
    if (autoRate == AR_RATE_BY_TIME_REVERSE && senses.read(homeSenseHandle)) autoSlewStop();
  }
  Y;

  // slewing
  if (autoRate != AR_NONE) {
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
        freq = (getOriginOrTargetDistance()/slewAccelerationDistance)*slewFreq + settings.backlashFreq;
        if (freq < settings.backlashFreq) freq = settings.backlashFreq;
        if (freq > slewFreq) freq = slewFreq;
        if (motor->getTargetDistanceSteps() < 0) freq = -freq;
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
        motor->setSlewing(false);
        autoRate = AR_NONE;
        freq = 0.0F;
        motor->setSynchronized(true);
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
      if (freq > abortMpspcs) freq -= abortMpspcs; else if (freq < -abortMpspcs) freq += abortMpspcs; else freq = 0.0F;
      if (fabs(freq) <= abortMpspcs) {
        motor->setSlewing(false);
        autoRate = AR_NONE;
        freq = 0.0F;
        V(axisPrefix); VLF("slew aborted");
      }
    } else freq = 0.0F;
  } else freq = 0.0F;
  Y;

  setFrequency(freq);
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
  if (!isnan(slewAccelTime)) slewMpspcs = (slewFreq/slewAccelTime)/100.0F;
  if (!isnan(abortAccelTime)) abortMpspcs = (slewFreq/abortAccelTime)/100.0F;
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
    motor->setFrequencySteps((frequency + baseFreq)*settings.stepsPerMeasure);
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

bool Axis::motionError(Direction direction) {
  if (motor->getDriverStatus().fault) { V(axisPrefix); VLF("motion error driver fault"); return true; }
  if (direction == DIR_FORWARD) {
    bool result = (limitsCheck && getInstrumentCoordinate() > settings.limits.max) || errors.maxLimitSensed;
    if (result == true) { V(axisPrefix); VLF("motion error forward limit"); }
    return result;
  }
  if (direction == DIR_REVERSE) {
    bool result =(limitsCheck && getInstrumentCoordinate() < settings.limits.min) || errors.minLimitSensed;
    if (result == true) { V(axisPrefix); VLF("motion error reverse limit"); }
    return result;
  }
  return false;
}

#endif