// -----------------------------------------------------------------------------------
// controller settings
#pragma once

// host name for this microcontroller, by default used for the following if enabled/supported:
// PRODUCT_DESCRIPTION    the user friendly name for this device, appears on websites etc.
// HOST_NAME              the name ESP WiFi provides to any DHCP server (Ethernet doesn't support this)
// MDNS_NAME              the name mDNS (Multicast DNS) clients see for IP address resolution
// AP_SSID                the SSID WiFi clients see when the ESP WiFi Soft Access Point is enabled
// SERIAL_BT_NAME         the name Bluetooth Servers see when the ESP32 Bluetooth client is enabled
#ifndef HOST_NAME
#define HOST_NAME                    "OnStep"
#endif

// settings identification
#ifndef PRODUCT_DESCRIPTION
#define PRODUCT_DESCRIPTION           HOST_NAME
#endif

// use the HAL specified default NV driver
#ifndef NV_DRIVER
#define NV_DRIVER                     NV_DEFAULT
#endif

// pinmap
#ifndef PINMAP
#define PINMAP                        OFF
#endif

// debug
#ifndef DEBUG
#define DEBUG                         OFF
#endif
#ifndef DEBUG_ECHO_COMMANDS
#define DEBUG_ECHO_COMMANDS           OFF
#endif
#ifndef SERIAL_DEBUG
#define SERIAL_DEBUG                  Serial
#endif
#ifndef SERIAL_DEBUG_BAUD
#define SERIAL_DEBUG_BAUD             9600
#endif

// flag hardware SPI as active
#ifdef DRIVER_TMC_STEPPER_HW_SPI
#ifndef DRIVER_TMC_STEPPER
#define DRIVER_TMC_STEPPER
#endif
#define USES_HW_SPI
#endif

// serial ports
#ifndef SERIAL_A_BAUD_DEFAULT
#define SERIAL_A_BAUD_DEFAULT         9600
#endif
#ifndef SERIAL_B_BAUD_DEFAULT
#define SERIAL_B_BAUD_DEFAULT         OFF
#endif
#ifndef SERIAL_B_ESP_FLASHING
#define SERIAL_B_ESP_FLASHING         OFF
#endif
#ifndef SERIAL_C_BAUD_DEFAULT
#define SERIAL_C_BAUD_DEFAULT         OFF
#endif
#ifndef SERIAL_D_BAUD_DEFAULT
#define SERIAL_D_BAUD_DEFAULT         OFF
#endif
#ifndef SERIAL_E_BAUD_DEFAULT
#define SERIAL_E_BAUD_DEFAULT         OFF
#endif
#ifndef SERIAL_GPS_BAUD
#define SERIAL_GPS_BAUD               9600
#endif

// ESP32 automatically set wifi radio for bluetooth or IP modes 
#ifdef SERIAL_RADIO
#if SERIAL_RADIO == BLUETOOTH
#define SERIAL_BT_MODE SLAVE
#elif SERIAL_RADIO == WIFI_ACCESS_POINT
#define SERIAL_IP_MODE WIFI_ACCESS_POINT
#define WEB_SERVER ON
#elif SERIAL_RADIO == WIFI_STATION
#define SERIAL_IP_MODE WIFI_STATION
#define WEB_SERVER ON
#endif
#endif

// ESP32 virtual serial bluetooth command channel
#ifndef SERIAL_BT_MODE
#define SERIAL_BT_MODE                OFF                         // use SLAVE to enable the interface (ESP32 only)
#endif
#ifndef SERIAL_BT_NAME
#define SERIAL_BT_NAME                HOST_NAME                   // Bluetooth name of command channel
#endif
#ifndef SERIAL_BT_PASSKEY
#define SERIAL_BT_PASSKEY             ""                          // Bluetooth four digit passkey
#endif

// enable and customize WiFi functionality
// for other default IP settings see the file:
// src/lib/wifi/WifiManager.defaults.h

#ifndef MDNS_SERVER
#define MDNS_SERVER                   ON
#endif

#ifndef MDNS_NAME
#define MDNS_NAME                     HOST_NAME
#endif

// translate Config.h IP settings into low level library settings
#ifndef SERIAL_IP_MODE
#define SERIAL_IP_MODE                OFF                         // use settings shown below to enable the interface
#endif
#ifndef SERIAL_SERVER
#define SERIAL_SERVER                 BOTH                        // STANDARD (port 9999) or PERSISTENT (ports 9996 to 9998)
#endif

#if SERIAL_IP_MODE == ETHERNET_W5500
#define OPERATIONAL_MODE ETHERNET_W5500
#elif SERIAL_IP_MODE == ETHERNET_W5100
#define OPERATIONAL_MODE ETHERNET_W5100
#elif SERIAL_IP_MODE == WIFI_ACCESS_POINT
#define OPERATIONAL_MODE WIFI
#define AP_ENABLED true
#elif SERIAL_IP_MODE == WIFI_STATION
#define OPERATIONAL_MODE WIFI
#define STA_ENABLED true
#elif SERIAL_IP_MODE == BOTH
#define OPERATIONAL_MODE WIFI
#define AP_ENABLED true
#define STA_ENABLED true
#endif

#ifndef STA_AP_FALLBACK
#define STA_AP_FALLBACK               true                        // activate SoftAP if station fails to connect
#endif

#ifndef STA_AUTO_RECONNECT
#define STA_AUTO_RECONNECT            true                        // automatically reconnect if connection is dropped
#endif

#ifndef TIME_IP_ADDR
#define TIME_IP_ADDR                  {129,6,15,28}               // for NTP if enabled we often use an address like
#endif                                                            // time-a-g.nist.gov at 129,6,15,28 or 129,6,15,29, 129,6,15,30, etc.

// some CAN defaults
#ifndef CAN_PLUS
#define CAN_PLUS                      OFF                         // Select from CAN_SAN, CAN_ESP32, CAN_MCP2515, CANn_TEENSY4
#endif
#ifndef CAN_BAUD
#define CAN_BAUD                      500000                      // 500000 baud default
#endif
#ifndef CAN_SEND_RATE_MS
#define CAN_SEND_RATE_MS              25                          // 40 Hz CAN controller send message processing rate
#endif
#ifndef CAN_RECV_RATE_MS
#define CAN_RECV_RATE_MS              5                           // 200 Hz CAN controller recv. message processing rate
#endif
#ifndef CAN_RX_PIN
#define CAN_RX_PIN                    OFF                         // for ESP32 CAN interface
#endif
#ifndef CAN_TX_PIN
#define CAN_TX_PIN                    OFF                         // for ESP32 CAN interface
#endif
#ifndef CAN_CS_PIN
#define CAN_CS_PIN                    OFF                         // for MCP2515 SPI CAN controller
#endif
#ifndef CAN_INT_PIN
#define CAN_INT_PIN                   OFF                         // for MCP2515 SPI CAN controller
#endif

// sensors
#ifndef WEATHER
#define WEATHER                       OFF
#endif

// step signal
#ifndef STEP_WAVE_FORM
#define STEP_WAVE_FORM                PULSE
#endif

#if AXIS1_STEP_STATE == AXIS2_STEP_STATE == AXIS3_STEP_STATE == \
    AXIS4_STEP_STATE == AXIS5_STEP_STATE == AXIS6_STEP_STATE == \
    AXIS7_STEP_STATE == AXIS8_STEP_STATE == AXIS9_STEP_STATE == HIGH
  #define DRIVER_STEP_DEFAULTS
#endif

// gpio device
// DS2413: for 2-ch or 4-ch using 1-wire gpio's (one or two devices.)
// SWS: for 8-ch Serial gpio (normally 4 unused encoder pins.)
// MCP23008: for 8-ch I2C gpio.
// MCP23017, X9555, or X8575: for 16-ch I2C gpio.
// SSR74HC595: for up to 32-ch gpio (serial shift register, output only.)
// Works w/most OnStep features, channels assigned in order pin# 512 and up.
#ifndef GPIO_DEVICE
#define GPIO_DEVICE                   OFF
#endif

#ifndef FileVersionConfig
#warning "Configuration (Config.h): FileVersionConfig is undefined, assuming version 5."
#define FileVersionConfig 5
#endif

// shared driver enable pin configuration
#ifndef SHARED_ENABLE_STATE
#define SHARED_ENABLE_STATE           LOW                         // default state of shared ENable pin for motor power on
#endif
#ifndef SHARED2_ENABLE_STATE
#define SHARED2_ENABLE_STATE          LOW
#endif
#ifndef SHARED3_ENABLE_STATE
#define SHARED3_ENABLE_STATE          LOW
#endif

// -----------------------------------------------------------------------------------
// mount settings

// axes
#ifndef AXIS1_DRIVER_MODEL
#define AXIS1_DRIVER_MODEL            OFF                         // specify a driver model to enable
#endif
#ifndef AXIS1_STEPS_PER_DEGREE
#define AXIS1_STEPS_PER_DEGREE        12800.0                     // (micro)steps per degree
#endif
#ifndef AXIS1_REVERSE
#define AXIS1_REVERSE                 OFF                         // reverse rotation direction
#endif
#ifndef AXIS1_POWER_DOWN
#define AXIS1_POWER_DOWN              OFF                         // power down if at standstill
#endif
#ifndef AXIS1_POWER_DOWN_TIME
#define AXIS1_POWER_DOWN_TIME         30000                       // power down time in milliseconds
#endif
#ifndef AXIS1_ENABLE_STATE
#define AXIS1_ENABLE_STATE            LOW                         // default state of ENable pin for motor power on
#endif
#ifndef AXIS1_LIMIT_MIN
#define AXIS1_LIMIT_MIN               -180                        // in degrees
#endif
#ifndef AXIS1_LIMIT_MAX
#define AXIS1_LIMIT_MAX               180                         // in degrees
#endif
#ifdef AXIS1_SYNC_THRESHOLD_DEGREES                               // maximum distance from absolute encoder pos in degrees for syncs
#define AXIS1_SYNC_THRESHOLD lround(AXIS1_SYNC_THRESHOLD_DEGREES*AXIS1_STEPS_PER_DEGREE)
#endif
#ifndef AXIS1_SYNC_THRESHOLD
#define AXIS1_SYNC_THRESHOLD          OFF                         // sync threshold in counts (required for absolute encoders) or OFF
#endif
#ifndef AXIS1_SENSE_HOME
#define AXIS1_SENSE_HOME              OFF                         // HIGH or LOW state when clockwise of home position, seen from front
#endif
#ifndef AXIS1_SENSE_HOME_INIT
#define AXIS1_SENSE_HOME_INIT         INPUT_PULLUP                // pin mode for home sensing
#endif
#ifndef AXIS1_SENSE_HOME_OFFSET
#define AXIS1_SENSE_HOME_OFFSET       0                           // default offset in arc-seconds to home from the sense position
#endif
#ifndef AXIS1_SENSE_HOME_DIST_LIMIT
#define AXIS1_SENSE_HOME_DIST_LIMIT   180.0                       // max distance in degrees
#endif
#ifndef AXIS1_SENSE_LIMIT_MIN
#define AXIS1_SENSE_LIMIT_MIN         LIMIT_SENSE                 // HIGH/LOW to activate min limit
#endif
#ifndef AXIS1_SENSE_LIMIT_MAX
#define AXIS1_SENSE_LIMIT_MAX         LIMIT_SENSE                 // HIGH/LOW to activate max limit
#endif
#ifndef AXIS1_SENSE_LIMIT_INIT
#define AXIS1_SENSE_LIMIT_INIT        LIMIT_SENSE_INIT            // pin mode for limit sensing
#endif

#if AXIS1_DRIVER_MODEL >= STEP_DIR_DRIVER_FIRST && AXIS1_DRIVER_MODEL <= STEP_DIR_DRIVER_LAST
  #define AXIS1_STEP_DIR_PRESENT

  #if AXIS1_DRIVER_MODEL >= TMC_DRIVER_FIRST
    #if AXIS1_DRIVER_MODEL >= TMC_UART_DRIVER_FIRST 
      #define AXIS1_STEP_DIR_TMC_UART
    #else
      #define AXIS1_STEP_DIR_TMC_SPI
    #endif
  #else
    #define AXIS1_STEP_DIR_LEGACY
  #endif

  #ifndef AXIS1_STEP_STATE
  #define AXIS1_STEP_STATE              HIGH                      // default signal transition state for a step
  #endif
  #ifndef AXIS1_M2_ON_STATE
  #define AXIS1_M2_ON_STATE             HIGH                      // default ON state for M2
  #endif
  #ifndef AXIS1_DRIVER_MICROSTEPS
  #define AXIS1_DRIVER_MICROSTEPS       OFF                       // normal microstep mode 
  #endif
  #ifndef AXIS1_DRIVER_MICROSTEPS_GOTO
  #define AXIS1_DRIVER_MICROSTEPS_GOTO  OFF                       // microstep mode to use during slews
  #endif
  #ifndef AXIS1_DRIVER_INTPOL
  #define AXIS1_DRIVER_INTPOL           ON                        // use interpolation if available
  #endif
  #ifndef AXIS1_DRIVER_DECAY
  #define AXIS1_DRIVER_DECAY            OFF                       // OFF for default, TMC STEALTHCHOP
  #endif
  #ifndef AXIS1_DRIVER_DECAY_GOTO
  #define AXIS1_DRIVER_DECAY_GOTO       OFF                       // OFF for default, TMC SPREADCYCLE
  #endif
  #ifndef AXIS1_DRIVER_IHOLD
  #define AXIS1_DRIVER_IHOLD            OFF                       // in mA
  #endif
  #ifndef AXIS1_DRIVER_IRUN
  #define AXIS1_DRIVER_IRUN             OFF                       // in mA
  #endif
  #ifndef AXIS1_DRIVER_IGOTO
  #define AXIS1_DRIVER_IGOTO            OFF                       // in mA
  #endif
  #ifndef AXIS1_DRIVER_STATUS
  #define AXIS1_DRIVER_STATUS           OFF                       // driver status reporting (ON for TMC SPI or HIGH/LOW for fault pin)
  #endif
#endif

