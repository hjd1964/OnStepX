// -------------------------------------------------------------------------------------------------
// Pin map for JTW Manticore (ESP32S)
#pragma once

#if defined(ESP32)

// let any special processing the pinmap needs happen
#define PIN_INIT() { \
  pinMode(33, INPUT_PULLDOWN); \
  delay(100); \
  if (digitalRead(33) != HIGH) { \
    do { \
      SERIAL_DEBUG.begin(SERIAL_DEBUG_BAUD); \
      SERIAL_DEBUG.println("ERR: OnStepX firmware detected the SWS MCU; you must upload the SWS firmware here!"); \
      SERIAL_DEBUG.println(analogRead(33)); \
      delay(2000); \
    } while (true); \
  } \
  pinMode(33, INPUT); \
}

// Serial ports (see Pins.defaults.h for SERIAL_A)
// Serial0: RX Pin GPIO3, TX Pin GPIO1 (to USB serial adapter)
// Serial1: RX1 Pin GPIO10, TX1 Pin GPIO9 (on SPI Flash pins, must be moved to be used)
// Serial2: RX2 Pin GPIO16, TX2 Pin GPIO17

#if SERIAL_A_BAUD_DEFAULT != OFF
  #define SERIAL_A              Serial
#endif
#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B              Serial2
  #define SERIAL_B_RX           39
  #define SERIAL_B_TX           18
#endif

// Use the following settings for any TMC UART driver (TMC2209) that may be present
#if defined(STEP_DIR_TMC_UART_PRESENT) || defined(SERVO_TMC2209_PRESENT)
  #define SERIAL_TMC_HARDWARE_UART
  #define SERIAL_TMC            Serial1          // Use a single hardware serial port to up to four drivers
  #define SERIAL_TMC_BAUD       460800           // Baud rate
  #define SERIAL_TMC_RX         17               // Recieving data
  #define SERIAL_TMC_TX         16               // Transmit data
  #define SERIAL_TMC_ADDRESS_MAP(x) ((x==4)?2 : x) // Axis1(0) is 0, Axis2(1) is 1, Axis3(2) is 2, Axis4(3) is 3, Axis5(4) is 2
#endif

// Specify the ESP32 I2C pins
#define I2C_SDA_PIN             21
#define I2C_SCL_PIN             22

// The multi-purpose pins (Aux3..Aux8 can be analog pwm/dac if supported)
#define AUX3_PIN                OFF              // Home SW for Axis1
#define AUX4_PIN                OFF              // Home SW for Axis2

#define ADDON_SELECT_PIN        15

// Misc. pins
#ifndef ONE_WIRE_PIN
  #define ONE_WIRE_PIN          OFF              // Default Pin for OneWire bus
#endif

// The PEC index sense is a logic level input, resets the PEC index on rising edge then waits for 60 seconds before allowing another reset
#ifndef PEC_SENSE_PIN
  #define PEC_SENSE_PIN         36               // PEC Sense, analog (A0) or digital
#endif

// The status LED w/ 4.7k resistor in series to limit the current to the LED
#ifdef STATUS_LED_ON_STATE
  #undef STATUS_LED_ON_STATE
#endif
#define STATUS_LED_ON_STATE HIGH
#ifndef STATUS_LED_PIN
  #define STATUS_LED_PIN        2                // Default LED Anode (+)
#endif
#define MOUNT_LED_PIN           STATUS_LED_PIN   // Default LED Anode (+)
#ifndef RETICLE_LED_PIN
  #define RETICLE_LED_PIN       OFF              // Default LED Cathode (-)
#endif

// For a piezo buzzer
#ifndef STATUS_BUZZER_PIN
  #define STATUS_BUZZER_PIN     2                // Tone
#endif

// The PPS pin is a 3.3V logic input, OnStep measures time between rising edges and adjusts the internal sidereal clock frequency
#ifndef PPS_SENSE_PIN
  #define PPS_SENSE_PIN         23               // PPS time source, GPS for example
#endif

// The limit switch sense is a logic level input normally pull high (2k resistor,) shorted to ground it stops gotos/tracking
#ifndef LIMIT_SENSE_PIN
  #define LIMIT_SENSE_PIN       GPIO_PIN(4)
#endif

#define SHARED_DIRECTION_PINS                    // Hint that the direction pins are shared
#define SHARED_ENABLE_PIN       5                // Hint that the enable pins are shared

// Axis1 RA/Azm step/dir driver
#define AXIS1_ENABLE_PIN        SHARED           // Enable pin control
#define AXIS1_M0_PIN            OFF              // hardwired for TMC UART address 0
#define AXIS1_M1_PIN            OFF              // hardwired for TMC UART address 0
#define AXIS1_M2_PIN            OFF              // UART TX
#define AXIS1_M3_PIN            OFF              // UART RX
#define AXIS1_STEP_PIN          4
#define AXIS1_DIR_PIN           0                // [must be high at boot 0]
#ifndef AXIS1_SENSE_HOME_PIN
  #define AXIS1_SENSE_HOME_PIN  GPIO_PIN(0)
#endif
#define AXIS1_ENCODER_A_PIN     14
#define AXIS1_ENCODER_B_PIN     27

// Axis2 Dec/Alt step/dir driver
#define AXIS2_ENABLE_PIN        SHARED
#define AXIS2_M0_PIN            OFF              // hardwired for TMC UART address 1
#define AXIS2_M1_PIN            OFF              // hardwired for TMC UART address 1
#define AXIS2_M2_PIN            OFF              // N/C
#define AXIS2_M3_PIN            OFF              // UART RX
#define AXIS2_STEP_PIN          12               // [low at boot]
#define AXIS2_DIR_PIN           13
#ifndef AXIS2_SENSE_HOME_PIN
  #define AXIS2_SENSE_HOME_PIN  GPIO_PIN(1)
#endif
#define AXIS2_ENCODER_A_PIN     26
#define AXIS2_ENCODER_B_PIN     25

// ST4 interface
#define ST4_RA_W_PIN            34               // [input only 34] ST4 RA- West
#define ST4_DEC_S_PIN           32               // ST4 DE- South
#define ST4_DEC_N_PIN           33               // ST4 DE+ North
#define ST4_RA_E_PIN            35               // [input only 35] ST4 RA+ East

#else
#error "Wrong processor for this configuration!"

#endif
