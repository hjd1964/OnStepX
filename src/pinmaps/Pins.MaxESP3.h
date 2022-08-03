// -------------------------------------------------------------------------------------------------
// Pin map for OnStep MaxESP Version 3.x (ESP32S)
#pragma once

#if defined(ESP32)

// Serial ports (see Pins.defaults.h for SERIAL_A)
// Serial0: RX Pin GPIO3, TX Pin GPIO1 (to USB serial adapter)
// Serial1: RX1 Pin GPIO10, TX1 Pin GPIO9 (on SPI Flash pins, must be moved to be used)
// Serial2: RX2 Pin GPIO16, TX2 Pin GPIO17

#if SERIAL_A_BAUD_DEFAULT != OFF
  #define SERIAL_A              Serial
#endif
#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B              Serial2
#endif

// Use the following settings for any TMC UART driver (TMC2209U) that may be present
#define DRIVER_UART_ADDRESS_REMAP_AXIS5          // Map driver axis5 to axis3 in hardware serial mode

#ifndef DRIVER_UART_HARDWARE_SERIAL
  #define DRIVER_UART_HARDWARE_SERIAL OFF        // Default is software serial for this board
#endif

#if DRIVER_UART_HARDWARE_SERIAL == ON
  #define SERIAL_TMC            Serial1          // Use a single hardware serial port to up to four drivers
  #define SERIAL_TMC_BAUD       460800           // Baud rate
  #define SERIAL_TMC_RX         39               // Recieving data
  #define SERIAL_TMC_TX         23               // Transmit data
#elif DRIVER_UART_HARDWARE_SERIAL == OFF
  #define SERIAL_TMC            SoftSerial       // Use software serial with TX on M3 (CS) of each axis
  #define SERIAL_TMC_BAUD       115200           // Baud rate
  #define SERIAL_TMC_NO_RX                       // Recieving data doesn't work with software serial
#else
  #error "Configuration (Config.h): For MaxESP3, set DRIVER_UART_HARDWARE_SERIAL to ON (pins TX23,RX39) or OFF (uses CS.)"
#endif

// Specify the ESP32 I2C pins
#define I2C_SDA_PIN             21
#define I2C_SCL_PIN             22

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define AUX2_PIN                4                // ESP8266 RST control, or MISO for Axis1&2, or Axis4 EN support
#define AUX3_PIN                21               // Home SW for Axis1, or I2C SDA
#define AUX4_PIN                22               // Home SW for Axis2, or I2C SCL
#define AUX7_PIN                39               // Limit SW, PPS, etc.
#define AUX8_PIN                25               // 1-Wire, Status LED, Status2 LED, Reticle LED, Tone, etc.

// Misc. pins
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN          AUX8_PIN         // Default Pin for OneWire bus
#endif
#define ADDON_GPIO0_PIN         26               // ESP8266 GPIO0 (Dir2)
#ifndef ADDON_RESET_PIN
  #define ADDON_RESET_PIN       AUX2_PIN         // ESP8266 RST
#endif

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#ifndef PEC_SENSE_PIN
  #define PEC_SENSE_PIN         36               // [input only 36] PEC Sense, analog (A0) or digital (GPIO36)
#endif

// The status LED is a two wire jumper with a 2k resistor in series to limit the current to the LED
#ifndef STATUS_LED_PIN
  #define STATUS_LED_PIN        AUX8_PIN         // Default LED Cathode (-)
#endif
#define MOUNT_STATUS_LED_PIN    STATUS_LED_PIN   // Default LED Cathode (-)
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
#define AXIS1_ENABLE_PIN        12               // [must be low at boot 12]
#define AXIS1_M0_PIN            13               // SPI MOSI
#define AXIS1_M1_PIN            14               // SPI SCK
#define AXIS1_M2_PIN            23               // SPI CS (UART TX)
#define AXIS1_M3_PIN            AUX2_PIN         // SPI MISO (UART RX)
#define AXIS1_STEP_PIN          18
#define AXIS1_DIR_PIN           0                // [must be high at boot 0]
#define AXIS1_DECAY_PIN         AXIS1_M2_PIN
#ifndef AXIS1_SENSE_HOME_PIN
  #define AXIS1_SENSE_HOME_PIN  AUX3_PIN
#endif

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN        SHARED
#define AXIS2_M0_PIN            13               // SPI MOSI
#define AXIS2_M1_PIN            14               // SPI SCK
#define AXIS2_M2_PIN            5                // SPI CS (UART TX)
#define AXIS2_M3_PIN            AUX2_PIN         // SPI MISO (UART RX)
#define AXIS2_STEP_PIN          27
#define AXIS2_DIR_PIN           26
#define AXIS2_DECAY_PIN         AXIS2_M2_PIN
#ifndef AXIS2_SENSE_HOME_PIN
  #define AXIS2_SENSE_HOME_PIN  AUX4_PIN
#endif

// For rotator stepper driver
#define AXIS3_ENABLE_PIN        OFF              // No enable pin control (always enabled)
#define AXIS3_M0_PIN            OFF              // SPI MOSI
#define AXIS3_M1_PIN            OFF              // SPI SCK
#define AXIS3_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS3_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS3_STEP_PIN          2                // [must be low at boot 2]
#define SHARED_DIRECTION_PINS                    // Hint that the direction pins are shared
#define AXIS3_DIR_PIN           15

// For focuser1 stepper driver
#if DRIVER_UART_HARDWARE_SERIAL == ON
  #define AXIS4_ENABLE_PIN      OFF
#else
  #define AXIS4_ENABLE_PIN      AUX2_PIN
#endif
#define AXIS4_M0_PIN            OFF              // SPI MOSI
#define AXIS4_M1_PIN            OFF              // SPI SCK
#define AXIS4_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS4_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS4_STEP_PIN          19
#define AXIS4_DIR_PIN           15

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN        OFF              // No enable pin control (always enabled)
#define AXIS5_M0_PIN            OFF              // SPI MOSI
#define AXIS5_M1_PIN            OFF              // SPI SCK
#define AXIS5_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS5_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS5_STEP_PIN          2
#define AXIS5_DIR_PIN           15

// ST4 interface
#define ST4_RA_W_PIN            34               // [input only 34] ST4 RA- West
#define ST4_DEC_S_PIN           32               // ST4 DE- South
#define ST4_DEC_N_PIN           33               // ST4 DE+ North
#define ST4_RA_E_PIN            35               // [input only 35] ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
