// -------------------------------------------------------------------------------------------------
// Pinmap for the BTT Kraken Version 1.0 (STM32H723)
#pragma once

#if defined(STM32H723xx)

// Serial: Built in USB (not TTL)
// Serial3: RX3 Pin PD9, TX3 Pin PD8

#if SERIAL_A_BAUD_DEFAULT != OFF
  #define SERIAL_A              Serial
#endif
#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B              HardSerial
  #define SERIAL_B_RX           PD9
  #define SERIAL_B_TX           PD8
#endif

// Use software serial for GPS

// The multi-purpose pins (automatically associated with Auxiliary Features)
#define AUX1_PIN                PD14             // Connector Labeled HE0
#define AUX2_PIN                PB0              // Connector Labeled HE1
#define AUX3_PIN                PD12             // Connector Labeled HE2
#define AUX4_PIN                PC8              // Connector Labeled HE3
#define AUX5_PIN                PF5              // Connector Labeled BED-OUT
#define AUX6_PIN                PA0              // Connector Labeled FAN0
#define AUX7_PIN                PA1              // Connector Labeled FAN1
#define AUX8_PIN                PA2              // Connector Labeled FAN2

// Thermistor (temperature) sensor inputs have built-in 4.7K Ohm pullups and a 10uF cap for noise supression
#ifndef FEATURE1_TEMPERATURE_PIN
  #define FEATURE1_TEMPERATURE_PIN PA7           // Connector Labeled TH0
#endif
#ifndef FEATURE2_TEMPERATURE_PIN
  #define FEATURE2_TEMPERATURE_PIN PC4           // Connector Labeled TH1
#endif
#ifndef FEATURE3_TEMPERATURE_PIN
  #define FEATURE3_TEMPERATURE_PIN PC5           // Connector Labeled TH2
#endif
#ifndef FEATURE4_TEMPERATURE_PIN
  #define FEATURE4_TEMPERATURE_PIN PB1           // Connector Labeled TH3
#endif
#ifndef FEATURE5_TEMPERATURE_PIN
  #define FEATURE5_TEMPERATURE_PIN PB0           // Connector Labeled THB
#endif
#ifndef FOCUSER_TEMPERATURE_PIN
  #define FOCUSER_TEMPERATURE_PIN  PB1           // Connector Labeled TH3
#endif

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#ifndef PEC_SENSE_PIN
  #define PEC_SENSE_PIN         PC2              // Connector labeled IND-DET (Analog or Digital)
#endif

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#ifndef STATUS_LED_PIN
  #define STATUS_LED_PIN        PA3              // Connector Labeled FAN3
#endif
#define MOUNT_LED_PIN           PA4              // Connector Labeled FAN4
#ifndef RETICLE_LED_PIN 
  #define RETICLE_LED_PIN       PA5              // Connector Labeled FAN5
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
  #define STATUS_BUZZER_PIN     PA6              // Connector Labeled FAN6
#endif

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#ifndef PPS_SENSE_PIN
  #define PPS_SENSE_PIN         PG6              // Connector Labeled EXP2
#endif

// The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking
#ifndef LIMIT_SENSE_PIN
  #define LIMIT_SENSE_PIN       PC13             // Limit Connector
#endif

// These are all TMC2160 drivers only
#if (AXIS1_DRIVER_MODEL != OFF && AXIS1_DRIVER_MODEL != TMC2160) || \
    (AXIS2_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != TMC2160) || \
    (AXIS3_DRIVER_MODEL != OFF && AXIS3_DRIVER_MODEL != TMC2160) || \
    (AXIS4_DRIVER_MODEL != OFF && AXIS4_DRIVER_MODEL != TMC2160) || \
    (AXIS5_DRIVER_MODEL != OFF && AXIS5_DRIVER_MODEL != TMC2160) || \
    (AXIS6_DRIVER_MODEL != OFF && AXIS6_DRIVER_MODEL != TMC2160) || \
    (AXIS7_DRIVER_MODEL != OFF && AXIS7_DRIVER_MODEL != TMC2160) || \
    (AXIS8_DRIVER_MODEL != OFF && AXIS8_DRIVER_MODEL != TMC2160)
  #error "Configuration (Config.h): This PINMAP only supports TMC2160 stepper drivers"
#endif

// Use the Kraken Rsense values for the first four axes (the others are 0.075 ohms by default)
#define TMC2160_RSENSE_KRAKEN   0.022

