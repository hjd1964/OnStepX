//--------------------------------------------------------------------------------------------------
// telescope rotator control, axis instance

#include "Rotator.h"

#ifdef ROTATOR_PRESENT

#ifdef AXIS3_SERVO_PRESENT
  const ServoDriverPins ServoPinsAxis3 = {AXIS3_SERVO_PH1_PIN, AXIS3_SERVO_PH1_STATE, AXIS3_SERVO_PH2_PIN, AXIS3_SERVO_PH2_STATE, AXIS3_ENABLE_PIN, AXIS3_ENABLE_STATE, AXIS3_FAULT_PIN};
  const ServoDriverSettings ServoSettingsAxis3 = {AXIS3_DRIVER_MODEL, AXIS3_DRIVER_STATUS};
  ServoDriver servoDriver3(3, &ServoPinsAxis3, &ServoSettingsAxis3);

  ServoControl control3;
  #if AXIS3_SERVO_ENCODER == ENC_AB
    Encoder encAxis3(AXIS3_SERVO_ENC1_PIN, AXIS3_SERVO_ENC2_PIN);
  #else
    Encoder encAxis3(AXIS3_SERVO_ENC1_PIN, AXIS3_SERVO_ENC2_PIN, AXIS3_SERVO_ENCODER, AXIS3_SERVO_ENCODER_TRIGGER, &control3.directionHint);
  #endif
  #if AXIS3_SERVO_FEEDBACK == FB_PID
    Pid pidAxis3;
  #endif

  ServoMotor motor3(3, &encAxis3, &pidAxis3, &servoDriver3, &control3);
  void moveAxis3() { motor3.move(); }
#endif

#ifdef AXIS3_DRIVER_PRESENT
  const DriverModePins DriverPinsAxis3 = {AXIS3_M0_PIN, AXIS3_M1_PIN, AXIS3_M2_PIN, AXIS3_M3_PIN, AXIS3_DECAY_PIN, AXIS3_FAULT_PIN};
  const DriverSettings DriverSettingsAxis3 = {AXIS3_DRIVER_MODEL, AXIS3_DRIVER_MICROSTEPS, AXIS3_DRIVER_MICROSTEPS_GOTO, AXIS3_DRIVER_IHOLD, AXIS3_DRIVER_IRUN, AXIS3_DRIVER_IGOTO, AXIS3_DRIVER_DECAY, AXIS3_DRIVER_DECAY_GOTO, AXIS3_DRIVER_STATUS};
  StepDirDriver stepDirDriver3(3, &DriverPinsAxis3, &DriverSettingsAxis3);

  const StepDirPins StepDirPinsAxis3 = {AXIS3_STEP_PIN, AXIS3_STEP_STATE, AXIS3_DIR_PIN, AXIS3_ENABLE_PIN, AXIS3_ENABLE_STATE};
  StepDirMotor motor3(3, &StepDirPinsAxis3, &stepDirDriver3);
  void moveAxis3() { motor3.move(AXIS3_STEP_PIN); }
#endif

const AxisPins PinsAxis3 = {AXIS3_SENSE_LIMIT_MIN_PIN, AXIS3_SENSE_HOME_PIN, AXIS3_SENSE_LIMIT_MAX_PIN, {AXIS3_SENSE_HOME, AXIS3_SENSE_HOME_INIT, AXIS3_SENSE_LIMIT_MIN, AXIS3_SENSE_LIMIT_MAX, AXIS3_SENSE_LIMIT_INIT}};
const AxisSettings SettingsAxis3 = {AXIS3_STEPS_PER_DEGREE, AXIS3_REVERSE, AXIS3_PARAMETER1, AXIS3_PARAMETER2, AXIS3_PARAMETER3, {AXIS3_LIMIT_MIN, AXIS3_LIMIT_MAX}, AXIS3_BACKLASH_RATE};
Axis axis3(3, &PinsAxis3, &SettingsAxis3);
void pollAxis3() { axis3.poll(); }

#endif
