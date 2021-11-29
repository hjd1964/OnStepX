// -------------------------------------------------------------------------------------------------
// Pin map for Greg's ESP32 board
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

// Hint that direction pins use a GPIO device
#define GPIO_DIRECTION_PINS

// The multi-purpose pins
#define AUX1_PIN                GPIO_PIN(8)      // Dew heater 1
#define AUX2_PIN                GPIO_PIN(9)      // Dew heater 2
#define AUX3_PIN                GPIO_PIN(10)     // Dew heater 3
#define AUX4_PIN                GPIO_PIN(2)      // Spare 1
#define AUX5_PIN                GPIO_PIN(3)      // Spare 2

// Misc. pins
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN          OFF              // Default Pin for OneWire bus
#endif
#define ADDON_GPIO0_PIN         OFF              // ESP8266 GPIO0 (Dir2)
#define ADDON_RESET_PIN         OFF              // ESP8266 RST

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#define PEC_SENSE_PIN           36               // PEC Sense

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#define STATUS_LED_PIN          OFF              // Default LED Cathode (-)
#define MOUNT_STATUS_LED_PIN    OFF              // Default LED Cathode (-)
#define STATUS_ROTATOR_LED_PIN  OFF              // Default LED Cathode (-)
#define STATUS_FOCUSER_LED_PIN  OFF              // Default LED Cathode (-)
#define RETICLE_LED_PIN         OFF              // Default LED Cathode (-)

// For a piezo buzzer
#ifndef STATUS_BUZZER_PIN
  #define STATUS_BUZZER_PIN     OFF              // Tone
#endif

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#define PPS_SENSE_PIN           39               // PPS time source, GPS for example

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN        GPIO_PIN(11)
#define AXIS1_M0_PIN            23               // SPI MOSI
#define AXIS1_M1_PIN            18               // SPI SCK
#define AXIS1_M2_PIN            0                // SPI CS
#define AXIS1_M3_PIN            19               // SPI MISO
#define AXIS1_STEP_PIN          26
#define AXIS1_DIR_PIN           GPIO_PIN(12)
#define AXIS1_DECAY_PIN         AXIS1_M2_PIN
#define AXIS1_SENSE_HOME_PIN    GPIO_PIN(0)
#define AXIS1_SENSE_LIMIT_MIN_PIN GPIO_PIN(4)
#define AXIS1_SENSE_LIMIT_MAX_PIN GPIO_PIN(4)

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN        SHARED
#define AXIS2_M0_PIN            23               // SPI MOSI
#define AXIS2_M1_PIN            18               // SPI SCK
#define AXIS2_M2_PIN            2                // SPI CS
#define AXIS2_M3_PIN            19               // SPI MISO
#define AXIS2_STEP_PIN          25
#define AXIS2_DIR_PIN           GPIO_PIN(13)
#define AXIS2_DECAY_PIN         AXIS2_M2_PIN
#define AXIS2_SENSE_HOME_PIN    GPIO_PIN(1)
#define AXIS2_SENSE_LIMIT_MIN_PIN GPIO_PIN(5)
#define AXIS2_SENSE_LIMIT_MAX_PIN GPIO_PIN(5)

// For rotator stepper driver
#define AXIS3_ENABLE_PIN        SHARED           // No enable pin control (always enabled)
#define AXIS3_M0_PIN            23               // SPI MOSI
#define AXIS3_M1_PIN            18               // SPI SCK
#define AXIS3_M2_PIN            4                // SPI CS
#define AXIS3_M3_PIN            19               // SPI MISO
#define AXIS3_STEP_PIN          15
#define AXIS3_DIR_PIN           GPIO_PIN(14)
#define AXIS3_SENSE_LIMIT_MIN_PIN GPIO_PIN(6)
#define AXIS3_SENSE_LIMIT_MAX_PIN GPIO_PIN(6)

// For focuser1 stepper driver
#define AXIS4_ENABLE_PIN        SHARED           // Enable pin on AUX2_PIN but can be turned OFF during validation
#define AXIS4_M0_PIN            23               // SPI MOSI
#define AXIS4_M1_PIN            18               // SPI SCK
#define AXIS4_M2_PIN            14               // SPI CS
#define AXIS4_M3_PIN            19               // SPI MISO
#define AXIS4_STEP_PIN          5
#define AXIS4_DIR_PIN           GPIO_PIN(15)
#define AXIS4_SENSE_LIMIT_MIN_PIN GPIO_PIN(7)
#define AXIS4_SENSE_LIMIT_MAX_PIN GPIO_PIN(7)

// For focuser2 stepper driver
#define AXIS5_ENABLE_PIN        SHARED           // No enable pin control (always enabled)
#define AXIS5_M0_PIN            23               // SPI MOSI
#define AXIS5_M1_PIN            18               // SPI SCK
#define AXIS5_M2_PIN            4                // SPI CS
#define AXIS5_M3_PIN            19               // SPI MISO
#define AXIS5_STEP_PIN          15
#define AXIS5_DIR_PIN           15
#define AXIS5_SENSE_LIMIT_MIN_PIN GPIO_PIN(6)
#define AXIS5_SENSE_LIMIT_MAX_PIN GPIO_PIN(6)

// ST4 interface
#define ST4_RA_W_PIN            34               // ST4 RA- West
#define ST4_DEC_S_PIN           32               // ST4 DE- South
#define ST4_DEC_N_PIN           33               // ST4 DE+ North
#define ST4_RA_E_PIN            35               // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
