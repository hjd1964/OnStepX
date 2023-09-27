// -------------------------------------------------------------------------------------------------
// Pin map for OnStep JTW PCB rev 2.1 STM32F411CE PCB (or the Blackpill https://github.com/WeActTC/MiniF4-STM32F4x1)
#pragma once

#if defined(STM32F411xE) || defined(STM32F401xC)

#if SERIAL_A_BAUD_DEFAULT != OFF
  #define SERIAL_A              HardSerial
  #define SERIAL_A_RX           PA10
  #define SERIAL_A_TX           PA9
#endif
#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B              HardSerial
  #define SERIAL_B_RX           PA3
  #define SERIAL_B_TX           PA2
#endif

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define AUX0_PIN                PB12             // Status LED
#define AUX2_PIN                PC14             // PPS
#define AUX5_PIN                PA9              // TX1 
#define AUX6_PIN                PA10             // RX1

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define STATUS_LED_PIN          AUX0_PIN         // Default LED Cathode (-)
#define MOUNT_LED_PIN           AUX0_PIN         // Default LED Cathode (-)
#ifndef RETICLE_LED_PIN
  #define RETICLE_LED_PIN       AUX8_PIN         // Default LED Cathode (-)
#endif

// For a piezo buzzer
#define STATUS_BUZZER_PIN       PC15             // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#ifndef PPS_SENSE_PIN
  #define PPS_SENSE_PIN       AUX2_PIN         // PPS time source, GPS for example (MaxSTM version 3.61 and later)
#endif

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN        PB13
#define AXIS1_M0_PIN            PA7              // SPI MOSI
#define AXIS1_M1_PIN            PA5              // SPI SCK
#define AXIS1_M2_PIN            PA1              // SPI CS (UART TX)
#define AXIS1_M3_PIN            PA6              // SPI MISO (UART RX)
#define AXIS1_STEP_PIN          PB10
#define AXIS1_DIR_PIN           PB2

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN        PB14
#define AXIS2_M0_PIN            PA7              // SPI MOSI
#define AXIS2_M1_PIN            PA5              // SPI SCK
#define AXIS2_M2_PIN            PA0              // SPI CS (UART TX)
#define AXIS2_M3_PIN            PA6              // SPI MISO (UART RX)
#define AXIS2_STEP_PIN          PA4
#define AXIS2_DIR_PIN           PB0

// ST4 interface
#define ST4_RA_W_PIN            PA15             // ST4 RA- West
#define ST4_DEC_S_PIN           PB3              // ST4 DE- South
#define ST4_DEC_N_PIN           PB4              // ST4 DE+ North
#define ST4_RA_E_PIN            PB5              // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
