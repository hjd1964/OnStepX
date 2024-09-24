// -------------------------------------------------------------------------------------------------
// Pin map for Microscope PCB Version 0.2 (ESP32S, experimental and may be removed at any point!)

// !!! USE AT YOUR OWN RISK !!!

#pragma once

#if defined(ESP32)

// Serial0: RX Pin GPIO3, TX Pin GPIO1 (to USB serial adapter)
// Serial1: RX1 Pin GPIO10, TX1 Pin GPIO9 (on SPI Flash pins, must be moved to be used)
// Serial2: RX2 Pin GPIO16, TX2 Pin GPIO17

#if SERIAL_A_BAUD_DEFAULT != OFF
  #define SERIAL_A              Serial
#endif
#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B              Serial2
#endif

// Specify the ESP32 I2C pins
#define I2C_SDA_PIN             21
#define I2C_SCL_PIN             22

// Misc. pins
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN          26               // Default Pin for OneWire bus
#endif
#define ADDON_GPIO0_PIN         OFF              // 26 ESP8266 GPIO0 (Dir2)
#ifndef ADDON_RESET_PIN
  #define ADDON_RESET_PIN       OFF              // 4 ESP8266 RST
#endif

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#ifndef PEC_SENSE_PIN
  #define PEC_SENSE_PIN         36                // [input only 36] PEC Sense, analog (A0) or digital (GPIO36)
#endif

// The status LED is a two wire jumper with a 2k resistor in series to limit the current to the LED
#ifndef STATUS_LED_PIN
  #define STATUS_LED_PIN        25               // 25 Default LED Cathode (-)
#endif
#define MOUNT_LED_PIN           25               // 25 Default LED Cathode (-)
#ifndef RETICLE_LED_PIN 
  #define RETICLE_LED_PIN       25               // 25 Default LED Cathode (-)
#endif

// For a piezo buzzer
#ifndef STATUS_BUZZER_PIN
  #define STATUS_BUZZER_PIN     OFF              // 25 Tone
#endif

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#ifndef PPS_SENSE_PIN
  #define PPS_SENSE_PIN         39               // 37 PPS time source, GPS for example
#endif

// The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking
#ifndef LIMIT_SENSE_PIN
  #define LIMIT_SENSE_PIN       OFF              // 37
#endif

#define SHARED_ENABLE_PIN       GPIO_PIN(13)     // Hint that the enable pins are shared

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN        SHARED           // [must be low at boot 12]
#define AXIS1_M0_PIN            23               // 13 SPI MOSI
#define AXIS1_M1_PIN            18               // 14 SPI SCK
#define AXIS1_M2_PIN            25               // 23 SPI CS (UART TX)
#define AXIS1_M3_PIN            19               // 4 SPI MISO (UART RX)
#define AXIS1_STEP_PIN          2                // 18
#define AXIS1_DIR_PIN           GPIO_PIN(15)     // [must be high at boot 0]
#define AXIS1_SENSE_HOME_PIN    GPIO_PIN(1)      // 21

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN        SHARED
#define AXIS2_M0_PIN            23               // 13 SPI MOSI
#define AXIS2_M1_PIN            18               // 14 SPI SCK
#define AXIS2_M2_PIN            14               // 5 SPI CS (UART TX)
#define AXIS2_M3_PIN            19               // 4 SPI MISO (UART RX)
#define AXIS2_STEP_PIN          12               // 27
#define AXIS2_DIR_PIN           GPIO_PIN(16)     // 26
#define AXIS2_SENSE_HOME_PIN    GPIO_PIN(0)      // 22

// For rotator stepper driver
#define AXIS3_ENABLE_PIN        SHARED           // No enable pin control (always enabled)
#define AXIS3_M0_PIN            23               // SPI MOSI
#define AXIS3_M1_PIN            18               // SPI SCK
#define AXIS3_M2_PIN            4                // SPI CS (UART TX)
#define AXIS3_M3_PIN            19               // SPI MISO (UART RX)
#define AXIS3_STEP_PIN          15               // [must be low at boot 2]
#define AXIS3_DIR_PIN           GPIO_PIN(17)     // 15

// For focuser1 stepper driver
#define AXIS4_ENABLE_PIN        SHARED           // 4s
#define AXIS4_M0_PIN            23               // SPI MOSI
#define AXIS4_M1_PIN            18               // SPI SCK
#define AXIS4_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS4_M3_PIN            19               // SPI MISO (UART RX)
#define AXIS4_STEP_PIN          5                // 19
#define AXIS4_DIR_PIN           GPIO_PIN(14)     // 15

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN        SHARED           // No enable pin control (always enabled)
#define AXIS5_M0_PIN            23               // SPI MOSI
#define AXIS5_M1_PIN            18               // SPI SCK
#define AXIS5_M2_PIN            4                // SPI CS (UART TX)
#define AXIS5_M3_PIN            19               // SPI MISO (UART RX)
#define AXIS5_STEP_PIN          15               // 2
#define AXIS5_DIR_PIN           GPIO_PIN(17)     // 15

// ST4 interface
#define ST4_RA_W_PIN            33               // [input only 34] ST4 RA- West
#define ST4_DEC_S_PIN           32               // [i/o 32] ST4 DE- South
#define ST4_DEC_N_PIN           35               // [i/o 33] ST4 DE+ North
#define ST4_RA_E_PIN            34               // [input only 35] ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
