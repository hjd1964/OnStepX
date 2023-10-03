// -------------------------------------------------------------------------------------------------
// Pinmap for FYSETC E4 Version 1.0 (ESP32)
#pragma once

#if defined(ESP32)

// Serial ports (see Pins.defaults.h for SERIAL_A)
// Serial0: RX Pin GPIO3, TX Pin GPIO1 (to USB serial adapter)
// Serial1: RX1 Pin GPIO10, TX1 Pin GPIO9 (on SPI Flash pins, must be moved to be used)
// Serial2: RX2 Pin GPIO16, TX2 Pin GPIO17

#if SERIAL_A_BAUD_DEFAULT != OFF
  #define SERIAL_A              Serial
#endif

#define I2C_SDA_PIN             21
#define I2C_SCL_PIN             22

// Use the following settings for the 4x TMC UART drivers
#if defined(STEP_DIR_TMC_UART_PRESENT) || defined(SERVO_TMC2209_PRESENT)
  #define SERIAL_TMC_HARDWARE_UART
  #define SERIAL_TMC            Serial1          // Use a single hardware serial port to up to four drivers
  #define SERIAL_TMC_BAUD       460800           // Baud rate
  #if SERIAL_A_BAUD_DEFAULT == OFF
    // if SERIAL_A is OFF map the hardware serial UART to the Serial0 pins
    // you will need to remove jumpers to allow USB to work for an E4 fimware update
    #define SERIAL_TMC_RX       3                // Recieving data
    #define SERIAL_TMC_TX       1                // Transmit data
    #define SPARE_RX_PIN        OFF              // Set _RX above to 0 (GPIO0) and use 3 here
  #else
    #ifndef SERIAL_TMC_RX
      #define SERIAL_TMC_RX     0                // Recieving data (GPIO0 unused except for flashing)
    #endif
    #define SERIAL_TMC_TX       15               // Transmit data (Z-MIN)
    #define SPARE_RX_PIN        OFF              // Not supported in this case
  #endif
  // map the driver addresses so axis Axis1(0) is X, Axis2(1) is Y, Axis3(2) is Z, Axis4(3) is E0...
  //                                  Axis1(0) is 1, Axis2(1) is 3, Axis3(2) is 0, Axis4(3) is 2, Axis5(4) is 0
  #define SERIAL_TMC_ADDRESS_MAP(x) (((x)==0)?1 : (((x)==1)?3 : (((x)==2)?0 : ((x)==3)?2 : 0)))
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

#ifndef FOCUSER_TEMPERATURE_PIN
  #define FOCUSER_TEMPERATURE_PIN  TEMP0_PIN
#endif
#ifndef FEATURE1_TEMPERATURE_PIN
  #define FEATURE1_TEMPERATURE_PIN TEMP0_PIN
#endif
#ifndef FEATURE2_TEMPERATURE_PIN
  #define FEATURE2_TEMPERATURE_PIN TEMP1_PIN
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
#define MOUNT_LED_PIN           STATUS_LED_PIN   // Default LED Cathode (-)
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

#define SHARED_ENABLE_PIN       25               // Hint that the enable pins are shared

// Axis1 RA/Azm step/dir driver (X-AXIS)
#define AXIS1_ENABLE_PIN        SHARED
#define AXIS1_M0_PIN            OFF              // SPI MOSI
#define AXIS1_M1_PIN            OFF              // SPI SCK
#define AXIS1_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS1_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS1_STEP_PIN          27
#define AXIS1_DIR_PIN           26
#ifndef AXIS1_SENSE_HOME_PIN
  #define AXIS1_SENSE_HOME_PIN  AUX3_PIN
#endif
#ifndef AXIS1_SERVO_PH1_PIN
  #define AXIS1_SERVO_PH1_PIN   OFF
#endif
#ifndef AXIS1_SERVO_PH2_PIN
  #define AXIS1_SERVO_PH2_PIN   OFF
#endif
#ifndef AXIS1_ENCODER_A_PIN
  #define AXIS1_ENCODER_A_PIN   OFF
#endif
#ifndef AXIS1_ENCODER_B_PIN
  #define AXIS1_ENCODER_B_PIN   OFF
#endif

// Axis2 Dec/Alt step/dir driver (Y-AXIS)
#define AXIS2_ENABLE_PIN        SHARED
#define AXIS2_M0_PIN            OFF              // SPI MOSI
#define AXIS2_M1_PIN            OFF              // SPI SCK
#define AXIS2_M2_PIN            OFF              // SPI CS (UART TX)
#define AXIS2_M3_PIN            OFF              // SPI MISO (UART RX)
#define AXIS2_STEP_PIN          33
#define AXIS2_DIR_PIN           32
#ifndef AXIS2_SENSE_HOME_PIN
  #define AXIS2_SENSE_HOME_PIN  AUX4_PIN
#endif
#ifndef AXIS2_SERVO_PH1_PIN
  #define AXIS2_SERVO_PH1_PIN   OFF
#endif
#ifndef AXIS2_SERVO_PH2_PIN
  #define AXIS2_SERVO_PH2_PIN   OFF
#endif
#ifndef AXIS2_ENCODER_PH1_PIN
  #define AXIS2_ENCODER_A_PIN   OFF
#endif
#ifndef AXIS2_ENCODER_B_PIN
  #define AXIS2_ENCODER_B_PIN   OFF
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
