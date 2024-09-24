// -------------------------------------------------------------------------------------------------
// Pin map for OnStep MaxPCB (Teensy3.5/3.6)
#pragma once

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)

// Serial1: RX1 Pin 0, TX1 Pin 1
// Serial4: RX4 Pin 31, TX4 Pin 32

#if SERIAL_A_BAUD_DEFAULT != OFF
  #define SERIAL_A              Serial
#endif
#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B              Serial1
#endif
#if SERIAL_C_BAUD_DEFAULT != OFF
  #define SERIAL_C              Serial4
#endif
#if defined(USB_DUAL_SERIAL) || defined(USB_TRIPLE_SERIAL)
  #define SERIAL_D              SerialUSB1
  #define SERIAL_D_BAUD_DEFAULT 9600
#endif
#if defined(USB_TRIPLE_SERIAL)
  #define SERIAL_E              SerialUSB2
  #define SERIAL_E_BAUD_DEFAULT 9600
#endif

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define AUX0_PIN               19                // Status LED
#define AUX1_PIN               18                // ESP8266 GPIO0, SPI MISO/Fault
#define AUX2_PIN               5                 // ESP8266 RST, SPI MISO/Fault
#define AUX3_PIN               36                // Home SW
#define AUX4_PIN               39                // OneWire, Home SW
#define AUX5_PIN               DAC_PIN(A21)      // AXIS3_EN_PIN; true analog output
#define AUX6_PIN               DAC_PIN(A22)      // AXIS4_EN_PIN; true analog output
#define AUX7_PIN               4                 // Limit SW
#define AUX8_PIN               22                // Reticle LED

// Misc. pins
#ifndef DS3234_CS_PIN
  #define DS3234_CS_PIN        10                // Default CS Pin for DS3234 on SPI
#endif
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN         AUX4_PIN          // Default Pin for OneWire bus (note: this pin has a 0.1uF capacitor that must be removed for OneWire to function)
#endif
#define ADDON_GPIO0_PIN        AUX1_PIN          // ESP8266 GPIO0 or SPI MISO/Fault
#define ADDON_RESET_PIN        AUX2_PIN          // ESP8266 RST or SPI MISO/Fault

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#ifndef PEC_SENSE_PIN
  #define PEC_SENSE_PIN        23                // PEC Sense, analog or digital
#endif

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define STATUS_LED_PIN         AUX0_PIN          // Default LED Cathode (-)
#define MOUNT_LED_PIN          AUX0_PIN          // Default LED Cathode (-)
#ifndef RETICLE_LED_PIN
  #define RETICLE_LED_PIN      AUX8_PIN          // Default LED Cathode (-)
#endif

// For a piezo buzzer
#define STATUS_BUZZER_PIN      29                // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#ifndef PPS_SENSE_PIN
  #define PPS_SENSE_PIN        28                // PPS time source, GPS for example
#endif

#ifndef LIMIT_SENSE_PIN
  #define LIMIT_SENSE_PIN      AUX7_PIN          // The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking
#endif

// hint that the driver mode pins are dedicated (not shared SPI bus except possibly MISO)
#define DEDICATED_MODE_PINS

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN       14
#define AXIS1_M0_PIN           15                // SPI MOSI
#define AXIS1_M1_PIN           16                // SPI SCK
#define AXIS1_M2_PIN           17                // SPI CS (UART TX)
#define AXIS1_M3_PIN           AUX1_PIN          // SPI MISO (UART RX)
#define AXIS1_STEP_PIN         20
#define AXIS1_DIR_PIN          21
#define AXIS1_FAULT_PIN        AXIS1_M3_PIN
#ifndef AXIS1_SENSE_HOME_PIN
  #define AXIS1_SENSE_HOME_PIN AUX3_PIN
#endif

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN       9
#define AXIS2_M0_PIN           8                 // SPI MOSI
#define AXIS2_M1_PIN           7                 // SPI SCK
#define AXIS2_M2_PIN           6                 // SPI CS (UART TX)
#define AXIS2_M3_PIN           AUX2_PIN          // SPI MISO (UART RX)
#define AXIS2_STEP_PIN         3
#define AXIS2_DIR_PIN          2
#define AXIS2_FAULT_PIN        AXIS2_M3_PIN
#ifndef AXIS2_SENSE_HOME_PIN
  #define AXIS2_SENSE_HOME_PIN AUX4_PIN
#endif

// For rotator stepper driver
#define AXIS3_ENABLE_PIN       AUX5_PIN
#define AXIS3_M0_PIN           OFF               // SPI MOSI
#define AXIS3_M1_PIN           OFF               // SPI SCK
#define AXIS3_M2_PIN           OFF               // SPI CS
#define AXIS3_M3_PIN           OFF               // SPI MISO
#define AXIS3_STEP_PIN         30
#define AXIS3_DIR_PIN          33

// For focuser1 stepper driver
#define AXIS4_ENABLE_PIN       AUX6_PIN
#define AXIS4_M0_PIN           OFF               // SPI MOSI
#define AXIS4_M1_PIN           OFF               // SPI SCK
#define AXIS4_M2_PIN           OFF               // SPI CS
#define AXIS4_M3_PIN           OFF               // SPI MISO
#define AXIS4_STEP_PIN         34
#define AXIS4_DIR_PIN          35

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN       AUX5_PIN
#define AXIS5_M0_PIN           OFF               // SPI MOSI
#define AXIS5_M1_PIN           OFF               // SPI SCK
#define AXIS5_M2_PIN           OFF               // SPI CS
#define AXIS5_M3_PIN           OFF               // SPI MISO
#define AXIS5_STEP_PIN         30
#define AXIS5_DIR_PIN          33

// ST4 interface
#define ST4_RA_W_PIN           24                // ST4 RA- West
#define ST4_DEC_S_PIN          25                // ST4 DE- South
#define ST4_DEC_N_PIN          26                // ST4 DE+ North
#define ST4_RA_E_PIN           27                // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
