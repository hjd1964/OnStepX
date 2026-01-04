//--------------------------------------------------------------------------------------------------
// telescope focuser control, axis8

#include "Focuser.h"

// note there is a prototype file that should be copy/pasted to build files
// for axes 4 to 9 by replacing all occurances of '@' with the axis number

#if FOCUSER_PRESENT

  namespace {

  #if defined(AXIS8_KTECH_PRESENT)
    const KTechDriverSettings DriverSettingsAxis8 = {AXIS8_DRIVER_MODEL, AXIS8_DRIVER_STATUS};
    KTechMotor motor_8(8, AXIS8_REVERSE, &DriverSettingsAxis8);

  #elif defined(AXIS8_MKS42D_PRESENT)
    const MksDriverSettings DriverSettingsAxis8 = {AXIS8_DRIVER_MODEL, AXIS8_DRIVER_STATUS};
    Mks42DMotor motor_8(8, AXIS8_REVERSE, &DriverSettingsAxis8, AXIS8_STEPS_PER_DEGREE);

  #elif defined(AXIS8_SERVO_PRESENT)
    ServoControl servoControlAxis8;

    #if AXIS8_ENCODER == AB
      Quadrature encAxis8(8, AXIS8_ENCODER_A_PIN, AXIS8_ENCODER_B_PIN);
    #elif AXIS8_ENCODER == AB_ESP32
      QuadratureEsp32 encAxis8(8, AXIS8_ENCODER_A_PIN, AXIS8_ENCODER_B_PIN);
    #elif AXIS8_ENCODER == CW_CCW
      CwCcw encAxis8(8, AXIS8_ENCODER_A_PIN, AXIS8_ENCODER_B_PIN);
    #elif AXIS8_ENCODER == PULSE_DIR
      PulseDir encAxis8(8, AXIS8_ENCODER_A_PIN, AXIS8_ENCODER_B_PIN);
    #elif AXIS8_ENCODER == PULSE_ONLY
      PulseOnly encAxis8(8, AXIS8_ENCODER_A_PIN, &servoControlAxis8.directionHint);
    #elif AXIS8_ENCODER == VIRTUAL
      VirtualEnc encAxis8(8);
    #elif AXIS8_ENCODER == SERIAL_BRIDGE
      SerialBridge encAxis8(8);
    #endif

    #if AXIS8_SERVO_FEEDBACK == PID
      Pid feedbackAxis8(AXIS8_PID_P, AXIS8_PID_I, AXIS8_PID_D);
    #elif AXIS8_SERVO_FEEDBACK == DUAL_PID
      DualPid feedbackAxis8(AXIS8_PID_P, AXIS8_PID_I, AXIS8_PID_D, AXIS8_PID_P_GOTO, AXIS8_PID_I_GOTO, AXIS8_PID_D_GOTO, AXIS8_PID_SENSITIVITY);
    #endif

    #if AXIS8_SERVO_FLTR == KALMAN
      KalmanFilter filterAxis8(AXIS8_SERVO_FLTR_MEAS_U, AXIS8_SERVO_FLTR_VARIANCE);
    #elif AXIS8_SERVO_FLTR == ROLLING
      RollingFilter filterAxis8(AXIS8_SERVO_FLTR_WSIZE);
    #elif AXIS8_SERVO_FLTR == WINDOWING
      WindowingFilter filterAxis8(AXIS8_SERVO_FLTR_WSIZE);
    #elif AXIS8_SERVO_FLTR == OFF
      Filter filterAxis8;
    #endif

    const ServoPins DriverPinsAxis8 = {AXIS8_SERVO_PH1_PIN, AXIS8_SERVO_PH1_STATE, AXIS8_SERVO_PH2_PIN, AXIS8_SERVO_PH2_STATE, AXIS8_ENABLE_PIN, AXIS8_ENABLE_STATE, AXIS8_M0_PIN, AXIS8_M1_PIN, AXIS8_M2_PIN, AXIS8_M3_PIN, AXIS8_FAULT_PIN};
    const ServoSettings DriverSettingsAxis8 = {AXIS8_DRIVER_MODEL, AXIS8_DRIVER_STATUS, AXIS8_SERVO_ACCELERATION};

    #if AXIS8_DRIVER_MODEL == SERVO_EE
      ServoEE driver8(8, &DriverPinsAxis8, &DriverSettingsAxis8, AXIS8_SERVO_DC_PWR_MIN, AXIS8_SERVO_DC_PWR_MAX);
    #elif AXIS8_DRIVER_MODEL == SERVO_PE
      ServoPE driver8(8, &DriverPinsAxis8, &DriverSettingsAxis8, AXIS8_SERVO_DC_PWR_MIN, AXIS8_SERVO_DC_PWR_MAX);
    #elif AXIS8_DRIVER_MODEL == SERVO_TMC2130_DC
      ServoTmc2130DC driver8(8, &DriverPinsAxis8, &DriverSettingsAxis8, AXIS8_SERVO_DC_PWR_MIN, AXIS8_SERVO_DC_PWR_MAX);
    #elif AXIS8_DRIVER_MODEL == SERVO_TMC5160_DC
      ServoTmc5160DC driver8(8, &DriverPinsAxis8, &DriverSettingsAxis8, AXIS8_SERVO_DC_PWR_MIN, AXIS8_SERVO_DC_PWR_MAX);
    #elif AXIS8_DRIVER_MODEL == TMC2209 || AXIS8_DRIVER_MODEL == TMC2226
      ServoTmc2209 driver8(8, &DriverPinsAxis8, &DriverSettingsAxis8, AXIS8_MOTOR_STEPS_PER_MICRON/AXIS8_STEPS_PER_MICRON, AXIS8_DRIVER_MICROSTEPS, AXIS8_DRIVER_IRUN, AXIS8_DRIVER_DECAY, AXIS8_DRIVER_DECAY_GOTO);
    #elif AXIS8_DRIVER_MODEL == SERVO_TMC5160
      ServoTmc5160 driver8(8, &DriverPinsAxis8, &DriverSettingsAxis8, AXIS8_MOTOR_STEPS_PER_MICRON/AXIS8_STEPS_PER_MICRON, AXIS8_DRIVER_MICROSTEPS, AXIS8_DRIVER_IRUN, AXIS8_DRIVER_DECAY, AXIS8_DRIVER_DECAY_GOTO);
    #elif AXIS8_DRIVER_MODEL == SERVO_KTECH
      ServoKTech driver8(8, &DriverSettingsAxis8, AXIS8_MOTOR_STEPS_PER_MICRON/AXIS8_STEPS_PER_MICRON);
    #endif

    ServoMotor motor_8(8, AXIS8_REVERSE, ((ServoDriver*)&driver8), &filterAxis8, &encAxis8, AXIS8_ENCODER_ORIGIN, AXIS8_ENCODER_REVERSE == ON, &feedbackAxis8, &servoControlAxis8, AXIS8_SYNC_THRESHOLD);

  #elif defined(AXIS8_STEP_DIR_PRESENT)
    const StepDirDriverPins DriverPinsAxis8 = {AXIS8_M0_PIN, AXIS8_M1_PIN, AXIS8_M2_PIN, AXIS8_M2_ON_STATE, AXIS8_M3_PIN, AXIS8_DECAY_PIN, AXIS8_FAULT_PIN};
    const StepDirDriverSettings DriverSettingsAxis8 = {AXIS8_DRIVER_MODEL, AXIS8_DRIVER_STATUS, AXIS8_DRIVER_MICROSTEPS, AXIS8_DRIVER_MICROSTEPS_GOTO, AXIS8_DRIVER_DECAY, AXIS8_DRIVER_DECAY_GOTO};
    #if AXIS8_DRIVER_MODEL >= STEP_DIR_DRIVER_FIRST && AXIS8_DRIVER_MODEL < TMC_DRIVER_FIRST
      StepDirGeneric driver8(8, &DriverPinsAxis8, &DriverSettingsAxis8);
    #elif AXIS8_DRIVER_MODEL == TMC2130
      StepDirTmc2130 driver8(8, &DriverPinsAxis8, &DriverSettingsAxis8, AXIS8_DRIVER_IHOLD, AXIS8_DRIVER_IRUN, AXIS8_DRIVER_IGOTO, AXIS8_DRIVER_INTPOL);
    #elif AXIS8_DRIVER_MODEL == TMC2160
      StepDirTmc2160 driver8(8, &DriverPinsAxis8, &DriverSettingsAxis8, AXIS8_DRIVER_IHOLD, AXIS8_DRIVER_IRUN, AXIS8_DRIVER_IGOTO, AXIS8_DRIVER_INTPOL);
    #elif AXIS8_DRIVER_MODEL == TMC2208
      StepDirTmc2208 driver8(8, &DriverPinsAxis8, &DriverSettingsAxis8, AXIS8_DRIVER_IHOLD, AXIS8_DRIVER_IRUN, AXIS8_DRIVER_IGOTO, AXIS8_DRIVER_INTPOL);
    #elif AXIS8_DRIVER_MODEL == TMC2209
      StepDirTmc2209 driver8(8, &DriverPinsAxis8, &DriverSettingsAxis8, AXIS8_DRIVER_IHOLD, AXIS8_DRIVER_IRUN, AXIS8_DRIVER_IGOTO, AXIS8_DRIVER_INTPOL);
    #elif AXIS8_DRIVER_MODEL == TMC2660
      StepDirTmc2660 driver8(8, &DriverPinsAxis8, &DriverSettingsAxis8, AXIS8_DRIVER_IHOLD, AXIS8_DRIVER_IRUN, AXIS8_DRIVER_IGOTO, AXIS8_DRIVER_INTPOL);
    #elif AXIS8_DRIVER_MODEL == TMC5160
      StepDirTmc5160 driver8(8, &DriverPinsAxis8, &DriverSettingsAxis8, AXIS8_DRIVER_IHOLD, AXIS8_DRIVER_IRUN, AXIS8_DRIVER_IGOTO, AXIS8_DRIVER_INTPOL);
    #elif AXIS8_DRIVER_MODEL == TMC5161
      StepDirTmc5161 driver8(8, &DriverPinsAxis8, &DriverSettingsAxis8, AXIS8_DRIVER_IHOLD, AXIS8_DRIVER_IRUN, AXIS8_DRIVER_IGOTO, AXIS8_DRIVER_INTPOL);
    #endif

    const StepDirPins StepDirPinsAxis8 = {AXIS8_STEP_PIN, AXIS8_STEP_STATE, AXIS8_DIR_PIN, AXIS8_ENABLE_PIN, AXIS8_ENABLE_STATE};
    StepDirMotor motor_8(8, AXIS8_REVERSE, &StepDirPinsAxis8, ((StepDirDriver*)&driver8));
  #else
    #error "Configuration (Config.h): Focuser has an unrecognized AXIS8_DRIVER_MODEL this should never happen!"
  #endif

  const AxisPins PinsAxis8 = {AXIS8_SENSE_LIMIT_MIN_PIN, AXIS8_SENSE_HOME_PIN, AXIS8_SENSE_LIMIT_MAX_PIN, {AXIS8_SENSE_HOME, AXIS8_SENSE_HOME_INIT, AXIS8_SENSE_HOME_DIST_LIMIT*1000.0F, AXIS8_SENSE_LIMIT_MIN, AXIS8_SENSE_LIMIT_MAX, AXIS8_SENSE_LIMIT_INIT}};
  const AxisSettings SettingsAxis8 = {AXIS8_STEPS_PER_MICRON, {AXIS8_LIMIT_MIN*1000.0F, AXIS8_LIMIT_MAX*1000.0F}, AXIS8_BACKLASH_RATE};
  }

  Motor& motor8 = motor_8;

  Axis axis8(8, &PinsAxis8, &SettingsAxis8, AXIS_MEASURE_MICRONS);
#endif
