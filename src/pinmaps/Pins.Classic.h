// -------------------------------------------------------------------------------------------------
// Pin map for legacy OnStep Classic (Teensy3.x)
#pragma once

#if defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)

// Serial ports (see Pins.defaults.h for SERIAL_A)
// Serial1: RX1 Pin 0, TX1 Pin 1

#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B               Serial1
#endif
#if SERIAL_C_BAUD_DEFAULT != OFF
  #error "Configuration (Config.h): SerialC isn't supported, disable this option."
#endif

// The PEC index sense resets the PEC index then waits for 60 seconds before allowing another reset
#ifdef PEC_SENSE_ANALOG
  #define PEC_SENSE_PIN          14              // PEC Sense, analog
#else
  #define PEC_SENSE_PIN          2               // PEC Sense, digital
#endif

// The limit switch sense is a 3.3V logic input which uses the internal pull up, shorted to ground it stops gotos/tracking
#define LIMIT_SENSE_PIN          3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define STATUS_LED_VCC_PIN       8               // LED Annode
#define STATUS_LED_PIN           9               // Default LED Cathode (-)
#define MOUNT_STATUS_LED_PIN     7               // Default LED Cathode (-)
#define STATUS_ROTATOR_LED_PIN   7               // Default LED Cathode (-)
#define STATUS_FOCUSER_LED_PIN   7               // Default LED Cathode (-)
#define RETICLE_LED_PIN          9               // Default LED Cathode (-)

// For a piezo buzzer
#define STATUS_BUZZER_PIN        29              // Tone

// The PPS pin is a logic level input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PPS_SENSE_PIN            23              // PPS time source, GPS for example

// Obsolete pins that would power stepper drivers in the old days
#define POWER_SUPPLY_PINS_OFF
#define Axis15vPin               11              // Pin 11 (3.3V)
#define Axis25vPin               5               // Pin 5 (3.3V)
#define Axis2GndPin              7               // Pin 7 (GND)

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN         16
#define AXIS1_M0_PIN             13              // SPI MOSI
#define AXIS1_M1_PIN             14              // SPI CLK
#define AXIS1_M2_PIN             15              // SPI CS (UART TX)
#define AXIS1_M3_PIN             17              // SPI MISO (UART RX)
#define AXIS1_STEP_PIN           12
#define AXIS1_DIR_PIN            10
#define AXIS1_FAULT_PIN          17

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN         21
#define AXIS2_M0_PIN             18              // SPI MOSI
#define AXIS2_M1_PIN             19              // SPI CLK
#define AXIS2_M2_PIN             20              // SPI CS (UART TX)
#define AXIS2_M3_PIN             22              // SPI MISO (UART RX)
#define AXIS2_STEP_PIN           6
#define AXIS2_DIR_PIN            4
#define AXIS2_FAULT_PIN          22

// For rotator stepper driver
#define AXIS3_ENABLE_PIN         OFF
#define AXIS3_STEP_PIN           30
#define AXIS3_DIR_PIN            33

// For focuser1 stepper driver  
#define AXIS4_ENABLE_PIN         OFF
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
  // teensy3.5/3.6
  #define AXIS4_STEP_PIN         34
  #define AXIS4_DIR_PIN          35
#else
  // teensy3.2
  #define AXIS4_STEP_PIN         31
  #define AXIS4_DIR_PIN          32
#endif

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN         OFF
#define AXIS5_STEP_PIN           30
#define AXIS5_DIR_PIN            33

// ST4 interface
#define ST4_RA_W_PIN             24              // ST4 RA- West
#define ST4_DEC_S_PIN            25              // ST4 DE- South
#define ST4_DEC_N_PIN            26              // ST4 DE+ North
#define ST4_RA_E_PIN             27              // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
