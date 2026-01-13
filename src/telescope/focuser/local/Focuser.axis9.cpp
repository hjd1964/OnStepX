//--------------------------------------------------------------------------------------------------
// telescope focuser control, axis9

#include "Focuser.h"

// note there is a prototype file that should be copy/pasted to build files
// for axes 4 to 9 by replacing all occurances of '@' with the axis number

#if AXIS9_DRIVER_MODEL != OFF

  namespace {

  #if defined(AXIS9_KTECH_PRESENT)
    const KTechDriverSettings DriverSettingsAxis9 = {AXIS9_DRIVER_MODEL, AXIS9_DRIVER_STATUS};
    KTechMotor motor_9(9, AXIS9_REVERSE, &DriverSettingsAxis9);

  #elif defined(AXIS9_MKS42D_PRESENT)
    const MksDriverSettings DriverSettingsAxis9 = {AXIS9_DRIVER_MODEL, AXIS9_DRIVER_STATUS};
    Mks42DMotor motor_9(9, AXIS9_REVERSE, &DriverSettingsAxis9, AXIS9_STEPS_PER_DEGREE);

  #elif defined(AXIS9_SERVO_PRESENT)
    ServoControl servoControlAxis9;

    #if AXIS9_ENCODER == AB
      Quadrature encAxis9(9, AXIS9_ENCODER_A_PIN, AXIS9_ENCODER_B_PIN);
    #elif AXIS9_ENCODER == AB_ESP32
      QuadratureEsp32 encAxis9(9, AXIS9_ENCODER_A_PIN, AXIS9_ENCODER_B_PIN);
    #elif AXIS9_ENCODER == CW_CCW
      CwCcw encAxis9(9, AXIS9_ENCODER_A_PIN, AXIS9_ENCODER_B_PIN);
    #elif AXIS9_ENCODER == PULSE_DIR
      PulseDir encAxis9(9, AXIS9_ENCODER_A_PIN, AXIS9_ENCODER_B_PIN);
    #elif AXIS9_ENCODER == PULSE_ONLY
      PulseOnly encAxis9(9, AXIS9_ENCODER_A_PIN, &servoControlAxis9.directionHint);
    #elif AXIS9_ENCODER == VIRTUAL
      VirtualEnc encAxis9(9);
    #elif AXIS9_ENCODER == SERIAL_BRIDGE
      SerialBridge encAxis9(9);
    #endif

    #if AXIS9_SERVO_FEEDBACK == PID
      Pid feedbackAxis9(AXIS9_PID_P, AXIS9_PID_I, AXIS9_PID_D);
    #elif AXIS9_SERVO_FEEDBACK == DUAL_PID
      DualPid feedbackAxis9(AXIS9_PID_P, AXIS9_PID_I, AXIS9_PID_D, AXIS9_PID_P_GOTO, AXIS9_PID_I_GOTO, AXIS9_PID_D_GOTO, AXIS9_PID_SENSITIVITY);
    #endif

    #if AXIS9_SERVO_FLTR == KALMAN
      KalmanFilter filterAxis9(AXIS9_SERVO_FLTR_MEAS_U, AXIS9_SERVO_FLTR_VARIANCE);
    #elif AXIS9_SERVO_FLTR == ROLLING
      RollingFilter filterAxis9(AXIS9_SERVO_FLTR_WSIZE);
    #elif AXIS9_SERVO_FLTR == WINDOWING
      WindowingFilter filterAxis9(AXIS9_SERVO_FLTR_WSIZE);
    #elif AXIS9_SERVO_FLTR == OFF
      Filter filterAxis9;
    #endif

    const ServoPins DriverPinsAxis9 = {AXIS9_SERVO_PH1_PIN, AXIS9_SERVO_PH1_STATE, AXIS9_SERVO_PH2_PIN, AXIS9_SERVO_PH2_STATE, AXIS9_ENABLE_PIN, AXIS9_ENABLE_STATE, AXIS9_M0_PIN, AXIS9_M1_PIN, AXIS9_M2_PIN, AXIS9_M3_PIN, AXIS9_FAULT_PIN};
    const ServoSettings DriverSettingsAxis9 = {AXIS9_DRIVER_MODEL, AXIS9_DRIVER_STATUS, AXIS9_SERVO_ACCELERATION};

    #if AXIS9_DRIVER_MODEL == SERVO_EE
      ServoEE driver9(9, &DriverPinsAxis9, &DriverSettingsAxis9, AXIS9_SERVO_DC_PWR_MIN, AXIS9_SERVO_DC_PWR_MAX);
    #elif AXIS9_DRIVER_MODEL == SERVO_PE
      ServoPE driver9(9, &DriverPinsAxis9, &DriverSettingsAxis9, AXIS9_SERVO_DC_PWR_MIN, AXIS9_SERVO_DC_PWR_MAX);
    #elif AXIS9_DRIVER_MODEL == SERVO_TMC2130_DC
      ServoTmc2130DC driver9(9, &DriverPinsAxis9, &DriverSettingsAxis9, AXIS9_SERVO_DC_PWR_MIN, AXIS9_SERVO_DC_PWR_MAX);
    #elif AXIS9_DRIVER_MODEL == SERVO_TMC5160_DC
      ServoTmc5160DC driver9(9, &DriverPinsAxis9, &DriverSettingsAxis9, AXIS9_SERVO_DC_PWR_MIN, AXIS9_SERVO_DC_PWR_MAX);
    #elif AXIS9_DRIVER_MODEL == TMC2209 || AXIS9_DRIVER_MODEL == TMC2226
      ServoTmc2209 driver9(9, &DriverPinsAxis9, &DriverSettingsAxis9, AXIS9_MOTOR_STEPS_PER_MICRON/AXIS9_STEPS_PER_MICRON, AXIS9_DRIVER_MICROSTEPS, AXIS9_DRIVER_IRUN, AXIS9_DRIVER_DECAY, AXIS9_DRIVER_DECAY_GOTO);
    #elif AXIS9_DRIVER_MODEL == SERVO_TMC5160
      ServoTmc5160 driver9(9, &DriverPinsAxis9, &DriverSettingsAxis9, AXIS9_MOTOR_STEPS_PER_MICRON/AXIS9_STEPS_PER_MICRON, AXIS9_DRIVER_MICROSTEPS, AXIS9_DRIVER_IRUN, AXIS9_DRIVER_DECAY, AXIS9_DRIVER_DECAY_GOTO);
    #elif AXIS9_DRIVER_MODEL == SERVO_KTECH
      ServoKTech driver9(9, &DriverSettingsAxis9, AXIS9_MOTOR_STEPS_PER_MICRON/AXIS9_STEPS_PER_MICRON);
    #endif

    ServoMotor motor_9(9, AXIS9_REVERSE, ((ServoDriver*)&driver9), &filterAxis9, &encAxis9, AXIS9_ENCODER_ORIGIN, AXIS9_ENCODER_REVERSE == ON, &feedbackAxis9, &servoControlAxis9, AXIS9_SYNC_THRESHOLD);

  #elif defined(AXIS9_STEP_DIR_PRESENT)
    const StepDirDriverPins DriverPinsAxis9 = {AXIS9_M0_PIN, AXIS9_M1_PIN, AXIS9_M2_PIN, AXIS9_M2_ON_STATE, AXIS9_M3_PIN, AXIS9_DECAY_PIN, AXIS9_FAULT_PIN};
    const StepDirDriverSettings DriverSettingsAxis9 = {AXIS9_DRIVER_MODEL, AXIS9_DRIVER_STATUS, AXIS9_DRIVER_MICROSTEPS, AXIS9_DRIVER_MICROSTEPS_GOTO, AXIS9_DRIVER_DECAY, AXIS9_DRIVER_DECAY_GOTO};
    #if AXIS9_DRIVER_MODEL >= STEP_DIR_DRIVER_FIRST && AXIS9_DRIVER_MODEL < TMC_DRIVER_FIRST
      StepDirGeneric driver9(9, &DriverPinsAxis9, &DriverSettingsAxis9);
    #elif AXIS9_DRIVER_MODEL == TMC2130
      StepDirTmc2130 driver9(9, &DriverPinsAxis9, &DriverSettingsAxis9, AXIS9_DRIVER_IHOLD, AXIS9_DRIVER_IRUN, AXIS9_DRIVER_IGOTO, AXIS9_DRIVER_INTPOL);
    #elif AXIS9_DRIVER_MODEL == TMC2160
      StepDirTmc2160 driver9(9, &DriverPinsAxis9, &DriverSettingsAxis9, AXIS9_DRIVER_IHOLD, AXIS9_DRIVER_IRUN, AXIS9_DRIVER_IGOTO, AXIS9_DRIVER_INTPOL);
    #elif AXIS9_DRIVER_MODEL == TMC2208
      StepDirTmc2208 driver9(9, &DriverPinsAxis9, &DriverSettingsAxis9, AXIS9_DRIVER_IHOLD, AXIS9_DRIVER_IRUN, AXIS9_DRIVER_IGOTO, AXIS9_DRIVER_INTPOL);
    #elif AXIS9_DRIVER_MODEL == TMC2209 || AXIS9_DRIVER_MODEL == TMC2226
      StepDirTmc2209 driver9(9, &DriverPinsAxis9, &DriverSettingsAxis9, AXIS9_DRIVER_IHOLD, AXIS9_DRIVER_IRUN, AXIS9_DRIVER_IGOTO, AXIS9_DRIVER_INTPOL);
    #elif AXIS9_DRIVER_MODEL == TMC2660
      StepDirTmc2660 driver9(9, &DriverPinsAxis9, &DriverSettingsAxis9, AXIS9_DRIVER_IHOLD, AXIS9_DRIVER_IRUN, AXIS9_DRIVER_IGOTO, AXIS9_DRIVER_INTPOL);
    #elif AXIS9_DRIVER_MODEL == TMC5160
      StepDirTmc5160 driver9(9, &DriverPinsAxis9, &DriverSettingsAxis9, AXIS9_DRIVER_IHOLD, AXIS9_DRIVER_IRUN, AXIS9_DRIVER_IGOTO, AXIS9_DRIVER_INTPOL);
    #elif AXIS9_DRIVER_MODEL == TMC5161
      StepDirTmc5161 driver9(9, &DriverPinsAxis9, &DriverSettingsAxis9, AXIS9_DRIVER_IHOLD, AXIS9_DRIVER_IRUN, AXIS9_DRIVER_IGOTO, AXIS9_DRIVER_INTPOL);
    #endif

    const StepDirPins StepDirPinsAxis9 = {AXIS9_STEP_PIN, AXIS9_STEP_STATE, AXIS9_DIR_PIN, AXIS9_ENABLE_PIN, AXIS9_ENABLE_STATE};
    StepDirMotor motor_9(9, AXIS9_REVERSE, &StepDirPinsAxis9, ((StepDirDriver*)&driver9));
  #else
    #error "Configuration (Config.h): Focuser has an unrecognized AXIS9_DRIVER_MODEL this should never happen!"
  #endif

  const AxisPins PinsAxis9 = {AXIS9_SENSE_LIMIT_MIN_PIN, AXIS9_SENSE_HOME_PIN, AXIS9_SENSE_LIMIT_MAX_PIN, {AXIS9_SENSE_HOME, AXIS9_SENSE_HOME_INIT, AXIS9_SENSE_HOME_DIST_LIMIT*1000.0F, AXIS9_SENSE_LIMIT_MIN, AXIS9_SENSE_LIMIT_MAX, AXIS9_SENSE_LIMIT_INIT}};
  const AxisSettings SettingsAxis9 = {AXIS9_STEPS_PER_MICRON, {AXIS9_LIMIT_MIN*1000.0F, AXIS9_LIMIT_MAX*1000.0F}, AXIS9_BACKLASH_RATE};
  }

  Motor& motor9 = motor_9;

  Axis axis9(9, &PinsAxis9, &SettingsAxis9, AXIS_MEASURE_MICRONS);
#endif
