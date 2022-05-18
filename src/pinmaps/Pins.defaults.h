// -------------------------------------------------------------------------------------------------
// Null pin map, assigns OFF to all values not already assigned
#pragma once

/*
// Example for a board using SoftwareSerial ports to any number of drivers
#define SERIAL_TMC              SoftSerial       // Use software serial with RX on M2 and TX on M3 of axis
#define SERIAL_TMC_BAUD         115200           // Baud rate
#define SERIAL_TMC_NO_RX                         // Recieving data doesn't work with software serial

// Example for a board using one HardwareSerial port to all four drivers
#define SERIAL_TMC              Serial1          // Use a single hardware serial port to up to four drivers
#define SERIAL_TMC_BAUD         500000           // Baud rate
#define SERIAL_TMC_TX           11               // Transmit data
#define SERIAL_TMC_RX           12               // Recieving data
#define SERIAL_TMC_AXIS5_REMAP                   // Identify Focuser2 (Axis5) as Axis3
*/

#ifndef SDA_PIN
#define SDA_PIN                    21
#endif

#ifndef SCL_PIN
#define SCL_PIN                    22
#endif

#ifndef AUX0_PIN
#define AUX0_PIN                    OFF
#endif

#ifndef AUX1_PIN
#define AUX1_PIN                    OFF
#endif

#ifndef AUX2_PIN
#define AUX2_PIN                    OFF
#endif

#ifndef AUX3_PIN
#define AUX3_PIN                    OFF
#endif

#ifndef AUX4_PIN
#define AUX4_PIN                    OFF
#endif

#ifndef AUX5_PIN
#define AUX5_PIN                    OFF
#endif

#ifndef AUX6_PIN
#define AUX6_PIN                    OFF
#endif

#ifndef AUX7_PIN
#define AUX7_PIN                    OFF
#endif

#ifndef AUX8_PIN
#define AUX8_PIN                    OFF
#endif

#ifndef ONE_WIRE_PIN
#define ONE_WIRE_PIN                OFF
#endif

#ifndef ADDON_TRIGR_PIN
#define ADDON_TRIGR_PIN             OFF
#endif

#ifndef ADDON_GPIO0_PIN
#define ADDON_GPIO0_PIN             OFF
#endif

#ifndef ADDON_RESET_PIN
#define ADDON_RESET_PIN             OFF
#endif

#ifndef PARK_SENSE_PIN
#define PARK_SENSE_PIN              OFF
#endif

#ifndef PARK_SIGNAL_PIN
#define PARK_SIGNAL_PIN             OFF
#endif

#ifndef PARK_STATUS_PIN
#define PARK_STATUS_PIN             OFF
#endif

#ifndef PEC_SENSE_PIN
#define PEC_SENSE_PIN               OFF
#endif

#ifndef STATUS_LED_PIN
#define STATUS_LED_PIN              OFF
#endif

#ifndef MOUNT_STATUS_LED_PIN
#define MOUNT_STATUS_LED_PIN        OFF
#endif

#ifndef RETICLE_LED_PIN
#define RETICLE_LED_PIN             OFF
#endif

#ifndef STATUS_BUZZER_PIN
#define STATUS_BUZZER_PIN           OFF
#endif

#ifndef PPS_SENSE_PIN
#define PPS_SENSE_PIN               OFF
#endif

#ifndef LIMIT_SENSE_PIN
#define LIMIT_SENSE_PIN             OFF
#endif

#ifndef ST4_RA_W_PIN
#define ST4_RA_W_PIN                OFF
#endif
#ifndef ST4_DEC_S_PIN
#define ST4_DEC_S_PIN               OFF
#endif
#ifndef ST4_DEC_N_PIN
#define ST4_DEC_N_PIN               OFF
#endif
#ifndef ST4_RA_E_PIN
#define ST4_RA_E_PIN                OFF
#endif

