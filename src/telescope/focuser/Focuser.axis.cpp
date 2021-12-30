//--------------------------------------------------------------------------------------------------
// telescope focuser control, axis instances

#include "Focuser.h"

#ifdef FOCUSER_PRESENT

#if AXIS4_DRIVER_MODEL != OFF
  #ifdef AXIS4_SERVO_PRESENT
    const ServoDriverPins ServoPinsAxis4 = {AXIS4_SERVO_PH1_PIN, AXIS4_SERVO_PH1_STATE, AXIS4_SERVO_PH2_PIN, AXIS4_SERVO_PH2_STATE, AXIS4_ENABLE_PIN, AXIS4_ENABLE_STATE, AXIS4_FAULT_PIN};
    const ServoDriverSettings ServoSettingsAxis4 = {AXIS4_DRIVER_MODEL, AXIS4_DRIVER_STATUS};
    ServoDriver servoDriver4(4, &ServoPinsAxis4, &ServoSettingsAxis4);

    ServoControl control4;
    #if AXIS4_SERVO_ENCODER == ENC_AB
      Encoder encAxis4(AXIS4_SERVO_ENC1_PIN, AXIS4_SERVO_ENC2_PIN);
    #else
      Encoder encAxis4(AXIS4_SERVO_ENC1_PIN, AXIS4_SERVO_ENC2_PIN, AXIS4_SERVO_ENCODER, AXIS4_SERVO_ENCODER_TRIGGER, &control4.directionHint);
    #endif
    #if AXIS4_SERVO_FEEDBACK == FB_PID
      Pid pidAxis4;
    #endif

    ServoMotor motor4(4, &encAxis4, &pidAxis4, &servoDriver4, &control4);
    void moveAxis4() { motor4.move(); }
  #endif
  #ifdef AXIS4_DRIVER_PRESENT
    const DriverModePins DriverPinsAxis4 = {AXIS4_M0_PIN, AXIS4_M1_PIN, AXIS4_M2_PIN, AXIS4_M3_PIN, AXIS4_DECAY_PIN, AXIS4_FAULT_PIN};
    const DriverSettings DriverSettingsAxis4 = {AXIS4_DRIVER_MODEL, AXIS4_DRIVER_MICROSTEPS, AXIS4_DRIVER_MICROSTEPS_GOTO, AXIS4_DRIVER_IHOLD, AXIS4_DRIVER_IRUN, AXIS4_DRIVER_IGOTO, AXIS4_DRIVER_DECAY, AXIS4_DRIVER_DECAY_GOTO, AXIS4_DRIVER_STATUS};
    StepDirDriver stepDirDriver4(4, &DriverPinsAxis4, &DriverSettingsAxis4);

    const StepDirPins StepDirPinsAxis4 = {AXIS4_STEP_PIN, AXIS4_STEP_STATE, AXIS4_DIR_PIN, AXIS4_ENABLE_PIN, AXIS4_ENABLE_STATE};
    StepDirMotor motor4(4, &StepDirPinsAxis4, &stepDirDriver4);
    void moveAxis4() { motor4.move(AXIS4_STEP_PIN); }
  #endif
  const AxisPins PinsAxis4 = {AXIS4_SENSE_LIMIT_MIN_PIN, AXIS4_SENSE_HOME_PIN, AXIS4_SENSE_LIMIT_MAX_PIN, {AXIS4_SENSE_HOME, AXIS4_SENSE_HOME_INIT, AXIS4_SENSE_LIMIT_MIN, AXIS4_SENSE_LIMIT_MAX, AXIS4_SENSE_LIMIT_INIT}};
  const AxisSettings SettingsAxis4 = {AXIS4_STEPS_PER_MICRON, AXIS4_REVERSE, AXIS4_PARAMETER1, AXIS4_PARAMETER2, AXIS4_PARAMETER3, {AXIS4_LIMIT_MIN*1000.0F, AXIS4_LIMIT_MAX*1000.0F}, AXIS4_BACKLASH_RATE};
  Axis axis4(4, &PinsAxis4, &SettingsAxis4);
  void pollAxis4() { axis4.poll(); }