#if AXIS1_DRIVER_MODEL >= SERVO_DRIVER_FIRST && AXIS1_DRIVER_MODEL <= SERVO_DRIVER_LAST
  #define AXIS1_SERVO_PRESENT

  #ifndef AXIS1_SERVO_PH1_STATE
  #define AXIS1_SERVO_PH1_STATE         LOW                       // default (inactive) motor driver state, IN1 (SERVO_EE) or PHASE (SERVO_PE) pin
  #endif
  #ifndef AXIS1_SERVO_PH2_STATE
  #define AXIS1_SERVO_PH2_STATE         LOW                       // default (inactive) motor driver state, IN2 or ENABLE (pwm) pin
  #endif

  #ifdef AXIS1_SERVO_VELOCITY_MAX_DPS
  #define AXIS1_SERVO_VELOCITY_MAX      lround(AXIS1_SERVO_VELOCITY_MAX_DPS*AXIS1_MOTOR_STEPS_PER_DEGREE)
  #endif
  #ifndef AXIS1_SERVO_VELOCITY_MAX
  #define AXIS1_SERVO_VELOCITY_MAX      100                       // max velocity, in % for DC motors, in steps/s for stepper motors
  #endif
  #ifndef AXIS1_SERVO_VELOCITY_FACTOR
  #define AXIS1_SERVO_VELOCITY_FACTOR   frequency*0               // converts frequency (counts per second) to velocity (in steps per second or DC motor PWM ADU range)
  #endif
  #ifndef AXIS1_SERVO_VELOCITY_PWMTHRS                            // velocity (in steps per second) to switch from stealthChop to
  #define AXIS1_SERVO_VELOCITY_PWMTHRS  OFF                       // spreadCycle mode, should happen just above the 2x sidereal rate
  #endif                                                          // for TMC2209 or TMC5160 only
  #ifndef AXIS1_SERVO_ACCELERATION
  #define AXIS1_SERVO_ACCELERATION      20                        // acceleration, in %/s
  #endif
  #ifndef AXIS1_SERVO_FEEDBACK
  #define AXIS1_SERVO_FEEDBACK          FB_PID                    // type of feedback: FB_PID
  #endif

  #ifndef AXIS1_SERVO_FLTR
  #define AXIS1_SERVO_FLTR              OFF                       // servo encoder filter: OFF
  #endif

  #ifndef AXIS1_PID_P
  #define AXIS1_PID_P                   2.0                       // P = proportional
  #endif
  #ifndef AXIS1_PID_I
  #define AXIS1_PID_I                   5.0                       // I = integral
  #endif
  #ifndef AXIS1_PID_D
  #define AXIS1_PID_D                   1.0                       // D = derivative
  #endif
  #ifndef AXIS1_PID_P_GOTO
  #define AXIS1_PID_P_GOTO              AXIS1_PID_P               // P = proportional
  #endif
  #ifndef AXIS1_PID_I_GOTO
  #define AXIS1_PID_I_GOTO              AXIS1_PID_I               // I = integral
  #endif
  #ifndef AXIS1_PID_D_GOTO
  #define AXIS1_PID_D_GOTO              AXIS1_PID_D               // D = derivative
  #endif
  #ifndef AXIS1_PID_SENSITIVITY
  #define AXIS1_PID_SENSITIVITY         0                         // 0 to use slewing state, or % power for 100% pid set two (_GOTO)
  #endif

  #ifndef AXIS1_ENCODER
  #define AXIS1_ENCODER                 AB                        // type of encoder: AB, CW_CCW, PULSE_DIR, PULSE_ONLY, SERIAL_BRIDGE
  #endif
  #ifndef AXIS1_ENCODER_ORIGIN
  #define AXIS1_ENCODER_ORIGIN          0                         // +/- offset so encoder count is 0 at home (for absolute encoders)
  #endif
  #ifndef AXIS1_ENCODER_REVERSE
  #define AXIS1_ENCODER_REVERSE         OFF                       // reverse count direction of encoder
  #endif
#endif
#if AXIS1_DRIVER_MODEL == ODRIVE
  #define AXIS1_ODRIVE_PRESENT
  #ifndef AXIS1_ODRIVE_P
  #define AXIS1_ODRIVE_P                2.0                       // P = proportional
  #endif
  #ifndef AXIS1_ODRIVE_I
  #define AXIS1_ODRIVE_I                5.0                       // I = integral
  #endif
  #ifndef AXIS1_ODRIVE_D
  #define AXIS1_ODRIVE_D                1.0                       // D = derivative
  #endif
#endif
#if AXIS1_DRIVER_MODEL == KTECH
  #define AXIS1_KTECH_PRESENT
#endif

#ifndef AXIS2_DRIVER_MODEL
#define AXIS2_DRIVER_MODEL            OFF                         // specify a driver to enable
#endif
#ifndef AXIS2_STEPS_PER_DEGREE
#define AXIS2_STEPS_PER_DEGREE        12800.0                     // (micro)steps per degree of movement
#endif
#ifndef AXIS2_REVERSE
#define AXIS2_REVERSE                 OFF                         // reverse movement
#endif
#ifndef AXIS2_POWER_DOWN
#define AXIS2_POWER_DOWN              OFF                         // power down at standstill
#endif
#ifndef AXIS2_POWER_DOWN_TIME
#define AXIS2_POWER_DOWN_TIME         30000                       // power off time in milliseconds
#endif
#ifndef AXIS2_ENABLE_STATE
#define AXIS2_ENABLE_STATE            LOW                         // stepper enable state
#endif
#ifndef AXIS2_LIMIT_MIN
#define AXIS2_LIMIT_MIN               -90                         // in degrees
#endif
#ifndef AXIS2_LIMIT_MAX
#define AXIS2_LIMIT_MAX               90                          // in degrees
#endif
#ifdef AXIS2_SYNC_THRESHOLD_DEGREES
#define AXIS2_SYNC_THRESHOLD lround(AXIS2_SYNC_THRESHOLD_DEGREES*AXIS2_STEPS_PER_DEGREE)
#endif
#ifndef AXIS2_SYNC_THRESHOLD
#define AXIS2_SYNC_THRESHOLD          OFF
#endif
#ifndef AXIS2_SENSE_HOME
#define AXIS2_SENSE_HOME              OFF                         // HIGH or LOW state when clockwise of home position, seen from above
#endif
#ifndef AXIS2_SENSE_HOME_INIT
#define AXIS2_SENSE_HOME_INIT         INPUT_PULLUP                // pin mode for home sensing
#endif
#ifndef AXIS2_SENSE_HOME_OFFSET
#define AXIS2_SENSE_HOME_OFFSET       0                           // default offset in arc-seconds to home from the sense position
#endif
#ifndef AXIS2_SENSE_HOME_DIST_LIMIT
#define AXIS2_SENSE_HOME_DIST_LIMIT   180.0                       // max distance in degrees
#endif
#ifndef AXIS2_SENSE_LIMIT_MIN
#define AXIS2_SENSE_LIMIT_MIN         LIMIT_SENSE                 // HIGH/LOW to activate min limit
#endif
#ifndef AXIS2_SENSE_LIMIT_MAX
#define AXIS2_SENSE_LIMIT_MAX         LIMIT_SENSE                 // HIGH/LOW to activate max limit
#endif
#ifndef AXIS2_SENSE_LIMIT_INIT
#define AXIS2_SENSE_LIMIT_INIT        LIMIT_SENSE_INIT            // pin mode for limit sensing
#endif
#if AXIS2_DRIVER_MODEL >= STEP_DIR_DRIVER_FIRST && AXIS2_DRIVER_MODEL <= STEP_DIR_DRIVER_LAST
  #define AXIS2_STEP_DIR_PRESENT
  #if AXIS2_DRIVER_MODEL >= TMC_DRIVER_FIRST
    #if AXIS2_DRIVER_MODEL >= TMC_UART_DRIVER_FIRST 
      #define AXIS2_STEP_DIR_TMC_UART
    #else
      #define AXIS2_STEP_DIR_TMC_SPI
    #endif
  #else
    #define AXIS2_STEP_DIR_LEGACY
  #endif

  #ifndef AXIS2_STEP_STATE
  #define AXIS2_STEP_STATE              HIGH
  #endif
  #ifndef AXIS2_M2_ON_STATE
  #define AXIS2_M2_ON_STATE             HIGH
  #endif
  #ifndef AXIS2_DRIVER_MICROSTEPS
  #define AXIS2_DRIVER_MICROSTEPS       OFF
  #endif
  #ifndef AXIS2_DRIVER_MICROSTEPS_GOTO
  #define AXIS2_DRIVER_MICROSTEPS_GOTO  OFF
  #endif
  #ifndef AXIS2_DRIVER_INTPOL
  #define AXIS2_DRIVER_INTPOL           ON
  #endif
  #ifndef AXIS2_DRIVER_DECAY
  #define AXIS2_DRIVER_DECAY            OFF
  #endif
  #ifndef AXIS2_DRIVER_DECAY_GOTO
  #define AXIS2_DRIVER_DECAY_GOTO       OFF
  #endif
  #ifndef AXIS2_DRIVER_IHOLD
  #define AXIS2_DRIVER_IHOLD            OFF
  #endif
  #ifndef AXIS2_DRIVER_IRUN
  #define AXIS2_DRIVER_IRUN             OFF
  #endif
  #ifndef AXIS2_DRIVER_IGOTO
  #define AXIS2_DRIVER_IGOTO            OFF
  #endif
  #ifndef AXIS2_DRIVER_STATUS
  #define AXIS2_DRIVER_STATUS           OFF
  #endif
#endif

#if AXIS2_DRIVER_MODEL >= SERVO_DRIVER_FIRST && AXIS2_DRIVER_MODEL <= SERVO_DRIVER_LAST
  #define AXIS2_SERVO_PRESENT

  #ifndef AXIS2_SERVO_PH1_STATE
  #define AXIS2_SERVO_PH1_STATE         LOW
  #endif
  #ifndef AXIS2_SERVO_PH2_STATE
  #define AXIS2_SERVO_PH2_STATE         LOW
  #endif

  #ifdef AXIS2_SERVO_VELOCITY_MAX_DPS
  #define AXIS2_SERVO_VELOCITY_MAX      lround(AXIS2_SERVO_VELOCITY_MAX_DPS*AXIS2_MOTOR_STEPS_PER_DEGREE)
  #endif
  #ifndef AXIS2_SERVO_VELOCITY_MAX
  #define AXIS2_SERVO_VELOCITY_MAX      100
  #endif
  #ifndef AXIS2_SERVO_VELOCITY_FACTOR
  #define AXIS2_SERVO_VELOCITY_FACTOR   frequency*0
  #endif
  #ifndef AXIS2_SERVO_VELOCITY_PWMTHRS
  #define AXIS2_SERVO_VELOCITY_PWMTHRS  OFF
  #endif
  #ifndef AXIS2_SERVO_ACCELERATION
  #define AXIS2_SERVO_ACCELERATION      20
  #endif
  #ifndef AXIS2_SERVO_FEEDBACK
  #define AXIS2_SERVO_FEEDBACK          FB_PID
  #endif

  #ifndef AXIS2_SERVO_FLTR
  #define AXIS2_SERVO_FLTR              OFF
  #endif

  #ifndef AXIS2_PID_P
  #define AXIS2_PID_P                   2.0
  #endif
  #ifndef AXIS2_PID_I
  #define AXIS2_PID_I                   5.0
  #endif
  #ifndef AXIS2_PID_D
  #define AXIS2_PID_D                   1.0
  #endif
  #ifndef AXIS2_PID_P_GOTO
  #define AXIS2_PID_P_GOTO              AXIS2_PID_P
  #endif
  #ifndef AXIS2_PID_I_GOTO
  #define AXIS2_PID_I_GOTO              AXIS2_PID_I
  #endif
  #ifndef AXIS2_PID_D_GOTO
  #define AXIS2_PID_D_GOTO              AXIS2_PID_D
  #endif
  #ifndef AXIS2_PID_SENSITIVITY
  #define AXIS2_PID_SENSITIVITY         0
  #endif

  #ifndef AXIS2_ENCODER
  #define AXIS2_ENCODER                 AB
  #endif
  #ifndef AXIS2_ENCODER_ORIGIN
  #define AXIS2_ENCODER_ORIGIN          0
  #endif
  #ifndef AXIS2_ENCODER_REVERSE
  #define AXIS2_ENCODER_REVERSE         OFF
  #endif
#endif
#if AXIS2_DRIVER_MODEL == ODRIVE
  #define AXIS2_ODRIVE_PRESENT
  #ifndef AXIS2_ODRIVE_P
  #define AXIS2_ODRIVE_P                 2.0                       // P = proportional
  #endif
  #ifndef AXIS2_ODRIVE_I
  #define AXIS2_ODRIVE_I                 5.0                       // I = integral
  #endif
  #ifndef AXIS2_ODRIVE_D
  #define AXIS2_ODRIVE_D                 1.0                       // D = derivative
  #endif
#endif
#if AXIS2_DRIVER_MODEL == KTECH
  #define AXIS2_KTECH_PRESENT
#endif

// decode internal mount type, tangent arm, azm wrap
#ifndef MOUNT_TYPE
#define MOUNT_TYPE                    GEM
#endif
#if MOUNT_TYPE == GEM
#define MOUNT_SUBTYPE                 GEM
#endif
#if MOUNT_TYPE == GEM_TA
#define MOUNT_SUBTYPE                 GEM
#define AXIS2_TANGENT_ARM             ON
#endif
#if MOUNT_TYPE == GEM_TAC
#define MOUNT_SUBTYPE                 GEM
#define AXIS2_TANGENT_ARM             ON
#define AXIS2_TANGENT_ARM_CORRECTION  ON
#endif
#if MOUNT_TYPE == FORK
#define MOUNT_SUBTYPE                 FORK
#endif
#if MOUNT_TYPE == FORK_TA
#define MOUNT_SUBTYPE                 FORK
#define AXIS2_TANGENT_ARM             ON
#endif
#if MOUNT_TYPE == FORK_TAC
#define MOUNT_SUBTYPE                 FORK
#define AXIS2_TANGENT_ARM             ON
#define AXIS2_TANGENT_ARM_CORRECTION  ON
#endif
#if MOUNT_TYPE == ALTALT
#define MOUNT_SUBTYPE                 ALTALT
#endif
#if MOUNT_TYPE == ALTAZM
#define MOUNT_SUBTYPE                 ALTAZM
#endif
#if MOUNT_TYPE == ALTAZM_UNL
#define MOUNT_SUBTYPE                 ALTAZM
#define AXIS1_WRAP                    ON
#endif