// Axis1 RA/Azm step/dir driver (Labeled S1)
#define AXIS1_ENABLE_PIN        PE6
#define AXIS1_M0_PIN            PC8              // SPI MOSI
#define AXIS1_M1_PIN            PC6              // SPI SCK
#define AXIS1_M2_PIN            PD6              // SPI CS
#define AXIS1_M3_PIN            PC7              // SPI MISO
#define AXIS1_STEP_PIN          PC14
#define AXIS1_DIR_PIN           PC13

// Axis2 RA/Azm step/dir driver (Labeled S2)
#define AXIS2_ENABLE_PIN        PE3
#define AXIS2_M0_PIN            PC8              // SPI MOSI
#define AXIS2_M1_PIN            PC6              // SPI SCK
#define AXIS2_M2_PIN            PD5              // SPI CS
#define AXIS2_M3_PIN            PC7              // SPI MISO
#define AXIS2_STEP_PIN          PE5
#define AXIS2_DIR_PIN           PE4

// Axis3 RA/Azm step/dir driver (Labeled S3)
#define AXIS3_ENABLE_PIN        PE0
#define AXIS3_M0_PIN            PC8              // SPI MOSI
#define AXIS3_M1_PIN            PC6              // SPI SCK
#define AXIS3_M2_PIN            PD4              // SPI CS
#define AXIS3_M3_PIN            PC7              // SPI MISO
#define AXIS3_STEP_PIN          PE2
#define AXIS3_DIR_PIN           PE1

// Axis4 RA/Azm step/dir driver (Labeled S4)
#define AXIS4_ENABLE_PIN        PB7
#define AXIS4_M0_PIN            PC8              // SPI MOSI
#define AXIS4_M1_PIN            PC6              // SPI SCK
#define AXIS4_M2_PIN            PD3              // SPI CS
#define AXIS4_M3_PIN            PC7              // SPI MISO
#define AXIS4_STEP_PIN          PB9
#define AXIS4_DIR_PIN           PB8

// Axis5 RA/Azm step/dir driver (Labeled S5)
#define AXIS5_ENABLE_PIN        PG13
#define AXIS5_M0_PIN            PC8              // SPI MOSI
#define AXIS5_M1_PIN            PC6              // SPI SCK
#define AXIS5_M2_PIN            PD2              // SPI CS
#define AXIS5_M3_PIN            PC7              // SPI MISO
#define AXIS5_STEP_PIN          PG9
#define AXIS5_DIR_PIN           PG10

// Axis6 RA/Azm step/dir driver (Labeled S6)
#define AXIS6_ENABLE_PIN        PG12
#define AXIS6_M0_PIN            PC8              // SPI MOSI
#define AXIS6_M1_PIN            PC6              // SPI SCK
#define AXIS6_M2_PIN            PA15             // SPI CS
#define AXIS6_M3_PIN            PC7              // SPI MISO
#define AXIS6_STEP_PIN          PG11
#define AXIS6_DIR_PIN           PD7

// Axis7 RA/Azm step/dir driver (Labeled S7)
#define AXIS7_ENABLE_PIN        PB5
#define AXIS7_M0_PIN            PC8              // SPI MOSI
#define AXIS7_M1_PIN            PC6              // SPI SCK
#define AXIS7_M2_PIN            PA9              // SPI CS
#define AXIS7_M3_PIN            PC7              // SPI MISO
#define AXIS7_STEP_PIN          PB4
#define AXIS7_DIR_PIN           PB3

// Axis8 RA/Azm step/dir driver (Labeled S8)
#define AXIS8_ENABLE_PIN        PB6
#define AXIS8_M0_PIN            PC8              // SPI MOSI
#define AXIS8_M1_PIN            PC6              // SPI SCK
#define AXIS8_M2_PIN            PA10             // SPI CS
#define AXIS8_M3_PIN            PC7              // SPI MISO
#define AXIS8_STEP_PIN          PG15
#define AXIS8_DIR_PIN           PB6

// ST4 interface (Connector Labeled EXP1 which has Gnd and +5V so near ideal)
#define ST4_RA_W_PIN            PD12             // ST4 RA- West
#define ST4_DEC_S_PIN           PD13             // ST4 DE- South
#define ST4_DEC_N_PIN           PD14             // ST4 DE+ North
#define ST4_RA_E_PIN            PE15             // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
