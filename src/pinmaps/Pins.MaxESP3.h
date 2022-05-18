// -------------------------------------------------------------------------------------------------
// Pin map for OnStep MaxESP Version 3.x (ESP32S)
#pragma once

#if defined(ESP32)

// Serial ports (see Pins.defaults.h for SERIAL_A)
// Serial0: RX Pin GPIO3, TX Pin GPIO1 (to USB serial adapter)
// Serial1: RX1 Pin GPIO10, TX1 Pin GPIO9 (on SPI Flash pins, must be moved to be used)
// Serial2: RX2 Pin GPIO16, TX2 Pin GPIO17

#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B              Serial2
#endif
#if SERIAL_C_BAUD_DEFAULT != OFF
  #error "Configuration (Config.h): SerialC isn't supported, disable this option."
#endif

#if DRIVER_UART_HARDWARE_SERIAL == ON
  // Use the following settings for any TMC2209 that may be present
  #define SERIAL_TMC            Serial1          // Use a single hardware serial port to up to four drivers
  #define SERIAL_TMC_BAUD       460800           // Baud rate
  #define SERIAL_TMC_TX         23               // Transmit data
  #define SERIAL_TMC_RX         39               // Recieving data
#else
  // Use the following settings for any TMC2209 that may be present
  #define SERIAL_TMC            SoftSerial       // Use software serial with RX on M2 and TX on M3 of axis
  #define SERIAL_TMC_BAUD       115200           // Baud rate
  #define SERIAL_TMC_NO_RX                       // Recieving data doesn't work with software serial
#endif

// SDA/SCL pins. 21/22 are the default values
#define SDA_PIN                 21
#define SCL_PIN                 22

// GPIO SSR74HC595 pins (if used, code below only works for pins 0 to 31)
#define GPIO_SSR74HC595_LATCH_PIN OFF
#define GPIO_SSR74HC595_CLOCK_PIN OFF
#define GPIO_SSR74HC595_DATA_PIN  OFF
#define GPIO_SSR74HC595_COUNT     8              // 8, 16, 24, or 32 (for 1, 2, 3, or 4 74HC595's)
#if GPIO_SSR74HC595_LATCH_PIN != OFF
  #define GPIO_SSR74HC595_LATCH_LOW() { GPIO.out_w1tc = ((uint32_t)1 << GPIO_SSR74HC595_LATCH_PIN); }
  #define GPIO_SSR74HC595_LATCH_HIGH() { GPIO.out_w1ts = ((uint32_t)1 << GPIO_SSR74HC595_LATCH_PIN); }
#endif
#if GPIO_SSR74HC595_CLOCK_PIN != OFF
  #define GPIO_SSR74HC595_CLOCK_LOW() { GPIO.out_w1tc = ((uint32_t)1 << GPIO_SSR74HC595_CLOCK_PIN); }
  #define GPIO_SSR74HC595_CLOCK_HIGH() { GPIO.out_w1ts = ((uint32_t)1 << GPIO_SSR74HC595_CLOCK_PIN); }
#endif
#if GPIO_SSR74HC595_DATA_PIN != OFF
  #define GPIO_SSR74HC595_DATA_LOW() { GPIO.out_w1tc = ((uint32_t)1 << GPIO_SSR74HC595_DATA_PIN); }
  #define GPIO_SSR74HC595_DATA_HIGH() { GPIO.out_w1ts = ((uint32_t)1 << GPIO_SSR74HC595_DATA_PIN); }
#endif

// Hint that the direction pins are shared
#define SHARED_DIRECTION_PINS

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
#define AXIS3_DIR_PIN           15

// For focuser1 stepper driver
#if DRIVER_UART_HARDWARE_SERIAL == OFF
  #define AXIS4_ENABLE_PIN      OFF
#else
  #define AXIS4_ENABLE_PIN      AUX2_PIN         // Enable pin on AUX2_PIN but can be turned OFF during validation
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
