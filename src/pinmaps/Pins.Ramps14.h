// -------------------------------------------------------------------------------------------------
// Pin map for OnStep using RAMPS 1.4 Pin defs (Arduino Mega2560 & Arduino DUE)
#pragma once

#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__) || defined(__SAM3X8E__)

// Thermistor (temperature) sensor inputs have built-in 4.7K Ohm pullups and a 10uF cap for noise supression
#define Temp0Pin              A15                // Thermo0
#define Temp1Pin              A14                // Thermo1
#define Temp2Pin              A13                // Thermo2

#if FOCUSER_TEMPERATURE_PIN == OFF
  #undef FOCUSER_TEMPERATURE_PIN
  #define FOCUSER_TEMPERATURE_PIN Temp0Pin
#endif
#if FEATURE1_TEMP_PIN == OFF
  #undef FEATURE1_TEMP_PIN
  #define FEATURE1_TEMP_PIN   Temp1Pin
#endif
#if FEATURE2_TEMP_PIN == OFF
  #undef FEATURE2_TEMP_PIN
  #define FEATURE2_TEMP_PIN   Temp2Pin
#endif

// The multi-purpose pins (Aux3..Aux8 can be analog (pwm/dac) if supported)
#define AUX0_PIN               11               // Status LED
#define AUX1_PIN               29               // ESP8266 GPIO0, SPI MISO/Fault
#define AUX2_PIN               37               // ESP8266 RST, SPI MISO/Fault
#if PINMAP == MksGenL1
  #define AUX3_PIN             14               // Home SW (GenL1 Y- endstop)
  #define AUX4_PIN             15               // 1-Wire (requires h/w modification,) Home SW (GenL1 Y+ endstop)
#else
  #define AUX3_PIN             32               // Home SW; note modified pinmap 10/2/19 Aux3 and Aux4 were changed, 9/30/19 Aux5 was removed (Aux3 not present Gen-L1)
  #define AUX4_PIN             39               // 1-Wire, Home SW (Aux4 not present Gen-L1)
#endif
#define AUX5_PIN               7                // Dew Heater0 (E1 , MKS Gen-L)
#define AUX6_PIN               8                // Dew Heater1 (BED, MKS Gen-L, RAMPS1.4)
#define AUX7_PIN               9                // Dew Heater2 (FAN, MKS Gen-L, RAMPS1.4)
#define AUX8_PIN               10               // Dew Heater3 (E0 , MKS Gen-L, RAMPS1.4)

#ifndef DS3234_CS_PIN
  #define DS3234_CS_PIN        53               // Default CS Pin for DS3234 on SPI
#endif
#ifndef BMx280_CS_PIN
  #define BMx280_CS_PIN        49               // Default CS Pin for BME280 or BMP280 on SPI
#endif
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN         AUX4_PIN         // Default Pin for OneWire bus
#endif
#ifndef ADDON_GPIO0_PIN
  #define ADDON_GPIO0_PIN      AUX1_PIN         // ESP8266 GPIO0 or SPI MISO/Fault
#endif
#ifndef ADDON_RESET_PIN
  #define ADDON_RESET_PIN      AUX2_PIN         // ESP8266 RST or SPI MISO/Fault
#endif

// For software SPI
#if PINMAP == MksGenL2 || PINMAP == MksGenL21
  #define SSPI_SHARED
#endif
#define SSPI_SCK               52
#define SSPI_MISO              50
#define SSPI_MOSI              51

// The PEC index sense is a 5V logic input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#if PINMAP == MksGenL1
  #define PEC_SENSE_PIN        41               // GenL1 EXP2
#else
  #define PEC_SENSE_PIN        57               // RAMPS AUX1, A-OUT (1=+5V, 2=GND, 3=PEC)
#endif

// The limit switch sense is a 5V logic input which uses the internal (or external 2k) pull up, shorted to ground it stops gotos/tracking
#define SENSE_LIMIT_PIN        3                // RAMPS X- (1=LMT, 2=GND, 3=+5)

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define STATUS_LED_PIN         AUX0_PIN         // Drain RAMPS SERVO1 (1=GND, 2=+5, 3=LED-)
#define STATUS_MOUNT_LED_PIN   6                // Drain RAMPS SERVO2 (1=GND, 2=+5, 3=LED-)
#define STATUS_ROTATOR_LED_PIN 6                // Drain RAMPS SERVO2 (1=GND, 2=+5, 3=LED-)
#define STATUS_FOCUSER_LED_PIN 6                // Drain RAMPS SERVO2 (1=GND, 2=+5, 3=LED-)
#define RETICLE_LED_PIN        5                // Drain RAMPS SERVO3 (1=GND, 2=+5, 3=LED-)

// Pin for a piezo buzzer output on RAMPS Y-MIN
#define STATUS_BUZZER_PIN      4                // RAMPS SERVO4 (1=GND, 2=+5, 3=TONE+) (active HIGH)

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define SENSE_PPS_PIN          2                // RAMPS X+, Interrupt 0 on Pin 2

// Pins to Axis1 RA/Azm on RAMPS X
#define AXIS1_ENABLE_PIN       38
#if PINMAP == MksGenL2 || PINMAP == MksGenL21
  #define AXIS1_M0_PIN         51               // SPI MOSI
  #define AXIS1_M1_PIN         52               // SPI SCK
  #define AXIS1_M2_PIN         A9               // SPI CS
  #define AXIS1_M3_PIN         50               // SPI MISO
