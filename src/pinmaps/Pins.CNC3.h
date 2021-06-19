// -------------------------------------------------------------------------------------------------
// Pin map for CNC Shield Version 3 (with WeMos D1 R32)
#pragma once

// Note: SPI for TMC drivers: GPIO32 (MOSI), GPIO33 (SCK), GPIO15 (CS Axis1) on WeMos D1 R32 PCB are more difficult to wire into but optional. GPIO18 (CS Axis2) is on top.

#if defined(ESP32)

// Serial2 (SerialB) needs to be re-mapped to GPIO13 (RX) and GPIO5 (TX)
#define SERIAL_B_RX               13
#define SERIAL_B_TX               5

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define Aux2                      18             // ESP8266 RST control, or Axis2 CS support
#define Aux3                      21             // Home SW for Axis1, or I2C SDA {assuming default SCL/SDA pins are present}
#define Aux4                      22             // Home SW for Axis2, or I2C SCL
#define Aux7                      39             // Limit SW, PPS, etc.
#define Aux8                      23             // 1-Wire, Status LED1, Status LED2, Reticle LED, etc.

// Misc. pins
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN            Aux8           // Default Pin for 1-wire bus
#endif
#define ADDON_GPIO0_PIN           27             // ESP8266 GPIO0 (Dir2)
#define ADDON_RESET_PIN           Aux2           // ESP8266 RST

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PEC_SENSE_PIN             36             // [input only 36] (also analog A0)

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LED_STATUS_PIN            Aux8           // Default LED Cathode (-)
#define LED_MOUNT_STATUS_PIN      Aux8           // Default LED Cathode (-)
#define LED_ROTATOR_STATUS_PIN    Aux8           // Default LED Cathode (-)
#define LED_FOCUSER_STATUS_PIN    Aux8           // Default LED Cathode (-)
#define RETICLE_LED_PIN           Aux8           // Default LED Cathode (-)

// For a piezo buzzer
#define BUZZER_PIN                2

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define SENSE_PPS_PIN             Aux7           // PPS time source, GPS for example

// The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking
#define SENSE_LIMIT_PIN           Aux7

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN          12             // [must be low at boot 12]
#define AXIS1_M0_PIN              32             // SPI MOSI
#define AXIS1_M1_PIN              33             // SPI SCK
#define AXIS1_M2_PIN              15             // SPI CS
#define AXIS1_M3_PIN              Aux2           // SPI MISO/Fault
#define AXIS1_STEP_PIN            26
#define AXIS1_DIR_PIN             16
#define AXIS1_DECAY_PIN           AXIS1_M2_PIN
#define AXIS1_SENSE_HOME_PIN      Aux3

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN          SHARED
#define AXIS2_M0_PIN              32             // SPI MOSI
#define AXIS2_M1_PIN              33             // SPI SCK
#define AXIS2_M2_PIN              0              // SPI CS
#define AXIS2_M3_PIN              Aux2           // SPI MISO
#define AXIS2_STEP_PIN            25
#define AXIS2_DIR_PIN             27
#define AXIS2_DECAY_PIN           AXIS2_M2_PIN
#define AXIS2_SENSE_HOME_PIN      Aux4

// For rotator stepper driver
#define AXIS3_ENABLE_PIN          SHARED
#define AXIS3_M0_PIN              OFF            // SPI MOSI
#define AXIS3_M1_PIN              OFF            // SPI SCK
#define AXIS3_M2_PIN              OFF            // SPI CS
#define AXIS3_M3_PIN              OFF            // SPI MISO
#define AXIS3_STEP_PIN            19
#define AXIS3_DIR_PIN             14

// For focuser1 stepper driver
#define AXIS4_ENABLE_PIN          SHARED
#define AXIS4_M0_PIN              OFF            // SPI MOSI
#define AXIS4_M1_PIN              OFF            // SPI SCK
#define AXIS4_M2_PIN              OFF            // SPI CS
#define AXIS4_M3_PIN              OFF            // SPI MISO
#define AXIS4_STEP_PIN            17
#define AXIS4_DIR_PIN             14

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN          SHARED
#define AXIS5_M0_PIN              OFF            // SPI MOSI
#define AXIS5_M1_PIN              OFF            // SPI SCK
#define AXIS5_M2_PIN              OFF            // SPI CS
#define AXIS5_M3_PIN              OFF            // SPI MISO
#define AXIS5_STEP_PIN            19
#define AXIS5_DIR_PIN             14

// ST4 interface
#define ST4_RA_W_PIN              34             // ST4 RA- West [input only 34]
#define ST4_DEC_S_PIN             18             // ST4 DE- South
#define ST4_DEC_N_PIN             4              // ST4 DE+ North
#define ST4_RA_E_PIN              35             // ST4 RA+ East [input only 35]

#else
#error "Wrong processor for this configuration!"

#endif
