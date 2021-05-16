// -------------------------------------------------------------------------------------------------
// Pin map for CNC Shield Version 3 (with Arduino UNO)
#pragma once

#if defined(__AVR_ATmega328P__)

// Misc. pins
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN            OFF            // Default Pin for 1-wire bus
#endif
#define ADDON_GPIO0_PIN           OFF
#define ADDON_RESET_PIN           OFF

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PEC_SENSE_PIN             OFF
#define PEC_ANALG_PIN             OFF

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define STATUS_LED1_PIN           OFF            // Drain
#define STATUS_LED2_PIN           OFF            // Drain
#define RETICLE_LED_PIN           OFF            // Drain

// For a piezo buzzer
#define BUZZER_PIN                OFF

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define SENSE_PPS_PIN             OFF           // PPS time source, GPS for example

// The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking
#define SENSE_LIMIT_PIN           OFF

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN          OFF
#define AXIS1_M0_PIN              OFF
#define AXIS1_M1_PIN              OFF
#define AXIS1_M2_PIN              OFF
#define AXIS1_M3_PIN              OFF
#define AXIS1_STEP_PIN            OFF
#define AXIS1_DIR_PIN             OFF
#define AXIS1_DECAY_PIN           OFF
#define AXIS1_SENSE_HOME_PIN      OFF

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN          OFF
#define AXIS2_M0_PIN              OFF
#define AXIS2_M1_PIN              OFF
#define AXIS2_M2_PIN              OFF
#define AXIS2_M3_PIN              OFF
#define AXIS2_STEP_PIN            OFF
#define AXIS2_DIR_PIN             OFF
#define AXIS2_DECAY_PIN           OFF
#define AXIS2_SENSE_HOME_PIN      OFF

// For rotator stepper driver
#define AXIS3_ENABLE_PIN          OFF
#define AXIS3_M0_PIN              OFF
#define AXIS3_M1_PIN              OFF
#define AXIS3_M2_PIN              OFF
#define AXIS3_M3_PIN              OFF
#define AXIS3_STEP_PIN            OFF
#define AXIS3_DIR_PIN             OFF

// For focuser1 stepper driver
#define AXIS4_ENABLE_PIN          OFF
#define AXIS4_M0_PIN              OFF
#define AXIS4_M1_PIN              OFF
#define AXIS4_M2_PIN              OFF
#define AXIS4_M3_PIN              OFF
#define AXIS4_STEP_PIN            OFF
#define AXIS4_DIR_PIN             OFF

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN          OFF
#define AXIS5_M0_PIN              OFF
#define AXIS5_M1_PIN              OFF
#define AXIS5_M2_PIN              OFF
#define AXIS5_M3_PIN              OFF
#define AXIS5_STEP_PIN            OFF
#define AXIS5_DIR_PIN             OFF

// ST4 interface
#define ST4_RA_W_PIN              OFF
#define ST4_DEC_S_PIN             OFF
#define ST4_DEC_N_PIN             OFF
#define ST4_RA_E_PIN              OFF

#else
#error "Wrong processor for this configuration!"

#endif
