//--------------------------------------------------------------------------------------------------
// telescope focuser control, axis5

#include "Focuser.h"

// note there is a prototype file that should be copy/pasted to build files
// for axes 4 to 9 by replacing all occurances of '@' with the axis number

#if FOCUSER_PRESENT

  namespace {

  #if defined(AXIS5_KTECH_PRESENT)
    const KTechDriverSettings DriverSettingsAxis5 = {AXIS5_DRIVER_MODEL, AXIS5_DRIVER_STATUS};
    KTechMotor motor_5(5, AXIS5_REVERSE, &DriverSettingsAxis5);

  #elif defined(AXIS5_MKS42D_PRESENT)
    const MksDriverSettings DriverSettingsAxis5 = {AXIS5_DRIVER_MODEL, AXIS5_DRIVER_STATUS};
    Mks42DMotor motor_5(5, AXIS5_REVERSE, &DriverSettingsAxis5, AXIS5_STEPS_PER_DEGREE);

  #elif defined(AXIS5_SERVO_PRESENT)
    ServoControl servoControlAxis5;

    #if AXIS5_ENCODER == AB
      Quadrature encAxis5(5, AXIS5_ENCODER_A_PIN, AXIS5_ENCODER_B_PIN);
    #elif AXIS5_ENCODER == AB_ESP32
      QuadratureEsp32 encAxis5(5, AXIS5_ENCODER_A_PIN, AXIS5_ENCODER_B_PIN);
    #elif AXIS5_ENCODER == CW_CCW
      CwCcw encAxis5(5, AXIS5_ENCODER_A_PIN, AXIS5_ENCODER_B_PIN);
    #elif AXIS5_ENCODER == PULSE_DIR
      PulseDir encAxis5(5, AXIS5_ENCODER_A_PIN, AXIS5_ENCODER_B_PIN);
    #elif AXIS5_ENCODER == PULSE_ONLY
      PulseOnly encAxis5(5, AXIS5_ENCODER_A_PIN, &servoControlAxis5.directionHint);
    #elif AXIS5_ENCODER == VIRTUAL
      VirtualEnc encAxis5(5);
    #elif AXIS5_ENCODER == SERIAL_BRIDGE
      SerialBridge encAxis5(5);
    #endif

    #if AXIS5_SERVO_FEEDBACK == PID
      Pid feedbackAxis5(AXIS5_PID_P, AXIS5_PID_I, AXIS5_PID_D);
    #elif AXIS5_SERVO_FEEDBACK == DUAL_PID
      DualPid feedbackAxis5(AXIS5_PID_P, AXIS5_PID_I, AXIS5_PID_D, AXIS5_PID_P_GOTO, AXIS5_PID_I_GOTO, AXIS5_PID_D_GOTO, AXIS5_PID_SENSITIVITY);
    #endif

    #if AXIS5_SERVO_FLTR == KALMAN
      KalmanFilter filterAxis5(AXIS5_SERVO_FLTR_MEAS_U, AXIS5_SERVO_FLTR_VARIANCE);
    #elif AXIS5_SERVO_FLTR == ROLLING
      RollingFilter filterAxis5(AXIS5_SERVO_FLTR_WSIZE);
    #elif AXIS5_SERVO_FLTR == WINDOWING
      WindowingFilter filterAxis5(AXIS5_SERVO_FLTR_WSIZE);
    #elif AXIS5_SERVO_FLTR == OFF
      Filter filterAxis5;
    #endif

    const ServoPins DriverPinsAxis5 = {AXIS5_SERVO_PH1_PIN, AXIS5_SERVO_PH1_STATE, AXIS5_SERVO_PH2_PIN, AXIS5_SERVO_PH2_STATE, AXIS5_ENABLE_PIN, AXIS5_ENABLE_STATE, AXIS5_M0_PIN, AXIS5_M1_PIN, AXIS5_M2_PIN, AXIS5_M3_PIN, AXIS5_FAULT_PIN};
    const ServoSettings DriverSettingsAxis5 = {AXIS5_DRIVER_MODEL, AXIS5_DRIVER_STATUS, AXIS5_SERVO_ACCELERATION};

    #if AXIS5_DRIVER_MODEL == SERVO_EE
      ServoEE driver5(5, &DriverPinsAxis5, &DriverSettingsAxis5, AXIS5_SERVO_DC_PWR_MIN, AXIS5_SERVO_DC_PWR_MAX);
    #elif AXIS5_DRIVER_MODEL == SERVO_PE
      ServoPE driver5(5, &DriverPinsAxis5, &DriverSettingsAxis5, AXIS5_SERVO_DC_PWR_MIN, AXIS5_SERVO_DC_PWR_MAX);
    #elif AXIS5_DRIVER_MODEL == SERVO_TMC2130_DC
      ServoTmc2130DC driver5(5, &DriverPinsAxis5, &DriverSettingsAxis5, AXIS5_SERVO_DC_PWR_MIN, AXIS5_SERVO_DC_PWR_MAX);
    #elif AXIS5_DRIVER_MODEL == SERVO_TMC5160_DC
      ServoTmc5160DC driver5(5, &DriverPinsAxis5, &DriverSettingsAxis5, AXIS5_SERVO_DC_PWR_MIN, AXIS5_SERVO_DC_PWR_MAX);
    #elif AXIS5_DRIVER_MODEL == TMC2209 || AXIS5_DRIVER_MODEL == TMC2226
      ServoTmc2209 driver5(5, &DriverPinsAxis5, &DriverSettingsAxis5, AXIS5_MOTOR_STEPS_PER_MICRON/AXIS5_STEPS_PER_MICRON, AXIS5_DRIVER_MICROSTEPS, AXIS5_DRIVER_IRUN, AXIS5_DRIVER_DECAY, AXIS5_DRIVER_DECAY_GOTO);
    #elif AXIS5_DRIVER_MODEL == SERVO_TMC5160
      ServoTmc5160 driver5(5, &DriverPinsAxis5, &DriverSettingsAxis5, AXIS5_MOTOR_STEPS_PER_MICRON/AXIS5_STEPS_PER_MICRON, AXIS5_DRIVER_MICROSTEPS, AXIS5_DRIVER_IRUN, AXIS5_DRIVER_DECAY, AXIS5_DRIVER_DECAY_GOTO);
    #elif AXIS5_DRIVER_MODEL == SERVO_KTECH
      ServoKTech driver5(5, &DriverSettingsAxis5, AXIS5_MOTOR_STEPS_PER_MICRON/AXIS5_STEPS_PER_MICRON);
    #endif

    ServoMotor motor_5(5, AXIS5_REVERSE, ((ServoDriver*)&driver5), &filterAxis5, &encAxis5, AXIS5_ENCODER_ORIGIN, AXIS5_ENCODER_REVERSE == ON, &feedbackAxis5, &servoControlAxis5, AXIS5_SYNC_THRESHOLD);

  #elif defined(AXIS5_STEP_DIR_PRESENT)
    const StepDirDriverPins DriverPinsAxis5 = {AXIS5_M0_PIN, AXIS5_M1_PIN, AXIS5_M2_PIN, AXIS5_M2_ON_STATE, AXIS5_M3_PIN, AXIS5_DECAY_PIN, AXIS5_FAULT_PIN};
    const StepDirDriverSettings DriverSettingsAxis5 = {AXIS5_DRIVER_MODEL, AXIS5_DRIVER_STATUS, AXIS5_DRIVER_MICROSTEPS, AXIS5_DRIVER_MICROSTEPS_GOTO, AXIS5_DRIVER_DECAY, AXIS5_DRIVER_DECAY_GOTO};
    #if AXIS5_DRIVER_MODEL >= STEP_DIR_DRIVER_FIRST && AXIS5_DRIVER_MODEL < TMC_DRIVER_FIRST
      StepDirGeneric driver5(5, &DriverPinsAxis5, &DriverSettingsAxis5);
    #elif AXIS5_DRIVER_MODEL == TMC2130
      StepDirTmc2130 driver5(5, &DriverPinsAxis5, &DriverSettingsAxis5, AXIS5_DRIVER_IHOLD, AXIS5_DRIVER_IRUN, AXIS5_DRIVER_IGOTO, AXIS5_DRIVER_INTPOL);
    #elif AXIS5_DRIVER_MODEL == TMC2160
      StepDirTmc2160 driver5(5, &DriverPinsAxis5, &DriverSettingsAxis5, AXIS5_DRIVER_IHOLD, AXIS5_DRIVER_IRUN, AXIS5_DRIVER_IGOTO, AXIS5_DRIVER_INTPOL);
    #elif AXIS5_DRIVER_MODEL == TMC2208
      StepDirTmc2208 driver5(5, &DriverPinsAxis5, &DriverSettingsAxis5, AXIS5_DRIVER_IHOLD, AXIS5_DRIVER_IRUN, AXIS5_DRIVER_IGOTO, AXIS5_DRIVER_INTPOL);
    #elif AXIS5_DRIVER_MODEL == TMC2209
      StepDirTmc2209 driver5(5, &DriverPinsAxis5, &DriverSettingsAxis5, AXIS5_DRIVER_IHOLD, AXIS5_DRIVER_IRUN, AXIS5_DRIVER_IGOTO, AXIS5_DRIVER_INTPOL);
    #elif AXIS5_DRIVER_MODEL == TMC2660
      StepDirTmc2660 driver5(5, &DriverPinsAxis5, &DriverSettingsAxis5, AXIS5_DRIVER_IHOLD, AXIS5_DRIVER_IRUN, AXIS5_DRIVER_IGOTO, AXIS5_DRIVER_INTPOL);
    #elif AXIS5_DRIVER_MODEL == TMC5160
      StepDirTmc5160 driver5(5, &DriverPinsAxis5, &DriverSettingsAxis5, AXIS5_DRIVER_IHOLD, AXIS5_DRIVER_IRUN, AXIS5_DRIVER_IGOTO, AXIS5_DRIVER_INTPOL);
    #elif AXIS5_DRIVER_MODEL == TMC5161
      StepDirTmc5161 driver5(5, &DriverPinsAxis5, &DriverSettingsAxis5, AXIS5_DRIVER_IHOLD, AXIS5_DRIVER_IRUN, AXIS5_DRIVER_IGOTO, AXIS5_DRIVER_INTPOL);
    #endif

    const StepDirPins StepDirPinsAxis5 = {AXIS5_STEP_PIN, AXIS5_STEP_STATE, AXIS5_DIR_PIN, AXIS5_ENABLE_PIN, AXIS5_ENABLE_STATE};
    StepDirMotor motor_5(5, AXIS5_REVERSE, &StepDirPinsAxis5, ((StepDirDriver*)&driver5));
  #else
    #error "Configuration (Config.h): Focuser has an unrecognized AXIS5_DRIVER_MODEL this should never happen!"
  #endif

  const AxisPins PinsAxis5 = {AXIS5_SENSE_LIMIT_MIN_PIN, AXIS5_SENSE_HOME_PIN, AXIS5_SENSE_LIMIT_MAX_PIN, {AXIS5_SENSE_HOME, AXIS5_SENSE_HOME_INIT, AXIS5_SENSE_HOME_DIST_LIMIT*1000.0F, AXIS5_SENSE_LIMIT_MIN, AXIS5_SENSE_LIMIT_MAX, AXIS5_SENSE_LIMIT_INIT}};
  const AxisSettings SettingsAxis5 = {AXIS5_STEPS_PER_MICRON, {AXIS5_LIMIT_MIN*1000.0F, AXIS5_LIMIT_MAX*1000.0F}, AXIS5_BACKLASH_RATE};
  }

  Motor& motor5 = motor_5;

  Axis axis5(5, &PinsAxis5, &SettingsAxis5, AXIS_MEASURE_MICRONS);
#endif
