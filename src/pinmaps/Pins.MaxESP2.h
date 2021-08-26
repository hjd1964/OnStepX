// -------------------------------------------------------------------------------------------------
// Pin map for OnStep MaxESP Version 2.x (ESP32S)
#pragma once

#if defined(ESP32)

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define AUX1_PIN                12               // pullup driver RST, SPI MISO/Fault
#define AUX3_PIN                21               // Home SW, I2C SDA
#define AUX4_PIN                22               // Home SW, I2C SCL
#define AUX7_PIN                39               // PPS, Limit SW, etc.
#define AUX8_PIN                25               // 1-Wire, Status LED, Status2 LED, Reticle LED, Tone, etc.

// Misc. pins
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN          AUX8_PIN         // Default Pin for 1-Wire bus
#endif

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PEC_SENSE_PIN           36               // PEC Sense, analog (A0) or digital (GPIO36)

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define STATUS_LED_PIN          AUX8_PIN         // Default LED Cathode (-)
#define STATUS_MOUNT_LED_PIN    AUX8_PIN         // Default LED Cathode (-)
#define STATUS_ROTATOR_LED_PIN  AUX8_PIN         // Default LED Cathode (-)
#define STATUS_FOCUSER_LED_PIN  AUX8_PIN         // Default LED Cathode (-)
#define RETICLE_LED_PIN         AUX8_PIN         // Default LED Cathode (-)

// For a piezo buzzer
#define STATUS_BUZZER_PIN       AUX8_PIN         // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define SENSE_PPS_PIN           AUX7_PIN         // PPS time source, GPS for example

// The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking
#define SENSE_LIMIT_PIN         AUX7_PIN

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN        4
#define AXIS1_M0_PIN            13               // SPI MOSI
#define AXIS1_M1_PIN            14               // SPI SCK
#define AXIS1_M2_PIN            23               // SPI CS
#define AXIS1_M3_PIN            AUX1_PIN         // SPI MISO
#define AXIS1_STEP_PIN          18
#define AXIS1_DIR_PIN           19
#define AXIS1_DECAY_PIN         AXIS1_M2_PIN
#define AXIS1_FAULT_PIN         AXIS1_M3_PIN
#define AXIS1_SENSE_HOME_PIN    AUX3_PIN

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN        SHARED
#define AXIS2_M0_PIN            13               // SPI MOSI
#define AXIS2_M1_PIN            14               // SPI SCK
#define AXIS2_M2_PIN            5                // SPI CS
#define AXIS2_M3_PIN            AUX1_PIN         // SPI MISO
#define AXIS2_STEP_PIN          27
#define AXIS2_DIR_PIN           26
#define AXIS2_DECAY_PIN         AXIS2_M2_PIN
#define AXIS2_FAULT_PIN         AXIS2_M3_PIN
#define AXIS2_SENSE_HOME_PIN    AUX4_PIN

// For rotator stepper driver
#define AXIS3_ENABLE_PIN        0
#define AXIS3_M0_PIN            OFF              // SPI MOSI
#define AXIS3_M1_PIN            OFF              // SPI SCK
#define AXIS3_M2_PIN            OFF              // SPI CS
#define AXIS3_M3_PIN            OFF              // SPI MISO
#define AXIS3_STEP_PIN          2
#define AXIS3_DIR_PIN           15

// For focuser1 stepper driver
#define AXIS4_ENABLE_PIN        0
#define AXIS4_M0_PIN            OFF              // SPI MOSI
#define AXIS4_M1_PIN            OFF              // SPI SCK
#define AXIS4_M2_PIN            OFF              // SPI CS
#define AXIS4_M3_PIN            OFF              // SPI MISO
#define AXIS4_STEP_PIN          2
#define AXIS4_DIR_PIN           15

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN        OFF
#define AXIS5_M0_PIN            OFF              // SPI MOSI
#define AXIS5_M1_PIN            OFF              // SPI SCK
#define AXIS5_M2_PIN            OFF              // SPI CS
#define AXIS5_M3_PIN            OFF              // SPI MISO
#define AXIS5_STEP_PIN          OFF
#define AXIS5_DIR_PIN           OFF

// ST4 interface
#define ST4_RA_W_PIN            34               // ST4 RA- West
#define ST4_DEC_S_PIN           32               // ST4 DE- South
#define ST4_DEC_N_PIN           33               // ST4 DE+ North
#define ST4_RA_E_PIN            35               // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