#endif

#if AXIS5_DRIVER_MODEL != OFF
  #ifdef AXIS5_SERVO_PRESENT
    const ServoDriverPins ServoPinsAxis5 = {AXIS5_SERVO_PH1_PIN, AXIS5_SERVO_PH1_STATE, AXIS5_SERVO_PH2_PIN, AXIS5_SERVO_PH2_STATE, AXIS5_ENABLE_PIN, AXIS5_ENABLE_STATE, AXIS5_FAULT_PIN};
    const ServoDriverSettings ServoSettingsAxis5 = {AXIS5_DRIVER_MODEL, AXIS5_DRIVER_STATUS};
    ServoDriver servoDriver5(5, &ServoPinsAxis5, &ServoSettingsAxis5);

    ServoControl control5;
    #if AXIS5_SERVO_ENCODER == ENC_AB
      Encoder encAxis5(AXIS5_SERVO_ENC1_PIN, AXIS5_SERVO_ENC2_PIN);
    #else
      Encoder encAxis5(AXIS5_SERVO_ENC1_PIN, AXIS5_SERVO_ENC2_PIN, AXIS5_SERVO_ENCODER, AXIS5_SERVO_ENCODER_TRIGGER, &control5.directionHint);
    #endif
    #if AXIS5_SERVO_FEEDBACK == FB_PID
      Pid pidAxis5;
    #endif

    ServoMotor motor5(5, &encAxis5, &pidAxis5, &servoDriver5, &control5);
    void moveAxis5() { motor5.move(); }
  #endif
  #ifdef AXIS5_DRIVER_PRESENT
    const DriverModePins DriverPinsAxis5 = {AXIS5_M0_PIN, AXIS5_M1_PIN, AXIS5_M2_PIN, AXIS5_M3_PIN, AXIS5_DECAY_PIN, AXIS5_FAULT_PIN};
    const DriverSettings DriverSettingsAxis5 = {AXIS5_DRIVER_MODEL, AXIS5_DRIVER_MICROSTEPS, AXIS5_DRIVER_MICROSTEPS_GOTO, AXIS5_DRIVER_IHOLD, AXIS5_DRIVER_IRUN, AXIS5_DRIVER_IGOTO, AXIS5_DRIVER_DECAY, AXIS5_DRIVER_DECAY_GOTO, AXIS5_DRIVER_STATUS};
    StepDirDriver stepDirDriver5(5, &DriverPinsAxis5, &DriverSettingsAxis5);

    const StepDirPins StepDirPinsAxis5 = {AXIS5_STEP_PIN, AXIS5_STEP_STATE, AXIS5_DIR_PIN, AXIS5_ENABLE_PIN, AXIS5_ENABLE_STATE};
    StepDirMotor motor5(5, &StepDirPinsAxis5, &stepDirDriver5);
    void moveAxis5() { motor5.move(AXIS5_STEP_PIN); }
  #endif
  const AxisPins PinsAxis5 = {AXIS5_SENSE_LIMIT_MIN_PIN, AXIS5_SENSE_HOME_PIN, AXIS5_SENSE_LIMIT_MAX_PIN, {AXIS5_SENSE_HOME, AXIS5_SENSE_HOME_INIT, AXIS5_SENSE_LIMIT_MIN, AXIS5_SENSE_LIMIT_MAX, AXIS5_SENSE_LIMIT_INIT}};
  const AxisSettings SettingsAxis5 = {AXIS5_STEPS_PER_MICRON, AXIS5_REVERSE, AXIS5_PARAMETER1, AXIS5_PARAMETER2, AXIS5_PARAMETER3, {AXIS5_LIMIT_MIN*1000.0F, AXIS5_LIMIT_MAX*1000.0F}, AXIS5_BACKLASH_RATE};
  Axis axis5(5, &PinsAxis5, &SettingsAxis5);
  void pollAxis5() { axis5.poll(); }