#ifndef AXIS1_ENABLE_PIN
#define AXIS1_ENABLE_PIN            OFF
#endif
#ifndef AXIS1_M0_PIN
#define AXIS1_M0_PIN                OFF
#endif
#ifndef AXIS1_M1_PIN
#define AXIS1_M1_PIN                OFF
#endif
#ifndef AXIS1_M2_PIN
#define AXIS1_M2_PIN                OFF
#endif
#ifndef AXIS1_M3_PIN
#define AXIS1_M3_PIN                OFF
#endif
#ifndef AXIS1_STEP_PIN
#define AXIS1_STEP_PIN              OFF
#endif
#ifndef AXIS1_DIR_PIN
#define AXIS1_DIR_PIN               OFF
#endif
#ifndef AXIS1_DECAY_PIN
#define AXIS1_DECAY_PIN             OFF
#endif
#ifndef AXIS1_SERVO_PH1_PIN
#define AXIS1_SERVO_PH1_PIN         OFF
#endif
#ifndef AXIS1_SERVO_PH2_PIN
#define AXIS1_SERVO_PH2_PIN         OFF
#endif
#ifndef AXIS1_SERVO_ENC1_PIN
#define AXIS1_SERVO_ENC1_PIN        OFF
#endif
#ifndef AXIS1_SERVO_ENC2_PIN
#define AXIS1_SERVO_ENC2_PIN        OFF
#endif
#ifndef AXIS1_FAULT_PIN
#define AXIS1_FAULT_PIN             OFF
#endif
#ifndef AXIS1_SENSE_HOME_PIN
#define AXIS1_SENSE_HOME_PIN        OFF
#endif
#ifndef AXIS1_SENSE_LIMIT_MIN_PIN
#define AXIS1_SENSE_LIMIT_MIN_PIN LIMIT_SENSE_PIN
#endif
#ifndef AXIS1_SENSE_LIMIT_MAX_PIN
#define AXIS1_SENSE_LIMIT_MAX_PIN LIMIT_SENSE_PIN
#endif

#ifndef AXIS2_ENABLE_PIN
#define AXIS2_ENABLE_PIN            OFF
#endif
#ifndef AXIS2_M0_PIN
#define AXIS2_M0_PIN                OFF
#endif
#ifndef AXIS2_M1_PIN
#define AXIS2_M1_PIN                OFF
#endif
#ifndef AXIS2_M2_PIN
#define AXIS2_M2_PIN                OFF
#endif
#ifndef AXIS2_M3_PIN
#define AXIS2_M3_PIN                OFF
#endif
#ifndef AXIS2_STEP_PIN
#define AXIS2_STEP_PIN              OFF
#endif
#ifndef AXIS2_DIR_PIN
#define AXIS2_DIR_PIN               OFF
#endif
#ifndef AXIS2_DECAY_PIN
#define AXIS2_DECAY_PIN             OFF
#endif
#ifndef AXIS2_SERVO_PH1_PIN
#define AXIS2_SERVO_PH1_PIN         OFF
#endif
#ifndef AXIS2_SERVO_PH2_PIN
#define AXIS2_SERVO_PH2_PIN         OFF
#endif
#ifndef AXIS2_SERVO_ENC1_PIN
#define AXIS2_SERVO_ENC1_PIN        OFF
#endif
#ifndef AXIS2_SERVO_ENC2_PIN
#define AXIS2_SERVO_ENC2_PIN        OFF
#endif
#ifndef AXIS2_FAULT_PIN
#define AXIS2_FAULT_PIN             OFF
#endif
#ifndef AXIS2_SENSE_HOME_PIN
#define AXIS2_SENSE_HOME_PIN        OFF
#endif
#ifndef AXIS2_SENSE_LIMIT_MIN_PIN
#define AXIS2_SENSE_LIMIT_MIN_PIN LIMIT_SENSE_PIN
#endif
#ifndef AXIS2_SENSE_LIMIT_MAX_PIN
#define AXIS2_SENSE_LIMIT_MAX_PIN LIMIT_SENSE_PIN
#endif

