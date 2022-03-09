// -------------------------------------------------------------------------------------------------
// Pin map for OnStep Instein ESP1 (ESP32)
#pragma once

#if defined(ESP32)

#warning "This an an experimental PINMAP, use at your own risk!!!"

// Serial ports (see Pins.defaults.h for SERIAL_A)
// Serial0: RX Pin GPIO3, TX Pin GPIO1 (to USB serial adapter)
// Serial1: RX1 Pin GPIO21, TX1 Pin GPIO22
// Serial2: RX2 Pin GPIO16, TX2 Pin GPIO17

#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B              Serial2
#endif
#ifndef SERIAL_C_BAUD_DEFAULT
  #define SERIAL_C_BAUD_DEFAULT 9600
  #define SERIAL_C              Serial1
  #define SERIAL_C_RX           21
  #define SERIAL_C_TX           22
#endif

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define AUX3_PIN                21               // Home SW for Axis1 (or I2C SDA)
#define AUX4_PIN                22               // Home SW for Axis2 (or I2C SCL)
#define AUX7_PIN                39               // Limit SW, PPS, etc.
#define AUX8_PIN                25               // Status LED, Status2 LED, Reticle LED, Tone, OneWire, etc.

// Misc. pins
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN          AUX8_PIN         // Default Pin for 1-wire bus
#endif

// two pins are custom pins used by Instein to allow writing ESP8266 flash
#define ADDON_TRIGR_PIN         36               // ESP8266 pin to trigger serial passthrough mode
#define ADDON_GPIO0_PIN         OFF              // ESP8266 GPIO0 (disabled)
#define ADDON_RESET_PIN         2                // ESP8266 RST

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PEC_SENSE_PIN           36               // PEC Sense, analog (A0) or digital (GPIO36)

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define STATUS_LED_PIN          AUX8_PIN         // Default LED Cathode (-)
#define MOUNT_STATUS_LED_PIN    AUX8_PIN         // Default LED Cathode (-)
#define STATUS_ROTATOR_LED_PIN  AUX8_PIN         // Default LED Cathode (-)
#define STATUS_FOCUSER_LED_PIN  AUX8_PIN         // Default LED Cathode (-)
#define RETICLE_LED_PIN         AUX8_PIN         // Default LED Cathode (-)

// For a piezo buzzer
#define STATUS_BUZZER_PIN       AUX8_PIN         // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PPS_SENSE_PIN           AUX7_PIN         // PPS time source, GPS for example

// The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking
#define LIMIT_SENSE_PIN         AUX7_PIN

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN        12
#define AXIS1_M0_PIN            13               // SPI MOSI
#define AXIS1_M1_PIN            14               // SPI SCK
#define AXIS1_M2_PIN            23               // SPI CS (UART TX)
#define AXIS1_M3_PIN            4                // SPI MISO (UART RX)
#define AXIS1_DECAY_PIN         AXIS1_M2_PIN
#define AXIS1_STEP_PIN          18
#define AXIS1_DIR_PIN           19
#define AXIS1_SENSE_HOME_PIN    AUX3_PIN
#define AXIS1_FAULT_PIN         AXIS1_M3_PIN

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN        SHARED
#define AXIS2_M0_PIN            13               // SPI MOSI
#define AXIS2_M1_PIN            14               // SPI SCK
#define AXIS2_M2_PIN            5                // SPI CS (UART TX)
#define AXIS2_M3_PIN            4                // SPI MISO (UART RX)
#define AXIS2_DECAY_PIN         AXIS2_M2_PIN
#define AXIS2_STEP_PIN          27
#define AXIS2_DIR_PIN           26
#define AXIS2_SENSE_HOME_PIN    AUX4_PIN
#define AXIS2_FAULT_PIN         AXIS2_M3_PIN

// For rotator stepper driver
#define AXIS3_ENABLE_PIN        OFF
#define AXIS3_M0_PIN            OFF              // SPI MOSI
#define AXIS3_M1_PIN            OFF              // SPI SCK
#define AXIS3_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS3_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS3_STEP_PIN          2
#define AXIS3_DIR_PIN           15

// For focuser1 stepper driver
#define AXIS4_ENABLE_PIN        4
#define AXIS4_M0_PIN            OFF              // SPI MOSI
#define AXIS4_M1_PIN            OFF              // SPI SCK
#define AXIS4_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS4_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS4_STEP_PIN          19
#define AXIS4_DIR_PIN           15

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN        OFF
#define AXIS5_M0_PIN            OFF              // SPI MOSI
#define AXIS5_M1_PIN            OFF              // SPI SCK
#define AXIS5_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS5_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS5_STEP_PIN          2
#define AXIS5_DIR_PIN           15

// ST4 interface
#define ST4_RA_W_PIN            34               // ST4 RA- West
#define ST4_DEC_S_PIN           32               // ST4 DE- South
#define ST4_DEC_N_PIN           33               // ST4 DE+ North
#define ST4_RA_E_PIN            35               // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