#ifndef MOUNT_SUBTYPE
#define MOUNT_SUBTYPE                 OFF
#endif
#ifndef MOUNT_ALTERNATE_ORIENTATION
#define MOUNT_ALTERNATE_ORIENTATION   OFF                         // use ON or OFF, for FORK and ALTAZM mounts only
#endif
#ifndef MOUNT_COORDS
#define MOUNT_COORDS                  TOPOCENTRIC                 // mount coordinate system
#endif
#ifndef MOUNT_COORDS_MEMORY
#define MOUNT_COORDS_MEMORY           OFF                         // ON Enables mount position memory
#endif
#ifndef MOUNT_ENABLE_IN_STANDBY
#define MOUNT_ENABLE_IN_STANDBY       OFF                         // ON Enables mount motor drivers in standby
#endif
#ifndef MOUNT_AUTO_HOME_DEFAULT
#define MOUNT_AUTO_HOME_DEFAULT       OFF                         // ON default find home at boot
#endif
#ifndef MOUNT_HOME_AT_OFFSETS
#define MOUNT_HOME_AT_OFFSETS         OFF                         // ON to incorporate runtime offsets into home position (switches)
#endif
#ifndef MOUNT_HORIZON_AVOIDANCE
#define MOUNT_HORIZON_AVOIDANCE       ON                          // ON allows eq mode horizon avoidance
#endif

#ifndef AXIS1_TARGET_TOLERANCE
#define AXIS1_TARGET_TOLERANCE        0.0F                        // distance in arc-seconds when goto is at destination
#endif
#ifndef AXIS1_HOME_TOLERANCE                                      // distance in arc-seconds when at home
#define AXIS1_HOME_TOLERANCE          AXIS1_TARGET_TOLERANCE + (1800.0/AXIS1_STEPS_PER_DEGREE)
#endif
#ifndef AXIS1_SECTOR_GEAR
#define AXIS1_SECTOR_GEAR             OFF                         // special case of a Sector Gear RA with limited travel
#endif
#ifndef AXIS1_WRAP
#define AXIS1_WRAP                    OFF                         // allow unlimited motion about an Azimuth axis
#endif

#ifndef AXIS2_TARGET_TOLERANCE
#define AXIS2_TARGET_TOLERANCE        0.0F                        // in arc-seconds
#endif
#ifndef AXIS2_HOME_TOLERANCE                                      // in arc-seconds
#define AXIS2_HOME_TOLERANCE          AXIS2_TARGET_TOLERANCE + (1800.0/AXIS2_STEPS_PER_DEGREE)
#endif
#ifndef AXIS2_TANGENT_ARM
#define AXIS2_TANGENT_ARM             OFF                         // special case of a Tangent Arm Dec with limited travel
#endif
#ifndef AXIS2_TANGENT_ARM_CORRECTION
#define AXIS2_TANGENT_ARM_CORRECTION  OFF                         // apply formula below to correct for Tangent Arm geometry
#endif
#ifndef TANGENT_ARM_INSTRUMENT_TO_MOUNT
#define TANGENT_ARM_INSTRUMENT_TO_MOUNT(a) (atan(a))              // returns angle in radians
#endif
#ifndef TANGENT_ARM_MOUNT_TO_INSTRUMENT
#define TANGENT_ARM_MOUNT_TO_INSTRUMENT(a) (tan(a))               // angle (a) in radians
#endif

// user feedback
#ifndef STATUS_LED
#define STATUS_LED                    OFF
#endif
#ifndef STATUS_LED_ON_STATE
#define STATUS_LED_ON_STATE           LOW
#endif
#ifndef STATUS_MOUNT_LED
#define STATUS_MOUNT_LED              OFF
#endif
#ifndef MOUNT_LED_ON_STATE
#define MOUNT_LED_ON_STATE            LOW
#endif
#ifndef STATUS_BUZZER
#define STATUS_BUZZER                 OFF
#endif
#ifndef STATUS_BUZZER_ON_STATE
#define STATUS_BUZZER_ON_STATE        HIGH
#endif
#ifndef STATUS_BUZZER_DEFAULT
#define STATUS_BUZZER_DEFAULT         OFF
#endif
#ifndef STATUS_BUZZER_MEMORY
#define STATUS_BUZZER_MEMORY          OFF
#endif

// reticle
#ifndef RETICLE_LED_DEFAULT
#define RETICLE_LED_DEFAULT           OFF
#endif
#ifndef RETICLE_LED_MEMORY
#define RETICLE_LED_MEMORY            OFF
#endif
#ifndef RETICLE_LED_INVERT
#define RETICLE_LED_INVERT            OFF
#endif

// time and location
#ifndef TIME_LOCATION_SOURCE
#define TIME_LOCATION_SOURCE          OFF                        // specify device to get date/time and optionally location
#endif
#ifndef TIME_LOCATION_SOURCE_FALLBACK                            // alternate TLS, must be differnet than above and not GPS or NTP
#define TIME_LOCATION_SOURCE_FALLBACK OFF
#endif
#ifndef TIME_LOCATION_PPS_SENSE
#define TIME_LOCATION_PPS_SENSE       OFF
#endif
#ifndef TIME_LOCATION_PPS_SYNC
#define TIME_LOCATION_PPS_SYNC        OFF                         // adjust timer rates to keep time in sync with PPS
#endif

// limits
#ifndef LIMIT_SENSE
#define LIMIT_SENSE                   OFF                         // applied to AXIS1 and AXIS2
#endif
#ifndef LIMIT_SENSE_INIT
#define LIMIT_SENSE_INIT              INPUT_PULLUP                // applied to AXIS1 and AXIS2
#endif
#ifndef LIMIT_SENSE_STRICT
#define LIMIT_SENSE_STRICT            OFF                         // for all axes, ON blocks motion if min/max are on the same pin
#endif
#ifndef LIMIT_STRICT
#define LIMIT_STRICT                  ON                          // ON enables Mount limits at startup if date/time are set
#endif

// st4
#ifndef ST4_INTERFACE
#define ST4_INTERFACE                 OFF
#endif
#ifndef ST4_INTERFACE_INIT
#define ST4_INTERFACE_INIT            INPUT_PULLUP
#endif
#ifndef ST4_HAND_CONTROL
#define ST4_HAND_CONTROL              OFF
#endif
#ifndef ST4_HAND_CONTROL_FOCUSER
#define ST4_HAND_CONTROL_FOCUSER      OFF
#endif

// park
#ifndef PARK_SENSE
#define PARK_SENSE                    OFF
#endif
#ifndef PARK_SENSE_INIT
#define PARK_SENSE_INIT               INPUT_PULLUP
#endif

#ifndef PARK_SIGNAL
#define PARK_SIGNAL                   OFF
#endif
#ifndef PARK_SIGNAL_INIT
#define PARK_SIGNAL_INIT              INPUT_PULLUP
#endif

#ifndef PARK_STATUS
#define PARK_STATUS                   OFF
#endif

#ifndef PARK_STRICT
#define PARK_STRICT                   OFF
#endif

// pec
#ifndef PEC_STEPS_PER_WORM_ROTATION
#define PEC_STEPS_PER_WORM_ROTATION   0
#endif
#ifndef PEC_BUFFER_SIZE_LIMIT
#define PEC_BUFFER_SIZE_LIMIT         720                         // fixed PEC buffer maximum size
#endif
#ifndef PEC_SENSE
#define PEC_SENSE                     OFF
#endif
#ifndef PEC_SENSE_INIT
#define PEC_SENSE_INIT                INPUT_PULLUP
#endif

// guiding
#ifndef GUIDE_TIME_LIMIT
#define GUIDE_TIME_LIMIT              0                           // in seconds, 0 to disable
#endif
#ifndef GUIDE_DISABLE_BACKLASH
#define GUIDE_DISABLE_BACKLASH        OFF                         // disables backlash while pulse-guiding
#endif
#ifndef GUIDE_SEPARATE_PULSE_RATE
#define GUIDE_SEPARATE_PULSE_RATE     ON                          // normally always enabled
#endif

// tracking
#ifndef TRACK_AUTOSTART
#define TRACK_AUTOSTART               OFF                         // begin tracking at startup
#endif
#ifndef TRACK_WITHOUT_LIMITS
#define TRACK_WITHOUT_LIMITS          OFF                         // allow tracking even if limits are disabled
#endif
#ifndef TRACK_COMPENSATION_DEFAULT
#define TRACK_COMPENSATION_DEFAULT    OFF
#endif
#ifndef TRACK_COMPENSATION_MEMORY
#define TRACK_COMPENSATION_MEMORY     OFF
#endif
#ifndef TRACK_BACKLASH_RATE
#define TRACK_BACKLASH_RATE           25
#endif
#ifndef TRACKING_RATE_DEFAULT_HZ
  #define TRACKING_RATE_DEFAULT_HZ    SIDEREAL_RATE_HZ
#endif

// slewing
#ifndef GOTO_FEATURE
#define GOTO_FEATURE                  ON                          // OFF disables goto functionality
#endif
#ifndef SLEW_RATE_BASE_DESIRED
#define SLEW_RATE_BASE_DESIRED        1.0                         // *desired* maximum slew rate, actual slew rate depends on many factors
#endif
#ifndef SLEW_RATE_MEMORY
#define SLEW_RATE_MEMORY              OFF                         // ON to retain runtime slew rate settings across power cycles
#endif
#ifndef AXIS2_SLEW_RATE_PERCENT
#define AXIS2_SLEW_RATE_PERCENT       100.0F                      // allows max slew rate slow down of Axis2 relative to Axis1
#endif
#ifndef SLEW_ACCELERATION_DIST
#define SLEW_ACCELERATION_DIST        5.0                         // distance in degrees to complete acceleration/deceleration
#endif
#ifndef SLEW_RAPID_STOP_DIST
#define SLEW_RAPID_STOP_DIST          2.0                         // distance in degrees for emergency stop
#endif
#ifndef GOTO_OFFSET
#define GOTO_OFFSET                   0.25                        // distance in degrees for goto target unidirectional approach, 0.0 disables
#endif
#ifndef GOTO_OFFSET_ALIGN
#define GOTO_OFFSET_ALIGN             OFF                         // skip final phase of goto for align stars so user tends to
#endif                                                            // approach from the correct direction when centering
#ifndef GOTO_SETTLE_TIME
#define GOTO_SETTLE_TIME             1500                         // settle time in milliseconds for final phase of goto offset
#endif                                                            // allows for settle and encoder sync if available
#ifndef GOTO_REFINE_STAGES
#define GOTO_REFINE_STAGES           1                            // number of times to perform the goto refinement stage
#endif

// meridian flip, pier side
#ifndef MFLIP_SKIP_HOME
#define MFLIP_SKIP_HOME               OFF
#endif
#ifndef MFLIP_PAUSE_HOME_DEFAULT
#define MFLIP_PAUSE_HOME_DEFAULT      OFF
#endif
#ifndef MFLIP_PAUSE_HOME_MEMORY
#define MFLIP_PAUSE_HOME_MEMORY       OFF
#endif
#ifndef MFLIP_AUTOMATIC_DEFAULT
#define MFLIP_AUTOMATIC_DEFAULT       OFF
#endif
#ifndef MFLIP_AUTOMATIC_MEMORY
#define MFLIP_AUTOMATIC_MEMORY        OFF
#endif
#ifndef PIER_SIDE_SYNC_CHANGE_SIDES
#define PIER_SIDE_SYNC_CHANGE_SIDES   OFF
#endif
#ifndef PIER_SIDE_PREFERRED_DEFAULT
#define PIER_SIDE_PREFERRED_DEFAULT   BEST
#endif
#ifndef PIER_SIDE_PREFERRED_MEMORY
#define PIER_SIDE_PREFERRED_MEMORY    OFF
#endif

// align
#ifndef ALIGN_MAX_STARS
#define ALIGN_MAX_STARS               AUTO                        // max num align stars, AUTO for HAL specified default
#endif

#ifndef ALIGN_AUTO_HOME
#define ALIGN_AUTO_HOME               OFF                         // uses home switches to find home before starting the align
#endif

#ifndef ALIGN_MODEL_MEMORY
#define ALIGN_MODEL_MEMORY            OFF                         // restores any pointing model saved in NV at startup
#endif

#define HIGH_SPEED_ALIGN

