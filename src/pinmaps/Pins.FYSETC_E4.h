// -------------------------------------------------------------------------------------------------
// Pinmap for FYSETC E4 Version 1.0 (ESP32)

// ********* EXPERIMENTAL, THIS PINMAP MAY BE CHANGED OR REMOVED AT ANY TIME **********

#pragma once

#if defined(ESP32)

// Serial ports (see Pins.defaults.h for SERIAL_A)
// Serial0: RX Pin GPIO3, TX Pin GPIO1 (to USB serial adapter)
// Serial1: RX1 Pin GPIO10, TX1 Pin GPIO9 (on SPI Flash pins, must be moved to be used)
// Serial2: RX2 Pin GPIO16, TX2 Pin GPIO17

#if SERIAL_A_BAUD_DEFAULT != OFF
  #define SERIAL_A              Serial
#endif

// Use the following settings for any TMC UART driver (TMC2209U) that may be present
#define TMC_UART_DRIVER_ADDRESS_REMAP_AXIS5      // Map driver axis5 to axis3 in hardware serial mode

// map the driver addresses so axis X is 0, Y is 1, Z is 2, and E0 is 3 instead of the actual...
//                                  X is 1, Y is 3, Z is 0, and E0 is 2
#define TMC_UART_DRIVER_ADDRESS_REMAP(x) (((x)==0)?1 : (((x)==1)?3 : (((x)==2)?0 : 2)))

#ifndef DRIVER_UART_HARDWARE_SERIAL
  #define DRIVER_UART_HARDWARE_SERIAL ON         // Default is hardware serial for this board
#endif

#define SERIAL_TMC              Serial1          // Use a single hardware serial port to up to four drivers
#define SERIAL_TMC_BAUD         460800           // Baud rate
#if DRIVER_UART_HARDWARE_SERIAL == ON
  #define SERIAL_TMC_RX         0                // Recieving data (GPIO0 unused except for flashing)
  #define SERIAL_TMC_TX         15               // Transmit data (Z-MIN)
  #define SPARE_RX_PIN          OFF              // Not supported in this case
  #define I2C_SDA_PIN           21
  #define I2C_SCL_PIN           22
#elif DRIVER_UART_HARDWARE_SERIAL == ALT
  #if SERIAL_A_BAUD_DEFAULT == OFF
    // if SERIAL_A is OFF map the hardware serial UART to the Serial0 pins
    // you will need to remove jumpers to allow USB to work for an E4 fimware update
    #define SERIAL_TMC_RX       3                // Recieving data
    #define SERIAL_TMC_TX       1                // Transmit data
    #define SPARE_RX_PIN        OFF              // Set _RX above to 0 (GPIO0) and use 3 here
    #define I2C_SDA_PIN         21
    #define I2C_SCL_PIN         22
  #else
    // if SERIAL_A is ON map the hardware serial UART to the I2C pins, and disable I2C
    #define SERIAL_TMC_RX       21               // Recieving data
    #define SERIAL_TMC_TX       22               // Transmit data
    // SDA/SCL pins are disabled
    #define I2C_SDA_PIN         OFF
    #define I2C_SCL_PIN         OFF
    #define SPARE_RX_PIN        OFF              // Set _RX above to 0 (GPIO0) and use 21 here
  #endif
#else
  #error "Configuration (Config.h): For FYSETC_E4, set DRIVER_UART_HARDWARE_SERIAL to ON (recommended, Z-MIN TX only) or ALT (Serial pins or I2C pins.)"
#endif

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define AUX3_PIN                34               // [input only 34] Home SW for Axis1 (X_MIN)
#define AUX4_PIN                35               // [input only 35] Home SW for Axis2 (Y_MIN)
#define AUX5_PIN                2                // [must be low at boot 2] Dew heater, etc. (HEAT_E0)
#define AUX6_PIN                4                // Dew heater, etc. (HEAT_BED)
#define AUX7_PIN                SPARE_RX_PIN     // Option for 1-Wire, etc.
#define AUX8_PIN                13               // Status LED or Buzzer, Dew heater, etc. (FAN_E0)

// Thermistor (temperature) sensor inputs have built-in 4.7K Ohm pullups and a 10uF cap for noise supression
#define TEMP0_PIN               36               // [input only 36] PEC (TE)
#define TEMP1_PIN               39               // [input only 39] Limit (TB)

