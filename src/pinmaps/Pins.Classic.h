// -------------------------------------------------------------------------------------------------
// Pin map for legacy OnStep Classic (Mega2560 or Teensy3.x)
#pragma once

#if defined(__AVR_ATmega2560__)

#if PINMAP == ClassicInstein
  #warning "This an an highly experimental PINMAP, use at your own risk!!!"
#endif

// Misc. pins
#if defined(ST4_ALTERNATE_PINS_ON) || (PINMAP == ClassicShield)
  #ifndef DS3234_CS_PIN
    #define DS3234_CS_PIN        53              // Default CS Pin for DS3234 on SPI
  #endif
  #ifndef BME280_CS_PIN
    #define BME280_CS_PIN        48              // Default CS Pin for BME280 on SPI
  #endif
#endif

// The PEC index sense resets the PEC index then waits for 60 seconds before allowing another reset
#ifdef PEC_SENSE_ANALOG
  #define PEC_SENSE_PIN          1               // PEC Sense, analog
#else
  #define PEC_SENSE_PIN          2               // PEC Sense, digital
#endif

// The limit switch sense is a 5V logic input which uses the internal (or external 2k) pull up, shorted to ground it stops gotos/tracking
#define SENSE_LIMIT_PIN          3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LED_STATUS_VCC_PIN       8               // LED
#define STATUS_LED_PIN           9               // GND
#define STATUS_MOUNT_LED_PIN     10              // PGND
#define RETICLE_LED_PIN          44              // PGND

// For a piezo buzzer
#define STATUS_BUZZER_PIN        42              // Tone

// The PPS pin is a 5V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define SENSE_PPS_PIN            21              // Interrupt 2 on Pin 21 (alternate Int3 on Pin20)

// Obsolete pins that would power Big Easy Drivers (A4988) back in the old days
#define POWER_SUPPLY_PINS_OFF
#define Axis15vPin               12              // 5V
#define Axis25vPin               5               // 5V
#define Axis2GndPin              7               // GND

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN         25
#define AXIS1_M0_PIN             22              // SPI MOSI
#define AXIS1_M1_PIN             23              // SPI CLK
#define AXIS1_M2_PIN             24              // SPI CS
#define AXIS1_M3_PIN             26              // SPI MISO
#define AXIS1_STEP_PIN           13              // (PB7)
#define AXIS1_DIR_PIN            11              // (PB5)
#define AXIS1_DECAY_PIN          32
#define AXIS1_FAULT_PIN          26

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN         30
#define AXIS2_M0_PIN             27              // SPI MOSI
#define AXIS2_M1_PIN             28              // SPI CLK
#define AXIS2_M2_PIN             29              // SPI CS
#define AXIS2_M3_PIN             31              // SPI MISO
#define AXIS2_STEP_PIN           6               // (PH3)
#define AXIS2_DIR_PIN            4               // (PG5)
#define AXIS2_DECAY_PIN          33
#define AXIS2_FAULT_PIN          31

// Pins to rotator stepper driver
#define AXIS3_ENABLE_PIN         OFF
#define AXIS3_M0_PIN             OFF             // SPI MOSI
#define AXIS3_M1_PIN             OFF             // SPI SCK
#define AXIS3_M2_PIN             OFF             // SPI CS
#define AXIS3_M3_PIN             OFF             // SPI MISO
#define AXIS3_STEP_PIN           A9
#define AXIS3_DIR_PIN            A8

// Pins to focuser1 stepper driver
#define AXIS4_ENABLE_PIN         OFF
#define AXIS4_M0_PIN             OFF             // SPI MOSI
#define AXIS4_M1_PIN             OFF             // SPI SCK
#define AXIS4_M2_PIN             OFF             // SPI CS
#define AXIS4_M3_PIN             OFF             // SPI MISO
#define AXIS4_STEP_PIN           A11
#define AXIS4_DIR_PIN            A10

// Pins to focuser2 stepper driver
#define AXIS5_ENABLE_PIN         OFF
#define AXIS5_M0_PIN             OFF             // SPI MOSI
#define AXIS5_M1_PIN             OFF             // SPI SCK
#define AXIS5_M2_PIN             OFF             // SPI CS
#define AXIS5_M3_PIN             OFF             // SPI MISO
#define AXIS5_STEP_PIN           A13
#define AXIS5_DIR_PIN            A12