#endif

#if AXIS6_DRIVER_MODEL != OFF
  #ifdef AXIS6_SERVO_PRESENT
    const ServoDriverPins ServoPinsAxis6 = {AXIS6_SERVO_PH1_PIN, AXIS6_SERVO_PH1_STATE, AXIS6_SERVO_PH2_PIN, AXIS6_SERVO_PH2_STATE, AXIS6_ENABLE_PIN, AXIS6_ENABLE_STATE, AXIS6_FAULT_PIN};
    const ServoDriverSettings ServoSettingsAxis6 = {AXIS6_DRIVER_MODEL, AXIS6_DRIVER_STATUS};
    ServoDriver servoDriver6(6, &ServoPinsAxis6, &ServoSettingsAxis6);

    ServoControl control6;
    #if AXIS6_SERVO_ENCODER == ENC_AB
      Encoder encAxis6(AXIS6_SERVO_ENC1_PIN, AXIS6_SERVO_ENC2_PIN);
    #else
      Encoder encAxis6(AXIS6_SERVO_ENC1_PIN, AXIS6_SERVO_ENC2_PIN, AXIS6_SERVO_ENCODER, AXIS6_SERVO_ENCODER_TRIGGER, &control6.directionHint);
    #endif
    #if AXIS6_SERVO_FEEDBACK == FB_PID
      Pid pidAxis6;
    #endif

    ServoMotor motor6(6, &encAxis6, &pidAxis6, &servoDriver6, &control6);
    void moveAxis6() { motor6.move(); }
  #endif
  #ifdef AXIS6_DRIVER_PRESENT
    const DriverModePins DriverPinsAxis6 = {AXIS6_M0_PIN, AXIS6_M1_PIN, AXIS6_M2_PIN, AXIS6_M3_PIN, AXIS6_DECAY_PIN, AXIS6_FAULT_PIN};
    const DriverSettings DriverSettingsAxis6 = {AXIS6_DRIVER_MODEL, AXIS6_DRIVER_MICROSTEPS, AXIS6_DRIVER_MICROSTEPS_GOTO, AXIS6_DRIVER_IHOLD, AXIS6_DRIVER_IRUN, AXIS6_DRIVER_IGOTO, AXIS6_DRIVER_DECAY, AXIS6_DRIVER_DECAY_GOTO, AXIS6_DRIVER_STATUS};
    StepDirDriver stepDirDriver6(6, &DriverPinsAxis6, &DriverSettingsAxis6);

    const StepDirPins StepDirPinsAxis6 = {AXIS6_STEP_PIN, AXIS6_STEP_STATE, AXIS6_DIR_PIN, AXIS6_ENABLE_PIN, AXIS6_ENABLE_STATE};
    StepDirMotor motor6(6, &StepDirPinsAxis6, &stepDirDriver6);
    void moveAxis6() { motor6.move(AXIS6_STEP_PIN); }
  #endif
  const AxisPins PinsAxis6 = {AXIS6_SENSE_LIMIT_MIN_PIN, AXIS6_SENSE_HOME_PIN, AXIS6_SENSE_LIMIT_MAX_PIN, {AXIS6_SENSE_HOME, AXIS6_SENSE_HOME_INIT, AXIS6_SENSE_LIMIT_MIN, AXIS6_SENSE_LIMIT_MAX, AXIS6_SENSE_LIMIT_INIT}};
  const AxisSettings SettingsAxis6 = {AXIS6_STEPS_PER_MICRON, AXIS6_REVERSE, AXIS6_PARAMETER1, AXIS6_PARAMETER2, AXIS6_PARAMETER3, {AXIS6_LIMIT_MIN*1000.0F, AXIS6_LIMIT_MAX*1000.0F}, AXIS6_BACKLASH_RATE};
  Axis axis6(6, &PinsAxis6, &SettingsAxis6);
  void pollAxis6() { axis6.poll(); }
