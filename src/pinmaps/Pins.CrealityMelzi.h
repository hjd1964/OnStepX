// -------------------------------------------------------------------------------------------------
// Null pin map, assigns OFF to all values not already assigned
#pragma once

#if defined(__AVR_ATmega1284P__)
// --------------------------------------------------------------------------------------------------------
// Serial, SPI, and I2C interface pins

#if SERIAL_A_BAUD_DEFAULT != OFF
  #define SERIAL_A              Serial
#endif
#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B              Serial1
#endif

#define SERIAL_B_RX                 10 // PD2 TX1 for ESP8266
#define SERIAL_B_TX                 11 // PD3 RX1 for ESP8266

#define I2C_SCL_PIN                 16 // PC0 SCL BTN_ENC
#define I2C_SDA_PIN                 17 // PC1 SDA LCS_PINS_ENABLE

// --------------------------------------------------------------------------------------------------------
#define AUX0_PIN                    12 // PD4 HEATER_BED_PIN
#define AUX1_PIN                    13 // PD5 HEATER_0_PIN
#define AUX2_PIN                     4 // PB4 FAN_PIN
#define AUX3_PIN                    24 // PA7 TEMP_0_PIN
#define AUX4_PIN                    25 // PA6 TEMP_BED_PIN 

#define ONE_WIRE_PIN                OFF
#define ADDON_TRIGR_PIN             OFF
#define ADDON_GPIO0_PIN             OFF
#define ADDON_RESET_PIN             OFF
#define PARK_SENSE_PIN              OFF
#define PARK_SIGNAL_PIN             OFF
#define PARK_STATUS_PIN             OFF
#define PEC_SENSE_PIN               OFF
#define STATUS_LED_PIN              OFF
#define MOUNT_LED_PIN               OFF
#define RETICLE_LED_PIN             OFF
#define STATUS_BUZZER_PIN           OFF
#define PPS_SENSE_PIN               OFF
#define LIMIT_SENSE_PIN             OFF

#define ST4_RA_W_PIN                OFF
#define ST4_DEC_S_PIN               OFF
#define ST4_DEC_N_PIN               OFF
#define ST4_RA_E_PIN                OFF

#define AXIS1_ENABLE_PIN            14
#define AXIS1_M0_PIN                OFF
#define AXIS1_M1_PIN                OFF
#define AXIS1_M2_PIN                OFF
#define AXIS1_M3_PIN                OFF
#define AXIS1_STEP_PIN              15
#define AXIS1_DIR_PIN               21
#define AXIS1_SENSE_HOME_PIN        18

#define AXIS2_ENABLE_PIN            SHARED
#define AXIS2_M0_PIN                OFF
#define AXIS2_M1_PIN                OFF
#define AXIS2_M2_PIN                OFF
#define AXIS2_M3_PIN                OFF
#define AXIS2_STEP_PIN              22
#define AXIS2_DIR_PIN               23
#define AXIS2_SENSE_HOME_PIN        19

#define AXIS3_ENABLE_PIN            SHARED
#define AXIS3_M0_PIN                OFF
#define AXIS3_M1_PIN                OFF
#define AXIS3_M2_PIN                OFF
#define AXIS3_M3_PIN                OFF
#define AXIS3_STEP_PIN              OFF
#define AXIS3_DIR_PIN               OFF
#define AXIS3_SENSE_HOME_PIN        20

#define AXIS4_ENABLE_PIN            SHARED
#define AXIS4_M0_PIN                OFF
#define AXIS4_M1_PIN                OFF
#define AXIS4_M2_PIN                OFF
#define AXIS4_M3_PIN                OFF
#define AXIS4_STEP_PIN              OFF
#define AXIS4_DIR_PIN               OFF

#ifndef FOCUSER_TEMPERATURE_PIN
#define FOCUSER_TEMPERATURE_PIN       OFF // analog pin
#endif
#ifndef FOCUSER_BUTTON_SENSE_IN_PIN
#define FOCUSER_BUTTON_SENSE_IN_PIN   OFF
#endif
#ifndef FOCUSER_BUTTON_SENSE_OUT_PIN
#define FOCUSER_BUTTON_SENSE_OUT_PIN  OFF
#endif

#ifndef FEATURE1_TEMPERATURE_PIN
#define FEATURE1_TEMPERATURE_PIN      OFF // analog pin
#endif
#ifndef FEATURE2_TEMPERATURE_PIN
#define FEATURE2_TEMPERATURE_PIN      OFF
#endif
#ifndef FEATURE3_TEMPERATURE_PIN
#define FEATURE3_TEMPERATURE_PIN      OFF
#endif
#ifndef FEATURE4_TEMPERATURE_PIN
#define FEATURE4_TEMPERATURE_PIN      OFF
#endif
#ifndef FEATURE5_TEMPERATURE_PIN
#define FEATURE5_TEMPERATURE_PIN      OFF
#endif
#ifndef FEATURE6_TEMPERATURE_PIN
#define FEATURE6_TEMPERATURE_PIN      OFF
#endif
#ifndef FEATURE7_TEMPERATURE_PIN
#define FEATURE7_TEMPERATURE_PIN      OFF
#endif
#ifndef FEATURE8_TEMPERATURE_PIN
#define FEATURE8_TEMPERATURE_PIN      OFF
#endif

#else
  #error "Wrong processor for this configuration!"
#endif
