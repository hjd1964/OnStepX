// -------------------------------------------------------------------------------------------------
// Pin map for CNC Shield Version 3 (with WeMos D1 R32)
#pragma once

// Note: SPI for TMC drivers: GPIO32 (MOSI), GPIO33 (SCK), GPIO15 (CS Axis1)
// on WeMos D1 R32 PCB are somewhat difficult to wire into, but optional
// GPIO18 (CS Axis2) is on top

#if defined(ESP32)

// Serial0: RX Pin GPIO3, TX Pin GPIO1 (to USB serial adapter)
// Serial1: RX1 Pin GPIO10, TX1 Pin GPIO9 (on SPI Flash pins, must be moved to be used)
// Serial2: RX2 Pin GPIO13, TX2 Pin GPIO5

#if SERIAL_A_BAUD_DEFAULT != OFF
  #define SERIAL_A              Serial
#endif
#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B              Serial2
  #define SERIAL_B_RX           13
  #define SERIAL_B_TX           5
#endif

// Specify the ESP32 I2C pins
#define I2C_SDA_PIN             21
#define I2C_SCL_PIN             22

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define AUX2_PIN                18               // ESP8266 RST control, or SPI MISO Axis1&2
#define AUX3_PIN                21               // Home SW for Axis1, or I2C SDA {assuming default SCL/SDA pins are present}
#define AUX4_PIN                22               // Home SW for Axis2, or I2C SCL
#define AUX7_PIN                39               // Limit SW, PPS, etc.
#define AUX8_PIN                23               // 1-Wire, Status LED1, Status LED2, Reticle LED, etc.

// Misc. pins
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN          AUX8_PIN         // Default Pin for 1-wire bus
#endif
#define ADDON_GPIO0_PIN         27               // ESP8266 GPIO0 (Dir2)
#define ADDON_RESET_PIN         AUX2_PIN         // ESP8266 RST

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#ifndef PEC_SENSE_PIN
  #define PEC_SENSE_PIN         36               // [input only 36] (also analog A0)
#endif

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#ifndef STATUS_LED_PIN
  #define STATUS_LED_PIN        AUX8_PIN         // Default LED Cathode (-)
#endif
#define MOUNT_LED_PIN           STATUS_LED_PIN   // Default LED Cathode (-)
#define RETICLE_LED_PIN         STATUS_LED_PIN   // Default LED Cathode (-)

// For a piezo buzzer
#ifndef STATUS_BUZZER_PIN
  #define STATUS_BUZZER_PIN     2                // [must be low at boot 2]
#endif

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#ifndef PPS_SENSE_PIN
  #define PPS_SENSE_PIN         AUX7_PIN         // PPS time source, GPS for example
#endif

// The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking
#ifndef LIMIT_SENSE_PIN
  #define LIMIT_SENSE_PIN       AUX7_PIN
#endif

#define SHARED_DIRECTION_PINS                    // Hint that the direction pins are shared
#define SHARED_ENABLE_PIN       12               // Hint that the enable pins are shared

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN        SHARED           // [must be low at boot 12]
#define AXIS1_M0_PIN            32               // SPI MOSI
#define AXIS1_M1_PIN            33               // SPI SCK
#define AXIS1_M2_PIN            15               // SPI CS (UART TX)
#define AXIS1_M3_PIN            AUX2_PIN         // SPI MISO (UART RX)
#define AXIS1_STEP_PIN          26
#define AXIS1_DIR_PIN           16
#ifndef AXIS1_SENSE_HOME_PIN
  #define AXIS1_SENSE_HOME_PIN  AUX3_PIN
#endif

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN        SHARED
#define AXIS2_M0_PIN            32               // SPI MOSI
#define AXIS2_M1_PIN            33               // SPI SCK
#define AXIS2_M2_PIN            0                // [must be high at boot 0] SPI CS (UART TX)
#define AXIS2_M3_PIN            AUX2_PIN         // SPI MISO (UART RX)
#define AXIS2_STEP_PIN          25
#define AXIS2_DIR_PIN           27
#ifndef AXIS2_SENSE_HOME_PIN
  #define AXIS2_SENSE_HOME_PIN  AUX4_PIN
#endif

// For rotator stepper driver
#define AXIS3_ENABLE_PIN        SHARED
#define AXIS3_M0_PIN            OFF              // SPI MOSI
#define AXIS3_M1_PIN            OFF              // SPI SCK
#define AXIS3_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS3_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS3_STEP_PIN          19
#define AXIS3_DIR_PIN           14

// For focuser1 stepper driver
#define AXIS4_ENABLE_PIN        SHARED
#define AXIS4_M0_PIN            OFF              // SPI MOSI
#define AXIS4_M1_PIN            OFF              // SPI SCK
#define AXIS4_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS4_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS4_STEP_PIN          17
#define AXIS4_DIR_PIN           14

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN        SHARED
#define AXIS5_M0_PIN            OFF              // SPI MOSI
#define AXIS5_M1_PIN            OFF              // SPI SCK
#define AXIS5_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS5_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS5_STEP_PIN          19
#define AXIS5_DIR_PIN           14

// ST4 interface
#define ST4_RA_W_PIN            34               // [input only 34] ST4 RA- West
#define ST4_DEC_S_PIN           18               // ST4 DE- South
#define ST4_DEC_N_PIN           4                // ST4 DE+ North
#define ST4_RA_E_PIN            35               // [input only 35] ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
