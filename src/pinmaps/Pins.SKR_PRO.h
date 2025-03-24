// -------------------------------------------------------------------------------------------------
// Pinmap for the BTT SKR PRO Version 1.2 (STM32F407)
#pragma once

#if defined(STM32F407xx)

// Serial: Built in USB (not TTL)
// Serial1: RX1 Pin PA10, TX1 Pin PA9 (Connector Labeled TFT)
// Serial3: RX3 Pin PD9, TX3 Pin PD8 (Connector Labeled SERIAL3)
// Serial6: RX6 Pin PC7, TX6 Pin PC6 (Connector Labeled ESP-01)

#if SERIAL_A_BAUD_DEFAULT != OFF
  #define SERIAL_A              Serial
#endif
#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B              HardSerial
  #define SERIAL_B_RX           PC7
  #define SERIAL_B_TX           PC6
#endif
#if SERIAL_C_BAUD_DEFAULT != OFF
  #define SERIAL_C              HardSerial
  #define SERIAL_C_RX           PA10
  #define SERIAL_C_TX           PA9
 // #define SERIAL_C              Serial1
#endif

// Connector Labeled I2C uses PB6 (SCL) and PB7 (SDA) which are forced in the HAL

// Auto assign the Serial3 port pins for GPS
#if SERIAL_GPS_BAUD != OFF
  #ifndef SERIAL_GPS
    #define SERIAL_GPS          HardSerial
    #define SERIAL_GPS_RX       PD9
    #define SERIAL_GPS_TX       PD8
  #endif
#endif

// allow RX from TMC UART drivers
#ifndef SERIAL_TMC_RX_DISABLE
  #define SERIAL_TMC_RX_DISABLE false
#endif

// Allow resetting ESP8266 into firmware upload mode
#ifndef ADDON_GPIO0_PIN
  #define ADDON_GPIO0_PIN       PF14             // ESP8266 GPIO0
#endif
#ifndef ADDON_RESET_PIN
  #define ADDON_RESET_PIN       PG1              // ESP8266 RST
#endif

#define HEATER0_PIN             PB1              // Connector Labeled HEAT0
#define HEATER1_PIN             PD14             // Connector Labeled HEAT1
#define HEATER2_PIN             PB0              // Connector Labeled HEAT2
#define HEATER3_PIN             PD12             // Connector Labeled BED
#define HEATER4_PIN             PC8              // Connector Labeled FAN0

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define AUX1_PIN                HEATER0_PIN
#define AUX2_PIN                HEATER1_PIN
#define AUX3_PIN                HEATER2_PIN
#define AUX4_PIN                HEATER3_PIN
#define AUX5_PIN                HEATER4_PIN

// Thermistor (temperature) sensor inputs have built-in 4.7K Ohm pullups and a 10uF cap for noise supression
#define TEMP0_PIN               PF3              // Connector Labeled T0
#define TEMP1_PIN               PF4              // Connector Labeled T1
#define TEMP2_PIN               PF5              // Connector Labeled T2
#define TEMP3_PIN               PF6              // Connector Labeled T3

#ifndef FEATURE1_TEMPERATURE_PIN
  #define FEATURE1_TEMPERATURE_PIN TEMP0_PIN
#endif
#ifndef FEATURE2_TEMPERATURE_PIN
  #define FEATURE2_TEMPERATURE_PIN TEMP1_PIN
#endif
#ifndef FEATURE3_TEMPERATURE_PIN
  #define FEATURE3_TEMPERATURE_PIN TEMP2_PIN
#endif
#ifndef FOCUSER_TEMPERATURE_PIN
  #define FOCUSER_TEMPERATURE_PIN  TEMP3_PIN
#endif

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#ifndef PEC_SENSE_PIN
  #define PEC_SENSE_PIN         PF12             // Connector labeled EXP2 (Analog or Digital)
#endif

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#ifndef STATUS_LED_PIN
  #define STATUS_LED_PIN        PE6              // Connector Labeled FAN2 (has an LED built in too)
#endif
#define MOUNT_LED_PIN           PE6              // Connector Labeled FAN2 (shared)
#ifndef RETICLE_LED_PIN 
  #define RETICLE_LED_PIN       PE5              // Connector Labeled FAN1 (has an LED built in too) TIM9
#endif

#ifdef STATUS_LED_ON_STATE
#undef STATUS_LED_ON_STATE
#endif
#define STATUS_LED_ON_STATE     HIGH
#ifdef MOUNT_LED_ON_STATE
#undef MOUNT_LED_ON_STATE
#endif
#define MOUNT_LED_ON_STATE      HIGH

// For a piezo buzzer
#ifndef STATUS_BUZZER_PIN
  #define STATUS_BUZZER_PIN     PF11             // Connector Labeled EXP2
#endif

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#ifndef PPS_SENSE_PIN
  #define PPS_SENSE_PIN         PF13             // Connector Labeled EXP2
#endif

// The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking
#ifndef LIMIT_SENSE_PIN
  #define LIMIT_SENSE_PIN       PB10             // Limit Connector Labeled E0
#endif

// Axis1 RA/Azm step/dir driver (Labeled X)
#define AXIS1_ENABLE_PIN        PF2
#define AXIS1_M0_PIN            PC12             // SPI MOSI
#define AXIS1_M1_PIN            PC10             // SPI SCK
#if defined(STEP_DIR_TMC_UART_PRESENT) && !defined(SERIAL_TMC_HARDWARE_UART)
  #define AXIS1_M2_PIN          PE4              // TX
  #define AXIS1_M3_PIN          PC13             // RX
