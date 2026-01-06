//--------------------------------------------------------------------------------------------------
// telescope focuser control, axis@

#include "Focuser.h"

// note there is a prototype file that should be copy/pasted to build files
// for axes 4 to 9 by replacing all occurances of '@' with the axis number

//#if AXIS@_DRIVER_MODEL != OFF
#if false
  namespace {

  #if defined(AXIS@_KTECH_PRESENT)
    const KTechDriverSettings DriverSettingsAxis@ = {AXIS@_DRIVER_MODEL, AXIS@_DRIVER_STATUS};
    KTechMotor motor_@(@, AXIS@_REVERSE, &DriverSettingsAxis@);

  #elif defined(AXIS@_MKS42D_PRESENT)
    const MksDriverSettings DriverSettingsAxis@ = {AXIS@_DRIVER_MODEL, AXIS@_DRIVER_STATUS};
    Mks42DMotor motor_@(@, AXIS@_REVERSE, &DriverSettingsAxis@, AXIS@_STEPS_PER_DEGREE);

  #elif defined(AXIS@_SERVO_PRESENT)
    ServoControl servoControlAxis@;

    #if AXIS@_ENCODER == AB
      Quadrature encAxis@(@, AXIS@_ENCODER_A_PIN, AXIS@_ENCODER_B_PIN);
    #elif AXIS@_ENCODER == AB_ESP32
      QuadratureEsp32 encAxis@(@, AXIS@_ENCODER_A_PIN, AXIS@_ENCODER_B_PIN);
    #elif AXIS@_ENCODER == CW_CCW
      CwCcw encAxis@(@, AXIS@_ENCODER_A_PIN, AXIS@_ENCODER_B_PIN);
    #elif AXIS@_ENCODER == PULSE_DIR
      PulseDir encAxis@(@, AXIS@_ENCODER_A_PIN, AXIS@_ENCODER_B_PIN);
    #elif AXIS@_ENCODER == PULSE_ONLY
      PulseOnly encAxis@(@, AXIS@_ENCODER_A_PIN, &servoControlAxis@.directionHint);
    #elif AXIS@_ENCODER == VIRTUAL
      VirtualEnc encAxis@(@);
    #elif AXIS@_ENCODER == SERIAL_BRIDGE
      SerialBridge encAxis@(@);
    #endif

    #if AXIS@_SERVO_FEEDBACK == PID
      Pid feedbackAxis@(AXIS@_PID_P, AXIS@_PID_I, AXIS@_PID_D);
    #elif AXIS@_SERVO_FEEDBACK == DUAL_PID
      DualPid feedbackAxis@(AXIS@_PID_P, AXIS@_PID_I, AXIS@_PID_D, AXIS@_PID_P_GOTO, AXIS@_PID_I_GOTO, AXIS@_PID_D_GOTO, AXIS@_PID_SENSITIVITY);
    #endif

    #if AXIS@_SERVO_FLTR == KALMAN
      KalmanFilter filterAxis@(AXIS@_SERVO_FLTR_MEAS_U, AXIS@_SERVO_FLTR_VARIANCE);
    #elif AXIS@_SERVO_FLTR == ROLLING
      RollingFilter filterAxis@(AXIS@_SERVO_FLTR_WSIZE);
    #elif AXIS@_SERVO_FLTR == WINDOWING
      WindowingFilter filterAxis@(AXIS@_SERVO_FLTR_WSIZE);
    #elif AXIS@_SERVO_FLTR == OFF
      Filter filterAxis@;
    #endif

    const ServoPins DriverPinsAxis@ = {AXIS@_SERVO_PH1_PIN, AXIS@_SERVO_PH1_STATE, AXIS@_SERVO_PH2_PIN, AXIS@_SERVO_PH2_STATE, AXIS@_ENABLE_PIN, AXIS@_ENABLE_STATE, AXIS@_M0_PIN, AXIS@_M1_PIN, AXIS@_M2_PIN, AXIS@_M3_PIN, AXIS@_FAULT_PIN};
    const ServoSettings DriverSettingsAxis@ = {AXIS@_DRIVER_MODEL, AXIS@_DRIVER_STATUS, AXIS@_SERVO_ACCELERATION};

    #if AXIS@_DRIVER_MODEL == SERVO_EE
      ServoEE driver@(@, &DriverPinsAxis@, &DriverSettingsAxis@, AXIS@_SERVO_DC_PWR_MIN, AXIS@_SERVO_DC_PWR_MAX);
    #elif AXIS@_DRIVER_MODEL == SERVO_PE
      ServoPE driver@(@, &DriverPinsAxis@, &DriverSettingsAxis@, AXIS@_SERVO_DC_PWR_MIN, AXIS@_SERVO_DC_PWR_MAX);
    #elif AXIS@_DRIVER_MODEL == SERVO_TMC2130_DC
      ServoTmc2130DC driver@(@, &DriverPinsAxis@, &DriverSettingsAxis@, AXIS@_SERVO_DC_PWR_MIN, AXIS@_SERVO_DC_PWR_MAX);
    #elif AXIS@_DRIVER_MODEL == SERVO_TMC5160_DC
      ServoTmc5160DC driver@(@, &DriverPinsAxis@, &DriverSettingsAxis@, AXIS@_SERVO_DC_PWR_MIN, AXIS@_SERVO_DC_PWR_MAX);
    #elif AXIS@_DRIVER_MODEL == TMC2209 || AXIS@_DRIVER_MODEL == TMC2226
      ServoTmc2209 driver@(@, &DriverPinsAxis@, &DriverSettingsAxis@, AXIS@_MOTOR_STEPS_PER_MICRON/AXIS@_STEPS_PER_MICRON, AXIS@_DRIVER_MICROSTEPS, AXIS@_DRIVER_IRUN, AXIS@_DRIVER_DECAY, AXIS@_DRIVER_DECAY_GOTO);
    #elif AXIS@_DRIVER_MODEL == SERVO_TMC5160
      ServoTmc5160 driver@(@, &DriverPinsAxis@, &DriverSettingsAxis@, AXIS@_MOTOR_STEPS_PER_MICRON/AXIS@_STEPS_PER_MICRON, AXIS@_DRIVER_MICROSTEPS, AXIS@_DRIVER_IRUN, AXIS@_DRIVER_DECAY, AXIS@_DRIVER_DECAY_GOTO);
    #elif AXIS@_DRIVER_MODEL == SERVO_KTECH
      ServoKTech driver@(@, &DriverSettingsAxis@, AXIS@_MOTOR_STEPS_PER_MICRON/AXIS@_STEPS_PER_MICRON);
    #endif

    ServoMotor motor_@(@, AXIS@_REVERSE, ((ServoDriver*)&driver@), &filterAxis@, &encAxis@, AXIS@_ENCODER_ORIGIN, AXIS@_ENCODER_REVERSE == ON, &feedbackAxis@, &servoControlAxis@, AXIS@_SYNC_THRESHOLD);

  #elif defined(AXIS@_STEP_DIR_PRESENT)
    const StepDirDriverPins DriverPinsAxis@ = {AXIS@_M0_PIN, AXIS@_M1_PIN, AXIS@_M2_PIN, AXIS@_M2_ON_STATE, AXIS@_M3_PIN, AXIS@_DECAY_PIN, AXIS@_FAULT_PIN};
    const StepDirDriverSettings DriverSettingsAxis@ = {AXIS@_DRIVER_MODEL, AXIS@_DRIVER_STATUS, AXIS@_DRIVER_MICROSTEPS, AXIS@_DRIVER_MICROSTEPS_GOTO, AXIS@_DRIVER_DECAY, AXIS@_DRIVER_DECAY_GOTO};
    #if AXIS@_DRIVER_MODEL >= STEP_DIR_DRIVER_FIRST && AXIS@_DRIVER_MODEL < TMC_DRIVER_FIRST
      StepDirGeneric driver@(@, &DriverPinsAxis@, &DriverSettingsAxis@);
    #elif AXIS@_DRIVER_MODEL == TMC2130
      StepDirTmc2130 driver@(@, &DriverPinsAxis@, &DriverSettingsAxis@, AXIS@_DRIVER_IHOLD, AXIS@_DRIVER_IRUN, AXIS@_DRIVER_IGOTO, AXIS@_DRIVER_INTPOL);
    #elif AXIS@_DRIVER_MODEL == TMC2160
      StepDirTmc2160 driver@(@, &DriverPinsAxis@, &DriverSettingsAxis@, AXIS@_DRIVER_IHOLD, AXIS@_DRIVER_IRUN, AXIS@_DRIVER_IGOTO, AXIS@_DRIVER_INTPOL);
    #elif AXIS@_DRIVER_MODEL == TMC2208
      StepDirTmc2208 driver@(@, &DriverPinsAxis@, &DriverSettingsAxis@, AXIS@_DRIVER_IHOLD, AXIS@_DRIVER_IRUN, AXIS@_DRIVER_IGOTO, AXIS@_DRIVER_INTPOL);
    #elif AXIS@_DRIVER_MODEL == TMC2209
      StepDirTmc2209 driver@(@, &DriverPinsAxis@, &DriverSettingsAxis@, AXIS@_DRIVER_IHOLD, AXIS@_DRIVER_IRUN, AXIS@_DRIVER_IGOTO, AXIS@_DRIVER_INTPOL);
    #elif AXIS@_DRIVER_MODEL == TMC2660
      StepDirTmc2660 driver@(@, &DriverPinsAxis@, &DriverSettingsAxis@, AXIS@_DRIVER_IHOLD, AXIS@_DRIVER_IRUN, AXIS@_DRIVER_IGOTO, AXIS@_DRIVER_INTPOL);
    #elif AXIS@_DRIVER_MODEL == TMC5160
      StepDirTmc5160 driver@(@, &DriverPinsAxis@, &DriverSettingsAxis@, AXIS@_DRIVER_IHOLD, AXIS@_DRIVER_IRUN, AXIS@_DRIVER_IGOTO, AXIS@_DRIVER_INTPOL);
    #elif AXIS@_DRIVER_MODEL == TMC5161
      StepDirTmc5161 driver@(@, &DriverPinsAxis@, &DriverSettingsAxis@, AXIS@_DRIVER_IHOLD, AXIS@_DRIVER_IRUN, AXIS@_DRIVER_IGOTO, AXIS@_DRIVER_INTPOL);
    #endif

    const StepDirPins StepDirPinsAxis@ = {AXIS@_STEP_PIN, AXIS@_STEP_STATE, AXIS@_DIR_PIN, AXIS@_ENABLE_PIN, AXIS@_ENABLE_STATE};
    StepDirMotor motor_@(@, AXIS@_REVERSE, &StepDirPinsAxis@, ((StepDirDriver*)&driver@));
  #else
    #error "Configuration (Config.h): Focuser has an unrecognized AXIS@_DRIVER_MODEL this should never happen!"
  #endif

  const AxisPins PinsAxis@ = {AXIS@_SENSE_LIMIT_MIN_PIN, AXIS@_SENSE_HOME_PIN, AXIS@_SENSE_LIMIT_MAX_PIN, {AXIS@_SENSE_HOME, AXIS@_SENSE_HOME_INIT, AXIS@_SENSE_HOME_DIST_LIMIT*1000.0F, AXIS@_SENSE_LIMIT_MIN, AXIS@_SENSE_LIMIT_MAX, AXIS@_SENSE_LIMIT_INIT}};
  const AxisSettings SettingsAxis@ = {AXIS@_STEPS_PER_MICRON, {AXIS@_LIMIT_MIN*1000.0F, AXIS@_LIMIT_MAX*1000.0F}, AXIS@_BACKLASH_RATE};
  }

  Motor& motor@ = motor_@;

  Axis axis@(@, &PinsAxis@, &SettingsAxis@, AXIS_MEASURE_MICRONS);
#endif
