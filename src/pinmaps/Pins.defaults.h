// -------------------------------------------------------------------------------------------------
// Null pin map, assigns OFF to all values not already assigned
#pragma once

// --------------------------------------------------------------------------------------------------------
// Serial, SPI, and I2C interface pins

// usually the default serial port
#if SERIAL_A_BAUD_DEFAULT != OFF
#ifndef SERIAL_A
#define SERIAL_A                    Serial
#endif
#endif

// automatic setup of serial passthrough
#if SERIAL_B_ESP_FLASHING == ON
  #define SERIAL_PASSTHROUGH SERIAL_B
  #define SERIAL_PASSTHROUGH_BAUD_DEFAULT SERIAL_B_BAUD_DEFAULT
  #define SERIAL_PASSTHROUGH_RX SERIAL_B_RX
  #define SERIAL_PASSTHROUGH_TX SERIAL_B_TX
  #define SERIAL_PASSTHROUGH_RXTX_SET SERIAL_B_RXTX_SET
#endif

// default settings for any TMC UART drivers that may be present
#if defined(STEP_DIR_TMC_UART_PRESENT)
#ifndef SERIAL_TMC
#define SERIAL_TMC                  SoftSerial     // Use software serial with TX on M3 (CS) of each axis
#endif
#ifndef SERIAL_TMC_RX_DISABLE
#define SERIAL_TMC_RX_DISABLE       true           // Recieving data often doesn't work with software serial
#endif
#ifndef SERIAL_TMC_BAUD
#define SERIAL_TMC_BAUD             115200         // Baud rate
#endif
#ifndef SERIAL_TMC_ADDRESS_MAP
#define SERIAL_TMC_ADDRESS_MAP(x)   (0)            // driver addresses are 0 for all axes
#endif
#ifndef SERIAL_TMC_INVERT
#define SERIAL_TMC_INVERT           OFF            // invert the serial interface signal logic
#endif
#endif

// pin# for controlling the reset of W5500 so it comes up properly
#ifndef ETHERNET_RESET_PIN
#define ETHERNET_RESET_PIN          OFF
#endif

// Specify the default I2C pins (if they can be set via the HAL)
#ifndef I2C_SDA_PIN
#define I2C_SDA_PIN                 OFF
#endif

#ifndef I2C_SCL_PIN
#define I2C_SCL_PIN                 OFF
#endif

// --------------------------------------------------------------------------------------------------------
// GPIO SSR74HC595 pins
#ifndef GPIO_SSR74HC595_LATCH_PIN
#define GPIO_SSR74HC595_LATCH_PIN   OFF            // If used, only pins 0 to 31 are supported on the ESP32
#endif

#ifndef GPIO_SSR74HC595_CLOCK_PIN
#define GPIO_SSR74HC595_CLOCK_PIN   OFF            // If used, only pins 0 to 31 are supported on the ESP32
#endif

#ifndef GPIO_SSR74HC595_DATA_PIN
#define GPIO_SSR74HC595_DATA_PIN    OFF            // If used, only pins 0 to 31 are supported on the ESP32
#endif

#ifndef GPIO_SSR74HC595_COUNT
#define GPIO_SSR74HC595_COUNT       8              // 8, 16, 24, or 32 (for 1, 2, 3, or 4 74HC595's)
#endif

// --------------------------------------------------------------------------------------------------------
// define standard pins as inactive
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

#ifndef MOUNT_LED_PIN
#define MOUNT_LED_PIN               OFF
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
#ifndef AXIS1_ENCODER_A_PIN
#define AXIS1_ENCODER_A_PIN         OFF
#endif
#ifndef AXIS1_ENCODER_B_PIN
#define AXIS1_ENCODER_B_PIN         OFF
#endif
#ifndef AXIS1_FAULT_PIN
#define AXIS1_FAULT_PIN             OFF
#endif
#ifndef AXIS1_SENSE_HOME_PIN
#define AXIS1_SENSE_HOME_PIN        OFF
#endif
#ifndef AXIS1_SENSE_LIMIT_MIN_PIN
#define AXIS1_SENSE_LIMIT_MIN_PIN   LIMIT_SENSE_PIN
#endif
#ifndef AXIS1_SENSE_LIMIT_MAX_PIN
#define AXIS1_SENSE_LIMIT_MAX_PIN   LIMIT_SENSE_PIN
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
#ifndef AXIS2_ENCODER_A_PIN
#define AXIS2_ENCODER_A_PIN         OFF
#endif
#ifndef AXIS2_ENCODER_B_PIN
#define AXIS2_ENCODER_B_PIN         OFF
#endif
#ifndef AXIS2_FAULT_PIN
#define AXIS2_FAULT_PIN             OFF
#endif
#ifndef AXIS2_SENSE_HOME_PIN
#define AXIS2_SENSE_HOME_PIN        OFF
#endif
#ifndef AXIS2_SENSE_LIMIT_MIN_PIN
#define AXIS2_SENSE_LIMIT_MIN_PIN   LIMIT_SENSE_PIN
#endif
#ifndef AXIS2_SENSE_LIMIT_MAX_PIN
#define AXIS2_SENSE_LIMIT_MAX_PIN   LIMIT_SENSE_PIN
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
#ifndef AXIS3_ENCODER_A_PIN
#define AXIS3_ENCODER_A_PIN         OFF
#endif
#ifndef AXIS3_ENCODER_B_PIN
#define AXIS3_ENCODER_B_PIN         OFF
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
#ifndef AXIS4_ENCODER_A_PIN
#define AXIS4_ENCODER_A_PIN         OFF
#endif
#ifndef AXIS4_ENCODER_B_PIN
#define AXIS4_ENCODER_B_PIN         OFF
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
#ifndef AXIS5_ENCODER_A_PIN
#define AXIS5_ENCODER_A_PIN         OFF
#endif
#ifndef AXIS5_ENCODER_B_PIN
#define AXIS5_ENCODER_B_PIN         OFF
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
#ifndef AXIS6_ENCODER_A_PIN
#define AXIS6_ENCODER_A_PIN         OFF
#endif
#ifndef AXIS6_ENCODER_B_PIN
#define AXIS6_ENCODER_B_PIN         OFF
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
#ifndef AXIS7_ENCODER_A_PIN
#define AXIS7_ENCODER_A_PIN         OFF
#endif
#ifndef AXIS7_ENCODER_B_PIN
#define AXIS7_ENCODER_B_PIN         OFF
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
#ifndef AXIS8_ENCODER_A_PIN
#define AXIS8_ENCODER_A_PIN         OFF
#endif
#ifndef AXIS8_ENCODER_B_PIN
#define AXIS8_ENCODER_B_PIN         OFF
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
#ifndef AXIS9_ENCODER_A_PIN
#define AXIS9_ENCODER_A_PIN         OFF
#endif
#ifndef AXIS9_ENCODER_B_PIN
#define AXIS9_ENCODER_B_PIN         OFF
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