#endif

#if AXIS7_DRIVER_MODEL != OFF
  #ifdef AXIS7_SERVO_PRESENT
    const ServoDriverPins ServoPinsAxis7 = {AXIS7_SERVO_PH1_PIN, AXIS7_SERVO_PH1_STATE, AXIS7_SERVO_PH2_PIN, AXIS7_SERVO_PH2_STATE, AXIS7_ENABLE_PIN, AXIS7_ENABLE_STATE, AXIS7_FAULT_PIN};
    const ServoDriverSettings ServoSettingsAxis7 = {AXIS7_DRIVER_MODEL, AXIS7_DRIVER_STATUS};
    ServoDriver servoDriver7(7, &ServoPinsAxis7, &ServoSettingsAxis7);

    ServoControl control7;
    #if AXIS7_SERVO_ENCODER == ENC_AB
      Encoder encAxis7(AXIS7_SERVO_ENC1_PIN, AXIS7_SERVO_ENC2_PIN);
    #else
      Encoder encAxis7(AXIS7_SERVO_ENC1_PIN, AXIS7_SERVO_ENC2_PIN, AXIS7_SERVO_ENCODER, AXIS7_SERVO_ENCODER_TRIGGER, &control7.directionHint);
    #endif
    #if AXIS7_SERVO_FEEDBACK == FB_PID
      Pid pidAxis7;
    #endif

    ServoMotor motor7(7, &encAxis7, &pidAxis7, &servoDriver7, &control7);
    void moveAxis7() { motor7.move(); }
  #endif
  #ifdef AXIS7_DRIVER_PRESENT
    const DriverModePins DriverPinsAxis7 = {AXIS7_M0_PIN, AXIS7_M1_PIN, AXIS7_M2_PIN, AXIS7_M3_PIN, AXIS7_DECAY_PIN, AXIS7_FAULT_PIN};
    const DriverSettings DriverSettingsAxis7 = {AXIS7_DRIVER_MODEL, AXIS7_DRIVER_MICROSTEPS, AXIS7_DRIVER_MICROSTEPS_GOTO, AXIS7_DRIVER_IHOLD, AXIS7_DRIVER_IRUN, AXIS7_DRIVER_IGOTO, AXIS7_DRIVER_DECAY, AXIS7_DRIVER_DECAY_GOTO, AXIS7_DRIVER_STATUS};
    StepDirDriver stepDirDriver7(7, &DriverPinsAxis7, &DriverSettingsAxis7);

    const StepDirPins StepDirPinsAxis7 = {AXIS7_STEP_PIN, AXIS7_STEP_STATE, AXIS7_DIR_PIN, AXIS7_ENABLE_PIN, AXIS7_ENABLE_STATE};
    StepDirMotor motor7(7, &StepDirPinsAxis7, &stepDirDriver7);
    void moveAxis7() { motor7.move(AXIS7_STEP_PIN); }
  #endif
  const AxisPins PinsAxis7 = {AXIS7_SENSE_LIMIT_MIN_PIN, AXIS7_SENSE_HOME_PIN, AXIS7_SENSE_LIMIT_MAX_PIN, {AXIS7_SENSE_HOME, AXIS7_SENSE_HOME_INIT, AXIS7_SENSE_LIMIT_MIN, AXIS7_SENSE_LIMIT_MAX, AXIS7_SENSE_LIMIT_INIT}};
  const AxisSettings SettingsAxis7 = {AXIS7_STEPS_PER_MICRON, AXIS7_REVERSE, AXIS7_PARAMETER1, AXIS7_PARAMETER2, AXIS7_PARAMETER3, {AXIS7_LIMIT_MIN*1000.0F, AXIS7_LIMIT_MAX*1000.0F}, AXIS7_BACKLASH_RATE};
  Axis axis7(7, &PinsAxis7, &SettingsAxis7);
  void pollAxis7() { axis7.poll(); }
