//--------------------------------------------------------------------------------------------------
// telescope focuser control, axis6

#include "Focuser.h"

// note there is a prototype file that should be copy/pasted to build files
// for axes 4 to 9 by replacing all occurances of '@' with the axis number

#if FOCUSER_PRESENT

  namespace {

  #if defined(AXIS6_KTECH_PRESENT)
    const KTechDriverSettings DriverSettingsAxis6 = {AXIS6_DRIVER_MODEL, AXIS6_DRIVER_STATUS};
    KTechMotor motor_6(6, AXIS6_REVERSE, &DriverSettingsAxis6);

  #elif defined(AXIS6_MKS42D_PRESENT)
    const MksDriverSettings DriverSettingsAxis6 = {AXIS6_DRIVER_MODEL, AXIS6_DRIVER_STATUS};
    Mks42DMotor motor_6(6, AXIS6_REVERSE, &DriverSettingsAxis6, AXIS6_STEPS_PER_DEGREE);

  #elif defined(AXIS6_SERVO_PRESENT)
    ServoControl servoControlAxis6;

    #if AXIS6_ENCODER == AB
      Quadrature encAxis6(6, AXIS6_ENCODER_A_PIN, AXIS6_ENCODER_B_PIN);
    #elif AXIS6_ENCODER == AB_ESP32
      QuadratureEsp32 encAxis6(6, AXIS6_ENCODER_A_PIN, AXIS6_ENCODER_B_PIN);
    #elif AXIS6_ENCODER == CW_CCW
      CwCcw encAxis6(6, AXIS6_ENCODER_A_PIN, AXIS6_ENCODER_B_PIN);
    #elif AXIS6_ENCODER == PULSE_DIR
      PulseDir encAxis6(6, AXIS6_ENCODER_A_PIN, AXIS6_ENCODER_B_PIN);
    #elif AXIS6_ENCODER == PULSE_ONLY
      PulseOnly encAxis6(6, AXIS6_ENCODER_A_PIN, &servoControlAxis6.directionHint);
    #elif AXIS6_ENCODER == VIRTUAL
      VirtualEnc encAxis6(6);
    #elif AXIS6_ENCODER == SERIAL_BRIDGE
      SerialBridge encAxis6(6);
    #endif

    #if AXIS6_SERVO_FEEDBACK == PID
      Pid feedbackAxis6(AXIS6_PID_P, AXIS6_PID_I, AXIS6_PID_D);
    #elif AXIS6_SERVO_FEEDBACK == DUAL_PID
      DualPid feedbackAxis6(AXIS6_PID_P, AXIS6_PID_I, AXIS6_PID_D, AXIS6_PID_P_GOTO, AXIS6_PID_I_GOTO, AXIS6_PID_D_GOTO, AXIS6_PID_SENSITIVITY);
    #endif

    #if AXIS6_SERVO_FLTR == KALMAN
      KalmanFilter filterAxis6(AXIS6_SERVO_FLTR_MEAS_U, AXIS6_SERVO_FLTR_VARIANCE);
    #elif AXIS6_SERVO_FLTR == ROLLING
      RollingFilter filterAxis6(AXIS6_SERVO_FLTR_WSIZE);
    #elif AXIS6_SERVO_FLTR == WINDOWING
      WindowingFilter filterAxis6(AXIS6_SERVO_FLTR_WSIZE);
    #elif AXIS6_SERVO_FLTR == OFF
      Filter filterAxis6;
    #endif

    const ServoPins DriverPinsAxis6 = {AXIS6_SERVO_PH1_PIN, AXIS6_SERVO_PH1_STATE, AXIS6_SERVO_PH2_PIN, AXIS6_SERVO_PH2_STATE, AXIS6_ENABLE_PIN, AXIS6_ENABLE_STATE, AXIS6_M0_PIN, AXIS6_M1_PIN, AXIS6_M2_PIN, AXIS6_M3_PIN, AXIS6_FAULT_PIN};
    const ServoSettings DriverSettingsAxis6 = {AXIS6_DRIVER_MODEL, AXIS6_DRIVER_STATUS, AXIS6_SERVO_ACCELERATION};

    #if AXIS6_DRIVER_MODEL == SERVO_EE
      ServoEE driver6(6, &DriverPinsAxis6, &DriverSettingsAxis6, AXIS6_SERVO_DC_PWR_MIN, AXIS6_SERVO_DC_PWR_MAX);
    #elif AXIS6_DRIVER_MODEL == SERVO_PE
      ServoPE driver6(6, &DriverPinsAxis6, &DriverSettingsAxis6, AXIS6_SERVO_DC_PWR_MIN, AXIS6_SERVO_DC_PWR_MAX);
    #elif AXIS6_DRIVER_MODEL == SERVO_TMC2130_DC
      ServoTmc2130DC driver6(6, &DriverPinsAxis6, &DriverSettingsAxis6, AXIS6_SERVO_DC_PWR_MIN, AXIS6_SERVO_DC_PWR_MAX);
    #elif AXIS6_DRIVER_MODEL == SERVO_TMC5160_DC
      ServoTmc5160DC driver6(6, &DriverPinsAxis6, &DriverSettingsAxis6, AXIS6_SERVO_DC_PWR_MIN, AXIS6_SERVO_DC_PWR_MAX);
    #elif AXIS6_DRIVER_MODEL == TMC2209 || AXIS6_DRIVER_MODEL == TMC2226
      ServoTmc2209 driver6(6, &DriverPinsAxis6, &DriverSettingsAxis6, AXIS6_MOTOR_STEPS_PER_MICRON/AXIS6_STEPS_PER_MICRON, AXIS6_DRIVER_MICROSTEPS, AXIS6_DRIVER_IRUN, AXIS6_DRIVER_DECAY, AXIS6_DRIVER_DECAY_GOTO);
    #elif AXIS6_DRIVER_MODEL == SERVO_TMC5160
      ServoTmc5160 driver6(6, &DriverPinsAxis6, &DriverSettingsAxis6, AXIS6_MOTOR_STEPS_PER_MICRON/AXIS6_STEPS_PER_MICRON, AXIS6_DRIVER_MICROSTEPS, AXIS6_DRIVER_IRUN, AXIS6_DRIVER_DECAY, AXIS6_DRIVER_DECAY_GOTO);
    #elif AXIS6_DRIVER_MODEL == SERVO_KTECH
      ServoKTech driver6(6, &DriverSettingsAxis6, AXIS6_MOTOR_STEPS_PER_MICRON/AXIS6_STEPS_PER_MICRON);
    #endif

    ServoMotor motor_6(6, AXIS6_REVERSE, ((ServoDriver*)&driver6), &filterAxis6, &encAxis6, AXIS6_ENCODER_ORIGIN, AXIS6_ENCODER_REVERSE == ON, &feedbackAxis6, &servoControlAxis6, AXIS6_SYNC_THRESHOLD);

  #elif defined(AXIS6_STEP_DIR_PRESENT)
    const StepDirDriverPins DriverPinsAxis6 = {AXIS6_M0_PIN, AXIS6_M1_PIN, AXIS6_M2_PIN, AXIS6_M2_ON_STATE, AXIS6_M3_PIN, AXIS6_DECAY_PIN, AXIS6_FAULT_PIN};
    const StepDirDriverSettings DriverSettingsAxis6 = {AXIS6_DRIVER_MODEL, AXIS6_DRIVER_STATUS, AXIS6_DRIVER_MICROSTEPS, AXIS6_DRIVER_MICROSTEPS_GOTO, AXIS6_DRIVER_DECAY, AXIS6_DRIVER_DECAY_GOTO};
    #if AXIS6_DRIVER_MODEL >= STEP_DIR_DRIVER_FIRST && AXIS6_DRIVER_MODEL < TMC_DRIVER_FIRST
      StepDirGeneric driver6(6, &DriverPinsAxis6, &DriverSettingsAxis6);
    #elif AXIS6_DRIVER_MODEL == TMC2130
      StepDirTmc2130 driver6(6, &DriverPinsAxis6, &DriverSettingsAxis6, AXIS6_DRIVER_IHOLD, AXIS6_DRIVER_IRUN, AXIS6_DRIVER_IGOTO, AXIS6_DRIVER_INTPOL);
    #elif AXIS6_DRIVER_MODEL == TMC2160
      StepDirTmc2160 driver6(6, &DriverPinsAxis6, &DriverSettingsAxis6, AXIS6_DRIVER_IHOLD, AXIS6_DRIVER_IRUN, AXIS6_DRIVER_IGOTO, AXIS6_DRIVER_INTPOL);
    #elif AXIS6_DRIVER_MODEL == TMC2208
      StepDirTmc2208 driver6(6, &DriverPinsAxis6, &DriverSettingsAxis6, AXIS6_DRIVER_IHOLD, AXIS6_DRIVER_IRUN, AXIS6_DRIVER_IGOTO, AXIS6_DRIVER_INTPOL);
    #elif AXIS6_DRIVER_MODEL == TMC2209
      StepDirTmc2209 driver6(6, &DriverPinsAxis6, &DriverSettingsAxis6, AXIS6_DRIVER_IHOLD, AXIS6_DRIVER_IRUN, AXIS6_DRIVER_IGOTO, AXIS6_DRIVER_INTPOL);
    #elif AXIS6_DRIVER_MODEL == TMC2660
      StepDirTmc2660 driver6(6, &DriverPinsAxis6, &DriverSettingsAxis6, AXIS6_DRIVER_IHOLD, AXIS6_DRIVER_IRUN, AXIS6_DRIVER_IGOTO, AXIS6_DRIVER_INTPOL);
    #elif AXIS6_DRIVER_MODEL == TMC5160
      StepDirTmc5160 driver6(6, &DriverPinsAxis6, &DriverSettingsAxis6, AXIS6_DRIVER_IHOLD, AXIS6_DRIVER_IRUN, AXIS6_DRIVER_IGOTO, AXIS6_DRIVER_INTPOL);
    #elif AXIS6_DRIVER_MODEL == TMC5161
      StepDirTmc5161 driver6(6, &DriverPinsAxis6, &DriverSettingsAxis6, AXIS6_DRIVER_IHOLD, AXIS6_DRIVER_IRUN, AXIS6_DRIVER_IGOTO, AXIS6_DRIVER_INTPOL);
    #endif

    const StepDirPins StepDirPinsAxis6 = {AXIS6_STEP_PIN, AXIS6_STEP_STATE, AXIS6_DIR_PIN, AXIS6_ENABLE_PIN, AXIS6_ENABLE_STATE};
    StepDirMotor motor_6(6, AXIS6_REVERSE, &StepDirPinsAxis6, ((StepDirDriver*)&driver6));
  #else
    #error "Configuration (Config.h): Focuser has an unrecognized AXIS6_DRIVER_MODEL this should never happen!"
  #endif

  const AxisPins PinsAxis6 = {AXIS6_SENSE_LIMIT_MIN_PIN, AXIS6_SENSE_HOME_PIN, AXIS6_SENSE_LIMIT_MAX_PIN, {AXIS6_SENSE_HOME, AXIS6_SENSE_HOME_INIT, AXIS6_SENSE_HOME_DIST_LIMIT*1000.0F, AXIS6_SENSE_LIMIT_MIN, AXIS6_SENSE_LIMIT_MAX, AXIS6_SENSE_LIMIT_INIT}};
  const AxisSettings SettingsAxis6 = {AXIS6_STEPS_PER_MICRON, {AXIS6_LIMIT_MIN*1000.0F, AXIS6_LIMIT_MAX*1000.0F}, AXIS6_BACKLASH_RATE};
  }

  Motor& motor6 = motor_6;

  Axis axis6(6, &PinsAxis6, &SettingsAxis6, AXIS_MEASURE_MICRONS);
#endif
