// -------------------------------------------------------------------------------------------------
// Pin map for OnStep on STM32
#pragma once

// This pin map is for the STM32F103CB (or C8 with 128KB)
// and STM32F303xC "Blue Pill" boards...
// They run at 72MHz, with 48K of RAM, and 256K of flash
//
// More info, schematic at:
//   http://wiki.stm32duino.com/index.php?title=Blue_Pill
//
// Cost on eBay and AliExpress is less than US $2.50

#if defined(STM32F103xB) || defined(STM32F303xC)

// Serial ports
// Serial1 RX1 Pin PA10, TX1 Pin PA9 (to CP2102 USB serial adapter)
// Serial2 RX2 Pin PA3 , TX2 Pin PA2 (pins are used for other purposes)
// Serial3 RX3 Pin PB11, TX3 Pin PB10

#if SERIAL_A_BAUD_DEFAULT != OFF
  #define SERIAL_A              Serial1
#endif
#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B              HardSerial
  #define SERIAL_B_RX           PB11
  #define SERIAL_B_TX           PB10
#endif

// === Pins for DS3231 RTC/EEPROM
// The STM32 has no built in EEPROM. Therefore, we use a DS3231 RTC module 
// which has an EEPROM chip on the board. HAL takes care of its address and size:
//
// STM32 pin PB6 -> SCL on DS3231
// STM32 pin PB7 -> SDA on DS3231

#if PINMAP == STM32Blue

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define AUX0_PIN                PC13             // Status LED
#define AUX1_PIN                PB14             // ESP8266 GPIO0, SPI MISO/Fault
#define AUX2_PIN                PA1              // ESP8266 RST, SPI MISO/Fault
#define AUX3_PIN                PB8              // Reticle, Home SW
#define AUX4_PIN                PA13             // OneWire, Home SW

#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN          AUX4_PIN         // Default Pin for OneWire bus
#endif

// For ESP8266 control
#define ADDON_GPIO0_PIN         AUX1_PIN         // ESP8266 GPIO0
#define ADDON_RESET_PIN         AUX2_PIN         // ESP8266 RST

// The PEC index sense is a logic level input, resets the PEC index on rising
// edge then waits for 60 seconds before allowing another reset
#define PEC_SENSE_PIN           PC14             // PEC Sense

// This is the built in LED for the Black Pill board. There is a pin
// available from it too, in case you want to power another LED with a wire
#define STATUS_LED_PIN          AUX0_PIN         // Default LED Cathode (-)
#define MOUNT_LED_PIN           AUX0_PIN         // Default LED Cathode (-)
#ifndef RETICLE_LED_PIN 
  #define RETICLE_LED_PIN       AUX3_PIN         // Default LED Cathode (-)
#endif

// For a piezo buzzer
#define STATUS_BUZZER_PIN       PB9              // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and
// adjusts the internal sidereal clock frequency
#ifndef PPS_SENSE_PIN
  #define PPS_SENSE_PIN         PB5              // Pulse Per Second time source, e.g. GPS, RTC
#endif

// The limit switch sense is a logic level input which uses the internal pull up,
// shorted to ground it stops gotos/tracking
#ifndef LIMIT_SENSE_PIN
  #define LIMIT_SENSE_PIN       PA14
#endif

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN        PA12
#define AXIS1_M0_PIN            PA11             // SPI MOSI
#define AXIS1_M1_PIN            PA8              // SPI CLK
#define AXIS1_M2_PIN            PB15             // SPI CS (UART TX)
#define AXIS1_M3_PIN            AUX1_PIN         // SPI MISO (UART RX)
#define AXIS1_STEP_PIN          PB13
#define AXIS1_DIR_PIN           PB12
#define AXIS1_FAULT_PIN         AXIS1_M3_PIN
#ifndef AXIS1_SENSE_HOME_PIN
  #define AXIS1_SENSE_HOME_PIN  AUX3_PIN
#endif

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN        PA5
#define AXIS2_M0_PIN            PA4              // SPI MOSI
#define AXIS2_M1_PIN            PA3              // SPI CLK
#define AXIS2_M2_PIN            PA2              // SPI CS (UART TX)
#define AXIS2_M3_PIN            AUX2_PIN         // SPI MISO (UART RX)
#define AXIS2_STEP_PIN          PA0
#define AXIS2_DIR_PIN           PC15
#define AXIS2_FAULT_PIN         AXIS2_M3_PIN
#ifndef AXIS2_SENSE_HOME_PIN
  #define AXIS2_SENSE_HOME_PIN  AUX4_PIN
#endif

// For rotator stepper driver
#define AXIS3_ENABLE_PIN        PB4
#define AXIS3_M0_PIN            OFF              // SPI MOSI
#define AXIS3_M1_PIN            OFF              // SPI SCK
#define AXIS3_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS3_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS3_STEP_PIN          PB3
#define AXIS3_DIR_PIN           PA15

// Pins to focuser1 stepper driver
#define AXIS4_ENABLE_PIN        PB4
#define AXIS4_M0_PIN            OFF              // SPI MOSI
#define AXIS4_M1_PIN            OFF              // SPI SCK
#define AXIS4_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS4_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS4_STEP_PIN          PB3
#define AXIS4_DIR_PIN           PA15

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN        OFF
#define AXIS5_M0_PIN            OFF              // SPI MOSI
#define AXIS5_M1_PIN            OFF              // SPI SCK
#define AXIS5_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS5_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS5_STEP_PIN          OFF
#define AXIS5_DIR_PIN           OFF

// ST4 interface
#define ST4_DEC_N_PIN           PA7              // ST4 DE+ North
#define ST4_DEC_S_PIN           PA6              // ST4 DE- South
#define ST4_RA_W_PIN            PB1              // ST4 RA- West
#define ST4_RA_E_PIN            PB0              // ST4 RA+ East

#else
  #error "Unknown STM32 Board. This pinmap is only for Blue and Black Pill variants"
#endif

#else
  #error "Wrong processor for this configuration!"
#endif
