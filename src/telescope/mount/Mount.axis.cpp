//--------------------------------------------------------------------------------------------------
// telescope mount control, axis instances

#include "Mount.h"

#ifdef MOUNT_PRESENT

#ifdef AXIS1_SERVO_PRESENT
  const ServoDriverPins ServoPinsAxis1 = {AXIS1_SERVO_PH1_PIN, AXIS1_SERVO_PH1_STATE, AXIS1_SERVO_PH2_PIN, AXIS1_SERVO_PH2_STATE, AXIS1_ENABLE_PIN, AXIS1_ENABLE_STATE, AXIS1_FAULT_PIN};
  const ServoDriverSettings ServoSettingsAxis1 = {AXIS1_DRIVER_MODEL, AXIS1_SERVO_P, AXIS1_SERVO_I, AXIS1_SERVO_D, AXIS1_DRIVER_STATUS};
  ServoDriver servoDriver1(1, &ServoPinsAxis1, &ServoSettingsAxis1);

  ServoControl control1;
  #if AXIS1_SERVO_ENCODER == ENC_AB
    Encoder encAxis1(AXIS1_SERVO_ENC1_PIN, AXIS1_SERVO_ENC2_PIN);
  #else
    Encoder encAxis1(AXIS1_SERVO_ENC1_PIN, AXIS1_SERVO_ENC2_PIN, AXIS1_SERVO_ENCODER, AXIS1_SERVO_ENCODER_TRIGGER, &control1.directionHint);
  #endif
  PID pidAxis1(&control1.in, &control1.out, &control1.set, AXIS1_SERVO_P, AXIS1_SERVO_I, AXIS1_SERVO_D, DIRECT);

  ServoMotor motor1(1, &encAxis1, &pidAxis1, &servoDriver1, &control1);
  IRAM_ATTR void moveAxis1() { motor1.move(); }
#endif
#ifdef AXIS1_DRIVER_PRESENT
  const DriverModePins DriverPinsAxis1 = {AXIS1_M0_PIN, AXIS1_M1_PIN, AXIS1_M2_PIN, AXIS1_M3_PIN, AXIS1_DECAY_PIN, AXIS1_FAULT_PIN};
  const DriverSettings DriverSettingsAxis1 = {AXIS1_DRIVER_MODEL, AXIS1_DRIVER_MICROSTEPS, AXIS1_DRIVER_MICROSTEPS_GOTO, AXIS1_DRIVER_IHOLD, AXIS1_DRIVER_IRUN, AXIS1_DRIVER_IGOTO, AXIS1_DRIVER_DECAY, AXIS1_DRIVER_DECAY_GOTO, AXIS1_DRIVER_STATUS};
  StepDirDriver stepDirDriver1(1, &DriverPinsAxis1, &DriverSettingsAxis1);

  const StepDirPins StepDirPinsAxis1 = {AXIS1_STEP_PIN, AXIS1_STEP_STATE, AXIS1_DIR_PIN, AXIS1_ENABLE_PIN, AXIS1_ENABLE_STATE};
  StepDirMotor motor1(1, &StepDirPinsAxis1, &stepDirDriver1);
  IRAM_ATTR void moveAxis1() { motor1.move(AXIS1_STEP_PIN); }
  IRAM_ATTR void moveFFAxis1() { motor1.moveFF(AXIS1_STEP_PIN); }
  IRAM_ATTR void moveFRAxis1() { motor1.moveFR(AXIS1_STEP_PIN); }
#endif
const AxisPins PinsAxis1 = {AXIS1_SENSE_LIMIT_MIN_PIN, AXIS1_SENSE_HOME_PIN, AXIS1_SENSE_LIMIT_MAX_PIN, { AXIS1_SENSE_HOME, AXIS1_SENSE_HOME_INIT, AXIS1_SENSE_LIMIT_MIN, AXIS1_SENSE_LIMIT_MAX, AXIS1_SENSE_LIMIT_INIT}};
const AxisSettings SettingsAxis1 = {AXIS1_STEPS_PER_DEGREE*RAD_DEG_RATIO, AXIS1_REVERSE, AXIS1_PARAMETER1, AXIS1_PARAMETER2, { degToRadF(AXIS1_LIMIT_MIN), degToRadF(AXIS1_LIMIT_MAX)}, siderealToRad(TRACK_BACKLASH_RATE)};
Axis axis1(1, &PinsAxis1, &SettingsAxis1);
void pollAxis1() { axis1.poll(); }