#ifndef AXIS3_ENABLE_PIN
#define AXIS3_ENABLE_PIN            OFF
#endif
#ifndef AXIS3_M0_PIN
#define AXIS3_M0_PIN                OFF
#endif
#ifndef AXIS3_M1_PIN
#define AXIS3_M1_PIN                OFF
#endif
#ifndef AXIS3_M2_PIN
#define AXIS3_M2_PIN                OFF
#endif
#ifndef AXIS3_M3_PIN
#define AXIS3_M3_PIN                OFF
#endif
#ifndef AXIS3_STEP_PIN
#define AXIS3_STEP_PIN              OFF
#endif
#ifndef AXIS3_DIR_PIN
#define AXIS3_DIR_PIN               OFF
#endif
#ifndef AXIS3_DECAY_PIN
#define AXIS3_DECAY_PIN             OFF
#endif
#ifndef AXIS3_SERVO_PH1_PIN
#define AXIS3_SERVO_PH1_PIN         OFF
#endif
#ifndef AXIS3_SERVO_PH2_PIN
#define AXIS3_SERVO_PH2_PIN         OFF
#endif
#ifndef AXIS3_SERVO_ENC1_PIN
#define AXIS3_SERVO_ENC1_PIN        OFF
#endif
#ifndef AXIS3_SERVO_ENC2_PIN
#define AXIS3_SERVO_ENC2_PIN        OFF
#endif
#ifndef AXIS3_FAULT_PIN
#define AXIS3_FAULT_PIN             OFF
#endif
#ifndef AXIS3_SENSE_HOME_PIN
#define AXIS3_SENSE_HOME_PIN        OFF
#endif
#ifndef AXIS3_SENSE_LIMIT_MIN_PIN
#define AXIS3_SENSE_LIMIT_MIN_PIN   OFF
#endif
#ifndef AXIS3_SENSE_LIMIT_MAX_PIN
#define AXIS3_SENSE_LIMIT_MAX_PIN   OFF
#endif

#ifndef AXIS4_ENABLE_PIN
#define AXIS4_ENABLE_PIN            OFF
#endif
#ifndef AXIS4_M0_PIN
#define AXIS4_M0_PIN                OFF
#endif
#ifndef AXIS4_M1_PIN
#define AXIS4_M1_PIN                OFF
#endif
#ifndef AXIS4_M2_PIN
#define AXIS4_M2_PIN                OFF
#endif
#ifndef AXIS4_M3_PIN
#define AXIS4_M3_PIN                OFF
#endif
#ifndef AXIS4_STEP_PIN
#define AXIS4_STEP_PIN              OFF
#endif
#ifndef AXIS4_DIR_PIN
#define AXIS4_DIR_PIN               OFF
#endif
#ifndef AXIS4_DECAY_PIN
#define AXIS4_DECAY_PIN             OFF
#endif
#ifndef AXIS4_SERVO_PH1_PIN
#define AXIS4_SERVO_PH1_PIN         OFF
#endif
#ifndef AXIS4_SERVO_PH2_PIN
#define AXIS4_SERVO_PH2_PIN         OFF
#endif
#ifndef AXIS4_SERVO_ENC1_PIN
#define AXIS4_SERVO_ENC1_PIN        OFF
#endif
#ifndef AXIS4_SERVO_ENC2_PIN
#define AXIS4_SERVO_ENC2_PIN        OFF
#endif
#ifndef AXIS4_FAULT_PIN
#define AXIS4_FAULT_PIN             OFF
#endif
#ifndef AXIS4_SENSE_HOME_PIN
#define AXIS4_SENSE_HOME_PIN        OFF
#endif
#ifndef AXIS4_SENSE_LIMIT_MIN_PIN
#define AXIS4_SENSE_LIMIT_MIN_PIN   OFF
#endif
#ifndef AXIS4_SENSE_LIMIT_MAX_PIN
#define AXIS4_SENSE_LIMIT_MAX_PIN   OFF
#endif

#ifndef AXIS5_ENABLE_PIN
#define AXIS5_ENABLE_PIN            OFF
#endif
#ifndef AXIS5_M0_PIN
#define AXIS5_M0_PIN                OFF
#endif
#ifndef AXIS5_M1_PIN
#define AXIS5_M1_PIN                OFF
#endif
#ifndef AXIS5_M2_PIN
#define AXIS5_M2_PIN                OFF
#endif
#ifndef AXIS5_M3_PIN
#define AXIS5_M3_PIN                OFF
#endif
#ifndef AXIS5_STEP_PIN
#define AXIS5_STEP_PIN              OFF
#endif
#ifndef AXIS5_DIR_PIN
#define AXIS5_DIR_PIN               OFF
#endif
#ifndef AXIS5_DECAY_PIN
#define AXIS5_DECAY_PIN             OFF
#endif
#ifndef AXIS5_SERVO_PH1_PIN
#define AXIS5_SERVO_PH1_PIN         OFF
#endif
#ifndef AXIS5_SERVO_PH2_PIN
#define AXIS5_SERVO_PH2_PIN         OFF
#endif
#ifndef AXIS5_SERVO_ENC1_PIN
#define AXIS5_SERVO_ENC1_PIN        OFF
#endif
#ifndef AXIS5_SERVO_ENC2_PIN
#define AXIS5_SERVO_ENC2_PIN        OFF
#endif
#ifndef AXIS5_FAULT_PIN
#define AXIS5_FAULT_PIN             OFF
#endif
#ifndef AXIS5_SENSE_HOME_PIN
#define AXIS5_SENSE_HOME_PIN        OFF
#endif
#ifndef AXIS5_SENSE_LIMIT_MIN_PIN
#define AXIS5_SENSE_LIMIT_MIN_PIN   OFF
#endif
#ifndef AXIS5_SENSE_LIMIT_MAX_PIN
#define AXIS5_SENSE_LIMIT_MAX_PIN   OFF
#endif

