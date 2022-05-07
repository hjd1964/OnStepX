// -------------------------------------------------------------------------------------------------
// Pinmap for OnStep MaxESP Version 4.x (ESP32-S2)

// ********* EXPERIMENTAL, THIS PINMAP MAY BE CHANGED OR REMOVED AT ANY TIME **********

#pragma once

#if defined(ESP32)

// Serial ports (see Pins.defaults.h for SERIAL_A)
// Serial0: RX Pin GPIO44, TX Pin GPIO43 (to USB serial adapter)
// Serial1: RX1 Pin GPIO18, TX1 Pin GPIO17

#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B              Serial1
#endif
#if SERIAL_C_BAUD_DEFAULT != OFF
  #error "Configuration (Config.h): SerialC isn't supported, disable this option."
#endif

#if DRIVER_UART_HARDWARE_SERIAL == ON
  #error "Configuration (Config.h): TMC2209 hardware serial isn't supported, disable this option."
#else
  // Use the following settings for any TMC2209 that may be present
  #define SERIAL_TMC            SoftSerial       // Use software serial with RX on M2 and TX on M3 of axis
  #define SERIAL_TMC_BAUD       115200           // Baud rate
  #define SERIAL_TMC_NO_RX                       // Recieving data doesn't work with software serial
#endif

// Hint that the direction pins are shared
#define SHARED_DIRECTION_PINS

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define AUX3_PIN                20               // Home SW for Axis1
#define AUX4_PIN                19               // Home SW for Axis2
#define AUX7_PIN                42               // Limit SW, PPS, MISO, etc.
#define AUX8_PIN                41               // 1-Wire, Status LED, Status2 LED, Reticle LED, etc.

// Misc. pins
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN          AUX8_PIN         // Default Pin for OneWire bus
#endif

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#ifndef PEC_SENSE_PIN
  #define PEC_SENSE_PIN         14               // PEC Sense, analog or digital
#endif

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#ifndef STATUS_LED_PIN
  #define STATUS_LED_PIN        AUX8_PIN         // Default LED Cathode (-)
#endif
#define MOUNT_STATUS_LED_PIN    STATUS_LED_PIN   // Default LED Cathode (-)
#define STATUS_ROTATOR_LED_PIN  STATUS_LED_PIN   // Default LED Cathode (-)
#define STATUS_FOCUSER_LED_PIN  STATUS_LED_PIN   // Default LED Cathode (-)
#ifndef RETICLE_LED_PIN 
  #define RETICLE_LED_PIN       STATUS_LED_PIN   // Default LED Cathode (-)
#endif

// For a piezo buzzer
#ifndef STATUS_BUZZER_PIN
  #define STATUS_BUZZER_PIN     AUX8_PIN         // Tone
#endif

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#ifndef PPS_SENSE_PIN
  #define PPS_SENSE_PIN         AUX7_PIN         // PPS time source, GPS for example
#endif

// The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking
#ifndef LIMIT_SENSE_PIN
  #define LIMIT_SENSE_PIN       AUX7_PIN
#endif

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN        1                // Enable
#define AXIS1_M0_PIN            2                // SPI MOSI
#define AXIS1_M1_PIN            3                // SPI SCK
#define AXIS1_M2_PIN            4                // SPI CS (UART TX)
#define AXIS1_M3_PIN            AUX7_PIN         // SPI MISO (UART RX)
#define AXIS1_STEP_PIN          5
#define AXIS1_DIR_PIN           0                // [must be high at boot 0]
#define AXIS1_DECAY_PIN         AXIS1_M2_PIN
#ifndef AXIS1_SENSE_HOME_PIN
  #define AXIS1_SENSE_HOME_PIN  AUX3_PIN
#endif

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN        SHARED
#define AXIS2_M0_PIN            6                // SPI MOSI
#define AXIS2_M1_PIN            7                // SPI SCK
#define AXIS2_M2_PIN            10               // SPI CS (UART TX)
#define AXIS2_M3_PIN            AUX7_PIN         // SPI MISO (UART RX)
#define AXIS2_STEP_PIN          11
#define AXIS2_DIR_PIN           12
#define AXIS2_DECAY_PIN         AXIS2_M2_PIN
#ifndef AXIS2_SENSE_HOME_PIN
  #define AXIS2_SENSE_HOME_PIN  AUX4_PIN
#endif

// For rotator stepper driver
#define AXIS3_ENABLE_PIN        SHARED
#define AXIS3_M0_PIN            33               // SPI MOSI
#define AXIS3_M1_PIN            34               // SPI SCK
#define AXIS3_M2_PIN            35               // SPI CS (UART TX)
#define AXIS3_M3_PIN            AUX7_PIN         // SPI MISO (UART RX)
#define AXIS3_STEP_PIN          36
#define AXIS3_DIR_PIN           37               // DIR shared wth Axis4
#define AXIS3_DECAY_PIN         AXIS3_M2_PIN

// For focuser1 stepper driver
#define AXIS4_ENABLE_PIN        SHARED
#define AXIS4_M0_PIN            38               // SPI MOSI
#define AXIS4_M1_PIN            39               // SPI SCK
#define AXIS4_M2_PIN            40               // SPI CS (UART TX)
#define AXIS4_M3_PIN            AUX7_PIN         // SPI MISO (UART RX)
#define AXIS4_STEP_PIN          45               // [must be low at boot]
#define AXIS4_DIR_PIN           37               // DIR shared wth Axis3
#define AXIS4_DECAY_PIN         AXIS4_M2_PIN

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN        SHARED
#define AXIS5_M0_PIN            33               // SPI MOSI
#define AXIS5_M1_PIN            34               // SPI SCK
#define AXIS5_M2_PIN            35               // SPI CS (UART TX)
#define AXIS5_M3_PIN            AUX7_PIN         // SPI MISO (UART RX)
#define AXIS5_STEP_PIN          36
#define AXIS5_DIR_PIN           37
#define AXIS5_DECAY_PIN         AXIS5_M2_PIN

// ST4 interface
#define ST4_RA_W_PIN            26               // ST4 RA- West [PSRAM CS1 signal, assuming it will still work]
#define ST4_DEC_S_PIN           15               // ST4 DE- South
#define ST4_DEC_N_PIN           21               // ST4 DE+ North
#define ST4_RA_E_PIN            16               // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