// -----------------------------------------------------------------------------------
// rotator settings, ROTATOR
#ifndef AXIS3_DRIVER_MODEL
#define AXIS3_DRIVER_MODEL            OFF                         // specify a driver to enable
#endif
#ifndef AXIS3_STEPS_PER_DEGREE
#define AXIS3_STEPS_PER_DEGREE        64.0                        // (micro)steps per degree of movement
#endif
#ifndef AXIS3_REVERSE
#define AXIS3_REVERSE                 OFF                         // reverse movement
#endif
#ifndef AXIS3_POWER_DOWN
#define AXIS3_POWER_DOWN              OFF                         // power down at standstill
#endif
#ifndef AXIS3_POWER_DOWN_TIME
#define AXIS3_POWER_DOWN_TIME         30000                       // power down time in milliseconds
#endif
#ifndef AXIS3_ENABLE_STATE
#define AXIS3_ENABLE_STATE            LOW                         // stepper enable state
#endif
#ifndef AXIS3_SLEW_RATE_BASE_DESIRED
#define AXIS3_SLEW_RATE_BASE_DESIRED  3.0                         // in degrees/sec
#endif
#ifndef AXIS3_ACCELERATION_TIME
#define AXIS3_ACCELERATION_TIME       1.0                         // in seconds, to selected rate
#endif
#ifndef AXIS3_RAPID_STOP_TIME
#define AXIS3_RAPID_STOP_TIME         1.0                         // in seconds, to stop
#endif
#ifndef AXIS3_BACKLASH_RATE
#define AXIS3_BACKLASH_RATE           (AXIS3_SLEW_RATE_BASE_DESIRED/4) // in degrees/sec
#endif
#ifndef AXIS3_LIMIT_MIN
#define AXIS3_LIMIT_MIN               -180                        // in degrees
#endif
#ifndef AXIS3_LIMIT_MAX
#define AXIS3_LIMIT_MAX               180                         // in degrees
#endif
#ifndef AXIS3_SYNC_THRESHOLD
#define AXIS3_SYNC_THRESHOLD          OFF
#endif
#ifndef AXIS3_SENSE_HOME
#define AXIS3_SENSE_HOME              OFF
#endif
#ifndef AXIS3_SENSE_HOME_INIT
#define AXIS3_SENSE_HOME_INIT         INPUT_PULLUP
#endif
#ifndef AXIS3_SENSE_HOME_DIST_LIMIT
#define AXIS3_SENSE_HOME_DIST_LIMIT   180.0                       // max home sense distance in degrees
#endif
#ifndef AXIS3_SENSE_LIMIT_MIN
#define AXIS3_SENSE_LIMIT_MIN         OFF
#endif
#ifndef AXIS3_SENSE_LIMIT_MAX
#define AXIS3_SENSE_LIMIT_MAX         OFF
#endif
#ifndef AXIS3_SENSE_LIMIT_INIT
#define AXIS3_SENSE_LIMIT_INIT        INPUT_PULLUP
#endif
#if AXIS3_DRIVER_MODEL >= STEP_DIR_DRIVER_FIRST && AXIS3_DRIVER_MODEL <= STEP_DIR_DRIVER_LAST
  #define AXIS3_STEP_DIR_PRESENT
  #if AXIS3_DRIVER_MODEL >= TMC_DRIVER_FIRST
    #if AXIS3_DRIVER_MODEL >= TMC_UART_DRIVER_FIRST 
      #define AXIS3_STEP_DIR_TMC_UART
    #else
      #define AXIS3_STEP_DIR_TMC_SPI
    #endif
  #else
    #define AXIS3_STEP_DIR_LEGACY
  #endif

  #ifndef AXIS3_STEP_STATE
  #define AXIS3_STEP_STATE              HIGH
  #endif
  #ifndef AXIS3_M2_ON_STATE
  #define AXIS3_M2_ON_STATE             HIGH
  #endif
  #ifndef AXIS3_DRIVER_MICROSTEPS
  #define AXIS3_DRIVER_MICROSTEPS       OFF
  #endif
  #ifndef AXIS3_DRIVER_MICROSTEPS_GOTO
  #define AXIS3_DRIVER_MICROSTEPS_GOTO  OFF
  #endif
  #ifndef AXIS3_DRIVER_INTPOL
  #define AXIS3_DRIVER_INTPOL           ON
  #endif
  #ifndef AXIS3_DRIVER_DECAY
  #define AXIS3_DRIVER_DECAY            OFF
  #endif
  #ifndef AXIS3_DRIVER_DECAY_GOTO
  #define AXIS3_DRIVER_DECAY_GOTO       OFF
  #endif
  #ifndef AXIS3_DRIVER_IHOLD
  #define AXIS3_DRIVER_IHOLD            OFF
  #endif
  #ifndef AXIS3_DRIVER_IRUN
  #define AXIS3_DRIVER_IRUN             OFF
  #endif
  #ifndef AXIS3_DRIVER_IGOTO
  #define AXIS3_DRIVER_IGOTO            OFF
  #endif
  #ifndef AXIS3_DRIVER_STATUS
  #define AXIS3_DRIVER_STATUS           OFF
  #endif
#endif

#if AXIS3_DRIVER_MODEL >= SERVO_DRIVER_FIRST && AXIS2_DRIVER_MODEL <= SERVO_DRIVER_LAST
  #define AXIS3_SERVO_PRESENT

  #ifndef AXIS3_SERVO_PH1_STATE
  #define AXIS3_SERVO_PH1_STATE         LOW
  #endif
  #ifndef AXIS3_SERVO_PH2_STATE
  #define AXIS3_SERVO_PH2_STATE         LOW
  #endif

  #ifdef AXIS3_SERVO_VELOCITY_MAX_DPS
  #define AXIS3_SERVO_VELOCITY_MAX      lround(AXIS3_SERVO_VELOCITY_MAX_DPS*AXIS3_MOTOR_STEPS_PER_DEGREE)
  #endif
  #ifndef AXIS3_SERVO_VELOCITY_MAX
  #define AXIS3_SERVO_VELOCITY_MAX      100
  #endif
  #ifndef AXIS3_SERVO_VELOCITY_FACTOR
  #define AXIS3_SERVO_VELOCITY_FACTOR   frequency*0
  #endif
  #ifndef AXIS3_SERVO_ACCELERATION
  #define AXIS3_SERVO_ACCELERATION      20
  #endif
  #ifndef AXIS3_SERVO_FEEDBACK
  #define AXIS3_SERVO_FEEDBACK          FB_PID
  #endif

  #ifndef AXIS3_SERVO_FLTR
  #define AXIS3_SERVO_FLTR              OFF
  #endif

  #ifndef AXIS3_PID_P
  #define AXIS3_PID_P                   2.0
  #endif
  #ifndef AXIS3_PID_I
  #define AXIS3_PID_I                   5.0
  #endif
  #ifndef AXIS3_PID_D
  #define AXIS3_PID_D                   1.0
  #endif
  #ifndef AXIS3_PID_P_GOTO
  #define AXIS3_PID_P_GOTO              AXIS3_PID_P
  #endif
  #ifndef AXIS3_PID_I_GOTO
  #define AXIS3_PID_I_GOTO              AXIS3_PID_I
  #endif
  #ifndef AXIS3_PID_D_GOTO
  #define AXIS3_PID_D_GOTO              AXIS3_PID_D
  #endif
  #ifndef AXIS3_PID_SENSITIVITY
  #define AXIS3_PID_SENSITIVITY         0
  #endif

  #ifndef AXIS3_ENCODER
  #define AXIS3_ENCODER                 AB
  #endif
  #ifndef AXIS3_ENCODER_ORIGIN
  #define AXIS3_ENCODER_ORIGIN          0
  #endif
  #ifndef AXIS3_ENCODER_REVERSE
  #define AXIS3_ENCODER_REVERSE         OFF
  #endif
#endif
#if AXIS3_DRIVER_MODEL == KTECH
  #define AXIS3_KTECH_PRESENT
#endif

// -----------------------------------------------------------------------------------
// focuser settings, all

#ifndef FOCUSER_TEMPERATURE
#define FOCUSER_TEMPERATURE           OFF                         // activate and set focuser sensor type DS18B20, THERMISTOR, etc.
#endif
#ifndef FOCUSER_BUTTON_SENSE_OUT
#define FOCUSER_BUTTON_SENSE_OUT      OFF                         // moves focuser out
#endif
#ifndef FOCUSER_BUTTON_SENSE_IN
#define FOCUSER_BUTTON_SENSE_IN       OFF                         // moves focuser in
#endif
#ifndef FOCUSER_BUTTON_SENSE_INIT
#define FOCUSER_BUTTON_SENSE_INIT     INPUT_PULLUP                // pin mode for focuser buttons
#endif
#ifndef FOCUSER_BUTTON_FOCUSER_INDEX
#define FOCUSER_BUTTON_FOCUSER_INDEX  1                           // which focuser to associate pushbutton control with (1 to 6)
#endif
#ifndef FOCUSER_BUTTON_MOVE_RATE
#define FOCUSER_BUTTON_MOVE_RATE      0                           // focuser button move rate, 0 uses last set or specify fixed rate in um/sec
#endif

// -----------------------------------------------------------------------------------
// focuser settings, FOCUSER1
#ifndef AXIS4_DRIVER_MODEL
#define AXIS4_DRIVER_MODEL            OFF                         // specify a driver to enable
#endif
#ifndef AXIS4_STEPS_PER_MICRON
#define AXIS4_STEPS_PER_MICRON        0.5                         // (micro)steps per micron of movement
#endif
#ifndef AXIS4_REVERSE
#define AXIS4_REVERSE                 OFF                         // reverse movement direction
#endif
#ifndef AXIS4_POWER_DOWN
#define AXIS4_POWER_DOWN              OFF                         // power down at standstill
#endif
#ifndef AXIS4_POWER_DOWN_TIME
#define AXIS4_POWER_DOWN_TIME         30000                       // power down time in milliseconds
#endif
#ifndef AXIS4_SLAVED_TO_FOCUSER
#define AXIS4_SLAVED_TO_FOCUSER       0                           // focuser to slave to, or 0 to disable
#endif
#ifndef AXIS4_ENABLE_STATE
#define AXIS4_ENABLE_STATE            LOW                         // enable pin state when driver is active
#endif
#ifndef AXIS4_SLEW_RATE_MINIMUM
#define AXIS4_SLEW_RATE_MINIMUM       20                          // in microns/sec
#endif
#ifndef AXIS4_SLEW_RATE_BASE_DESIRED
#define AXIS4_SLEW_RATE_BASE_DESIRED  500                         // in microns/sec
#endif
#ifndef AXIS4_ACCELERATION_TIME
#define AXIS4_ACCELERATION_TIME       1.0                         // in seconds, to selected rate
#endif
#ifndef AXIS4_RAPID_STOP_TIME
#define AXIS4_RAPID_STOP_TIME         1.0                         // in seconds, to stop
#endif
#ifndef AXIS4_BACKLASH_RATE
#define AXIS4_BACKLASH_RATE           (AXIS4_SLEW_RATE_BASE_DESIRED/4) // in microns/sec
#endif
#ifndef AXIS4_LIMIT_MIN
#define AXIS4_LIMIT_MIN               0                           // in mm
#endif
#ifndef AXIS4_LIMIT_MAX
#define AXIS4_LIMIT_MAX               50                          // in mm
#endif
#ifndef AXIS4_SYNC_THRESHOLD
#define AXIS4_SYNC_THRESHOLD          OFF
#endif
#ifndef AXIS4_HOME_DEFAULT
#define AXIS4_HOME_DEFAULT            MIDDLE                      // use MINIMUM (zero), MIDDLE (half travel), MAXIMUM (full travel), or a position in microns
#endif
#ifndef AXIS4_SENSE_HOME
#define AXIS4_SENSE_HOME              OFF
#endif
#ifndef AXIS4_SENSE_HOME_INIT
#define AXIS4_SENSE_HOME_INIT         INPUT_PULLUP
#endif
#ifndef AXIS4_SENSE_HOME_DIST_LIMIT
#define AXIS4_SENSE_HOME_DIST_LIMIT   50                          // max home sense distance in mm
#endif
#ifndef AXIS4_SENSE_LIMIT_MIN
#define AXIS4_SENSE_LIMIT_MIN         OFF
#endif
#ifndef AXIS4_SENSE_LIMIT_MAX
#define AXIS4_SENSE_LIMIT_MAX         OFF
#endif
#ifndef AXIS4_SENSE_LIMIT_INIT
#define AXIS4_SENSE_LIMIT_INIT        INPUT_PULLUP
#endif
#if AXIS4_DRIVER_MODEL >= STEP_DIR_DRIVER_FIRST && AXIS4_DRIVER_MODEL <= STEP_DIR_DRIVER_LAST
  #define AXIS4_STEP_DIR_PRESENT
  #if AXIS4_DRIVER_MODEL >= TMC_DRIVER_FIRST
    #if AXIS4_DRIVER_MODEL >= TMC_UART_DRIVER_FIRST 
      #define AXIS4_STEP_DIR_TMC_UART
    #else
      #define AXIS4_STEP_DIR_TMC_SPI
    #endif
  #else
    #define AXIS4_STEP_DIR_LEGACY
  #endif

  #ifndef AXIS4_STEP_STATE
  #define AXIS4_STEP_STATE              HIGH
  #endif
  #ifndef AXIS4_M2_ON_STATE
  #define AXIS4_M2_ON_STATE             HIGH
  #endif
  #ifndef AXIS4_DRIVER_MICROSTEPS
  #define AXIS4_DRIVER_MICROSTEPS       OFF
  #endif
  #ifndef AXIS4_DRIVER_MICROSTEPS_GOTO
  #define AXIS4_DRIVER_MICROSTEPS_GOTO  OFF
  #endif
  #ifndef AXIS4_DRIVER_INTPOL
  #define AXIS4_DRIVER_INTPOL           ON
  #endif
  #ifndef AXIS4_DRIVER_DECAY
  #define AXIS4_DRIVER_DECAY            OFF
  #endif
  #ifndef AXIS4_DRIVER_DECAY_GOTO
  #define AXIS4_DRIVER_DECAY_GOTO       OFF
  #endif
  #ifndef AXIS4_DRIVER_IHOLD
  #define AXIS4_DRIVER_IHOLD            OFF
  #endif
  #ifndef AXIS4_DRIVER_IRUN
  #define AXIS4_DRIVER_IRUN             OFF
  #endif
  #ifndef AXIS4_DRIVER_IGOTO
  #define AXIS4_DRIVER_IGOTO            OFF
  #endif
  #ifndef AXIS4_DRIVER_STATUS
  #define AXIS4_DRIVER_STATUS           OFF
  #endif
#endif

#if AXIS4_DRIVER_MODEL >= SERVO_DRIVER_FIRST && AXIS2_DRIVER_MODEL <= SERVO_DRIVER_LAST
  #define AXIS4_SERVO_PRESENT

  #ifndef AXIS4_SERVO_PH1_STATE
  #define AXIS4_SERVO_PH1_STATE         LOW
  #endif
  #ifndef AXIS4_SERVO_PH2_STATE
  #define AXIS4_SERVO_PH2_STATE         LOW
  #endif

  #ifndef AXIS4_SERVO_VELOCITY_MAX
  #define AXIS4_SERVO_VELOCITY_MAX      100
  #endif
  #ifndef AXIS4_SERVO_VELOCITY_FACTOR
  #define AXIS4_SERVO_VELOCITY_FACTOR   frequency*0
  #endif
  #ifndef AXIS4_SERVO_ACCELERATION
  #define AXIS4_SERVO_ACCELERATION      20
  #endif
  #ifndef AXIS4_SERVO_FEEDBACK
  #define AXIS4_SERVO_FEEDBACK          FB_PID
  #endif

  #ifndef AXIS4_SERVO_FLTR
  #define AXIS4_SERVO_FLTR              OFF
  #endif

  #ifndef AXIS4_PID_P
  #define AXIS4_PID_P                 2.0
  #endif
  #ifndef AXIS4_PID_I
  #define AXIS4_PID_I                 5.0
  #endif
  #ifndef AXIS4_PID_D
  #define AXIS4_PID_D                 1.0
  #endif
  #ifndef AXIS4_PID_P_GOTO
  #define AXIS4_PID_P_GOTO            AXIS4_PID_P
  #endif
  #ifndef AXIS4_PID_I_GOTO
  #define AXIS4_PID_I_GOTO            AXIS4_PID_I
  #endif
  #ifndef AXIS4_PID_D_GOTO
  #define AXIS4_PID_D_GOTO            AXIS4_PID_D
  #endif
  #ifndef AXIS4_PID_SENSITIVITY
  #define AXIS4_PID_SENSITIVITY         0
  #endif

  #ifndef AXIS4_ENCODER
  #define AXIS4_ENCODER                 AB
  #endif
  #ifndef AXIS4_ENCODER_ORIGIN
  #define AXIS4_ENCODER_ORIGIN          0
  #endif
  #ifndef AXIS4_ENCODER_REVERSE
  #define AXIS4_ENCODER_REVERSE         OFF
  #endif