#if FOCUSER_TEMPERATURE_PIN == OFF
  #undef FOCUSER_TEMPERATURE_PIN
  #define FOCUSER_TEMPERATURE_PIN TEMP0_PIN
#endif
#if FEATURE1_TEMPERATURE_PIN == OFF
  #undef FEATURE1_TEMPERATURE_PIN
  #define FEATURE1_TEMPERATURE_PIN     TEMP0_PIN
#endif
#if FEATURE2_TEMPERATURE_PIN == OFF
  #undef FEATURE2_TEMPERATURE_PIN
  #define FEATURE2_TEMPERATURE_PIN     TEMP1_PIN
#endif

// Misc. pins
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN          AUX7_PIN         // Default Pin for OneWire bus
#endif

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#ifndef PEC_SENSE_PIN
  #define PEC_SENSE_PIN         36               // [input only 36] PEC Sense, analog or digital (TE)
#endif

// The status LED is a two wire jumper with a 10k resistor in series to limit the current to the LED
#ifndef STATUS_LED_PIN
  #define STATUS_LED_PIN        AUX8_PIN         // Default LED Cathode (-)
#endif
#define MOUNT_STATUS_LED_PIN    STATUS_LED_PIN   // Default LED Cathode (-)
#ifndef RETICLE_LED_PIN 
  #define RETICLE_LED_PIN       STATUS_LED_PIN   // Default LED Cathode (-)
#endif

// For a piezo buzzer
#ifndef STATUS_BUZZER_PIN
  #define STATUS_BUZZER_PIN     AUX8_PIN         // Tone
#endif

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#ifndef PPS_SENSE_PIN
  #define PPS_SENSE_PIN         OFF
#endif

// The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking
#ifndef LIMIT_SENSE_PIN
  #define LIMIT_SENSE_PIN       39               // [input only 39] (TB)
#endif

// Axis1 RA/Azm step/dir driver (X-AXIS)
#define AXIS1_ENABLE_PIN        25               // 
#define AXIS1_M0_PIN            OFF              // SPI MOSI
#define AXIS1_M1_PIN            OFF              // SPI SCK
#define AXIS1_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS1_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS1_STEP_PIN          27
#define AXIS1_DIR_PIN           26
#define AXIS1_DECAY_PIN         OFF
#ifndef AXIS1_SENSE_HOME_PIN
  #define AXIS1_SENSE_HOME_PIN  AUX3_PIN
#endif

// Axis2 Dec/Alt step/dir driver (Y-AXIS)
#define AXIS2_ENABLE_PIN        SHARED
#define AXIS2_M0_PIN            OFF              // SPI MOSI
#define AXIS2_M1_PIN            OFF              // SPI SCK
#define AXIS2_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS2_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS2_STEP_PIN          33
#define AXIS2_DIR_PIN           32
#define AXIS2_DECAY_PIN         OFF
#ifndef AXIS2_SENSE_HOME_PIN
  #define AXIS2_SENSE_HOME_PIN  AUX4_PIN
#endif

// For rotator stepper driver (Z-AXIS)
#define AXIS3_ENABLE_PIN        SHARED
#define AXIS3_M0_PIN            OFF              // SPI MOSI
#define AXIS3_M1_PIN            OFF              // SPI SCK
#define AXIS3_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS3_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS3_STEP_PIN          14 
#define AXIS3_DIR_PIN           12               // [must be low at boot 12]

// For focuser1 stepper driver (E0-AXIS)
#define AXIS4_ENABLE_PIN        SHARED
#define AXIS4_M0_PIN            OFF              // SPI MOSI
#define AXIS4_M1_PIN            OFF              // SPI SCK
#define AXIS4_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS4_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS4_STEP_PIN          16
#define AXIS4_DIR_PIN           17

// For focuser2 stepper driver (Z-AXIS)
#define AXIS5_ENABLE_PIN        SHARED
#define AXIS5_M0_PIN            OFF              // SPI MOSI
#define AXIS5_M1_PIN            OFF              // SPI SCK
#define AXIS5_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS5_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS5_STEP_PIN          14
#define AXIS5_DIR_PIN           12               // [must be low at boot 12]

// ST4 interface
#define ST4_RA_W_PIN            OFF              // ST4 RA- West
#define ST4_DEC_S_PIN           OFF              // ST4 DE- South
#define ST4_DEC_N_PIN           OFF              // ST4 DE+ North
#define ST4_RA_E_PIN            OFF              // ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
