// -------------------------------------------------------------------------------------------------
// Pin map for OnStep Instein ESP1 (ESP32)
#pragma once

#if defined(ESP32)

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define Aux1                    26               // ESP8266 GPIO0 control (Axis2DirPin)
#define Aux2                    4                // MISO for Axis1&2, or ESP8266 RST control, or Axis4 EN support
#define Aux3                    21               // Home SW for Axis1 (or I2C SDA)
#define Aux4                    22               // Home SW for Axis2 (or I2C SCL)
#define Aux7                    39               // Limit SW, PPS, etc.
#define Aux8                    25               // Status LED, Status2 LED, Reticle LED, Tone, OneWire, etc.

// Misc. pins
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN          Aux8             // Default Pin for 1-wire bus
#endif

// two pins are custom pins used by Instein to allow writing ESP8266 flash
#define ADDON_TRIGR_PIN         36               // ESP8266 pin to trigger serial passthrough mode
#define ADDON_GPIO0_PIN         OFF              // ESP8266 GPIO0 (disabled)
#define ADDON_RESET_PIN         2                // ESP8266 RST

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PEC_SENSE_PIN           36               // PEC Sense, analog (A0) or digital (GPIO36)

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define STATUS_LED1_PIN         Aux8             // Drain
#define STATUS_LED2_PIN         Aux8             // Drain
#define RETICLE_LED_PIN         Aux8             // Drain

// For a piezo buzzer
#define BUZZER_PIN              Aux8             // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define SENSE_PPS_PIN           Aux7             // PPS time source, GPS for example

// The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking
#define SENSE_LIMIT_PIN         Aux7

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN        12
#define AXIS1_M0_PIN            13               // SPI MOSI
#define AXIS1_M1_PIN            14               // SPI SCK
#define AXIS1_M2_PIN            23               // SPI CS
#define AXIS1_M3_PIN            4                // SPI MISO
#define AXIS1_DECAY_PIN         AXIS1_M2_PIN
#define AXIS1_STEP_PIN          18
#define AXIS1_DIR_PIN           19
#define AXIS1_SENSE_HOME_PIN    Aux3
#define AXIS1_FAULT_PIN         AXIS2_M3_PIN

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN        SHARED
#define AXIS2_M0_PIN            13               // SPI MOSI
#define AXIS2_M1_PIN            14               // SPI SCK
#define AXIS2_M2_PIN            5                // SPI CS
#define AXIS2_M3_PIN            4                // SPI MISO
#define AXIS2_DECAY_PIN         AXIS2_M2_PIN
#define AXIS2_STEP_PIN          27
#define AXIS2_DIR_PIN           26
#define AXIS2_SENSE_HOME_PIN    Aux4
#define AXIS2_FAULT_PIN         AXIS2_M3_PIN

// For rotator stepper driver
#define AXIS3_ENABLE_PIN        OFF
#define AXIS3_M0_PIN            OFF              // SPI MOSI
#define AXIS3_M1_PIN            OFF              // SPI SCK
#define AXIS3_M2_PIN            OFF              // SPI CS
#define AXIS3_M3_PIN            OFF              // SPI MISO
#define AXIS3_STEP_PIN          2
#define AXIS3_DIR_PIN           15

// For focuser1 stepper driver
#define AXIS4_ENABLE_PIN        Aux2
#define AXIS4_M0_PIN            OFF              // SPI MOSI
#define AXIS4_M1_PIN            OFF              // SPI SCK
#define AXIS4_M2_PIN            OFF              // SPI CS
#define AXIS4_M3_PIN            OFF              // SPI MISO
#define AXIS4_STEP_PIN          19
#define AXIS4_DIR_PIN           15

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN        OFF
#define AXIS5_M0_PIN            OFF              // SPI MOSI
#define AXIS5_M1_PIN            OFF              // SPI SCK
#define AXIS5_M2_PIN            OFF              // SPI CS
#define AXIS5_M3_PIN            OFF              // SPI MISO
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