#ifdef AXIS2_SERVO_PRESENT
  const ServoDriverPins ServoPinsAxis2 = {AXIS2_SERVO_PH1_PIN, AXIS2_SERVO_PH1_STATE, AXIS2_SERVO_PH2_PIN, AXIS2_SERVO_PH2_STATE, AXIS2_ENABLE_PIN, AXIS2_ENABLE_STATE, AXIS2_FAULT_PIN};
  const ServoDriverSettings ServoSettingsAxis2 = {AXIS2_DRIVER_MODEL, AXIS2_SERVO_P, AXIS2_SERVO_I, AXIS2_SERVO_D, AXIS2_DRIVER_STATUS};
  ServoDriver servoDriver2(2, &ServoPinsAxis2, &ServoSettingsAxis2);

  ServoControl control2;
  #if AXIS2_SERVO_ENCODER == ENC_AB
    Encoder encAxis2(AXIS2_SERVO_ENC1_PIN, AXIS2_SERVO_ENC2_PIN);
  #else
    Encoder encAxis2(AXIS2_SERVO_ENC1_PIN, AXIS2_SERVO_ENC2_PIN, AXIS2_SERVO_ENCODER, AXIS2_SERVO_ENCODER_TRIGGER, &control2.directionHint);
  #endif
  PID pidAxis2(&control2.in, &control2.out, &control2.set, AXIS2_SERVO_P, AXIS2_SERVO_I, AXIS2_SERVO_D, DIRECT);

  ServoMotor motor2(2, &encAxis2, &pidAxis2, &servoDriver2, &control2);
  IRAM_ATTR void moveAxis2() { motor2.move(); }
#endif
#ifdef AXIS2_DRIVER_PRESENT
  const DriverModePins DriverPinsAxis2 = {AXIS2_M0_PIN, AXIS2_M1_PIN, AXIS2_M2_PIN, AXIS2_M3_PIN, AXIS2_DECAY_PIN, AXIS2_FAULT_PIN};
  const DriverSettings DriverSettingsAxis2 = {AXIS2_DRIVER_MODEL, AXIS2_DRIVER_MICROSTEPS, AXIS2_DRIVER_MICROSTEPS_GOTO, AXIS2_DRIVER_IHOLD, AXIS2_DRIVER_IRUN, AXIS2_DRIVER_IGOTO, AXIS2_DRIVER_DECAY, AXIS2_DRIVER_DECAY_GOTO, AXIS2_DRIVER_STATUS};
  StepDirDriver stepDirDriver2(2, &DriverPinsAxis2, &DriverSettingsAxis2);

  const StepDirPins StepDirPinsAxis2 = {AXIS2_STEP_PIN, AXIS2_STEP_STATE, AXIS2_DIR_PIN, AXIS2_ENABLE_PIN, AXIS2_ENABLE_STATE};
  StepDirMotor motor2(2, &StepDirPinsAxis2, &stepDirDriver2);
  IRAM_ATTR void moveAxis2() { motor2.move(AXIS2_STEP_PIN); }
  IRAM_ATTR void moveFFAxis2() { motor2.moveFF(AXIS2_STEP_PIN); }
  IRAM_ATTR void moveFRAxis2() { motor2.moveFR(AXIS2_STEP_PIN); }
#endif
const AxisPins PinsAxis2 = {AXIS2_SENSE_LIMIT_MIN_PIN, AXIS2_SENSE_HOME_PIN, AXIS2_SENSE_LIMIT_MAX_PIN, {AXIS2_SENSE_HOME, AXIS2_SENSE_HOME_INIT, AXIS2_SENSE_LIMIT_MIN, AXIS2_SENSE_LIMIT_MAX, AXIS2_SENSE_LIMIT_INIT}};
const AxisSettings SettingsAxis2 = {AXIS2_STEPS_PER_DEGREE*RAD_DEG_RATIO, AXIS2_REVERSE, AXIS2_PARAMETER1, AXIS2_PARAMETER2, {degToRadF(AXIS2_LIMIT_MIN), degToRadF(AXIS2_LIMIT_MAX)}, siderealToRad(TRACK_BACKLASH_RATE)};
Axis axis2(2, &PinsAxis2, &SettingsAxis2);
void pollAxis2() { axis2.poll(); }

#endif