#endif

#if AXIS8_DRIVER_MODEL != OFF
  #ifdef AXIS8_SERVO_PRESENT
    const ServoDriverPins ServoPinsAxis8 = {AXIS8_SERVO_PH1_PIN, AXIS8_SERVO_PH1_STATE, AXIS8_SERVO_PH2_PIN, AXIS8_SERVO_PH2_STATE, AXIS8_ENABLE_PIN, AXIS8_ENABLE_STATE, AXIS8_FAULT_PIN};
    const ServoDriverSettings ServoSettingsAxis8 = {AXIS8_DRIVER_MODEL, AXIS8_DRIVER_STATUS};
    ServoDriver servoDriver8(8, &ServoPinsAxis8, &ServoSettingsAxis8);

    ServoControl control8;
    #if AXIS8_SERVO_ENCODER == ENC_AB
      Encoder encAxis8(AXIS8_SERVO_ENC1_PIN, AXIS8_SERVO_ENC2_PIN);
    #else
      Encoder encAxis8(AXIS8_SERVO_ENC1_PIN, AXIS8_SERVO_ENC2_PIN, AXIS8_SERVO_ENCODER, AXIS8_SERVO_ENCODER_TRIGGER, &control8.directionHint);
    #endif
    #if AXIS8_SERVO_FEEDBACK == FB_PID
      Pid pidAxis8;
    #endif

    ServoMotor motor8(8, &encAxis8, &pidAxis8, &servoDriver8, &control8);
    void moveAxis8() { motor8.move(); }
  #endif
  #ifdef AXIS8_DRIVER_PRESENT
    const DriverModePins DriverPinsAxis8 = {AXIS8_M0_PIN, AXIS8_M1_PIN, AXIS8_M2_PIN, AXIS8_M3_PIN, AXIS8_DECAY_PIN, AXIS8_FAULT_PIN};
    const DriverSettings DriverSettingsAxis8 = {AXIS8_DRIVER_MODEL, AXIS8_DRIVER_MICROSTEPS, AXIS8_DRIVER_MICROSTEPS_GOTO, AXIS8_DRIVER_IHOLD, AXIS8_DRIVER_IRUN, AXIS8_DRIVER_IGOTO, AXIS8_DRIVER_DECAY, AXIS8_DRIVER_DECAY_GOTO, AXIS8_DRIVER_STATUS};
    StepDirDriver stepDirDriver8(8, &DriverPinsAxis8, &DriverSettingsAxis8);

    const StepDirPins StepDirPinsAxis8 = {AXIS8_STEP_PIN, AXIS8_STEP_STATE, AXIS8_DIR_PIN, AXIS8_ENABLE_PIN, AXIS8_ENABLE_STATE};
    StepDirMotor motor8(8, &StepDirPinsAxis8, &stepDirDriver8);
    void moveAxis8() { motor8.move(AXIS8_STEP_PIN); }
  #endif
  const AxisPins PinsAxis8 = {AXIS8_SENSE_LIMIT_MIN_PIN, AXIS8_SENSE_HOME_PIN, AXIS8_SENSE_LIMIT_MAX_PIN, {AXIS8_SENSE_HOME, AXIS8_SENSE_HOME_INIT, AXIS8_SENSE_LIMIT_MIN, AXIS8_SENSE_LIMIT_MAX, AXIS8_SENSE_LIMIT_INIT}};
  const AxisSettings SettingsAxis8 = {AXIS8_STEPS_PER_MICRON, AXIS8_REVERSE, AXIS8_PARAMETER1, AXIS8_PARAMETER2, AXIS8_PARAMETER3, {AXIS8_LIMIT_MIN*1000.0F, AXIS8_LIMIT_MAX*1000.0F}, AXIS8_BACKLASH_RATE};
  Axis axis8(8, &PinsAxis8, &SettingsAxis8);
  void pollAxis8() { axis8.poll(); }
