// -------------------------------------------------------------------------------------------------
// Pin map for OnStep on STM32
#pragma once

// This pin map is for an STM32F303xC "Blue Pill" boards..
// They run at 72MHz, with 48K of RAM, and 256K of flash.
//
// More info, schematic at:
//   http://wiki.stm32duino.com/index.php?title=Blue_Pill
//
// Cost on eBay and AliExpress is less than US $2.50

#if defined(STM32F103xB) || defined(STM32F303xC)

// === Pins for USB 
// In HAL, Serial_A is mapped to USART1, with the following pins:
// STM32 pin PA9  TX -> RX on CP2102 UART to TTL board
// STM32 pin PA10 RX -> TX on CP2102 UART to TTL board

// === Pins for WiFi
// In HAL, Serial_B is mapped to USART3, with the following pins:
//
// STM32 pin PB10 TX -> RX on ESP8266
// STM32 pin PB11 RX -> TX on ESP8266

// === Pins for DS3231 RTC/EEPROM
// The STM32 has no built in EEPROM. Therefore, we use a DS3231 RTC module 
// which has an EEPROM chip on the board. HAL takes care of its address and size:
//
// STM32 pin PB6 -> SCL on DS3231
// STM32 pin PB7 -> SDA on DS3231

#if PINMAP == STM32Blue

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define Aux0                   PC13             // Status LED
#define Aux1                   PB14             // ESP8266 GPIO0, SPI MISO/Fault
#define Aux2                   PA1              // ESP8266 RST, SPI MISO/Fault
#define Aux3                   PB8              // Reticle, Home SW
#define Aux4                   PA13             // OneWire, Home SW

#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN         Aux4             // Default Pin for OneWire bus
#endif

// For ESP8266 control
#define ADDON_GPIO0_PIN        Aux1             // ESP8266 GPIO0
#define ADDON_RESET_PIN        Aux2             // ESP8266 RST

// The PEC index sense is a logic level input, resets the PEC index on rising
// edge then waits for 60 seconds before allowing another reset
#define PEC_SENSE_PIN          PC14             // PEC Sense

// This is the built in LED for the Black Pill board. There is a pin
// available from it too, in case you want to power another LED with a wire
#define STATUS_LED1_PIN        Aux0             // Drain
#define STATUS_LED2_PIN        OFF              // Drain
#define RETICLE_LED_PIN        Aux3             // Drain

// For a piezo buzzer
#define BUZZER_PIN             PB9              // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and
// adjusts the internal sidereal clock frequency
#define SENSE_PPS_PIN          PB5              // Pulse Per Second time source, e.g. GPS, RTC

// The limit switch sense is a logic level input which uses the internal pull up,
// shorted to ground it stops gotos/tracking
#define SENSE_LIMIT_PIN        PA14

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN       PA12
#define AXIS1_M0_PIN           PA11             // SPI MOSI
#define AXIS1_M0_PINPORT       GPIOA
#define AXIS1_M0_PINBIT        GPIO_PIN_11
#define AXIS1_M1_PIN           PA8              // SPI CLK
#define AXIS1_M1_PINPORT       GPIOA
#define AXIS1_M1_PINBIT        GPIO_PIN_8
#define AXIS1_M2_PIN           PB15             // SPI CS
#define AXIS1_M2_PINPORT       GPIOB
#define AXIS1_M2_PINBIT        GPIO_PIN_15
#define AXIS1_M3_PIN           Aux1             // SPI MISO
#define AXIS1_STEP_PIN         PB13
#define Axis1_StpPORT          GPIOB
#define Axis1_StpBIT           GPIO_PIN_13
#define AXIS1_DIR_PIN          PB12
#define Axis1_DirPORT          GPIOB
#define Axis1_DirBIT           GPIO_PIN_12
#define AXIS1_DECAY_PIN        AXIS1_M2_PIN
#define AXIS1_FAULT_PIN        Aux1
#define AXIS1_SENSE_HOME_PIN   Aux3

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN       PA5
#define AXIS2_M0_PIN           PA4              // SPI MOSI
#define AXIS2_M0_PINPORT       GPIOA
#define AXIS2_M0_PINBIT        GPIO_PIN_4
#define AXIS2_M1_PIN           PA3              // SPI CLK
#define AXIS2_M1_PINPORT       GPIOA
#define AXIS2_M1_PINBIT        GPIO_PIN_3
#define AXIS2_M2_PIN           PA2              // SPI CS
#define AXIS2_M2_PINPORT       GPIOA
#define AXIS2_M2_PINBIT        GPIO_PIN_2
#define AXIS2_M3_PIN           Aux2             // SPI MISO
#define AXIS2_STEP_PIN         PA0
#define Axis2_StpPORT          GPIOA
#define Axis2_StpBIT           GPIO_PIN_0
#define AXIS2_DIR_PIN          PC15
#define Axis2_DirPORT          GPIOC
#define Axis2_DirBIT           GPIO_PIN_15
#define AXIS2_DECAY_PIN        AXIS2_M2_PIN
#define AXIS2_FAULT_PIN        Aux2
#define AXIS2_SENSE_HOME_PIN   Aux4

// For rotator stepper driver
#define AXIS3_ENABLE_PIN       PB4
#define AXIS3_M0_PIN           OFF              // SPI MOSI
#define AXIS3_M1_PIN           OFF              // SPI SCK
#define AXIS3_M2_PIN           OFF              // SPI CS
#define AXIS3_M3_PIN           OFF              // SPI MISO
#define AXIS3_STEP_PIN         PB3
#define AXIS3_DIR_PIN          PA15

// Pins to focuser1 stepper driver
#define AXIS4_ENABLE_PIN       PB4
#define AXIS4_M0_PIN           OFF              // SPI MOSI
#define AXIS4_M1_PIN           OFF              // SPI SCK
#define AXIS4_M2_PIN           OFF              // SPI CS
#define AXIS4_M3_PIN           OFF              // SPI MISO
#define AXIS4_STEP_PIN         PB3
#define AXIS4_DIR_PIN          PA15

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN       OFF
#define AXIS5_M0_PIN           OFF              // SPI MOSI
#define AXIS5_M1_PIN           OFF              // SPI SCK
#define AXIS5_M2_PIN           OFF              // SPI CS
#define AXIS5_M3_PIN           OFF              // SPI MISO
#define AXIS5_STEP_PIN         OFF
#define AXIS5_DIR_PIN          OFF

// ST4 interface
#define ST4_DEC_N_PIN          PA7              // ST4 DE+ North
#define ST4_DEC_S_PIN          PA6              // ST4 DE- South
#define ST4_RA_W_PIN           PB1              // ST4 RA- West
#define ST4_RA_E_PIN           PB0              // ST4 RA+ East

#else
  #error "Unknown STM32 Board. This pinmap is only for Blue and Black Pill variants"
#endif

#else
  #error "Wrong processor for this configuration!"
#endif
