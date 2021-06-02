// -------------------------------------------------------------------------------------------------
// Pin map for FYSETC S6
#pragma once

#if defined(STM32F446xx)

// TX2/RX2 (PA2/PA3) is on the Y+ and Z+ end stops and is reserved for GPS (etc, no command channel is associated with this port)
// Use "#define SerialGPS SoftwareSerial2" in Config.h for the GPS only serial port

// Temperature sensors (From Marlin) perhaps these would make better Limit sw and 2x home switches, and PEC.
// They have built-in pullups and a 10uf cap for ESD but they are not labeled as such so leaving it be.
#define Temp0Pin              PC0                // Temp0   (on TE0, THERMO0)
#define Temp1Pin              PC1                // Temp1   (on TE1, THERMO1)
#define Temp2Pin              PC2                // Temp2   (on TE2, THERMO2)
#define Temp3Pin              PC3                // Temp3   (on TB , THERMO3)

// Fans (From Marlin) we use for Auxiliary Features (switches etc.)  Probably with a little crafty wiring these can be 3V3 or 5V.
#define Fan0Pin               PB0                // Fan0    (on FAN0)
#define Fan1Pin               PB1                // Fan1    (on FAN1)
#define Fan2Pin               PB2                // Fan2    (on FAN2)

// Heaters (From Marlin) we use for Auxiliary Features (switches, dew heaters, etc.)
#define Heater0Pin            PB3                // Heater0 (on E0-OUT, E0-Heater)
#define Heater1Pin            PB4                // Heater1 (on E1-OUT, E1-Heater)
#define Heater2Pin            PB15               // Heater2 (on E2-OUT, E2-Heater)
#define Heater3Pin            PC8                // Heater3 (on BED-OUT, Heated-Bed)

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
// I defined 7 Aux pins so they match up with the first 7 Auxiliary Feature slots avaliable in OnStep
// Aux1-3 can be used for pretty much anything
// Aux4-7 are more for dew-heaters
#define Aux1                  Fan0Pin
#define Aux2                  Fan1Pin
#define Aux3                  Fan2Pin
#define Aux4                  Heater0Pin
#define Aux5                  Heater1Pin
#define Aux6                  Heater2Pin
#define Aux7                  Heater3Pin

// Misc. pins
#ifndef DS3234_CS_PIN
  #define DS3234_CS_PIN       PA4                // Default CS Pin for DS3234 on SPI (on EXP2 shared with the ESP8266 RST pin)
#endif
#ifndef BME280_CS_PIN
  #define BME280_CS_PIN       PC7                // Default CS Pin for BME280 on SPI (on EXP2 shared with LED2/Reticle)
#endif
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN        PE0                // Default Pin for OneWire bus (on E2-MOT PD-EN, right hand pin)
#endif
#define ADDON_GPIO0_PIN       PD0                // ESP8266 GPIO0 (on EXP1)
#define ADDON_RESET_PIN       PA4                // ESP8266 RST (on EXP2 shared with the DS3234 CS pin)

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PEC_SENSE_PIN         PA1                // PEC Sense, analog or digital (on X+ so it can have 3v3 or 5v on adjacent pin)

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define STATUS_LED1_PIN       PC6                // Drain (on EXP2) One could perhaps move these to the RGB leds, there's a header but no +5V present on it.
#define STATUS_LED2_PIN       PC7                // Drain (on EXP2 shared with Reticle/BME280_CS)
#define RETICLE_LED_PIN       PC7                // Drain (on EXP2 shared with LED2/BME280_CS)

// For a piezo buzzer
#define BUZZER_PIN            PC9                // Tone (on BEEP/EXP1)

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define SENSE_PPS_PIN         PB10               // PPS time source, GPS for example (on EXP2)

#define SENSE_LIMIT_PIN       PA0                // Limit switch sense (on Z-)

// Soft SPI bus to stepper drivers
#if PINMAP == FYSETC_S6_2
  #define SS_MOSI             PE14
  #define AXIS1_M0_PINPORT    GPIOE
  #define AXIS1_M0_PINBIT     GPIO_PIN_14
  #define SS_SCK              PE12
  #define AXIS1_M1_PINPORT    GPIOE
  #define AXIS1_M1_PINBIT     GPIO_PIN_12
  #define SS_MISO             PE13
  #define AXIS1_M3_PINPORT    GPIOE
  #define AXIS1_M3_PINBIT     GPIO_PIN_13
