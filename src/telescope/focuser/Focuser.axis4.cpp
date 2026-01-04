//--------------------------------------------------------------------------------------------------
// telescope focuser control, axis4

#include "Focuser.h"

// note there is a prototype file that should be copy/pasted to build files
// for axes 4 to 9 by replacing all occurances of '@' with the axis number

#if FOCUSER_PRESENT

  namespace {

  #if defined(AXIS4_KTECH_PRESENT)
    const KTechDriverSettings DriverSettingsAxis4 = {AXIS4_DRIVER_MODEL, AXIS4_DRIVER_STATUS};
    KTechMotor motor_4(4, AXIS4_REVERSE, &DriverSettingsAxis4);

  #elif defined(AXIS4_MKS42D_PRESENT)
    const MksDriverSettings DriverSettingsAxis4 = {AXIS4_DRIVER_MODEL, AXIS4_DRIVER_STATUS};
    Mks42DMotor motor_4(4, AXIS4_REVERSE, &DriverSettingsAxis4, AXIS4_STEPS_PER_DEGREE);

  #elif defined(AXIS4_SERVO_PRESENT)
    ServoControl servoControlAxis4;

    #if AXIS4_ENCODER == AB
      Quadrature encAxis4(4, AXIS4_ENCODER_A_PIN, AXIS4_ENCODER_B_PIN);
    #elif AXIS4_ENCODER == AB_ESP32
      QuadratureEsp32 encAxis4(4, AXIS4_ENCODER_A_PIN, AXIS4_ENCODER_B_PIN);
    #elif AXIS4_ENCODER == CW_CCW
      CwCcw encAxis4(4, AXIS4_ENCODER_A_PIN, AXIS4_ENCODER_B_PIN);
    #elif AXIS4_ENCODER == PULSE_DIR
      PulseDir encAxis4(4, AXIS4_ENCODER_A_PIN, AXIS4_ENCODER_B_PIN);
    #elif AXIS4_ENCODER == PULSE_ONLY
      PulseOnly encAxis4(4, AXIS4_ENCODER_A_PIN, &servoControlAxis4.directionHint);
    #elif AXIS4_ENCODER == VIRTUAL
      VirtualEnc encAxis4(4);
    #elif AXIS4_ENCODER == SERIAL_BRIDGE
      SerialBridge encAxis4(4);
    #endif

    #if AXIS4_SERVO_FEEDBACK == PID
      Pid feedbackAxis4(AXIS4_PID_P, AXIS4_PID_I, AXIS4_PID_D);
    #elif AXIS4_SERVO_FEEDBACK == DUAL_PID
      DualPid feedbackAxis4(AXIS4_PID_P, AXIS4_PID_I, AXIS4_PID_D, AXIS4_PID_P_GOTO, AXIS4_PID_I_GOTO, AXIS4_PID_D_GOTO, AXIS4_PID_SENSITIVITY);
    #endif

    #if AXIS4_SERVO_FLTR == KALMAN
      KalmanFilter filterAxis4(AXIS4_SERVO_FLTR_MEAS_U, AXIS4_SERVO_FLTR_VARIANCE);
    #elif AXIS4_SERVO_FLTR == ROLLING
      RollingFilter filterAxis4(AXIS4_SERVO_FLTR_WSIZE);
    #elif AXIS4_SERVO_FLTR == WINDOWING
      WindowingFilter filterAxis4(AXIS4_SERVO_FLTR_WSIZE);
    #elif AXIS4_SERVO_FLTR == OFF
      Filter filterAxis4;
    #endif

    const ServoPins DriverPinsAxis4 = {AXIS4_SERVO_PH1_PIN, AXIS4_SERVO_PH1_STATE, AXIS4_SERVO_PH2_PIN, AXIS4_SERVO_PH2_STATE, AXIS4_ENABLE_PIN, AXIS4_ENABLE_STATE, AXIS4_M0_PIN, AXIS4_M1_PIN, AXIS4_M2_PIN, AXIS4_M3_PIN, AXIS4_FAULT_PIN};
    const ServoSettings DriverSettingsAxis4 = {AXIS4_DRIVER_MODEL, AXIS4_DRIVER_STATUS, AXIS4_SERVO_ACCELERATION};

    #if AXIS4_DRIVER_MODEL == SERVO_EE
      ServoEE driver4(4, &DriverPinsAxis4, &DriverSettingsAxis4, AXIS4_SERVO_DC_PWR_MIN, AXIS4_SERVO_DC_PWR_MAX);
    #elif AXIS4_DRIVER_MODEL == SERVO_PE
      ServoPE driver4(4, &DriverPinsAxis4, &DriverSettingsAxis4, AXIS4_SERVO_DC_PWR_MIN, AXIS4_SERVO_DC_PWR_MAX);
    #elif AXIS4_DRIVER_MODEL == SERVO_TMC2130_DC
      ServoTmc2130DC driver4(4, &DriverPinsAxis4, &DriverSettingsAxis4, AXIS4_SERVO_DC_PWR_MIN, AXIS4_SERVO_DC_PWR_MAX);
    #elif AXIS4_DRIVER_MODEL == SERVO_TMC5160_DC
      ServoTmc5160DC driver4(4, &DriverPinsAxis4, &DriverSettingsAxis4, AXIS4_SERVO_DC_PWR_MIN, AXIS4_SERVO_DC_PWR_MAX);
    #elif AXIS4_DRIVER_MODEL == TMC2209 || AXIS4_DRIVER_MODEL == TMC2226
      ServoTmc2209 driver4(4, &DriverPinsAxis4, &DriverSettingsAxis4, AXIS4_MOTOR_STEPS_PER_MICRON/AXIS4_STEPS_PER_MICRON, AXIS4_DRIVER_MICROSTEPS, AXIS4_DRIVER_IRUN, AXIS4_DRIVER_DECAY, AXIS4_DRIVER_DECAY_GOTO);
    #elif AXIS4_DRIVER_MODEL == SERVO_TMC5160
      ServoTmc5160 driver4(4, &DriverPinsAxis4, &DriverSettingsAxis4, AXIS4_MOTOR_STEPS_PER_MICRON/AXIS4_STEPS_PER_MICRON, AXIS4_DRIVER_MICROSTEPS, AXIS4_DRIVER_IRUN, AXIS4_DRIVER_DECAY, AXIS4_DRIVER_DECAY_GOTO);
    #elif AXIS4_DRIVER_MODEL == SERVO_KTECH
      ServoKTech driver4(4, &DriverSettingsAxis4, AXIS4_MOTOR_STEPS_PER_MICRON/AXIS4_STEPS_PER_MICRON);
    #endif

    ServoMotor motor_4(4, AXIS4_REVERSE, ((ServoDriver*)&driver4), &filterAxis4, &encAxis4, AXIS4_ENCODER_ORIGIN, AXIS4_ENCODER_REVERSE == ON, &feedbackAxis4, &servoControlAxis4, AXIS4_SYNC_THRESHOLD);

  #elif defined(AXIS4_STEP_DIR_PRESENT)
    const StepDirDriverPins DriverPinsAxis4 = {AXIS4_M0_PIN, AXIS4_M1_PIN, AXIS4_M2_PIN, AXIS4_M2_ON_STATE, AXIS4_M3_PIN, AXIS4_DECAY_PIN, AXIS4_FAULT_PIN};
    const StepDirDriverSettings DriverSettingsAxis4 = {AXIS4_DRIVER_MODEL, AXIS4_DRIVER_STATUS, AXIS4_DRIVER_MICROSTEPS, AXIS4_DRIVER_MICROSTEPS_GOTO, AXIS4_DRIVER_DECAY, AXIS4_DRIVER_DECAY_GOTO};
    #if AXIS4_DRIVER_MODEL >= STEP_DIR_DRIVER_FIRST && AXIS4_DRIVER_MODEL < TMC_DRIVER_FIRST
      StepDirGeneric driver4(4, &DriverPinsAxis4, &DriverSettingsAxis4);
    #elif AXIS4_DRIVER_MODEL == TMC2130
      StepDirTmc2130 driver4(4, &DriverPinsAxis4, &DriverSettingsAxis4, AXIS4_DRIVER_IHOLD, AXIS4_DRIVER_IRUN, AXIS4_DRIVER_IGOTO, AXIS4_DRIVER_INTPOL);
    #elif AXIS4_DRIVER_MODEL == TMC2160
      StepDirTmc2160 driver4(4, &DriverPinsAxis4, &DriverSettingsAxis4, AXIS4_DRIVER_IHOLD, AXIS4_DRIVER_IRUN, AXIS4_DRIVER_IGOTO, AXIS4_DRIVER_INTPOL);
    #elif AXIS4_DRIVER_MODEL == TMC2208
      StepDirTmc2208 driver4(4, &DriverPinsAxis4, &DriverSettingsAxis4, AXIS4_DRIVER_IHOLD, AXIS4_DRIVER_IRUN, AXIS4_DRIVER_IGOTO, AXIS4_DRIVER_INTPOL);
    #elif AXIS4_DRIVER_MODEL == TMC2209
      StepDirTmc2209 driver4(4, &DriverPinsAxis4, &DriverSettingsAxis4, AXIS4_DRIVER_IHOLD, AXIS4_DRIVER_IRUN, AXIS4_DRIVER_IGOTO, AXIS4_DRIVER_INTPOL);
    #elif AXIS4_DRIVER_MODEL == TMC2660
      StepDirTmc2660 driver4(4, &DriverPinsAxis4, &DriverSettingsAxis4, AXIS4_DRIVER_IHOLD, AXIS4_DRIVER_IRUN, AXIS4_DRIVER_IGOTO, AXIS4_DRIVER_INTPOL);
    #elif AXIS4_DRIVER_MODEL == TMC5160
      StepDirTmc5160 driver4(4, &DriverPinsAxis4, &DriverSettingsAxis4, AXIS4_DRIVER_IHOLD, AXIS4_DRIVER_IRUN, AXIS4_DRIVER_IGOTO, AXIS4_DRIVER_INTPOL);
    #elif AXIS4_DRIVER_MODEL == TMC5161
      StepDirTmc5161 driver4(4, &DriverPinsAxis4, &DriverSettingsAxis4, AXIS4_DRIVER_IHOLD, AXIS4_DRIVER_IRUN, AXIS4_DRIVER_IGOTO, AXIS4_DRIVER_INTPOL);
    #endif

    const StepDirPins StepDirPinsAxis4 = {AXIS4_STEP_PIN, AXIS4_STEP_STATE, AXIS4_DIR_PIN, AXIS4_ENABLE_PIN, AXIS4_ENABLE_STATE};
    StepDirMotor motor_4(4, AXIS4_REVERSE, &StepDirPinsAxis4, ((StepDirDriver*)&driver4));
  #else
    #error "Configuration (Config.h): Focuser has an unrecognized AXIS4_DRIVER_MODEL this should never happen!"
  #endif

  const AxisPins PinsAxis4 = {AXIS4_SENSE_LIMIT_MIN_PIN, AXIS4_SENSE_HOME_PIN, AXIS4_SENSE_LIMIT_MAX_PIN, {AXIS4_SENSE_HOME, AXIS4_SENSE_HOME_INIT, AXIS4_SENSE_HOME_DIST_LIMIT*1000.0F, AXIS4_SENSE_LIMIT_MIN, AXIS4_SENSE_LIMIT_MAX, AXIS4_SENSE_LIMIT_INIT}};
  const AxisSettings SettingsAxis4 = {AXIS4_STEPS_PER_MICRON, {AXIS4_LIMIT_MIN*1000.0F, AXIS4_LIMIT_MAX*1000.0F}, AXIS4_BACKLASH_RATE};
  }

  Motor& motor4 = motor_4;

  Axis axis4(4, &PinsAxis4, &SettingsAxis4, AXIS_MEASURE_MICRONS);
#endif
