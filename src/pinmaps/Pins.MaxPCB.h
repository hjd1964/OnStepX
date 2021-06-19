// -------------------------------------------------------------------------------------------------
// Pin map for OnStep MaxPCB (Teensy3.5/3.6)
#pragma once

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define Aux0                   19               // Status LED
#define Aux1                   18               // ESP8266 GPIO0, SPI MISO/Fault
#define Aux2                   5                // ESP8266 RST, SPI MISO/Fault
#define Aux3                   36               // Home SW
#define Aux4                   39               // OneWire, Home SW
#define Aux5                   A21              // AXIS3_EN_PIN; true analog output
#define Aux6                   A22              // AXIS4_EN_PIN; true analog output
#define Aux7                   4                // Limit SW
#define Aux8                   22               // Status2 LED, Reticle LED

// Misc. pins
#ifndef DS3234_CS_PIN
  #define DS3234_CS_PIN        10               // Default CS Pin for DS3234 on SPI
#endif
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN         Aux4             // Default Pin for OneWire bus (note: this pin has a 0.1uF capacitor that must be removed for OneWire to function)
#endif
#if PINMAP == MaxPCB3
  #define ADDON_GPIO0_PIN      2                // ESP8266 GPIO0 (shared with AXIS2_DIR_PIN)
  #define ADDON_RESET_PIN      Aux2             // ESP8266 RST
#elif PINMAP == MaxPCB
  #define ADDON_GPIO0_PIN      Aux1             // ESP8266 GPIO0 or SPI MISO/Fault
  #define ADDON_RESET_PIN      Aux2             // ESP8266 RST or SPI MISO/Fault
#endif

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PEC_SENSE_PIN          23               // PEC Sense, analog or digital

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LED_STATUS_PIN         Aux0             // Default LED Cathode (-)
#define LED_MOUNT_STATUS_PIN   Aux0             // Default LED Cathode (-)
#define LED_ROTATOR_STATUS_PIN Aux0             // Default LED Cathode (-)
#define LED_FOCUSER_STATUS_PIN Aux0             // Default LED Cathode (-)
#define RETICLE_LED_PIN        Aux8             // Default LED Cathode (-)

// For a piezo buzzer
#define BUZZER_PIN             29               // Tone

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define SENSE_PPS_PIN          28               // PPS time source, GPS for example

#define SENSE_LIMIT_PIN        Aux7             // The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN       14
#define AXIS1_M0_PIN           15               // SPI MOSI
#define AXIS1_M1_PIN           16               // SPI SCK
#define AXIS1_M2_PIN           17               // SPI CS
#define AXIS1_M3_PIN           Aux1             // SPI MISO
#define AXIS1_STEP_PIN         20
#define AXIS1_DIR_PIN          21
#define AXIS1_DECAY_PIN        AXIS1_M2_PIN
#define AXIS1_FAULT_PIN        Aux1
#define AXIS1_SENSE_HOME_PIN   Aux3

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN       9
#if PINMAP == MaxPCB3
  #define AXIS2_M0_PIN         15               // SPI MOSI
  #define AXIS2_M1_PIN         16               // SPI SCK
  #define AXIS2_M2_PIN         6                // SPI CS
  #define AXIS2_M3_PIN         Aux1             // SPI MISO
#else
  #define AXIS2_M0_PIN         8                // SPI MOSI
  #define AXIS2_M1_PIN         7                // SPI SCK
  #define AXIS2_M2_PIN         6                // SPI CS
  #define AXIS2_M3_PIN         Aux2             // SPI MISO
#endif
#define AXIS2_STEP_PIN         3
#define AXIS2_DIR_PIN          2
#define AXIS2_DECAY_PIN        AXIS2_M2_PIN
#define AXIS2_FAULT_PIN        Aux2
#define AXIS2_SENSE_HOME_PIN   Aux4

// For rotator stepper driver
#define AXIS3_ENABLE_PIN       Aux5
#if PINMAP == MaxPCB3
  #define AXIS3_M0_PIN         15               // SPI MOSI
  #define AXIS3_M1_PIN         16               // SPI SCK
  #define AXIS3_M2_PIN         7                // SPI CS
  #define AXIS3_M3_PIN         Aux1             // SPI MISO
#else
  #define AXIS3_M0_PIN         OFF              // SPI MOSI
  #define AXIS3_M1_PIN         OFF              // SPI SCK
  #define AXIS3_M2_PIN         OFF              // SPI CS
  #define AXIS3_M3_PIN         OFF              // SPI MISO
#endif
#define AXIS3_STEP_PIN         30
#define AXIS3_DIR_PIN          33

// For focuser1 stepper driver
#define AXIS4_ENABLE_PIN       Aux6
#if PINMAP == MaxPCB3
  #define AXIS4_M0_PIN         15               // SPI MOSI
  #define AXIS4_M1_PIN         16               // SPI SCK
  #define AXIS4_M2_PIN         8                // SPI CS
  #define AXIS4_M3_PIN         Aux1             // SPI MISO
#else
  #define AXIS4_M0_PIN         OFF              // SPI MOSI
  #define AXIS4_M1_PIN         OFF              // SPI SCK
  #define AXIS4_M2_PIN         OFF              // SPI CS
  #define AXIS4_M3_PIN         OFF              // SPI MISO
#endif
#define AXIS4_STEP_PIN         34
#define AXIS4_DIR_PIN          35

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN       Aux5
#if PINMAP == MaxPCB3
  #define AXIS5_M0_PIN         15               // SPI MOSI
  #define AXIS5_M1_PIN         16               // SPI SCK
  #define AXIS5_M2_PIN         7                // SPI CS
  #define AXIS5_M3_PIN         Aux1             // SPI MISO
#else
  #define AXIS5_M0_PIN         OFF              // SPI MOSI
  #define AXIS5_M1_PIN         OFF              // SPI SCK
  #define AXIS5_M2_PIN         OFF              // SPI CS
  #define AXIS5_M3_PIN         OFF              // SPI MISO
#endif
#define AXIS5_STEP_PIN         30
#define AXIS5_DIR_PIN          33

// ST4 interface
#define ST4_RA_W_PIN           24               // ST4 RA- West
#define ST4_DEC_S_PIN          25               // ST4 DE- South
#define ST4_DEC_N_PIN          26               // ST4 DE+ North
#define ST4_RA_E_PIN           27               // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
