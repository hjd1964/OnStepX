// -------------------------------------------------------------------------------------------------
// Pin map for OnStep MaxPCB4 Teensy4.1 PCB
#pragma once

#if defined(ARDUINO_TEENSY41)

// Serial1: RX1 Pin 0,  TX1 Pin 1
// Serial2: RX2 Pin 7,  TX2 Pin 8
// Serial8: RX8 Pin 34, TX8 Pin 35 (TMC2209 UART)

#if SERIAL_A_BAUD_DEFAULT != OFF
  #define SERIAL_A              Serial
#endif
#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B              Serial2
#endif
#if SERIAL_C_BAUD_DEFAULT != OFF
  #define SERIAL_C              Serial1
#endif
#if defined(USB_DUAL_SERIAL) || defined(USB_TRIPLE_SERIAL)
  #define SERIAL_D              SerialUSB1
  #define SERIAL_D_BAUD_DEFAULT 9600
#endif
#if defined(USB_TRIPLE_SERIAL)
  #define SERIAL_E              SerialUSB2
  #define SERIAL_E_BAUD_DEFAULT 9600
#endif

// Use the following settings for the 4x TMC UART drivers
#if defined(STEP_DIR_TMC_UART_PRESENT) || defined(SERVO_TMC2209_PRESENT)
  #define SERIAL_TMC_HARDWARE_UART
  #define DRIVER_TMC_STEPPER
  #define SERIAL_TMC            Serial8            // Use a single hardware serial port to up to four drivers
  #define SERIAL_TMC_BAUD       460800             // Baud rate
  #define SERIAL_TMC_ADDRESS_MAP(x) ((x==4)?2 : x) // Axis1(0) is 0, Axis2(1) is 1, Axis3(2) is 2, Axis4(3) is 3, Axis5(4) is 2
#endif

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define AUX0_PIN                23               // Status LED
#define AUX1_PIN                34               // TMC SPI MISO or UART RX (Serial8)
#define AUX2_PIN                20               // PPS
#define AUX3_PIN                21               // Home SW
#define AUX4_PIN                22               // Home SW
#define AUX5_PIN                1                // 1-Wire, TX1 
#define AUX6_PIN                0                // RX1
#define AUX7_PIN                9                // Limit SW
#define AUX8_PIN                10               // Reticle LED, 1-Wire alternate

// Misc. pins
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN          AUX5_PIN         // Default Pin for 1-Wire bus
#endif
#define ADDON_GPIO0_PIN         OFF              // ESP8266 GPIO0 (shared with AXIS2_DIR_PIN)
#define ADDON_RESET_PIN         OFF              // ESP8266 RST

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PEC_SENSE_PIN           2                // PEC Sense, analog or digital

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define STATUS_LED_PIN          AUX0_PIN         // Default LED Cathode (-)
#define MOUNT_LED_PIN           AUX0_PIN         // Default LED Cathode (-)
#ifndef RETICLE_LED_PIN 
  #define RETICLE_LED_PIN       AUX8_PIN         // Default LED Cathode (-)
#endif

// For a piezo buzzer
#ifndef STATUS_BUZZER_PIN 
  #define STATUS_BUZZER_PIN     17               // Tone
#endif

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#ifndef PPS_SENSE_PIN
  #define PPS_SENSE_PIN         AUX2_PIN         // PPS time source, GPS for example (MaxSTM version 3.61 and later)
#endif

// The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking
#ifndef LIMIT_SENSE_PIN
  #define LIMIT_SENSE_PIN       AUX7_PIN
#endif

// hint that the driver mode pins are dedicated (not shared SPI bus except possibly MISO)
#define DEDICATED_MODE_PINS

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN        33
#define AXIS1_M0_PIN            37               // SPI MOSI
#define AXIS1_M1_PIN            36               // SPI SCK
#define AXIS1_M2_PIN            35               // SPI CS (UART TX)
#define AXIS1_M3_PIN            AUX1_PIN         // SPI MISO (UART RX)
#define AXIS1_STEP_PIN          38
#define AXIS1_DIR_PIN           39
#ifndef AXIS1_SENSE_HOME_PIN
  #define AXIS1_SENSE_HOME_PIN  AUX3_PIN
#endif

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN        40
#define AXIS2_M0_PIN            41               // SPI MOSI
#define AXIS2_M1_PIN            13               // SPI SCK
#define AXIS2_M2_PIN            14               // SPI CS (UART TX)
#define AXIS2_M3_PIN            AUX1_PIN         // SPI MISO (UART RX)
#define AXIS2_STEP_PIN          15
#define AXIS2_DIR_PIN           16
#ifndef AXIS2_SENSE_HOME_PIN
  #define AXIS2_SENSE_HOME_PIN  AUX4_PIN
#endif

// For rotator stepper driver
#define AXIS3_ENABLE_PIN        11
#define AXIS3_M0_PIN            12               // SPI MOSI
#define AXIS3_M1_PIN            24               // SPI SCK
#define AXIS3_M2_PIN            25               // SPI CS (UART TX)
#define AXIS3_M3_PIN            AUX1_PIN         // SPI MISO (UART RX)
#define AXIS3_STEP_PIN          26
#define SHARED_DIRECTION_PINS                    // Hint that the direction pins are shared
#define AXIS3_DIR_PIN           27

// For focuser1 stepper driver
#define AXIS4_ENABLE_PIN        28
#define AXIS4_M0_PIN            29               // SPI MOSI
#define AXIS4_M1_PIN            30               // SPI SCK
#define AXIS4_M2_PIN            31               // SPI CS (UART TX)
#define AXIS4_M3_PIN            AUX1_PIN         // SPI MISO (UART RX)
#define AXIS4_STEP_PIN          32
#define AXIS4_DIR_PIN           27

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN        11
#define AXIS5_M0_PIN            12               // SPI MOSI
#define AXIS5_M1_PIN            24               // SPI SCK
#define AXIS5_M2_PIN            25               // SPI CS (UART TX)
#define AXIS5_M3_PIN            AUX1_PIN         // SPI MISO (UART RX)
#define AXIS5_STEP_PIN          26
#define AXIS5_DIR_PIN           27

// ST4 interface
#define ST4_RA_W_PIN            3                // ST4 RA- West
#define ST4_DEC_S_PIN           4                // ST4 DE- South
#define ST4_DEC_N_PIN           5                // ST4 DE+ North
#define ST4_RA_E_PIN            6                // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