#else
  #define AXIS1_M0_PIN         23               // SPI MOSI
  #define AXIS1_M1_PIN         25               // SPI SCK
  #define AXIS1_M2_PIN         27               // SPI CS
  #define AXIS1_M3_PIN         AUX1_PIN         // SPI MISO
#endif
#define AXIS1_STEP_PIN         54
#define AXIS1_DIR_PIN          55
#define AXIS1_DECAY_PIN        AXIS1_M2_PIN
#define AXIS1_FAULT_PIN        AUX1_PIN
#define AXIS1_SENSE_HOME_PIN   AUX3_PIN

// Axis2 Dec/Alt step/dir driver on RMAPS Y
#define AXIS2_ENABLE_PIN       56               // (Pin A2)
#if PINMAP == MksGenL2 || PINMAP == MksGenL21
  #define AXIS2_M0_PIN         51               // SPI MOSI
  #define AXIS2_M1_PIN         52               // SPI SCK
  #define AXIS2_M2_PIN         A10              // SPI CS
  #define AXIS2_M3_PIN         50               // SPI MISO
#else
  #define AXIS2_M0_PIN         31               // SPI MOSI
  #define AXIS2_M1_PIN         33               // SPI SCK
  #define AXIS2_M2_PIN         35               // SPI CS
  #define AXIS2_M3_PIN         AUX2_PIN         // SPI MISO
#endif
#define AXIS2_STEP_PIN         60               // (Pin A6)
#define AXIS2_DIR_PIN          61               // (Pin A7)
#define AXIS2_DECAY_PIN        AXIS2_M2_PIN
#define AXIS2_FAULT_PIN        AUX2_PIN
#define AXIS2_SENSE_HOME_PIN   AUX4_PIN

// For rotator stepper driver on RAMPS Z
#define AXIS3_ENABLE_PIN       62               // (Pin A8)
#if PINMAP == MksGenL2 || PINMAP == MksGenL21
  #define AXIS3_M0_PIN         51               // SPI MOSI
  #define AXIS3_M1_PIN         52               // SPI SCK
  #define AXIS3_M2_PIN         A11              // SPI CS
  #define AXIS3_M3_PIN         50               // SPI MISO
#else
  #define AXIS3_M0_PIN         OFF              // SPI MOSI
  #define AXIS3_M1_PIN         OFF              // SPI SCK
  #define AXIS3_M2_PIN         OFF              // SPI CS
  #define AXIS3_M3_PIN         OFF              // SPI MISO
#endif
#define AXIS3_STEP_PIN         46
#define AXIS3_DIR_PIN          48

// For focuser1 stepper driver on RAMPS E0
#define AXIS4_ENABLE_PIN       24
#if PINMAP == MksGenL2 || PINMAP == MksGenL21
  #define AXIS4_M0_PIN         51               // SPI MOSI
  #define AXIS4_M1_PIN         52               // SPI SCK
  #define AXIS4_M2_PIN         A12              // SPI CS
  #define AXIS4_M3_PIN         50               // SPI MISO
#else
  #define AXIS4_M0_PIN         OFF              // SPI MOSI
  #define AXIS4_M1_PIN         OFF              // SPI SCK
  #define AXIS4_M2_PIN         OFF              // SPI CS
  #define AXIS4_M3_PIN         OFF              // SPI MISO
#endif
#define AXIS4_STEP_PIN         26
#define AXIS4_DIR_PIN          28

// For focuser2 stepper driver on RAMPS E1
#define AXIS5_ENABLE_PIN       30
#if PINMAP == MksGenL2 || PINMAP == MksGenL21
  #define AXIS5_M0_PIN         51               // SPI MOSI
  #define AXIS5_M1_PIN         52               // SPI SCK
  #if PINMAP == MksGenL21
    #define AXIS5_M2_PIN       12               // SPI CS
  #else
    #define AXIS5_M2_PIN       21               // SPI CS
  #endif
  #define AXIS5_M3_PIN         50               // SPI MISO
#else
  #define AXIS5_M0_PIN         OFF              // SPI MOSI
  #define AXIS5_M1_PIN         OFF              // SPI SCK
  #define AXIS5_M2_PIN         OFF              // SPI CS
  #define AXIS5_M3_PIN         OFF              // SPI MISO
#endif
#define AXIS5_STEP_PIN         36
#define AXIS5_DIR_PIN          34

#if PINMAP == MksGenL2 || PINMAP == MksGenL21
  // ST4 interface on MksGenL2 EXP-1
  #define ST4_RA_W_PIN         27               // ST4 RA- West
  #define ST4_DEC_S_PIN        23               // ST4 DE- South
  #define ST4_DEC_N_PIN        25               // ST4 DE+ North
  #define ST4_RA_E_PIN         35               // ST4 RA+ East
#else
  // ST4 interface on RAMPS AUX-2
  #define ST4_RA_W_PIN         A9               // ST4 RA- West
  #define ST4_DEC_S_PIN        40               // ST4 DE- South
  #define ST4_DEC_N_PIN        42               // ST4 DE+ North
  #define ST4_RA_E_PIN         A11              // ST4 RA+ East
#endif

#else
#error "Wrong processor for this configuration!"

#endif
