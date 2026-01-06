//--------------------------------------------------------------------------------------------------
// telescope focuser control, axis7

#include "Focuser.h"

// note there is a prototype file that should be copy/pasted to build files
// for axes 4 to 9 by replacing all occurances of '@' with the axis number

#if AXIS7_DRIVER_MODEL != OFF

  namespace {

  #if defined(AXIS7_KTECH_PRESENT)
    const KTechDriverSettings DriverSettingsAxis7 = {AXIS7_DRIVER_MODEL, AXIS7_DRIVER_STATUS};
    KTechMotor motor_7(7, AXIS7_REVERSE, &DriverSettingsAxis7);

  #elif defined(AXIS7_MKS42D_PRESENT)
    const MksDriverSettings DriverSettingsAxis7 = {AXIS7_DRIVER_MODEL, AXIS7_DRIVER_STATUS};
    Mks42DMotor motor_7(7, AXIS7_REVERSE, &DriverSettingsAxis7, AXIS7_STEPS_PER_DEGREE);

  #elif defined(AXIS7_SERVO_PRESENT)
    ServoControl servoControlAxis7;

    #if AXIS7_ENCODER == AB
      Quadrature encAxis7(7, AXIS7_ENCODER_A_PIN, AXIS7_ENCODER_B_PIN);
    #elif AXIS7_ENCODER == AB_ESP32
      QuadratureEsp32 encAxis7(7, AXIS7_ENCODER_A_PIN, AXIS7_ENCODER_B_PIN);
    #elif AXIS7_ENCODER == CW_CCW
      CwCcw encAxis7(7, AXIS7_ENCODER_A_PIN, AXIS7_ENCODER_B_PIN);
    #elif AXIS7_ENCODER == PULSE_DIR
      PulseDir encAxis7(7, AXIS7_ENCODER_A_PIN, AXIS7_ENCODER_B_PIN);
    #elif AXIS7_ENCODER == PULSE_ONLY
      PulseOnly encAxis7(7, AXIS7_ENCODER_A_PIN, &servoControlAxis7.directionHint);
    #elif AXIS7_ENCODER == VIRTUAL
      VirtualEnc encAxis7(7);
    #elif AXIS7_ENCODER == SERIAL_BRIDGE
      SerialBridge encAxis7(7);
    #endif

    #if AXIS7_SERVO_FEEDBACK == PID
      Pid feedbackAxis7(AXIS7_PID_P, AXIS7_PID_I, AXIS7_PID_D);
    #elif AXIS7_SERVO_FEEDBACK == DUAL_PID
      DualPid feedbackAxis7(AXIS7_PID_P, AXIS7_PID_I, AXIS7_PID_D, AXIS7_PID_P_GOTO, AXIS7_PID_I_GOTO, AXIS7_PID_D_GOTO, AXIS7_PID_SENSITIVITY);
    #endif

    #if AXIS7_SERVO_FLTR == KALMAN
      KalmanFilter filterAxis7(AXIS7_SERVO_FLTR_MEAS_U, AXIS7_SERVO_FLTR_VARIANCE);
    #elif AXIS7_SERVO_FLTR == ROLLING
      RollingFilter filterAxis7(AXIS7_SERVO_FLTR_WSIZE);
    #elif AXIS7_SERVO_FLTR == WINDOWING
      WindowingFilter filterAxis7(AXIS7_SERVO_FLTR_WSIZE);
    #elif AXIS7_SERVO_FLTR == OFF
      Filter filterAxis7;
    #endif

    const ServoPins DriverPinsAxis7 = {AXIS7_SERVO_PH1_PIN, AXIS7_SERVO_PH1_STATE, AXIS7_SERVO_PH2_PIN, AXIS7_SERVO_PH2_STATE, AXIS7_ENABLE_PIN, AXIS7_ENABLE_STATE, AXIS7_M0_PIN, AXIS7_M1_PIN, AXIS7_M2_PIN, AXIS7_M3_PIN, AXIS7_FAULT_PIN};
    const ServoSettings DriverSettingsAxis7 = {AXIS7_DRIVER_MODEL, AXIS7_DRIVER_STATUS, AXIS7_SERVO_ACCELERATION};

    #if AXIS7_DRIVER_MODEL == SERVO_EE
      ServoEE driver7(7, &DriverPinsAxis7, &DriverSettingsAxis7, AXIS7_SERVO_DC_PWR_MIN, AXIS7_SERVO_DC_PWR_MAX);
    #elif AXIS7_DRIVER_MODEL == SERVO_PE
      ServoPE driver7(7, &DriverPinsAxis7, &DriverSettingsAxis7, AXIS7_SERVO_DC_PWR_MIN, AXIS7_SERVO_DC_PWR_MAX);
    #elif AXIS7_DRIVER_MODEL == SERVO_TMC2130_DC
      ServoTmc2130DC driver7(7, &DriverPinsAxis7, &DriverSettingsAxis7, AXIS7_SERVO_DC_PWR_MIN, AXIS7_SERVO_DC_PWR_MAX);
    #elif AXIS7_DRIVER_MODEL == SERVO_TMC5160_DC
      ServoTmc5160DC driver7(7, &DriverPinsAxis7, &DriverSettingsAxis7, AXIS7_SERVO_DC_PWR_MIN, AXIS7_SERVO_DC_PWR_MAX);
    #elif AXIS7_DRIVER_MODEL == TMC2209 || AXIS7_DRIVER_MODEL == TMC2226
      ServoTmc2209 driver7(7, &DriverPinsAxis7, &DriverSettingsAxis7, AXIS7_MOTOR_STEPS_PER_MICRON/AXIS7_STEPS_PER_MICRON, AXIS7_DRIVER_MICROSTEPS, AXIS7_DRIVER_IRUN, AXIS7_DRIVER_DECAY, AXIS7_DRIVER_DECAY_GOTO);
    #elif AXIS7_DRIVER_MODEL == SERVO_TMC5160
      ServoTmc5160 driver7(7, &DriverPinsAxis7, &DriverSettingsAxis7, AXIS7_MOTOR_STEPS_PER_MICRON/AXIS7_STEPS_PER_MICRON, AXIS7_DRIVER_MICROSTEPS, AXIS7_DRIVER_IRUN, AXIS7_DRIVER_DECAY, AXIS7_DRIVER_DECAY_GOTO);
    #elif AXIS7_DRIVER_MODEL == SERVO_KTECH
      ServoKTech driver7(7, &DriverSettingsAxis7, AXIS7_MOTOR_STEPS_PER_MICRON/AXIS7_STEPS_PER_MICRON);
    #endif

    ServoMotor motor_7(7, AXIS7_REVERSE, ((ServoDriver*)&driver7), &filterAxis7, &encAxis7, AXIS7_ENCODER_ORIGIN, AXIS7_ENCODER_REVERSE == ON, &feedbackAxis7, &servoControlAxis7, AXIS7_SYNC_THRESHOLD);

  #elif defined(AXIS7_STEP_DIR_PRESENT)
    const StepDirDriverPins DriverPinsAxis7 = {AXIS7_M0_PIN, AXIS7_M1_PIN, AXIS7_M2_PIN, AXIS7_M2_ON_STATE, AXIS7_M3_PIN, AXIS7_DECAY_PIN, AXIS7_FAULT_PIN};
    const StepDirDriverSettings DriverSettingsAxis7 = {AXIS7_DRIVER_MODEL, AXIS7_DRIVER_STATUS, AXIS7_DRIVER_MICROSTEPS, AXIS7_DRIVER_MICROSTEPS_GOTO, AXIS7_DRIVER_DECAY, AXIS7_DRIVER_DECAY_GOTO};
    #if AXIS7_DRIVER_MODEL >= STEP_DIR_DRIVER_FIRST && AXIS7_DRIVER_MODEL < TMC_DRIVER_FIRST
      StepDirGeneric driver7(7, &DriverPinsAxis7, &DriverSettingsAxis7);
    #elif AXIS7_DRIVER_MODEL == TMC2130
      StepDirTmc2130 driver7(7, &DriverPinsAxis7, &DriverSettingsAxis7, AXIS7_DRIVER_IHOLD, AXIS7_DRIVER_IRUN, AXIS7_DRIVER_IGOTO, AXIS7_DRIVER_INTPOL);
    #elif AXIS7_DRIVER_MODEL == TMC2160
      StepDirTmc2160 driver7(7, &DriverPinsAxis7, &DriverSettingsAxis7, AXIS7_DRIVER_IHOLD, AXIS7_DRIVER_IRUN, AXIS7_DRIVER_IGOTO, AXIS7_DRIVER_INTPOL);
    #elif AXIS7_DRIVER_MODEL == TMC2208
      StepDirTmc2208 driver7(7, &DriverPinsAxis7, &DriverSettingsAxis7, AXIS7_DRIVER_IHOLD, AXIS7_DRIVER_IRUN, AXIS7_DRIVER_IGOTO, AXIS7_DRIVER_INTPOL);
    #elif AXIS7_DRIVER_MODEL == TMC2209
      StepDirTmc2209 driver7(7, &DriverPinsAxis7, &DriverSettingsAxis7, AXIS7_DRIVER_IHOLD, AXIS7_DRIVER_IRUN, AXIS7_DRIVER_IGOTO, AXIS7_DRIVER_INTPOL);
    #elif AXIS7_DRIVER_MODEL == TMC2660
      StepDirTmc2660 driver7(7, &DriverPinsAxis7, &DriverSettingsAxis7, AXIS7_DRIVER_IHOLD, AXIS7_DRIVER_IRUN, AXIS7_DRIVER_IGOTO, AXIS7_DRIVER_INTPOL);
    #elif AXIS7_DRIVER_MODEL == TMC5160
      StepDirTmc5160 driver7(7, &DriverPinsAxis7, &DriverSettingsAxis7, AXIS7_DRIVER_IHOLD, AXIS7_DRIVER_IRUN, AXIS7_DRIVER_IGOTO, AXIS7_DRIVER_INTPOL);
    #elif AXIS7_DRIVER_MODEL == TMC5161
      StepDirTmc5161 driver7(7, &DriverPinsAxis7, &DriverSettingsAxis7, AXIS7_DRIVER_IHOLD, AXIS7_DRIVER_IRUN, AXIS7_DRIVER_IGOTO, AXIS7_DRIVER_INTPOL);
    #endif

    const StepDirPins StepDirPinsAxis7 = {AXIS7_STEP_PIN, AXIS7_STEP_STATE, AXIS7_DIR_PIN, AXIS7_ENABLE_PIN, AXIS7_ENABLE_STATE};
    StepDirMotor motor_7(7, AXIS7_REVERSE, &StepDirPinsAxis7, ((StepDirDriver*)&driver7));
  #else
    #error "Configuration (Config.h): Focuser has an unrecognized AXIS7_DRIVER_MODEL this should never happen!"
  #endif

  const AxisPins PinsAxis7 = {AXIS7_SENSE_LIMIT_MIN_PIN, AXIS7_SENSE_HOME_PIN, AXIS7_SENSE_LIMIT_MAX_PIN, {AXIS7_SENSE_HOME, AXIS7_SENSE_HOME_INIT, AXIS7_SENSE_HOME_DIST_LIMIT*1000.0F, AXIS7_SENSE_LIMIT_MIN, AXIS7_SENSE_LIMIT_MAX, AXIS7_SENSE_LIMIT_INIT}};
  const AxisSettings SettingsAxis7 = {AXIS7_STEPS_PER_MICRON, {AXIS7_LIMIT_MIN*1000.0F, AXIS7_LIMIT_MAX*1000.0F}, AXIS7_BACKLASH_RATE};
  }

  Motor& motor7 = motor_7;

  Axis axis7(7, &PinsAxis7, &SettingsAxis7, AXIS_MEASURE_MICRONS);
#endif