// ST4 interface
#if defined(ST4_ALTERNATE_PINS_ON) || (PINMAP == ClassicShield)
  #define ST4_RA_W_PIN           47              // ST4 RA- West
  #define ST4_DEC_S_PIN          43              // ST4 DE- South
  #define ST4_DEC_N_PIN          45              // ST4 DE+ North
  #define ST4_RA_E_PIN           49              // ST4 RA+ East
#elif PINMAP == ClassicInstein
  #define ST4_RA_W_PIN           49              // Socket RJ12 Pin 3 RA- West
  #define ST4_DEC_S_PIN          43              // Socket RJ12 Pin 4 DE- South
  #define ST4_DEC_N_PIN          47              // Socket RJ12 Pin 5 DE+ North
  #define ST4_RA_E_PIN           45              // Socket RJ12 Pin 6 RA+ East
#else
  #define ST4_RA_W_PIN           47              // ST4 RA- West
  #define ST4_DEC_S_PIN          49              // ST4 DE- South
  #define ST4_DEC_N_PIN          51              // ST4 DE+ North
  #define ST4_RA_E_PIN           53              // ST4 RA+ East
#endif

#elif defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)

// The PEC index sense resets the PEC index then waits for 60 seconds before allowing another reset
#ifdef PEC_SENSE_ANALOG
  #define PEC_SENSE_PIN          14              // PEC Sense, analog
#else
  #define PEC_SENSE_PIN          2               // PEC Sense, digital
#endif

// The limit switch sense is a 3.3V logic input which uses the internal pull up, shorted to ground it stops gotos/tracking
#define SENSE_LIMIT_PIN          3

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define LED_STATUS_VCC_PIN       8               // LED Annode
#define STATUS_LED_PIN           9               // Default LED Cathode (-)
#define STATUS_MOUNT_LED_PIN     7               // Default LED Cathode (-)
#define STATUS_ROTATOR_LED_PIN   7               // Default LED Cathode (-)
#define STATUS_FOCUSER_LED_PIN   7               // Default LED Cathode (-)
#define RETICLE_LED_PIN          9               // Default LED Cathode (-)

// For a piezo buzzer
#define STATUS_BUZZER_PIN        29              // Tone

// The PPS pin is a logic level input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define SENSE_PPS_PIN            23              // PPS time source, GPS for example

// Obsolete pins that would power stepper drivers in the old days
#define POWER_SUPPLY_PINS_OFF
#define Axis15vPin               11              // Pin 11 (3.3V)
#define Axis25vPin               5               // Pin 5 (3.3V)
#define Axis2GndPin              7               // Pin 7 (GND)

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN         16
#define AXIS1_M0_PIN             13              // SPI MOSI
#define AXIS1_M1_PIN             14              // SPI CLK
#define AXIS1_M2_PIN             15              // SPI CS
#define AXIS1_M3_PIN             17              // SPI MISO
#define AXIS1_STEP_PIN           12
#define AXIS1_DIR_PIN            10
#define AXIS1_FAULT_PIN          17

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN         21
#define AXIS2_M0_PIN             18              // SPI MOSI
#define AXIS2_M1_PIN             19              // SPI CLK
#define AXIS2_M2_PIN             20              // SPI CS
#define AXIS2_M3_PIN             22              // SPI MISO
#define AXIS2_STEP_PIN           6
#define AXIS2_DIR_PIN            4
#define AXIS2_FAULT_PIN          22

// For rotator stepper driver
#define AXIS3_ENABLE_PIN         OFF
#define AXIS3_STEP_PIN           30
#define AXIS3_DIR_PIN            33

// For focuser1 stepper driver  
#define AXIS4_ENABLE_PIN         OFF
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
  // teensy3.5/3.6
  #define AXIS4_STEP_PIN         34
  #define AXIS4_DIR_PIN          35
#else
  // teensy3.2
  #define AXIS4_STEP_PIN         31
  #define AXIS4_DIR_PIN          32
#endif

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN         OFF
#define AXIS5_STEP_PIN           30
#define AXIS5_DIR_PIN            33

// ST4 interface
#define ST4_RA_W_PIN             24              // ST4 RA- West
#define ST4_DEC_S_PIN            25              // ST4 DE- South
#define ST4_DEC_N_PIN            26              // ST4 DE+ North
#define ST4_RA_E_PIN             27              // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