#else
  #define AXIS1_M2_PIN          PA15             // SPI CS
  #define AXIS1_M3_PIN          PC11             // SPI MISO
#endif
#define AXIS1_STEP_PIN          PE9              // TIM1
#define AXIS1_DIR_PIN           PF1
#ifndef AXIS1_SENSE_HOME_PIN
  #define AXIS1_SENSE_HOME_PIN  PE10             // Home SW for Axis1 (Limit Connector Labeled E1)
#endif

// Axis2 Dec/Alt step/dir driver (Labeled Y)
#define AXIS2_ENABLE_PIN        PD7
#define AXIS2_M0_PIN            PC12
#define AXIS2_M1_PIN            PC10
#if defined(STEP_DIR_TMC_UART_PRESENT) && !defined(SERIAL_TMC_HARDWARE_UART)
  #define AXIS2_M2_PIN          PE2              // TX
  #define AXIS2_M3_PIN          PE3              // RX
#else
  #define AXIS2_M2_PIN          PB8              // SPI CS
  #define AXIS2_M3_PIN          PC11
#endif
#define AXIS2_STEP_PIN          PE11             // TIM1
#define AXIS2_DIR_PIN           PE8
#ifndef AXIS2_SENSE_HOME_PIN
  #define AXIS2_SENSE_HOME_PIN  PG5              // Home SW for Axis1 (Limit Connector Labeled E2)
#endif

// For rotator stepper driver (Labeled Z)
#define AXIS3_ENABLE_PIN        PC0
#define AXIS3_M0_PIN            PC12
#define AXIS3_M1_PIN            PC10
#if defined(STEP_DIR_TMC_UART_PRESENT) && !defined(SERIAL_TMC_HARDWARE_UART)
  #define AXIS3_M2_PIN          PE0              // TX
  #define AXIS3_M3_PIN          PE1              // RX
#else
  #define AXIS3_M2_PIN          PB9              // SPI CS
  #define AXIS3_M3_PIN          PC11
#endif
#define AXIS3_STEP_PIN          PE13
#define AXIS3_DIR_PIN           PC2

// For focuser1 stepper driver (Labeled E0)
#define AXIS4_ENABLE_PIN        PC3
#define AXIS4_M0_PIN            PC12
#define AXIS4_M1_PIN            PC10
#if defined(STEP_DIR_TMC_UART_PRESENT) && !defined(SERIAL_TMC_HARDWARE_UART)
  #define AXIS4_M2_PIN          PD2              // TX
  #define AXIS4_M3_PIN          PD4              // RX
#else
  #define AXIS4_M2_PIN          PB3              // SPI CS
  #define AXIS4_M3_PIN          PC11
#endif
#define AXIS4_STEP_PIN          PE14
#define AXIS4_DIR_PIN           PA0

// For focuser2 stepper driver (Labeled E1)
#define AXIS5_ENABLE_PIN        PA3
#define AXIS5_M0_PIN            PC12
#define AXIS5_M1_PIN            PC10
#if defined(STEP_DIR_TMC_UART_PRESENT) && !defined(SERIAL_TMC_HARDWARE_UART)
  #define AXIS5_M2_PIN          PD0              // TX
  #define AXIS5_M3_PIN          PD1              // RX
#else
  #define AXIS5_M2_PIN          PG15             // SPI CS
  #define AXIS5_M3_PIN          PC11
#endif
#define AXIS5_STEP_PIN          PD15
#define AXIS5_DIR_PIN           PE7

// For focuser3 stepper driver (Labeled E2)
#define AXIS6_ENABLE_PIN        PF0
#define AXIS6_M0_PIN            PC12
#define AXIS6_M1_PIN            PC10
#if defined(STEP_DIR_TMC_UART_PRESENT) && !defined(SERIAL_TMC_HARDWARE_UART)
  #define AXIS6_M2_PIN          PD5              // TX
  #define AXIS6_M3_PIN          PD6              // RX
#else
  #define AXIS6_M2_PIN          PG12             // SPI CS
  #define AXIS6_M3_PIN          PC11
#endif
#define AXIS6_STEP_PIN          PD13
#define AXIS6_DIR_PIN           PG9

// For focuser4 stepper driver (Labeled Z)
#define AXIS7_ENABLE_PIN        PC0
#define AXIS7_M0_PIN            PC12
#define AXIS7_M1_PIN            PC10
#if defined(STEP_DIR_TMC_UART_PRESENT) && !defined(SERIAL_TMC_HARDWARE_UART)
  #define AXIS7_M2_PIN          PE0              // TX
  #define AXIS7_M3_PIN          PE1              // RX
#else
  #define AXIS7_M2_PIN          PB9              // SPI CS
  #define AXIS7_M3_PIN          PC11
#endif
#define AXIS7_STEP_PIN          PE13
#define AXIS7_DIR_PIN           PC2

// ST4 interface (Connector Labeled EXTENSION-2 which has Gnd and +5V so near ideal)
#define ST4_RA_W_PIN            PD0              // ST4 RA- West
#define ST4_DEC_S_PIN           PD2              // ST4 DE- South
#define ST4_DEC_N_PIN           PD5              // ST4 DE+ North
#define ST4_RA_E_PIN            PE0              // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