#endif
#if AXIS4_DRIVER_MODEL == KTECH
  #define AXIS4_KTECH_PRESENT
#endif

// focuser settings, FOCUSER2
#ifndef AXIS5_DRIVER_MODEL
#define AXIS5_DRIVER_MODEL            OFF
#endif
#ifndef AXIS5_STEPS_PER_MICRON
#define AXIS5_STEPS_PER_MICRON        0.5
#endif
#ifndef AXIS5_REVERSE
#define AXIS5_REVERSE                 OFF
#endif
#ifndef AXIS5_POWER_DOWN
#define AXIS5_POWER_DOWN              OFF
#endif
#ifndef AXIS5_POWER_DOWN_TIME
#define AXIS5_POWER_DOWN_TIME         30000
#endif
#ifndef AXIS5_SLAVED_TO_FOCUSER
#define AXIS5_SLAVED_TO_FOCUSER       0
#endif
#ifndef AXIS5_ENABLE_STATE
#define AXIS5_ENABLE_STATE            LOW
#endif
#ifndef AXIS5_SLEW_RATE_MINIMUM
#define AXIS5_SLEW_RATE_MINIMUM       20
#endif
#ifndef AXIS5_SLEW_RATE_BASE_DESIRED
#define AXIS5_SLEW_RATE_BASE_DESIRED  500
#endif
#ifndef AXIS5_ACCELERATION_TIME
#define AXIS5_ACCELERATION_TIME       1.0
#endif
#ifndef AXIS5_RAPID_STOP_TIME
#define AXIS5_RAPID_STOP_TIME         1.0
#endif
#ifndef AXIS5_BACKLASH_RATE
#define AXIS5_BACKLASH_RATE           (AXIS5_SLEW_RATE_BASE_DESIRED/4)
#endif
#ifndef AXIS5_LIMIT_MIN
#define AXIS5_LIMIT_MIN               0
#endif
#ifndef AXIS5_LIMIT_MAX
#define AXIS5_LIMIT_MAX               50
#endif
#ifndef AXIS5_SYNC_THRESHOLD
#define AXIS5_SYNC_THRESHOLD          OFF
#endif
#ifndef AXIS5_HOME_DEFAULT
#define AXIS5_HOME_DEFAULT            MIDDLE
#endif
#ifndef AXIS5_SENSE_HOME
#define AXIS5_SENSE_HOME              OFF
#endif
#ifndef AXIS5_SENSE_HOME_INIT
#define AXIS5_SENSE_HOME_INIT         INPUT_PULLUP
#endif
#ifndef AXIS5_SENSE_HOME_DIST_LIMIT
#define AXIS5_SENSE_HOME_DIST_LIMIT   50
#endif
#ifndef AXIS5_SENSE_LIMIT_MIN
#define AXIS5_SENSE_LIMIT_MIN         OFF
#endif
#ifndef AXIS5_SENSE_LIMIT_MAX
#define AXIS5_SENSE_LIMIT_MAX         OFF
#endif
#ifndef AXIS5_SENSE_LIMIT_INIT
#define AXIS5_SENSE_LIMIT_INIT        INPUT_PULLUP
#endif
#if AXIS5_DRIVER_MODEL >= STEP_DIR_DRIVER_FIRST && AXIS5_DRIVER_MODEL <= STEP_DIR_DRIVER_LAST
  #define AXIS5_STEP_DIR_PRESENT
  #if AXIS5_DRIVER_MODEL >= TMC_DRIVER_FIRST
    #if AXIS5_DRIVER_MODEL >= TMC_UART_DRIVER_FIRST 
      #define AXIS5_STEP_DIR_TMC_UART
    #else
      #define AXIS5_STEP_DIR_TMC_SPI
    #endif
  #else
    #define AXIS5_STEP_DIR_LEGACY
  #endif

  #ifndef AXIS5_STEP_STATE
  #define AXIS5_STEP_STATE              HIGH
  #endif
  #ifndef AXIS5_M2_ON_STATE
  #define AXIS5_M2_ON_STATE             HIGH
  #endif
  #ifndef AXIS5_DRIVER_MICROSTEPS
  #define AXIS5_DRIVER_MICROSTEPS       OFF
  #endif
  #ifndef AXIS5_DRIVER_MICROSTEPS_GOTO
  #define AXIS5_DRIVER_MICROSTEPS_GOTO  OFF
  #endif
  #ifndef AXIS5_DRIVER_INTPOL
  #define AXIS5_DRIVER_INTPOL           ON
  #endif
  #ifndef AXIS5_DRIVER_DECAY
  #define AXIS5_DRIVER_DECAY            OFF
  #endif
  #ifndef AXIS5_DRIVER_DECAY_GOTO
  #define AXIS5_DRIVER_DECAY_GOTO       OFF
  #endif
  #ifndef AXIS5_DRIVER_IHOLD
  #define AXIS5_DRIVER_IHOLD            OFF
  #endif
  #ifndef AXIS5_DRIVER_IRUN
  #define AXIS5_DRIVER_IRUN             OFF
  #endif
  #ifndef AXIS5_DRIVER_IGOTO
  #define AXIS5_DRIVER_IGOTO            OFF
  #endif
  #ifndef AXIS5_DRIVER_STATUS
  #define AXIS5_DRIVER_STATUS           OFF
  #endif
#endif

#if AXIS5_DRIVER_MODEL >= SERVO_DRIVER_FIRST && AXIS2_DRIVER_MODEL <= SERVO_DRIVER_LAST
  #define AXIS5_SERVO_PRESENT

  #ifndef AXIS5_SERVO_PH1_STATE
  #define AXIS5_SERVO_PH1_STATE         LOW
  #endif
  #ifndef AXIS5_SERVO_PH2_STATE
  #define AXIS5_SERVO_PH2_STATE         LOW
  #endif

  #ifndef AXIS5_SERVO_VELOCITY_MAX
  #define AXIS5_SERVO_VELOCITY_MAX      100
  #endif
  #ifndef AXIS5_SERVO_VELOCITY_FACTOR
  #define AXIS5_SERVO_VELOCITY_FACTOR   frequency*0
  #endif
  #ifndef AXIS5_SERVO_ACCELERATION
  #define AXIS5_SERVO_ACCELERATION      20
  #endif
  #ifndef AXIS5_SERVO_FEEDBACK
  #define AXIS5_SERVO_FEEDBACK          FB_PID
  #endif

  #ifndef AXIS5_SERVO_FLTR
  #define AXIS5_SERVO_FLTR              OFF
  #endif

  #ifndef AXIS5_PID_P
  #define AXIS5_PID_P                   2.0
  #endif
  #ifndef AXIS5_PID_I
  #define AXIS5_PID_I                   5.0
  #endif
  #ifndef AXIS5_PID_D
  #define AXIS5_PID_D                   1.0
  #endif
  #ifndef AXIS5_PID_P_GOTO
  #define AXIS5_PID_P_GOTO              AXIS5_PID_P
  #endif
  #ifndef AXIS5_PID_I_GOTO
  #define AXIS5_PID_I_GOTO              AXIS5_PID_I
  #endif
  #ifndef AXIS5_PID_D_GOTO
  #define AXIS5_PID_D_GOTO              AXIS5_PID_D
  #endif
  #ifndef AXIS5_PID_SENSITIVITY
  #define AXIS5_PID_SENSITIVITY         0
  #endif

  #ifndef AXIS5_ENCODER
  #define AXIS5_ENCODER                 AB
  #endif
  #ifndef AXIS5_ENCODER_ORIGIN
  #define AXIS5_ENCODER_ORIGIN          0
  #endif
  #ifndef AXIS5_ENCODER_REVERSE
  #define AXIS5_ENCODER_REVERSE         OFF
  #endif
#endif
#if AXIS5_DRIVER_MODEL == KTECH
  #define AXIS5_KTECH_PRESENT
#endif

// focuser settings, FOCUSER3
#ifndef AXIS6_DRIVER_MODEL
#define AXIS6_DRIVER_MODEL            OFF
#endif
#ifndef AXIS6_STEPS_PER_MICRON
#define AXIS6_STEPS_PER_MICRON        0.5
#endif
#ifndef AXIS6_REVERSE
#define AXIS6_REVERSE                 OFF
#endif
#ifndef AXIS6_POWER_DOWN
#define AXIS6_POWER_DOWN              OFF
#endif
#ifndef AXIS6_POWER_DOWN_TIME
#define AXIS6_POWER_DOWN_TIME         30000
#endif
#ifndef AXIS6_SLAVED_TO_FOCUSER
#define AXIS6_SLAVED_TO_FOCUSER       0
#endif
#ifndef AXIS6_ENABLE_STATE
#define AXIS6_ENABLE_STATE            LOW
#endif
#ifndef AXIS6_SLEW_RATE_MINIMUM
#define AXIS6_SLEW_RATE_MINIMUM       20
#endif
#ifndef AXIS6_SLEW_RATE_BASE_DESIRED
#define AXIS6_SLEW_RATE_BASE_DESIRED  500
#endif
#ifndef AXIS6_ACCELERATION_TIME
#define AXIS6_ACCELERATION_TIME       1.0
#endif
#ifndef AXIS6_RAPID_STOP_TIME
#define AXIS6_RAPID_STOP_TIME         1.0
#endif
#ifndef AXIS6_BACKLASH_RATE
#define AXIS6_BACKLASH_RATE           (AXIS6_SLEW_RATE_BASE_DESIRED/4)
#endif
#ifndef AXIS6_LIMIT_MIN
#define AXIS6_LIMIT_MIN               0
#endif
#ifndef AXIS6_LIMIT_MAX
#define AXIS6_LIMIT_MAX               50
#endif
#ifndef AXIS6_SYNC_THRESHOLD
#define AXIS6_SYNC_THRESHOLD          OFF
#endif
#ifndef AXIS6_HOME_DEFAULT
#define AXIS6_HOME_DEFAULT            MIDDLE
#endif
#ifndef AXIS6_SENSE_HOME
#define AXIS6_SENSE_HOME              OFF
#endif
#ifndef AXIS6_SENSE_HOME_INIT
#define AXIS6_SENSE_HOME_INIT         INPUT_PULLUP
#endif
#ifndef AXIS6_SENSE_HOME_DIST_LIMIT
#define AXIS6_SENSE_HOME_DIST_LIMIT   50
#endif
#ifndef AXIS6_SENSE_LIMIT_MIN
#define AXIS6_SENSE_LIMIT_MIN         OFF
#endif
#ifndef AXIS6_SENSE_LIMIT_MAX
#define AXIS6_SENSE_LIMIT_MAX         OFF
#endif
#ifndef AXIS6_SENSE_LIMIT_INIT
#define AXIS6_SENSE_LIMIT_INIT        INPUT_PULLUP
#endif
#if AXIS6_DRIVER_MODEL >= STEP_DIR_DRIVER_FIRST && AXIS6_DRIVER_MODEL <= STEP_DIR_DRIVER_LAST
  #define AXIS6_STEP_DIR_PRESENT
  #if AXIS6_DRIVER_MODEL >= TMC_DRIVER_FIRST
    #if AXIS6_DRIVER_MODEL >= TMC_UART_DRIVER_FIRST 
      #define AXIS6_STEP_DIR_TMC_UART
    #else
      #define AXIS6_STEP_DIR_TMC_SPI
    #endif
  #else
    #define AXIS6_STEP_DIR_LEGACY
  #endif

  #ifndef AXIS6_STEP_STATE
  #define AXIS6_STEP_STATE              HIGH
  #endif
  #ifndef AXIS6_M2_ON_STATE
  #define AXIS6_M2_ON_STATE             HIGH
  #endif
  #ifndef AXIS6_DRIVER_MICROSTEPS
  #define AXIS6_DRIVER_MICROSTEPS       OFF
  #endif
  #ifndef AXIS6_DRIVER_MICROSTEPS_GOTO
  #define AXIS6_DRIVER_MICROSTEPS_GOTO  OFF
  #endif
  #ifndef AXIS6_DRIVER_INTPOL
  #define AXIS6_DRIVER_INTPOL           ON
  #endif
  #ifndef AXIS6_DRIVER_DECAY
  #define AXIS6_DRIVER_DECAY            OFF
  #endif
  #ifndef AXIS6_DRIVER_DECAY_GOTO
  #define AXIS6_DRIVER_DECAY_GOTO       OFF
  #endif
  #ifndef AXIS6_DRIVER_IHOLD
  #define AXIS6_DRIVER_IHOLD            OFF
  #endif
  #ifndef AXIS6_DRIVER_IRUN
  #define AXIS6_DRIVER_IRUN             OFF
  #endif
  #ifndef AXIS6_DRIVER_IGOTO
  #define AXIS6_DRIVER_IGOTO            OFF
  #endif
  #ifndef AXIS6_DRIVER_STATUS
  #define AXIS6_DRIVER_STATUS           OFF
  #endif
#endif