#ifndef AXIS6_ENABLE_PIN
#define AXIS6_ENABLE_PIN            OFF
#endif
#ifndef AXIS6_M0_PIN
#define AXIS6_M0_PIN                OFF
#endif
#ifndef AXIS6_M1_PIN
#define AXIS6_M1_PIN                OFF
#endif
#ifndef AXIS6_M2_PIN
#define AXIS6_M2_PIN                OFF
#endif
#ifndef AXIS6_M3_PIN
#define AXIS6_M3_PIN                OFF
#endif
#ifndef AXIS6_STEP_PIN
#define AXIS6_STEP_PIN              OFF
#endif
#ifndef AXIS6_DIR_PIN
#define AXIS6_DIR_PIN               OFF
#endif
#ifndef AXIS6_DECAY_PIN
#define AXIS6_DECAY_PIN             OFF
#endif
#ifndef AXIS6_SERVO_PH1_PIN
#define AXIS6_SERVO_PH1_PIN         OFF
#endif
#ifndef AXIS6_SERVO_PH2_PIN
#define AXIS6_SERVO_PH2_PIN         OFF
#endif
#ifndef AXIS6_SERVO_ENC1_PIN
#define AXIS6_SERVO_ENC1_PIN        OFF
#endif
#ifndef AXIS6_SERVO_ENC2_PIN
#define AXIS6_SERVO_ENC2_PIN        OFF
#endif
#ifndef AXIS6_FAULT_PIN
#define AXIS6_FAULT_PIN             OFF
#endif
#ifndef AXIS6_SENSE_HOME_PIN
#define AXIS6_SENSE_HOME_PIN        OFF
#endif
#ifndef AXIS6_SENSE_LIMIT_MIN_PIN
#define AXIS6_SENSE_LIMIT_MIN_PIN   OFF
#endif
#ifndef AXIS6_SENSE_LIMIT_MAX_PIN
#define AXIS6_SENSE_LIMIT_MAX_PIN   OFF
#endif

#ifndef AXIS7_ENABLE_PIN
#define AXIS7_ENABLE_PIN            OFF
#endif
#ifndef AXIS7_M0_PIN
#define AXIS7_M0_PIN                OFF
#endif
#ifndef AXIS7_M1_PIN
#define AXIS7_M1_PIN                OFF
#endif
#ifndef AXIS7_M2_PIN
#define AXIS7_M2_PIN                OFF
#endif
#ifndef AXIS7_M3_PIN
#define AXIS7_M3_PIN                OFF
#endif
#ifndef AXIS7_STEP_PIN
#define AXIS7_STEP_PIN              OFF
#endif
#ifndef AXIS7_DIR_PIN
#define AXIS7_DIR_PIN               OFF
#endif
#ifndef AXIS7_DECAY_PIN
#define AXIS7_DECAY_PIN             OFF
#endif
#ifndef AXIS7_SERVO_PH1_PIN
#define AXIS7_SERVO_PH1_PIN         OFF
#endif
#ifndef AXIS7_SERVO_PH2_PIN
#define AXIS7_SERVO_PH2_PIN         OFF
#endif
#ifndef AXIS7_SERVO_ENC1_PIN
#define AXIS7_SERVO_ENC1_PIN        OFF
#endif
#ifndef AXIS7_SERVO_ENC2_PIN
#define AXIS7_SERVO_ENC2_PIN        OFF
#endif
#ifndef AXIS7_FAULT_PIN
#define AXIS7_FAULT_PIN             OFF
#endif
#ifndef AXIS7_SENSE_HOME_PIN
#define AXIS7_SENSE_HOME_PIN        OFF
#endif
#ifndef AXIS7_SENSE_LIMIT_MIN_PIN
#define AXIS7_SENSE_LIMIT_MIN_PIN   OFF
#endif
#ifndef AXIS7_SENSE_LIMIT_MAX_PIN
#define AXIS7_SENSE_LIMIT_MAX_PIN   OFF
#endif