#else
  #define SS_MOSI             PA7
  #define AXIS1_M0_PINPORT    GPIOA
  #define AXIS1_M0_PINBIT     GPIO_PIN_7
  #define SS_SCK              PA5
  #define AXIS1_M1_PINPORT    GPIOA
  #define AXIS1_M1_PINBIT     GPIO_PIN_5
  #define SS_MISO             PA6
  #define AXIS1_M3_PINPORT    GPIOA
  #define AXIS1_M3_PINBIT     GPIO_PIN_6
#endif

// Axis1 RA/Azm step/dir driver
#if PINMAP == FYSETC_S6_2
  #define AXIS1_ENABLE_PIN    PE9
#else
  #define AXIS1_ENABLE_PIN    PE12
#endif
#define AXIS1_M0_PIN          SS_MOSI            // SPI MOSI
#define AXIS1_M1_PIN          SS_SCK             // SPI SCK
#define AXIS1_M2_PIN          PE7                // SPI CS
#define AXIS1_M2_PINPORT      GPIOE
#define AXIS1_M2_PINBIT       GPIO_PIN_7
#define AXIS1_M3_PIN          SS_MISO            // SPI MISO
#define AXIS1_STEP_PIN        PE11
#define Axis1_StpPORT         GPIOE
#define Axis1_StpBIT          GPIO_PIN_11
#define AXIS1_DIR_PIN         PE10
#define Axis1_DirPORT         GPIOE
#define Axis1_DirBIT          GPIO_PIN_10
#define AXIS1_DECAY_PIN       AXIS1_M2_PIN
#define AXIS1_FAULT_PIN       AXIS1_M3_PIN
#define AXIS1_SENSE_HOME_PIN  PB14               // (on X-)

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN      PD9
#define AXIS2_M0_PIN          SS_MOSI            // SPI MOSI
#define AXIS2_M0_PINPORT      AXIS1_M0_PINPORT
#define AXIS2_M0_PINBIT       AXIS1_M0_PINBIT
#define AXIS2_M1_PIN          SS_SCK             // SPI SCK
#define AXIS2_M1_PINPORT      AXIS1_M1_PINPORT
#define AXIS2_M1_PINBIT       AXIS1_M1_PINBIT
#define AXIS2_M2_PIN          PE15               // SPI CS
#define AXIS2_M2_PINPORT      GPIOE
#define AXIS2_M2_PINBIT       GPIO_PIN_15
#define AXIS2_M3_PIN          SS_MISO            // SPI MISO
#define AXIS2_STEP_PIN        PD8
#define Axis2_StpPORT         GPIOD
#define Axis2_StpBIT          GPIO_PIN_8
#define AXIS2_DIR_PIN         PB12
#define Axis2_DirPORT         GPIOB
#define Axis2_DirBIT          GPIO_PIN_12
#define AXIS2_DECAY_PIN       AXIS2_M2_PIN
#define AXIS2_FAULT_PIN       AXIS2_M3_PIN
#define AXIS2_SENSE_HOME_PIN  PB13               // (on Y-)

// For rotator stepper driver
#define AXIS3_ENABLE_PIN      PD15
#define AXIS3_M0_PIN          SS_MOSI            // SPI MOSI
#define AXIS3_M1_PIN          SS_SCK             // SPI SCK
#define AXIS3_M2_PIN          PD10               // SPI CS
#define AXIS3_M3_PIN          SS_MISO            // SPI MISO
#define AXIS3_STEP_PIN        PD14
#define AXIS3_DIR_PIN         PD13

// For focuser1 stepper driver
#define AXIS4_ENABLE_PIN      PD4
#define AXIS4_M0_PIN          SS_MOSI            // SPI MOSI
#define AXIS4_M1_PIN          SS_SCK             // SPI SCK
#define AXIS4_M2_PIN          PD7                // SPI CS
#define AXIS4_M3_PIN          SS_MISO            // SPI MISO
#define AXIS4_STEP_PIN        PD5
#define AXIS4_DIR_PIN         PD6

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN      PE5
#define AXIS5_M0_PIN          SS_MOSI            // SPI MOSI
#define AXIS5_M1_PIN          SS_SCK             // SPI SCK
#define AXIS5_M2_PIN          PC14               // SPI CS
#define AXIS5_M3_PIN          SS_MISO            // SPI MISO
#define AXIS5_STEP_PIN        PE6
#define AXIS5_DIR_PIN         PC13

// ST4 interface
#define ST4_RA_W_PIN          PA8                // ST4 RA- West  (on EXP1)
#define ST4_DEC_S_PIN         PD2                // ST4 DE- South (on EXP1)
#define ST4_DEC_N_PIN         PC12               // ST4 DE+ North (on EXP1)
#define ST4_RA_E_PIN          PD1                // ST4 RA+ East  (on EXP1)

#else
#error "Wrong processor for this configuration!"

#endif
