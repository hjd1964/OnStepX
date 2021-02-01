#define HAL_HAS_DIGITAL_FAST

#define SERIAL_A                    Serial

#define MOUNT_TYPE                  ALTAZM

#define AXIS1_STEPS_PER_DEGREE     12800.0
#define AXIS2_STEPS_PER_DEGREE     12800.0

#define AXIS1_STEP_PIN                  54
#define AXIS1_DIR_PIN                   55
#define AXIS1_ENABLE_PIN                38
#define AXIS1_M0_PIN                   OFF
#define AXIS1_M1_PIN                   OFF
#define AXIS1_M2_PIN                   OFF
#define AXIS1_M3_PIN                   OFF

#define AXIS1_DRIVER               TMC2130
#define AXIS1_DRIVER_MICROSTEPS        OFF
#define AXIS1_DRIVER_MICROSTEPS_GOTO   OFF
#define AXIS1_DRIVER_IHOLD             OFF
#define AXIS1_DRIVER_IRUN              OFF
#define AXIS1_DRIVER_IGOTO             OFF
#define AXIS1_DRIVER_DECAY             OFF
#define AXIS1_DRIVER_DECAY_GOTO        OFF
#define AXIS1_DECAY_PIN                OFF

#define AXIS2_STEP_PIN                  56
#define AXIS2_DIR_PIN                   60
#define AXIS2_ENABLE_PIN                61
#define AXIS2_M0_PIN                   OFF
#define AXIS2_M1_PIN                   OFF
#define AXIS2_M2_PIN                   OFF
#define AXIS2_M3_PIN                   OFF
#define AXIS2_DECAY_PIN                OFF

#define AXIS2_DRIVER               TMC2130
#define AXIS2_DRIVER_MICROSTEPS        OFF
#define AXIS2_DRIVER_MICROSTEPS_GOTO   OFF
#define AXIS2_DRIVER_IHOLD             OFF
#define AXIS2_DRIVER_IRUN              OFF
#define AXIS2_DRIVER_IGOTO             OFF
#define AXIS2_DRIVER_DECAY             OFF
#define AXIS2_DRIVER_DECAY_GOTO        OFF

#define AXIS3_DRIVER                   OFF
#define AXIS4_DRIVER                   OFF
#define AXIS5_DRIVER                   OFF
#define AXIS6_DRIVER                   OFF