#if AXIS6_DRIVER_MODEL >= SERVO_DRIVER_FIRST && AXIS2_DRIVER_MODEL <= SERVO_DRIVER_LAST
  #define AXIS6_SERVO_PRESENT

  #ifndef AXIS6_SERVO_PH1_STATE
  #define AXIS6_SERVO_PH1_STATE         LOW
  #endif
  #ifndef AXIS6_SERVO_PH2_STATE
  #define AXIS6_SERVO_PH2_STATE         LOW
  #endif

  #ifndef AXIS6_SERVO_VELOCITY_MAX
  #define AXIS6_SERVO_VELOCITY_MAX      100
  #endif
  #ifndef AXIS6_SERVO_VELOCITY_FACTOR
  #define AXIS6_SERVO_VELOCITY_FACTOR   frequency*0
  #endif
  #ifndef AXIS6_SERVO_ACCELERATION
  #define AXIS6_SERVO_ACCELERATION      20
  #endif
  #ifndef AXIS6_SERVO_FEEDBACK
  #define AXIS6_SERVO_FEEDBACK          FB_PID
  #endif

  #ifndef AXIS6_SERVO_FLTR
  #define AXIS6_SERVO_FLTR              OFF
  #endif

  #ifndef AXIS6_PID_P
  #define AXIS6_PID_P                   2.0
  #endif
  #ifndef AXIS6_PID_I
  #define AXIS6_PID_I                   5.0
  #endif
  #ifndef AXIS6_PID_D
  #define AXIS6_PID_D                   1.0
  #endif
  #ifndef AXIS6_PID_P_GOTO
  #define AXIS6_PID_P_GOTO              AXIS6_PID_P
  #endif
  #ifndef AXIS6_PID_I_GOTO
  #define AXIS6_PID_I_GOTO              AXIS6_PID_I
  #endif
  #ifndef AXIS6_PID_D_GOTO
  #define AXIS6_PID_D_GOTO              AXIS6_PID_D
  #endif
  #ifndef AXIS6_PID_SENSITIVITY
  #define AXIS6_PID_SENSITIVITY         0
  #endif

  #ifndef AXIS6_ENCODER
  #define AXIS6_ENCODER                 AB
  #endif
  #ifndef AXIS6_ENCODER_ORIGIN
  #define AXIS6_ENCODER_ORIGIN          0
  #endif
  #ifndef AXIS6_ENCODER_REVERSE
  #define AXIS6_ENCODER_REVERSE         OFF
  #endif
#endif
#if AXIS6_DRIVER_MODEL == KTECH
  #define AXIS6_KTECH_PRESENT
#endif

// focuser settings, FOCUSER4
#ifndef AXIS7_DRIVER_MODEL
#define AXIS7_DRIVER_MODEL            OFF
#endif
#ifndef AXIS7_STEPS_PER_MICRON
#define AXIS7_STEPS_PER_MICRON        0.5
#endif
#ifndef AXIS7_REVERSE
#define AXIS7_REVERSE                 OFF
#endif
#ifndef AXIS7_POWER_DOWN
#define AXIS7_POWER_DOWN              OFF
#endif
#ifndef AXIS7_POWER_DOWN_TIME
#define AXIS7_POWER_DOWN_TIME         30000
#endif
#ifndef AXIS7_SLAVED_TO_FOCUSER
#define AXIS7_SLAVED_TO_FOCUSER       0
#endif
#ifndef AXIS7_ENABLE_STATE
#define AXIS7_ENABLE_STATE            LOW
#endif
#ifndef AXIS7_SLEW_RATE_MINIMUM
#define AXIS7_SLEW_RATE_MINIMUM       20
#endif
#ifndef AXIS7_SLEW_RATE_BASE_DESIRED
#define AXIS7_SLEW_RATE_BASE_DESIRED  500
#endif
#ifndef AXIS7_ACCELERATION_TIME
#define AXIS7_ACCELERATION_TIME       1.0
#endif
#ifndef AXIS7_RAPID_STOP_TIME
#define AXIS7_RAPID_STOP_TIME         1.0
#endif
#ifndef AXIS7_BACKLASH_RATE
#define AXIS7_BACKLASH_RATE           (AXIS7_SLEW_RATE_BASE_DESIRED/4)
#endif
#ifndef AXIS7_LIMIT_MIN
#define AXIS7_LIMIT_MIN               0
#endif
#ifndef AXIS7_LIMIT_MAX
#define AXIS7_LIMIT_MAX               50
#endif
#ifndef AXIS7_SYNC_THRESHOLD
#define AXIS7_SYNC_THRESHOLD          OFF
#endif
#ifndef AXIS7_HOME_DEFAULT
#define AXIS7_HOME_DEFAULT            MIDDLE
#endif
#ifndef AXIS7_SENSE_HOME
#define AXIS7_SENSE_HOME              OFF
#endif
#ifndef AXIS7_SENSE_HOME_INIT
#define AXIS7_SENSE_HOME_INIT         INPUT_PULLUP
#endif
#ifndef AXIS7_SENSE_HOME_DIST_LIMIT
#define AXIS7_SENSE_HOME_DIST_LIMIT   50
#endif
#ifndef AXIS7_SENSE_LIMIT_MIN
#define AXIS7_SENSE_LIMIT_MIN         OFF
#endif
#ifndef AXIS7_SENSE_LIMIT_MAX
#define AXIS7_SENSE_LIMIT_MAX         OFF
#endif
#ifndef AXIS7_SENSE_LIMIT_INIT
#define AXIS7_SENSE_LIMIT_INIT        INPUT_PULLUP
#endif
#if AXIS7_DRIVER_MODEL >= STEP_DIR_DRIVER_FIRST && AXIS7_DRIVER_MODEL <= STEP_DIR_DRIVER_LAST
  #define AXIS7_STEP_DIR_PRESENT
  #if AXIS7_DRIVER_MODEL >= TMC_DRIVER_FIRST
    #if AXIS7_DRIVER_MODEL >= TMC_UART_DRIVER_FIRST 
      #define AXIS7_STEP_DIR_TMC_UART
    #else
      #define AXIS7_STEP_DIR_TMC_SPI
    #endif
  #else
    #define AXIS7_STEP_DIR_LEGACY
  #endif

  #ifndef AXIS7_STEP_STATE
  #define AXIS7_STEP_STATE              HIGH
  #endif
  #ifndef AXIS7_M2_ON_STATE
  #define AXIS7_M2_ON_STATE             HIGH
  #endif
  #ifndef AXIS7_DRIVER_MICROSTEPS
  #define AXIS7_DRIVER_MICROSTEPS       OFF
  #endif
  #ifndef AXIS7_DRIVER_MICROSTEPS_GOTO
  #define AXIS7_DRIVER_MICROSTEPS_GOTO  OFF
  #endif
  #ifndef AXIS7_DRIVER_INTPOL
  #define AXIS7_DRIVER_INTPOL           ON
  #endif
  #ifndef AXIS7_DRIVER_DECAY
  #define AXIS7_DRIVER_DECAY            OFF
  #endif
  #ifndef AXIS7_DRIVER_DECAY_GOTO
  #define AXIS7_DRIVER_DECAY_GOTO       OFF
  #endif
  #ifndef AXIS7_DRIVER_IHOLD
  #define AXIS7_DRIVER_IHOLD            OFF
  #endif
  #ifndef AXIS7_DRIVER_IRUN
  #define AXIS7_DRIVER_IRUN             OFF
  #endif
  #ifndef AXIS7_DRIVER_IGOTO
  #define AXIS7_DRIVER_IGOTO            OFF
  #endif
  #ifndef AXIS7_DRIVER_STATUS
  #define AXIS7_DRIVER_STATUS           OFF
  #endif
#endif

#if AXIS7_DRIVER_MODEL >= SERVO_DRIVER_FIRST && AXIS2_DRIVER_MODEL <= SERVO_DRIVER_LAST
  #define AXIS7_SERVO_PRESENT

  #ifndef AXIS7_SERVO_PH1_STATE
  #define AXIS7_SERVO_PH1_STATE         LOW
  #endif
  #ifndef AXIS7_SERVO_PH2_STATE
  #define AXIS7_SERVO_PH2_STATE         LOW
  #endif

  #ifndef AXIS7_SERVO_VELOCITY_MAX
  #define AXIS7_SERVO_VELOCITY_MAX      100
  #endif
  #ifndef AXIS7_SERVO_VELOCITY_FACTOR
  #define AXIS7_SERVO_VELOCITY_FACTOR   frequency*0
  #endif
  #ifndef AXIS7_SERVO_ACCELERATION
  #define AXIS7_SERVO_ACCELERATION      20
  #endif
  #ifndef AXIS7_SERVO_FEEDBACK
  #define AXIS7_SERVO_FEEDBACK          FB_PID
  #endif

  #ifndef AXIS7_SERVO_FLTR
  #define AXIS7_SERVO_FLTR              OFF
  #endif

  #ifndef AXIS7_PID_P
  #define AXIS7_PID_P                   2.0
  #endif
  #ifndef AXIS7_PID_I
  #define AXIS7_PID_I                   5.0
  #endif
  #ifndef AXIS7_PID_D
  #define AXIS7_PID_D                   1.0
  #endif
  #ifndef AXIS7_PID_P_GOTO
  #define AXIS7_PID_P_GOTO              AXIS7_PID_P
  #endif
  #ifndef AXIS7_PID_I_GOTO
  #define AXIS7_PID_I_GOTO              AXIS7_PID_I
  #endif
  #ifndef AXIS7_PID_D_GOTO
  #define AXIS7_PID_D_GOTO              AXIS7_PID_D
  #endif
  #ifndef AXIS7_PID_SENSITIVITY
  #define AXIS7_PID_SENSITIVITY         0
  #endif

  #ifndef AXIS7_ENCODER
  #define AXIS7_ENCODER                 AB
  #endif
  #ifndef AXIS7_ENCODER_ORIGIN
  #define AXIS7_ENCODER_ORIGIN          0
  #endif
  #ifndef AXIS7_ENCODER_REVERSE
  #define AXIS7_ENCODER_REVERSE         OFF
  #endif
#endif
#if AXIS7_DRIVER_MODEL == KTECH
  #define AXIS7_KTECH_PRESENT
#endif

// focuser settings, FOCUSER5
#ifndef AXIS8_DRIVER_MODEL
#define AXIS8_DRIVER_MODEL            OFF
#endif
#ifndef AXIS8_STEPS_PER_MICRON
#define AXIS8_STEPS_PER_MICRON        0.5
#endif
#ifndef AXIS8_REVERSE
#define AXIS8_REVERSE                 OFF
#endif
#ifndef AXIS8_POWER_DOWN
#define AXIS8_POWER_DOWN              OFF
#endif
#ifndef AXIS8_POWER_DOWN_TIME
#define AXIS8_POWER_DOWN_TIME         30000
#endif
#ifndef AXIS8_SLAVED_TO_FOCUSER
#define AXIS8_SLAVED_TO_FOCUSER       0
#endif
#ifndef AXIS8_ENABLE_STATE
#define AXIS8_ENABLE_STATE            LOW
#endif
#ifndef AXIS8_SLEW_RATE_MINIMUM
#define AXIS8_SLEW_RATE_MINIMUM       20
#endif
#ifndef AXIS8_SLEW_RATE_BASE_DESIRED
#define AXIS8_SLEW_RATE_BASE_DESIRED  500
#endif
#ifndef AXIS8_ACCELERATION_TIME
#define AXIS8_ACCELERATION_TIME       1.0
#endif
#ifndef AXIS8_RAPID_STOP_TIME
#define AXIS8_RAPID_STOP_TIME         1.0
#endif
#ifndef AXIS8_BACKLASH_RATE
#define AXIS8_BACKLASH_RATE           (AXIS8_SLEW_RATE_BASE_DESIRED/4)
#endif
#ifndef AXIS8_LIMIT_MIN
#define AXIS8_LIMIT_MIN               0
#endif
#ifndef AXIS8_LIMIT_MAX
#define AXIS8_LIMIT_MAX               50
#endif
#ifndef AXIS8_SYNC_THRESHOLD
#define AXIS8_SYNC_THRESHOLD          OFF
#endif
#ifndef AXIS8_HOME_DEFAULT
#define AXIS8_HOME_DEFAULT            MIDDLE
#endif
#ifndef AXIS8_SENSE_HOME
#define AXIS8_SENSE_HOME              OFF
#endif
#ifndef AXIS8_SENSE_HOME_INIT
#define AXIS8_SENSE_HOME_INIT         INPUT_PULLUP
#endif
#ifndef AXIS8_SENSE_HOME_DIST_LIMIT
#define AXIS8_SENSE_HOME_DIST_LIMIT   50
#endif
#ifndef AXIS8_SENSE_LIMIT_MIN
#define AXIS8_SENSE_LIMIT_MIN         OFF
#endif
#ifndef AXIS8_SENSE_LIMIT_MAX
#define AXIS8_SENSE_LIMIT_MAX         OFF
#endif
#ifndef AXIS8_SENSE_LIMIT_INIT
#define AXIS8_SENSE_LIMIT_INIT        INPUT_PULLUP
#endif
#if AXIS8_DRIVER_MODEL >= STEP_DIR_DRIVER_FIRST && AXIS8_DRIVER_MODEL <= STEP_DIR_DRIVER_LAST
  #define AXIS8_STEP_DIR_PRESENT
  #if AXIS8_DRIVER_MODEL >= TMC_DRIVER_FIRST
    #if AXIS8_DRIVER_MODEL >= TMC_UART_DRIVER_FIRST 
      #define AXIS8_STEP_DIR_TMC_UART
    #else
      #define AXIS8_STEP_DIR_TMC_SPI
    #endif
  #else
    #define AXIS8_STEP_DIR_LEGACY
  #endif

  #ifndef AXIS8_STEP_STATE
  #define AXIS8_STEP_STATE              HIGH
  #endif
  #ifndef AXIS8_M2_ON_STATE
  #define AXIS8_M2_ON_STATE             HIGH
  #endif
  #ifndef AXIS8_DRIVER_MICROSTEPS
  #define AXIS8_DRIVER_MICROSTEPS       OFF
  #endif
  #ifndef AXIS8_DRIVER_MICROSTEPS_GOTO
  #define AXIS8_DRIVER_MICROSTEPS_GOTO  OFF
  #endif
  #ifndef AXIS8_DRIVER_INTPOL
  #define AXIS8_DRIVER_INTPOL           ON
  #endif
  #ifndef AXIS8_DRIVER_DECAY
  #define AXIS8_DRIVER_DECAY            OFF
  #endif
  #ifndef AXIS8_DRIVER_DECAY_GOTO
  #define AXIS8_DRIVER_DECAY_GOTO       OFF
  #endif
  #ifndef AXIS8_DRIVER_IHOLD
  #define AXIS8_DRIVER_IHOLD            OFF
  #endif
  #ifndef AXIS8_DRIVER_IRUN
  #define AXIS8_DRIVER_IRUN             OFF
  #endif
  #ifndef AXIS8_DRIVER_IGOTO
  #define AXIS8_DRIVER_IGOTO            OFF
  #endif
  #ifndef AXIS8_DRIVER_STATUS
  #define AXIS8_DRIVER_STATUS           OFF
  #endif