#endif

#if AXIS9_DRIVER_MODEL != OFF
  #ifdef AXIS9_SERVO_PRESENT
    const ServoDriverPins ServoPinsAxis9 = {AXIS9_SERVO_PH1_PIN, AXIS9_SERVO_PH1_STATE, AXIS9_SERVO_PH2_PIN, AXIS9_SERVO_PH2_STATE, AXIS9_ENABLE_PIN, AXIS9_ENABLE_STATE, AXIS9_FAULT_PIN};
    const ServoDriverSettings ServoSettingsAxis9 = {AXIS9_DRIVER_MODEL, AXIS9_DRIVER_STATUS};
    ServoDriver servoDriver9(9, &ServoPinsAxis9, &ServoSettingsAxis9);

    ServoControl control9;
    #if AXIS9_SERVO_ENCODER == ENC_AB
      Encoder encAxis9(AXIS9_SERVO_ENC1_PIN, AXIS9_SERVO_ENC2_PIN);
    #else
      Encoder encAxis9(AXIS9_SERVO_ENC1_PIN, AXIS9_SERVO_ENC2_PIN, AXIS9_SERVO_ENCODER, AXIS9_SERVO_ENCODER_TRIGGER, &control9.directionHint);
    #endif
    #if AXIS9_SERVO_FEEDBACK == FB_PID
      Pid pidAxis9;
    #endif

    ServoMotor motor9(9, &encAxis9, &pidAxis9, &servoDriver9, &control9);
    void moveAxis9() { motor9.move(); }
  #endif
  #ifdef AXIS9_DRIVER_PRESENT
    const DriverModePins DriverPinsAxis9 = {AXIS9_M0_PIN, AXIS9_M1_PIN, AXIS9_M2_PIN, AXIS9_M3_PIN, AXIS9_DECAY_PIN, AXIS9_FAULT_PIN};
    const DriverSettings DriverSettingsAxis9 = {AXIS9_DRIVER_MODEL, AXIS9_DRIVER_MICROSTEPS, AXIS9_DRIVER_MICROSTEPS_GOTO, AXIS9_DRIVER_IHOLD, AXIS9_DRIVER_IRUN, AXIS9_DRIVER_IGOTO, AXIS9_DRIVER_DECAY, AXIS9_DRIVER_DECAY_GOTO, AXIS9_DRIVER_STATUS};
    StepDirDriver stepDirDriver9(9, &DriverPinsAxis9, &DriverSettingsAxis9);

    const StepDirPins StepDirPinsAxis9 = {AXIS9_STEP_PIN, AXIS9_STEP_STATE, AXIS9_DIR_PIN, AXIS9_ENABLE_PIN, AXIS9_ENABLE_STATE};
    StepDirMotor motor9(9, &StepDirPinsAxis9, &stepDirDriver9);
    void moveAxis9() { motor9.move(AXIS9_STEP_PIN); }
  #endif
  const AxisPins PinsAxis9 = {AXIS9_SENSE_LIMIT_MIN_PIN, AXIS9_SENSE_HOME_PIN, AXIS9_SENSE_LIMIT_MAX_PIN, {AXIS9_SENSE_HOME, AXIS9_SENSE_HOME_INIT, AXIS9_SENSE_LIMIT_MIN, AXIS9_SENSE_LIMIT_MAX, AXIS9_SENSE_LIMIT_INIT}};
  const AxisSettings SettingsAxis9 = {AXIS9_STEPS_PER_MICRON, AXIS9_REVERSE, AXIS9_PARAMETER1, AXIS9_PARAMETER2, AXIS9_PARAMETER3, {AXIS9_LIMIT_MIN*1000.0F, AXIS9_LIMIT_MAX*1000.0F}, AXIS9_BACKLASH_RATE};
  Axis axis9(9, &PinsAxis9, &SettingsAxis9);
  void pollAxis9() { axis9.poll(); }
#endif

#endif