#ifndef AXIS8_ENABLE_PIN
#define AXIS8_ENABLE_PIN            OFF
#endif
#ifndef AXIS8_M0_PIN
#define AXIS8_M0_PIN                OFF
#endif
#ifndef AXIS8_M1_PIN
#define AXIS8_M1_PIN                OFF
#endif
#ifndef AXIS8_M2_PIN
#define AXIS8_M2_PIN                OFF
#endif
#ifndef AXIS8_M3_PIN
#define AXIS8_M3_PIN                OFF
#endif
#ifndef AXIS8_STEP_PIN
#define AXIS8_STEP_PIN              OFF
#endif
#ifndef AXIS8_DIR_PIN
#define AXIS8_DIR_PIN               OFF
#endif
#ifndef AXIS8_DECAY_PIN
#define AXIS8_DECAY_PIN             OFF
#endif
#ifndef AXIS8_SERVO_PH1_PIN
#define AXIS8_SERVO_PH1_PIN         OFF
#endif
#ifndef AXIS8_SERVO_PH2_PIN
#define AXIS8_SERVO_PH2_PIN         OFF
#endif
#ifndef AXIS8_SERVO_ENC1_PIN
#define AXIS8_SERVO_ENC1_PIN        OFF
#endif
#ifndef AXIS8_SERVO_ENC2_PIN
#define AXIS8_SERVO_ENC2_PIN        OFF
#endif
#ifndef AXIS8_FAULT_PIN
#define AXIS8_FAULT_PIN             OFF
#endif
#ifndef AXIS8_SENSE_HOME_PIN
#define AXIS8_SENSE_HOME_PIN        OFF
#endif
#ifndef AXIS8_SENSE_LIMIT_MIN_PIN
#define AXIS8_SENSE_LIMIT_MIN_PIN   OFF
#endif
#ifndef AXIS8_SENSE_LIMIT_MAX_PIN
#define AXIS8_SENSE_LIMIT_MAX_PIN   OFF
#endif

#ifndef AXIS9_ENABLE_PIN
#define AXIS9_ENABLE_PIN            OFF
#endif
#ifndef AXIS9_M0_PIN
#define AXIS9_M0_PIN                OFF
#endif
#ifndef AXIS9_M1_PIN
#define AXIS9_M1_PIN                OFF
#endif
#ifndef AXIS9_M2_PIN
#define AXIS9_M2_PIN                OFF
#endif
#ifndef AXIS9_M3_PIN
#define AXIS9_M3_PIN                OFF
#endif
#ifndef AXIS9_STEP_PIN
#define AXIS9_STEP_PIN              OFF
#endif
#ifndef AXIS9_DIR_PIN
#define AXIS9_DIR_PIN               OFF
#endif
#ifndef AXIS9_DECAY_PIN
#define AXIS9_DECAY_PIN             OFF
#endif
#ifndef AXIS9_SERVO_PH1_PIN
#define AXIS9_SERVO_PH1_PIN         OFF
#endif
#ifndef AXIS9_SERVO_PH2_PIN
#define AXIS9_SERVO_PH2_PIN         OFF
#endif
#ifndef AXIS9_SERVO_ENC1_PIN
#define AXIS9_SERVO_ENC1_PIN        OFF
#endif
#ifndef AXIS9_SERVO_ENC2_PIN
#define AXIS9_SERVO_ENC2_PIN        OFF
#endif
#ifndef AXIS9_FAULT_PIN
#define AXIS9_FAULT_PIN             OFF
#endif
#ifndef AXIS9_SENSE_HOME_PIN
#define AXIS9_SENSE_HOME_PIN        OFF
#endif
#ifndef AXIS9_SENSE_LIMIT_MIN_PIN
#define AXIS9_SENSE_LIMIT_MIN_PIN   OFF
#endif
#ifndef AXIS9_SENSE_LIMIT_MAX_PIN
#define AXIS9_SENSE_LIMIT_MAX_PIN   OFF
#endif