#endif

#if AXIS8_DRIVER_MODEL >= SERVO_DRIVER_FIRST && AXIS2_DRIVER_MODEL <= SERVO_DRIVER_LAST
  #define AXIS8_SERVO_PRESENT

  #ifndef AXIS8_SERVO_PH1_STATE
  #define AXIS8_SERVO_PH1_STATE         LOW
  #endif
  #ifndef AXIS8_SERVO_PH2_STATE
  #define AXIS8_SERVO_PH2_STATE         LOW
  #endif

  #ifndef AXIS8_SERVO_VELOCITY_MAX
  #define AXIS8_SERVO_VELOCITY_MAX      100
  #endif
  #ifndef AXIS8_SERVO_VELOCITY_FACTOR
  #define AXIS8_SERVO_VELOCITY_FACTOR   frequency*0
  #endif
  #ifndef AXIS8_SERVO_ACCELERATION
  #define AXIS8_SERVO_ACCELERATION      20
  #endif
  #ifndef AXIS8_SERVO_FEEDBACK
  #define AXIS8_SERVO_FEEDBACK          FB_PID
  #endif

  #ifndef AXIS8_SERVO_FLTR
  #define AXIS8_SERVO_FLTR              OFF
  #endif

  #ifndef AXIS8_PID_P
  #define AXIS8_PID_P                   2.0
  #endif
  #ifndef AXIS8_PID_I
  #define AXIS8_PID_I                   5.0
  #endif
  #ifndef AXIS8_PID_D
  #define AXIS8_PID_D                   1.0
  #endif
  #ifndef AXIS8_PID_P_GOTO
  #define AXIS8_PID_P_GOTO              AXIS8_PID_P
  #endif
  #ifndef AXIS8_PID_I_GOTO
  #define AXIS8_PID_I_GOTO              AXIS8_PID_I
  #endif
  #ifndef AXIS8_PID_D_GOTO
  #define AXIS8_PID_D_GOTO              AXIS8_PID_D
  #endif
  #ifndef AXIS8_PID_SENSITIVITY
  #define AXIS8_PID_SENSITIVITY         0
  #endif

  #ifndef AXIS8_ENCODER
  #define AXIS8_ENCODER                 AB
  #endif
  #ifndef AXIS8_ENCODER_ORIGIN
  #define AXIS8_ENCODER_ORIGIN          0
  #endif
  #ifndef AXIS8_ENCODER_REVERSE
  #define AXIS8_ENCODER_REVERSE         OFF
  #endif
#endif
#if AXIS8_DRIVER_MODEL == KTECH
  #define AXIS8_KTECH_PRESENT
#endif

// focuser settings, FOCUSER6
#ifndef AXIS9_DRIVER_MODEL
#define AXIS9_DRIVER_MODEL            OFF
#endif
#ifndef AXIS9_STEPS_PER_MICRON
#define AXIS9_STEPS_PER_MICRON        0.5
#endif
#ifndef AXIS9_REVERSE
#define AXIS9_REVERSE                 OFF
#endif
#ifndef AXIS9_POWER_DOWN
#define AXIS9_POWER_DOWN              OFF
#endif
#ifndef AXIS9_POWER_DOWN_TIME
#define AXIS9_POWER_DOWN_TIME         30000
#endif
#ifndef AXIS9_SLAVED_TO_FOCUSER
#define AXIS9_SLAVED_TO_FOCUSER       0
#endif
#ifndef AXIS9_ENABLE_STATE
#define AXIS9_ENABLE_STATE            LOW
#endif
#ifndef AXIS9_SLEW_RATE_MINIMUM
#define AXIS9_SLEW_RATE_MINIMUM       20
#endif
#ifndef AXIS9_SLEW_RATE_BASE_DESIRED
#define AXIS9_SLEW_RATE_BASE_DESIRED  500
#endif
#ifndef AXIS9_ACCELERATION_TIME
#define AXIS9_ACCELERATION_TIME       1.0
#endif
#ifndef AXIS9_RAPID_STOP_TIME
#define AXIS9_RAPID_STOP_TIME         1.0
#endif
#ifndef AXIS9_BACKLASH_RATE
#define AXIS9_BACKLASH_RATE           (AXIS9_SLEW_RATE_BASE_DESIRED/4)
#endif
#ifndef AXIS9_LIMIT_MIN
#define AXIS9_LIMIT_MIN               0
#endif
#ifndef AXIS9_LIMIT_MAX
#define AXIS9_LIMIT_MAX               50
#endif
#ifndef AXIS9_SYNC_THRESHOLD
#define AXIS9_SYNC_THRESHOLD          OFF
#endif
#ifndef AXIS9_HOME_DEFAULT
#define AXIS9_HOME_DEFAULT            MIDDLE
#endif
#ifndef AXIS9_SENSE_HOME
#define AXIS9_SENSE_HOME              OFF
#endif
#ifndef AXIS9_SENSE_HOME_INIT
#define AXIS9_SENSE_HOME_INIT         INPUT_PULLUP
#endif
#ifndef AXIS9_SENSE_HOME_DIST_LIMIT
#define AXIS9_SENSE_HOME_DIST_LIMIT   50
#endif
#ifndef AXIS9_SENSE_LIMIT_MIN
#define AXIS9_SENSE_LIMIT_MIN         OFF
#endif
#ifndef AXIS9_SENSE_LIMIT_MAX
#define AXIS9_SENSE_LIMIT_MAX         OFF
#endif
#ifndef AXIS9_SENSE_LIMIT_INIT
#define AXIS9_SENSE_LIMIT_INIT        INPUT_PULLUP
#endif
#if AXIS9_DRIVER_MODEL >= STEP_DIR_DRIVER_FIRST && AXIS9_DRIVER_MODEL <= STEP_DIR_DRIVER_LAST
  #define AXIS9_STEP_DIR_PRESENT
  #if AXIS9_DRIVER_MODEL >= TMC_DRIVER_FIRST
    #if AXIS9_DRIVER_MODEL >= TMC_UART_DRIVER_FIRST 
      #define AXIS9_STEP_DIR_TMC_UART
    #else
      #define AXIS9_STEP_DIR_TMC_SPI
    #endif
  #else
    #define AXIS9_STEP_DIR_LEGACY
  #endif

  #ifndef AXIS9_STEP_STATE
  #define AXIS9_STEP_STATE              HIGH
  #endif
  #ifndef AXIS9_M2_ON_STATE
  #define AXIS9_M2_ON_STATE             HIGH
  #endif
  #ifndef AXIS9_DRIVER_MICROSTEPS
  #define AXIS9_DRIVER_MICROSTEPS       OFF
  #endif
  #ifndef AXIS9_DRIVER_MICROSTEPS_GOTO
  #define AXIS9_DRIVER_MICROSTEPS_GOTO  OFF
  #endif
  #ifndef AXIS9_DRIVER_INTPOL
  #define AXIS9_DRIVER_INTPOL           ON
  #endif
  #ifndef AXIS9_DRIVER_DECAY
  #define AXIS9_DRIVER_DECAY            OFF
  #endif
  #ifndef AXIS9_DRIVER_DECAY_GOTO
  #define AXIS9_DRIVER_DECAY_GOTO       OFF
  #endif
  #ifndef AXIS9_DRIVER_IHOLD
  #define AXIS9_DRIVER_IHOLD            OFF
  #endif
  #ifndef AXIS9_DRIVER_IRUN
  #define AXIS9_DRIVER_IRUN             OFF
  #endif
  #ifndef AXIS9_DRIVER_IGOTO
  #define AXIS9_DRIVER_IGOTO            OFF
  #endif
  #ifndef AXIS9_DRIVER_STATUS
  #define AXIS9_DRIVER_STATUS           OFF
  #endif
#endif

#if AXIS9_DRIVER_MODEL >= SERVO_DRIVER_FIRST && AXIS2_DRIVER_MODEL <= SERVO_DRIVER_LAST
  #define AXIS9_SERVO_PRESENT

  #ifndef AXIS9_SERVO_PH1_STATE
  #define AXIS9_SERVO_PH1_STATE         LOW
  #endif
  #ifndef AXIS9_SERVO_PH2_STATE
  #define AXIS9_SERVO_PH2_STATE         LOW
  #endif

  #ifndef AXIS9_SERVO_VELOCITY_MAX
  #define AXIS9_SERVO_VELOCITY_MAX      100
  #endif
  #ifndef AXIS9_SERVO_VELOCITY_FACTOR
  #define AXIS9_SERVO_VELOCITY_FACTOR   frequency*0
  #endif
  #ifndef AXIS9_SERVO_ACCELERATION
  #define AXIS9_SERVO_ACCELERATION      20
  #endif
  #ifndef AXIS9_SERVO_FEEDBACK
  #define AXIS9_SERVO_FEEDBACK          FB_PID
  #endif

  #ifndef AXIS9_SERVO_FLTR
  #define AXIS9_SERVO_FLTR              OFF
  #endif

  #ifndef AXIS9_PID_P
  #define AXIS9_PID_P                   2.0
  #endif
  #ifndef AXIS9_PID_I
  #define AXIS9_PID_I                   5.0
  #endif
  #ifndef AXIS9_PID_D
  #define AXIS9_PID_D                   1.0
  #endif
  #ifndef AXIS9_PID_P_GOTO
  #define AXIS9_PID_P_GOTO              AXIS9_PID_P
  #endif
  #ifndef AXIS9_PID_I_GOTO
  #define AXIS9_PID_I_GOTO              AXIS9_PID_I
  #endif
  #ifndef AXIS9_PID_D_GOTO
  #define AXIS9_PID_D_GOTO              AXIS9_PID_D
  #endif
  #ifndef AXIS9_PID_SENSITIVITY
  #define AXIS9_PID_SENSITIVITY         0
  #endif

  #ifndef AXIS9_ENCODER
  #define AXIS9_ENCODER                 AB
  #endif
  #ifndef AXIS9_ENCODER_ORIGIN
  #define AXIS9_ENCODER_ORIGIN          0
  #endif
  #ifndef AXIS9_ENCODER_REVERSE
  #define AXIS9_ENCODER_REVERSE         OFF
  #endif
#endif
#if AXIS9_DRIVER_MODEL == KTECH
  #define AXIS9_KTECH_PRESENT
#endif

#if defined(AXIS1_STEP_DIR_LEGACY) || defined(AXIS2_STEP_DIR_LEGACY) || defined(AXIS3_STEP_DIR_LEGACY) || \
    defined(AXIS4_STEP_DIR_LEGACY) || defined(AXIS5_STEP_DIR_LEGACY) || defined(AXIS6_STEP_DIR_LEGACY) || \
    defined(AXIS7_STEP_DIR_LEGACY) || defined(AXIS8_STEP_DIR_LEGACY) || defined(AXIS9_STEP_DIR_LEGACY)
  #define STEP_DIR_LEGACY_PRESENT
#endif

#if defined(AXIS1_STEP_DIR_TMC_SPI) || defined(AXIS2_STEP_DIR_TMC_SPI) || defined(AXIS3_STEP_DIR_TMC_SPI) || \
    defined(AXIS4_STEP_DIR_TMC_SPI) || defined(AXIS5_STEP_DIR_TMC_SPI) || defined(AXIS6_STEP_DIR_TMC_SPI) || \
    defined(AXIS7_STEP_DIR_TMC_SPI) || defined(AXIS8_STEP_DIR_TMC_SPI) || defined(AXIS9_STEP_DIR_TMC_SPI)
  #define STEP_DIR_TMC_SPI_PRESENT
#endif

#if defined(AXIS1_STEP_DIR_TMC_UART) || defined(AXIS2_STEP_DIR_TMC_UART) || defined(AXIS3_STEP_DIR_TMC_UART) || \
    defined(AXIS4_STEP_DIR_TMC_UART) || defined(AXIS5_STEP_DIR_TMC_UART) || defined(AXIS6_STEP_DIR_TMC_UART) || \
    defined(AXIS7_STEP_DIR_TMC_UART) || defined(AXIS8_STEP_DIR_TMC_UART) || defined(AXIS9_STEP_DIR_TMC_UART)
  #define STEP_DIR_TMC_UART_PRESENT
#endif

#if defined(STEP_DIR_LEGACY_PRESENT) || defined(STEP_DIR_TMC_SPI_PRESENT) || defined(STEP_DIR_TMC_UART_PRESENT)
  #define STEP_DIR_MOTOR_PRESENT
#endif

// flag presence of servo motors
#if AXIS1_DRIVER_MODEL == SERVO_PE || AXIS1_DRIVER_MODEL == SERVO_EE || \
    AXIS2_DRIVER_MODEL == SERVO_PE || AXIS2_DRIVER_MODEL == SERVO_EE || \
    AXIS3_DRIVER_MODEL == SERVO_PE || AXIS3_DRIVER_MODEL == SERVO_EE || \
    AXIS4_DRIVER_MODEL == SERVO_PE || AXIS4_DRIVER_MODEL == SERVO_EE || \
    AXIS5_DRIVER_MODEL == SERVO_PE || AXIS5_DRIVER_MODEL == SERVO_EE || \
    AXIS6_DRIVER_MODEL == SERVO_PE || AXIS6_DRIVER_MODEL == SERVO_EE || \
    AXIS7_DRIVER_MODEL == SERVO_PE || AXIS7_DRIVER_MODEL == SERVO_EE || \
    AXIS8_DRIVER_MODEL == SERVO_PE || AXIS8_DRIVER_MODEL == SERVO_EE || \
    AXIS9_DRIVER_MODEL == SERVO_PE || AXIS9_DRIVER_MODEL == SERVO_EE
  #define SERVO_DC_PRESENT
#endif

#if AXIS1_DRIVER_MODEL == SERVO_TMC2130_DC || AXIS1_DRIVER_MODEL == SERVO_TMC5160_DC || \
    AXIS2_DRIVER_MODEL == SERVO_TMC2130_DC || AXIS2_DRIVER_MODEL == SERVO_TMC5160_DC || \
    AXIS3_DRIVER_MODEL == SERVO_TMC2130_DC || AXIS3_DRIVER_MODEL == SERVO_TMC5160_DC || \
    AXIS4_DRIVER_MODEL == SERVO_TMC2130_DC || AXIS4_DRIVER_MODEL == SERVO_TMC5160_DC || \
    AXIS5_DRIVER_MODEL == SERVO_TMC2130_DC || AXIS5_DRIVER_MODEL == SERVO_TMC5160_DC || \
    AXIS6_DRIVER_MODEL == SERVO_TMC2130_DC || AXIS6_DRIVER_MODEL == SERVO_TMC5160_DC || \
    AXIS7_DRIVER_MODEL == SERVO_TMC2130_DC || AXIS7_DRIVER_MODEL == SERVO_TMC5160_DC || \
    AXIS8_DRIVER_MODEL == SERVO_TMC2130_DC || AXIS8_DRIVER_MODEL == SERVO_TMC5160_DC || \
    AXIS9_DRIVER_MODEL == SERVO_TMC2130_DC || AXIS9_DRIVER_MODEL == SERVO_TMC5160_DC
  #define SERVO_DC_TMC_SPI_PRESENT
#endif

#if AXIS1_DRIVER_MODEL == SERVO_TMC2209 || \
    AXIS2_DRIVER_MODEL == SERVO_TMC2209 || \
    AXIS3_DRIVER_MODEL == SERVO_TMC2209 || \
    AXIS4_DRIVER_MODEL == SERVO_TMC2209 || \
    AXIS5_DRIVER_MODEL == SERVO_TMC2209 || \
    AXIS6_DRIVER_MODEL == SERVO_TMC2209 || \
    AXIS7_DRIVER_MODEL == SERVO_TMC2209 || \
    AXIS8_DRIVER_MODEL == SERVO_TMC2209 || \
    AXIS9_DRIVER_MODEL == SERVO_TMC2209
  #define SERVO_TMC2209_PRESENT
#endif

#if AXIS1_DRIVER_MODEL == SERVO_TMC5160 || \
    AXIS2_DRIVER_MODEL == SERVO_TMC5160 || \
    AXIS3_DRIVER_MODEL == SERVO_TMC5160 || \
    AXIS4_DRIVER_MODEL == SERVO_TMC5160 || \
    AXIS5_DRIVER_MODEL == SERVO_TMC5160 || \
    AXIS6_DRIVER_MODEL == SERVO_TMC5160 || \
    AXIS7_DRIVER_MODEL == SERVO_TMC5160 || \
    AXIS8_DRIVER_MODEL == SERVO_TMC5160 || \
    AXIS9_DRIVER_MODEL == SERVO_TMC5160
  #define SERVO_TMC5160_PRESENT
#endif

#if AXIS1_DRIVER_MODEL == SERVO_KTECH || \
    AXIS2_DRIVER_MODEL == SERVO_KTECH || \
    AXIS3_DRIVER_MODEL == SERVO_KTECH || \
    AXIS4_DRIVER_MODEL == SERVO_KTECH || \
    AXIS5_DRIVER_MODEL == SERVO_KTECH || \
    AXIS6_DRIVER_MODEL == SERVO_KTECH || \
    AXIS7_DRIVER_MODEL == SERVO_KTECH || \
    AXIS8_DRIVER_MODEL == SERVO_KTECH || \
    AXIS9_DRIVER_MODEL == SERVO_KTECH
  #define SERVO_KTECH_PRESENT
#endif

#if defined(SERVO_DC_PRESENT) || defined(SERVO_DC_TMC_SPI_PRESENT) || \
    defined(SERVO_TMC2209_PRESENT) || defined(SERVO_TMC5160_PRESENT) || defined(SERVO_KTECH_PRESENT)
  #define SERVO_MOTOR_PRESENT
#endif

#if defined(AXIS1_ODRIVE_PRESENT) || defined(AXIS2_ODRIVE_PRESENT)
  #define ODRIVE_MOTOR_PRESENT

  #ifndef ODRIVE_COMM_MODE
  #define ODRIVE_COMM_MODE              OD_CAN                    // Use OD_UART or OD_CAN...I2C may be added later
  #endif
  #ifndef ODRIVE_SERIAL
  #define ODRIVE_SERIAL                 Serial3                   // Teensy HW Serial3 (if used,) for example
  #endif
  #ifndef ODRIVE_SERIAL_BAUD
  #define ODRIVE_SERIAL_BAUD            115200                    // 115200 baud default
  #endif
  #ifndef ODRIVE_UPDATE_MS
  #define ODRIVE_UPDATE_MS              100                       // 10 HZ update rate
  #endif
  #ifndef ODRIVE_SWAP_AXES
  #define ODRIVE_SWAP_AXES              ON                        // ODrive axis 0 = OnStep Axis2 = DEC or ALT
  #endif                                                          // ODrive axis 1 = OnStep Axis1 = RA or AZM
  #ifndef ODRIVE_SLEW_DIRECT
  #define ODRIVE_SLEW_DIRECT            OFF                       // ON=using ODrive trapezoidal move profile. OFF=using OnStep move profile
  #endif
  #ifndef ODRIVE_ABSOLUTE
  #define ODRIVE_ABSOLUTE               ON                        // using absolute encoders
  #endif
  #ifndef ODRIVE_SYNC_LIMIT
  #define ODRIVE_SYNC_LIMIT             80                        // in arc seconds..one encoder tick
  #endif                                                          // encoder resolution=2^14=16380; 16380/360=45.5 ticks/deg 
                                                                  // 45.5/60=0.7583 ticks/min; 0.7583/60 = .00126 ticks/sec
                                                                  // or 1/0.7583 = 1.32 arc-min/tick;  1.32*60 sec = 79.2 arc sec per encoder tick
#endif

#if defined(AXIS1_KTECH_PRESENT) || \
    defined(AXIS2_KTECH_PRESENT) || \
    defined(AXIS3_KTECH_PRESENT) || \
    defined(AXIS4_KTECH_PRESENT) || \
    defined(AXIS5_KTECH_PRESENT) || \
    defined(AXIS6_KTECH_PRESENT) || \
    defined(AXIS7_KTECH_PRESENT) || \
    defined(AXIS8_KTECH_PRESENT) || \
    defined(AXIS9_KTECH_PRESENT)
  #define KTECH_MOTOR_PRESENT
#endif

#if defined(SERVO_MOTOR_PRESENT) || defined(STEP_DIR_MOTOR_PRESENT) || defined(ODRIVE_MOTOR_PRESENT) || defined(KTECH_MOTOR_PRESENT)
  #define MOTOR_PRESENT
#endif

// -----------------------------------------------------------------------------------
// auxiliary feature settings

#ifndef FEATURE1_PURPOSE
#define FEATURE1_PURPOSE              OFF                         // OFF or SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER
#endif
#ifndef FEATURE1_NAME
#define FEATURE1_NAME                "FEATURE1"                   // user friendly name of feature, up to 10 chars
#endif
#ifndef FEATURE1_TEMP
#define FEATURE1_TEMP                 OFF                         // temperature sensor, thermistor or DS1820
#endif
#ifndef FEATURE1_PIN
#define FEATURE1_PIN                  OFF                         // OUTPUT control pin
#endif
#ifndef FEATURE1_VALUE_DEFAULT
#define FEATURE1_VALUE_DEFAULT        OFF                         // OUTPUT control pin default value/state ON, OFF, 0..255
#endif
#ifndef FEATURE1_VALUE_MEMORY
#define FEATURE1_VALUE_MEMORY         OFF                         // ON remembers feature value across power cycles
#endif
#ifndef FEATURE1_ON_STATE
#define FEATURE1_ON_STATE             HIGH                        // OUTPUT control pin ON (active) state
#endif

#ifndef FEATURE2_PURPOSE
#define FEATURE2_PURPOSE              OFF
#endif
#ifndef FEATURE2_NAME
#define FEATURE2_NAME                "FEATURE2"
#endif
#ifndef FEATURE2_TEMP
#define FEATURE2_TEMP                 OFF
#endif
#ifndef FEATURE2_PIN
#define FEATURE2_PIN                  OFF
#endif
#ifndef FEATURE2_VALUE_DEFAULT
#define FEATURE2_VALUE_DEFAULT        OFF
#endif
#ifndef FEATURE2_VALUE_MEMORY
#define FEATURE2_VALUE_MEMORY         OFF
#endif
#ifndef FEATURE2_ON_STATE
#define FEATURE2_ON_STATE             HIGH
#endif

#ifndef FEATURE3_PURPOSE
#define FEATURE3_PURPOSE              OFF
#endif
#ifndef FEATURE3_NAME
#define FEATURE3_NAME                "FEATURE3"
#endif
#ifndef FEATURE3_TEMP
#define FEATURE3_TEMP                 OFF
#endif
#ifndef FEATURE3_PIN
#define FEATURE3_PIN                  OFF
#endif
#ifndef FEATURE3_VALUE_DEFAULT
#define FEATURE3_VALUE_DEFAULT        OFF
#endif
#ifndef FEATURE3_VALUE_MEMORY
#define FEATURE3_VALUE_MEMORY         OFF
#endif
#ifndef FEATURE3_ON_STATE
#define FEATURE3_ON_STATE             HIGH
#endif

#ifndef FEATURE4_PURPOSE
#define FEATURE4_PURPOSE              OFF
#endif
#ifndef FEATURE4_NAME
#define FEATURE4_NAME                "FEATURE4"
#endif
#ifndef FEATURE4_TEMP
#define FEATURE4_TEMP                 OFF
#endif
#ifndef FEATURE4_PIN
#define FEATURE4_PIN                  OFF
#endif
#ifndef FEATURE4_VALUE_DEFAULT
#define FEATURE4_VALUE_DEFAULT        OFF
#endif
#ifndef FEATURE4_VALUE_MEMORY
#define FEATURE4_VALUE_MEMORY         OFF
#endif
#ifndef FEATURE4_ON_STATE
#define FEATURE4_ON_STATE             HIGH
#endif

#ifndef FEATURE5_PURPOSE
#define FEATURE5_PURPOSE              OFF
#endif
#ifndef FEATURE5_NAME
#define FEATURE5_NAME                "FEATURE5"
#endif
#ifndef FEATURE5_TEMP
#define FEATURE5_TEMP                 OFF
#endif
#ifndef FEATURE5_PIN
#define FEATURE5_PIN                  OFF
#endif
#ifndef FEATURE5_VALUE_DEFAULT
#define FEATURE5_VALUE_DEFAULT        OFF
#endif
#ifndef FEATURE5_VALUE_MEMORY
#define FEATURE5_VALUE_MEMORY         OFF
#endif
#ifndef FEATURE5_ON_STATE
#define FEATURE5_ON_STATE             HIGH
#endif

#ifndef FEATURE6_PURPOSE
#define FEATURE6_PURPOSE              OFF
#endif
#ifndef FEATURE6_NAME
#define FEATURE6_NAME                "FEATURE6"
#endif
#ifndef FEATURE6_TEMP
#define FEATURE6_TEMP                 OFF
#endif
#ifndef FEATURE6_PIN
#define FEATURE6_PIN                  OFF
#endif
#ifndef FEATURE6_VALUE_DEFAULT
#define FEATURE6_VALUE_DEFAULT        OFF
#endif
#ifndef FEATURE6_VALUE_MEMORY
#define FEATURE6_VALUE_MEMORY         OFF
#endif
#ifndef FEATURE6_ON_STATE
#define FEATURE6_ON_STATE             HIGH
#endif

#ifndef FEATURE7_PURPOSE
#define FEATURE7_PURPOSE              OFF
#endif
#ifndef FEATURE7_NAME
#define FEATURE7_NAME                "FEATURE7"
#endif
#ifndef FEATURE7_TEMP
#define FEATURE7_TEMP                 OFF
#endif
#ifndef FEATURE7_PIN
#define FEATURE7_PIN                  OFF
#endif
#ifndef FEATURE7_VALUE_DEFAULT
#define FEATURE7_VALUE_DEFAULT        OFF
#endif
#ifndef FEATURE7_VALUE_MEMORY
#define FEATURE7_VALUE_MEMORY         OFF
#endif
#ifndef FEATURE7_ON_STATE
#define FEATURE7_ON_STATE             HIGH
#endif

#ifndef FEATURE8_PURPOSE
#define FEATURE8_PURPOSE              OFF
#endif
#ifndef FEATURE8_NAME
#define FEATURE8_NAME                "FEATURE8"
#endif
#ifndef FEATURE8_TEMP
#define FEATURE8_TEMP                 OFF
#endif
#ifndef FEATURE8_PIN
#define FEATURE8_PIN                  OFF
#endif
#ifndef FEATURE8_VALUE_DEFAULT
#define FEATURE8_VALUE_DEFAULT        OFF
#endif
#ifndef FEATURE8_VALUE_MEMORY
#define FEATURE8_VALUE_MEMORY         OFF
#endif
#ifndef FEATURE8_ON_STATE
#define FEATURE8_ON_STATE             HIGH
#endif

// thermistor configuration settings to support two types

#ifndef THERMISTOR1_TNOM
#define THERMISTOR1_TNOM              25                          // nominal temperature (Celsius)
#endif
#ifndef THERMISTOR1_RNOM
#define THERMISTOR1_RNOM              10000                       // nominal resistance (Ohms) at nominal temperature
#endif
#ifndef THERMISTOR1_BETA
#define THERMISTOR1_BETA              3950                        // beta coefficient
#endif
#ifndef THERMISTOR1_RSERIES
#define THERMISTOR1_RSERIES           4700                        // series resistor value (Ohms)
#endif

#ifndef THERMISTOR2_TNOM
#define THERMISTOR2_TNOM              25                          // nominal temperature (Celsius)
#endif
#ifndef THERMISTOR2_RNOM
#define THERMISTOR2_RNOM              10000                       // nominal resistance (Ohms) at nominal temperature
#endif
#ifndef THERMISTOR2_BETA
#define THERMISTOR2_BETA              3950                        // beta coefficient
#endif
#ifndef THERMISTOR2_RSERIES
#define THERMISTOR2_RSERIES           4700                        // series resistor value (Ohms)
#endif
