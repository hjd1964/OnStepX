// -----------------------------------------------------------------------------------
// controller settings
#pragma once

#if AXIS1_STEP_STATE == AXIS2_STEP_STATE == AXIS3_STEP_STATE == \
    AXIS4_STEP_STATE == AXIS5_STEP_STATE == AXIS6_STEP_STATE == \
    AXIS7_STEP_STATE == AXIS8_STEP_STATE == AXIS9_STEP_STATE == HIGH
  #define DRIVER_STEP_DEFAULTS
#endif

// pinmap
#ifndef PINMAP
#define PINMAP                        OFF
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
#ifndef SERIAL_GPS_BAUD
#define SERIAL_GPS_BAUD               OFF
#endif

// ESP32 VIRTUAL SERIAL IP COMMAND CHANNELS
#ifndef SERIAL_IP_MODE
#define SERIAL_IP_MODE                OFF                         // use ACCESS_POINT or STATION to enable the interface (ESP32 only)
#endif
#ifndef SERIAL_SERVER
#define SERIAL_SERVER                BOTH                         // STANDARD (port 9999) or PERSISTENT (ports 9996 to 9998)
#endif

#if SERIAL_IP_MODE == ACCESS_POINT
#define OPERATIONAL_MODE WIFI
#define AP_ENABLED true
#endif
#if SERIAL_IP_MODE == STATION
#define OPERATIONAL_MODE WIFI
#define STA_ENABLED true
#endif
#if SERIAL_IP_MODE == BOTH
#define OPERATIONAL_MODE WIFI
#define AP_ENABLED true
#define STA_ENABLED true
#endif

#ifndef AP_SSID
#define AP_SSID                 "OnStepX"                         // Wifi Access Point SSID
#endif
#ifndef AP_PASSWORD
#define AP_PASSWORD            "password"                         // Wifi Access Point password
#endif
#ifndef AP_CHANNEL
#define AP_CHANNEL                      7                         // Wifi Access Point channel
#endif
#ifndef AP_IP_ADDR
#define AP_IP_ADDR          {192,168,0,1}                         // Wifi Access Point IP Address
#endif
#ifndef AP_GW_ADDR
#define AP_GW_ADDR          {192,168,0,1}                         // Wifi Access Point GATEWAY Address
#endif
#ifndef AP_SN_MASK
#define AP_SN_MASK        {255,255,255,0}                         // Wifi Access Point SUBNET Mask
#endif

#ifndef STA_AP_FALLBACK
#define STA_AP_FALLBACK              true                         // activate SoftAP if station fails to connect
#endif
#ifndef STA_SSID
#define STA_SSID                   "Home"                         // Station SSID to connnect to
#endif
#ifndef STA_PASSWORD
#define STA_PASSWORD           "password"                         // Wifi Station mode password
#endif
#ifndef STA_DHCP_ENABLED
#define STA_DHCP_ENABLED           false                          // true to use LAN DHCP addresses
#endif
#ifndef STA_IP_ADDR
#define STA_IP_ADDR         {192,168,0,2}                         // Wifi Station IP Address
#endif
#ifndef STA_GW_ADDR
#define STA_GW_ADDR         {192,168,0,1}                         // Wifi Station GATEWAY Address
#endif
#ifndef STA_SN_MASK
#define STA_SN_MASK       {255,255,255,0}                         // Wifi Station SUBNET Mask
#endif

// sensors
#ifndef WEATHER
#define WEATHER                       OFF
#endif

// step signal
#ifndef STEP_WAVE_FORM
#define STEP_WAVE_FORM                PULSE
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

// gpio device
#ifndef GPIO_DEVICE
#define GPIO_DEVICE                   OFF
#endif

#ifndef FileVersionConfig
#warning "Configuration (Config.h): FileVersionConfig is undefined, assuming version 5."
#define FileVersionConfig 5
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
#define AXIS1_POWER_DOWN              OFF                         // motor power off if at standstill > 30 sec
#endif
#ifndef AXIS1_ENABLE_STATE
#define AXIS1_ENABLE_STATE            LOW                         // default state of ENable pin for motor power on
#endif
#ifndef AXIS1_WRAP
#define AXIS1_WRAP                    OFF                         // wrap for unlimited range
#endif
#ifndef AXIS1_LIMIT_MIN
#define AXIS1_LIMIT_MIN               -180                        // in degrees
#endif
#ifndef AXIS1_LIMIT_MAX
#define AXIS1_LIMIT_MAX               180                         // in degrees
#endif
#ifndef AXIS1_SENSE_HOME
#define AXIS1_SENSE_HOME              OFF                         // HIGH or LOW state when clockwise of home position, seen from front
#endif
#ifndef AXIS1_SENSE_HOME_INIT
#define AXIS1_SENSE_HOME_INIT         INPUT_PULLUP                // pin mode for home sensing
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
#if AXIS1_DRIVER_MODEL >= DRIVER_FIRST && AXIS1_DRIVER_MODEL <= DRIVER_LAST
  #define AXIS1_DRIVER_PRESENT
  #if AXIS1_DRIVER_MODEL == TMC2130 || AXIS1_DRIVER_MODEL == TMC5160
  #define AXIS1_DRIVER_TMC_SPI
  #endif
  #ifndef AXIS1_STEP_STATE
  #define AXIS1_STEP_STATE              HIGH                      // default signal transition state for a step
  #endif
  #ifndef AXIS1_DRIVER_MICROSTEPS
  #define AXIS1_DRIVER_MICROSTEPS       OFF                       // normal microstep mode 
  #endif
  #ifndef AXIS1_DRIVER_MICROSTEPS_GOTO
  #define AXIS1_DRIVER_MICROSTEPS_GOTO  OFF                       // microstep mode to use during slews
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
  #define AXIS1_PARAMETER1              AXIS1_DRIVER_MICROSTEPS
  #define AXIS1_PARAMETER2              AXIS1_DRIVER_MICROSTEPS_GOTO
  #define AXIS1_PARAMETER3              AXIS1_DRIVER_IHOLD
  #define AXIS1_PARAMETER4              AXIS1_DRIVER_IRUN
  #define AXIS1_PARAMETER5              AXIS1_DRIVER_IGOTO
  #define AXIS1_PARAMETER6              OFF
#endif
#if AXIS1_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS1_SERVO_PRESENT
  #ifndef AXIS1_SERVO_P
  #define AXIS1_SERVO_P                 2.0                       // P = proportional
  #endif
  #ifndef AXIS1_SERVO_I
  #define AXIS1_SERVO_I                 5.0                       // I = integral
  #endif
  #ifndef AXIS1_SERVO_D
  #define AXIS1_SERVO_D                 1.0                       // D = derivative
  #endif
  #ifndef AXIS1_SERVO_P_GOTO
  #define AXIS1_SERVO_P_GOTO            AXIS1_SERVO_P             // P = proportional
  #endif
  #ifndef AXIS1_SERVO_I_GOTO
  #define AXIS1_SERVO_I_GOTO            AXIS1_SERVO_I             // I = integral
  #endif
  #ifndef AXIS1_SERVO_D_GOTO
  #define AXIS1_SERVO_D_GOTO            AXIS1_SERVO_D             // D = derivative
  #endif
  #ifndef AXIS1_SERVO_ENCODER
  #define AXIS1_SERVO_ENCODER           ENC_AB                    // type of encoder: ENC_AB, ENC_CW_CCW, ENC_PULSE_DIR, ENC_PULSE_ONLY
  #endif
  #ifndef AXIS1_SERVO_ENCODER_TRIGGER
  #define AXIS1_SERVO_ENCODER_TRIGGER   CHANGE                    // ignored for ENC_AB
  #endif
  #ifndef AXIS1_SERVO_FEEDBACK
  #define AXIS1_SERVO_FEEDBACK          FB_PID                    // type of feedback: FB_PID
  #endif
  #ifndef AXIS1_SERVO_PH1_STATE
  #define AXIS1_SERVO_PH1_STATE         LOW                       // default state motor driver IN1 (SERVO_II) or PHASE (SERVO_PE) pin
  #endif
  #ifndef AXIS1_SERVO_PH2_STATE
  #define AXIS1_SERVO_PH2_STATE         LOW                       // default state motor driver IN2 or ENABLE (pwm) pin
  #endif
  #define AXIS1_PARAMETER1              AXIS1_SERVO_P
  #define AXIS1_PARAMETER2              AXIS1_SERVO_I
  #define AXIS1_PARAMETER3              AXIS1_SERVO_D
  #define AXIS1_PARAMETER4              AXIS1_SERVO_P_GOTO
  #define AXIS1_PARAMETER5              AXIS1_SERVO_I_GOTO
  #define AXIS1_PARAMETER6              AXIS1_SERVO_D_GOTO
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
#ifndef AXIS2_ENABLE_STATE
#define AXIS2_ENABLE_STATE            LOW                         // stepper enable state
#endif
#ifndef AXIS2_TANGENT_ARM
#define AXIS2_TANGENT_ARM             OFF                         // ON to enable support for Dec tangent arm equatorial mounts
#endif
#ifndef AXIS2_TANGENT_ARM_CORRECTION
#define AXIS2_TANGENT_ARM_CORRECTION  OFF                         // ON enables tangent arm geometry correction for Axis2
#endif
#ifndef AXIS2_LIMIT_MIN
#define AXIS2_LIMIT_MIN               -90                         // in degrees
#endif
#ifndef AXIS2_LIMIT_MAX
#define AXIS2_LIMIT_MAX               90                          // in degrees
#endif
#ifndef AXIS2_SENSE_HOME
#define AXIS2_SENSE_HOME              OFF                         // HIGH or LOW state when clockwise of home position, seen from above
#endif
#ifndef AXIS2_SENSE_HOME_INIT
#define AXIS2_SENSE_HOME_INIT         INPUT_PULLUP                // pin mode for home sensing
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
#if AXIS2_DRIVER_MODEL >= DRIVER_FIRST && AXIS2_DRIVER_MODEL <= DRIVER_LAST
  #define AXIS2_DRIVER_PRESENT
  #if AXIS2_DRIVER_MODEL == TMC2130 || AXIS2_DRIVER_MODEL == TMC5160
  #define AXIS2_DRIVER_TMC_SPI
  #endif
  #ifndef AXIS2_STEP_STATE
  #define AXIS2_STEP_STATE              HIGH
  #endif
  #ifndef AXIS2_DRIVER_MICROSTEPS
  #define AXIS2_DRIVER_MICROSTEPS       OFF
  #endif
  #ifndef AXIS2_DRIVER_MICROSTEPS_GOTO
  #define AXIS2_DRIVER_MICROSTEPS_GOTO  OFF
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
  #define AXIS2_PARAMETER1              AXIS2_DRIVER_MICROSTEPS
  #define AXIS2_PARAMETER2              AXIS2_DRIVER_MICROSTEPS_GOTO
  #define AXIS2_PARAMETER3              AXIS2_DRIVER_IHOLD
  #define AXIS2_PARAMETER4              AXIS2_DRIVER_IRUN
  #define AXIS2_PARAMETER5              AXIS2_DRIVER_IGOTO
  #define AXIS2_PARAMETER6              OFF
#endif
#if AXIS2_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS2_SERVO_PRESENT
  #ifndef AXIS2_SERVO_P
  #define AXIS2_SERVO_P                 2.0
  #endif
  #ifndef AXIS2_SERVO_I
  #define AXIS2_SERVO_I                 5.0
  #endif
  #ifndef AXIS2_SERVO_D
  #define AXIS2_SERVO_D                 1.0
  #endif
  #ifndef AXIS2_SERVO_P_GOTO
  #define AXIS2_SERVO_P_GOTO            AXIS2_SERVO_P
  #endif
  #ifndef AXIS2_SERVO_I_GOTO
  #define AXIS2_SERVO_I_GOTO            AXIS2_SERVO_I
  #endif
  #ifndef AXIS2_SERVO_D_GOTO
  #define AXIS2_SERVO_D_GOTO            AXIS2_SERVO_D
  #endif
  #ifndef AXIS2_SERVO_ENCODER
  #define AXIS2_SERVO_ENCODER           ENC_AB
  #endif
  #ifndef AXIS2_SERVO_ENCODER_TRIGGER
  #define AXIS2_SERVO_ENCODER_TRIGGER   CHANGE
  #endif
  #ifndef AXIS2_SERVO_FEEDBACK
  #define AXIS2_SERVO_FEEDBACK          FB_PID
  #endif
  #ifndef AXIS2_SERVO_PH1_STATE
  #define AXIS2_SERVO_PH1_STATE         LOW
  #endif
  #ifndef AXIS2_SERVO_PH2_STATE
  #define AXIS2_SERVO_PH2_STATE         LOW
  #endif
  #define AXIS2_PARAMETER1              AXIS2_SERVO_P
  #define AXIS2_PARAMETER2              AXIS2_SERVO_I
  #define AXIS2_PARAMETER3              AXIS2_SERVO_D
  #define AXIS2_PARAMETER4              AXIS2_SERVO_P_GOTO
  #define AXIS2_PARAMETER5              AXIS2_SERVO_I_GOTO
  #define AXIS2_PARAMETER6              AXIS2_SERVO_D_GOTO
#endif

// mount type
#ifndef MOUNT_TYPE
#define MOUNT_TYPE                    GEM
#endif

// user feedback
#ifndef STATUS_LED
#define STATUS_LED                    ON
#endif
#ifndef STATUS_LED_ON_STATE
#define STATUS_LED_ON_STATE           LOW
#endif
#ifndef STATUS_MOUNT_LED
#define STATUS_MOUNT_LED              OFF
#endif
#ifndef STATUS_MOUNT_LED_ON_STATE
#define STATUS_MOUNT_LED_ON_STATE     LOW
#endif
#ifndef STATUS_ROTATOR_LED
#define STATUS_ROTATOR_LED            OFF
#endif
#ifndef STATUS_ROTATOR_LED_ON_STATE
#define STATUS_ROTATOR_LED_ON_STATE   LOW
#endif
#ifndef STATUS_FOCUSER_LED
#define STATUS_FOCUSER_LED            OFF
#endif
#ifndef STATUS_FOCUSER_LED_ON_STATE
#define STATUS_FOCUSER_LED_ON_STATE   LOW
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
#ifndef RETICLE_LED
#define RETICLE_LED                   OFF
#endif

// time and location
#ifndef TIME_LOCATION_SOURCE
#define TIME_LOCATION_SOURCE          OFF
#endif
#ifndef TIME_LOCATION_PPS_SENSE
#define TIME_LOCATION_PPS_SENSE       OFF
#endif

// sensors
#ifndef LIMIT_SENSE
#define LIMIT_SENSE                   OFF
#endif
#ifndef LIMIT_SENSE_INIT
#define LIMIT_SENSE_INIT              INPUT_PULLUP
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

// pec
#ifndef PEC_SENSE
#define PEC_SENSE                     OFF
#endif
#ifndef PEC_SENSE_INIT
#define PEC_SENSE_INIT                INPUT_PULLUP
#endif
#ifndef PEC_STEPS_PER_WORM_ROTATION
#define PEC_STEPS_PER_WORM_ROTATION   0
#endif

// guiding
#ifndef GUIDE_TIME_LIMIT
#define GUIDE_TIME_LIMIT              0                           // in seconds, 0 to disable
#endif
#ifndef GUIDE_HOME_TIME_LIMIT
#define GUIDE_HOME_TIME_LIMIT         5*60                        // in seconds, for home switches
#endif
#ifndef GUIDE_DISABLE_BACKLASH
#define GUIDE_DISABLE_BACKLASH        OFF                         // ON disables backlash while pulse-guiding
#endif

// tracking
#ifndef TRACK_AUTOSTART
#define TRACK_AUTOSTART               OFF
#endif
#ifndef TRACK_REFRACTION_RATE_DEFAULT
#define TRACK_REFRACTION_RATE_DEFAULT OFF
#endif
#ifndef TRACK_BACKLASH_RATE
#define TRACK_BACKLASH_RATE           25
#endif

// slewing
#ifndef SLEW_GOTO
#define SLEW_GOTO                     ON
#endif
#ifndef SLEW_RATE_BASE_DESIRED
#define SLEW_RATE_BASE_DESIRED        1.0
#endif
#ifndef SLEW_RATE_MEMORY
#define SLEW_RATE_MEMORY              OFF
#endif
#ifndef SLEW_ACCELERATION_DIST
#define SLEW_ACCELERATION_DIST        5.0
#endif
#ifndef SLEW_RAPID_STOP_DIST
#define SLEW_RAPID_STOP_DIST          2.0
#endif

// pier side
#ifndef MFLIP_SKIP_HOME
#define MFLIP_SKIP_HOME               OFF
#endif
#ifndef MFLIP_PAUSE_HOME_MEMORY
#define MFLIP_PAUSE_HOME_MEMORY       OFF
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

// parking
#ifndef STRICT_PARKING
#define STRICT_PARKING                OFF
#endif

// mount coordinates
#ifndef MOUNT_COORDS
#define MOUNT_COORDS                  TOPOCENTRIC
#endif

// mount guiding
#ifndef SEPARATE_PULSE_GUIDE_RATE
#define SEPARATE_PULSE_GUIDE_RATE     ON
#endif

// mount align
#ifndef ALIGN_MAX_STARS
#define ALIGN_MAX_STARS               AUTO
#endif

// mount pec
#ifndef PEC_BUFFER_SIZE_LIMIT
#define PEC_BUFFER_SIZE_LIMIT         720
#endif

// ESP32 virtual serial bluetooth command channel
#ifndef SERIAL_BT_MODE
#define SERIAL_BT_MODE                OFF
#endif
#ifndef SERIAL_BT_NAME
#define SERIAL_BT_NAME                "OnStep"
#endif

// use the HAL specified default NV driver
#ifndef NV_DRIVER
  #define NV_DRIVER                   NV_DEFAULT
#endif

// -----------------------------------------------------------------------------------
// rotator settings, ROTATOR
#ifndef AXIS3_DRIVER_MODEL
#define AXIS3_DRIVER_MODEL            OFF                         // specify a driver to enable
#endif
#ifndef AXIS3_STEPS_PER_DEGREE
#define AXIS3_STEPS_PER_DEGREE        64.0
#endif
#ifndef AXIS3_REVERSE
#define AXIS3_REVERSE                 OFF
#endif
#ifndef AXIS3_POWER_DOWN
#define AXIS3_POWER_DOWN              OFF
#endif
#ifndef AXIS3_ENABLE_STATE
#define AXIS3_ENABLE_STATE            LOW
#endif
#ifndef AXIS3_SLEW_RATE_DESIRED
#define AXIS3_SLEW_RATE_DESIRED       3.0                         // in degrees/sec
#endif
#ifndef AXIS3_ACCELERATION_TIME
#define AXIS3_ACCELERATION_TIME       2                           // in seconds, to selected rate
#endif
#ifndef AXIS3_RAPID_STOP_TIME
#define AXIS3_RAPID_STOP_TIME         1                           // in seconds, to stop
#endif
#ifndef AXIS3_BACKLASH_RATE
#define AXIS3_BACKLASH_RATE           (AXIS3_SLEW_RATE_DESIRED/4) // in degrees/sec
#endif
#ifndef AXIS3_LIMIT_MIN
#define AXIS3_LIMIT_MIN               -180                        // in degrees
#endif
#ifndef AXIS3_LIMIT_MAX
#define AXIS3_LIMIT_MAX               180                         // in degrees
#endif
#ifndef AXIS3_SENSE_HOME
#define AXIS3_SENSE_HOME              OFF
#endif
#ifndef AXIS3_SENSE_HOME_INIT
#define AXIS3_SENSE_HOME_INIT         INPUT_PULLUP
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
#if AXIS3_DRIVER_MODEL >= DRIVER_FIRST && AXIS3_DRIVER_MODEL <= DRIVER_LAST
  #define AXIS3_DRIVER_PRESENT
  #if AXIS3_DRIVER_MODEL == TMC2130 || AXIS3_DRIVER_MODEL == TMC5160
  #define AXIS3_DRIVER_TMC_SPI
  #endif
  #ifndef AXIS3_STEP_STATE
  #define AXIS3_STEP_STATE              HIGH
  #endif
  #ifndef AXIS3_DRIVER_MICROSTEPS
  #define AXIS3_DRIVER_MICROSTEPS       OFF
  #endif
  #ifndef AXIS3_DRIVER_MICROSTEPS_GOTO
  #define AXIS3_DRIVER_MICROSTEPS_GOTO  OFF
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
  #define AXIS3_PARAMETER1              AXIS3_DRIVER_MICROSTEPS
  #define AXIS3_PARAMETER2              AXIS3_DRIVER_MICROSTEPS_GOTO
  #define AXIS3_PARAMETER3              AXIS3_DRIVER_IHOLD
  #define AXIS3_PARAMETER4              AXIS3_DRIVER_IRUN
  #define AXIS3_PARAMETER5              AXIS3_DRIVER_IGOTO
  #define AXIS3_PARAMETER6              OFF
#endif
#if AXIS3_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS3_SERVO_PRESENT
  #ifndef AXIS3_SERVO_P
  #define AXIS3_SERVO_P                 2.0
  #endif
  #ifndef AXIS3_SERVO_I
  #define AXIS3_SERVO_I                 5.0
  #endif
  #ifndef AXIS3_SERVO_D
  #define AXIS3_SERVO_D                 1.0
  #endif
  #ifndef AXIS3_SERVO_P_GOTO
  #define AXIS3_SERVO_P_GOTO            AXIS3_SERVO_P
  #endif
  #ifndef AXIS3_SERVO_I_GOTO
  #define AXIS3_SERVO_I_GOTO            AXIS3_SERVO_I
  #endif
  #ifndef AXIS3_SERVO_D_GOTO
  #define AXIS3_SERVO_D_GOTO            AXIS3_SERVO_D
  #endif
  #ifndef AXIS3_SERVO_ENCODER
  #define AXIS3_SERVO_ENCODER           ENC_AB
  #endif
  #ifndef AXIS3_SERVO_ENCODER_TRIGGER
  #define AXIS3_SERVO_ENCODER_TRIGGER   CHANGE
  #endif
  #ifndef AXIS3_SERVO_FEEDBACK
  #define AXIS3_SERVO_FEEDBACK          FB_PID
  #endif
  #ifndef AXIS3_SERVO_PH1_STATE
  #define AXIS3_SERVO_PH1_STATE         LOW
  #endif
  #ifndef AXIS3_SERVO_PH2_STATE
  #define AXIS3_SERVO_PH2_STATE         LOW
  #endif
  #define AXIS3_PARAMETER1              AXIS3_SERVO_P
  #define AXIS3_PARAMETER2              AXIS3_SERVO_I
  #define AXIS3_PARAMETER3              AXIS3_SERVO_D
  #define AXIS3_PARAMETER4              AXIS3_SERVO_P_GOTO
  #define AXIS3_PARAMETER5              AXIS3_SERVO_I_GOTO
  #define AXIS3_PARAMETER6              AXIS3_SERVO_D_GOTO
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
#define AXIS4_POWER_DOWN              OFF                         // automatic power down at standstill
#endif
#ifndef AXIS4_ENABLE_STATE
#define AXIS4_ENABLE_STATE            LOW                         // enable pin state when driver is active
#endif
#ifndef AXIS4_SLEW_RATE_MINIMUM
#define AXIS4_SLEW_RATE_MINIMUM       2                           // in microns/sec
#endif
#ifndef AXIS4_SLEW_RATE_DESIRED
#define AXIS4_SLEW_RATE_DESIRED       500                         // in microns/sec
#endif
#ifndef AXIS4_ACCELERATION_TIME
#define AXIS4_ACCELERATION_TIME       2                           // in seconds, to selected rate
#endif
#ifndef AXIS4_RAPID_STOP_TIME
#define AXIS4_RAPID_STOP_TIME         1                           // in seconds, to stop
#endif
#ifndef AXIS4_BACKLASH_RATE
#define AXIS4_BACKLASH_RATE           (AXIS4_SLEW_RATE_DESIRED/4) // in microns/sec
#endif
#ifndef AXIS4_LIMIT_MIN
#define AXIS4_LIMIT_MIN               0                           // in mm
#endif
#ifndef AXIS4_LIMIT_MAX
#define AXIS4_LIMIT_MAX               50                          // in mm
#endif
#ifndef AXIS4_SENSE_HOME
#define AXIS4_SENSE_HOME              OFF
#endif
#ifndef AXIS4_SENSE_HOME_INIT
#define AXIS4_SENSE_HOME_INIT         INPUT_PULLUP
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
#if AXIS4_DRIVER_MODEL >= DRIVER_FIRST && AXIS4_DRIVER_MODEL <= DRIVER_LAST
  #define AXIS4_DRIVER_PRESENT
  #if AXIS4_DRIVER_MODEL == TMC2130 || AXIS4_DRIVER_MODEL == TMC5160
  #define AXIS4_DRIVER_TMC_SPI
  #endif
  #ifndef AXIS4_STEP_STATE
  #define AXIS4_STEP_STATE              HIGH
  #endif
  #ifndef AXIS4_DRIVER_MICROSTEPS
  #define AXIS4_DRIVER_MICROSTEPS       OFF
  #endif
  #ifndef AXIS4_DRIVER_MICROSTEPS_GOTO
  #define AXIS4_DRIVER_MICROSTEPS_GOTO  OFF
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
  #define AXIS4_PARAMETER1              AXIS4_DRIVER_MICROSTEPS
  #define AXIS4_PARAMETER2              AXIS4_DRIVER_MICROSTEPS_GOTO
  #define AXIS4_PARAMETER3              AXIS4_DRIVER_IHOLD
  #define AXIS4_PARAMETER4              AXIS4_DRIVER_IRUN
  #define AXIS4_PARAMETER5              AXIS4_DRIVER_IGOTO
  #define AXIS4_PARAMETER6              OFF
#endif
#if AXIS4_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS4_SERVO_PRESENT
  #ifndef AXIS4_SERVO_P
  #define AXIS4_SERVO_P                 2.0
  #endif
  #ifndef AXIS4_SERVO_I
  #define AXIS4_SERVO_I                 5.0
  #endif
  #ifndef AXIS4_SERVO_D
  #define AXIS4_SERVO_D                 1.0
  #endif
  #ifndef AXIS4_SERVO_P_GOTO
  #define AXIS4_SERVO_P_GOTO            AXIS4_SERVO_P
  #endif
  #ifndef AXIS4_SERVO_I_GOTO
  #define AXIS4_SERVO_I_GOTO            AXIS4_SERVO_I
  #endif
  #ifndef AXIS4_SERVO_D_GOTO
  #define AXIS4_SERVO_D_GOTO            AXIS4_SERVO_D
  #endif
  #ifndef AXIS4_SERVO_ENCODER
  #define AXIS4_SERVO_ENCODER           ENC_AB
  #endif
  #ifndef AXIS4_SERVO_ENCODER_TRIGGER
  #define AXIS4_SERVO_ENCODER_TRIGGER   CHANGE
  #endif
  #ifndef AXIS4_SERVO_FEEDBACK
  #define AXIS4_SERVO_FEEDBACK          FB_PID
  #endif
  #ifndef AXIS4_SERVO_PH1_STATE
  #define AXIS4_SERVO_PH1_STATE         LOW
  #endif
  #ifndef AXIS4_SERVO_PH2_STATE
  #define AXIS4_SERVO_PH2_STATE         LOW
  #endif
  #define AXIS4_PARAMETER1              AXIS4_SERVO_P
  #define AXIS4_PARAMETER2              AXIS4_SERVO_I
  #define AXIS4_PARAMETER2              AXIS4_SERVO_D
  #define AXIS4_PARAMETER4              AXIS4_SERVO_P_GOTO
  #define AXIS4_PARAMETER5              AXIS4_SERVO_I_GOTO
  #define AXIS4_PARAMETER6              AXIS4_SERVO_D_GOTO
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
#ifndef AXIS5_ENABLE_STATE
#define AXIS5_ENABLE_STATE            LOW
#endif
#ifndef AXIS5_SLEW_RATE_MINIMUM
#define AXIS5_SLEW_RATE_MINIMUM       2
#endif
#ifndef AXIS5_SLEW_RATE_DESIRED
#define AXIS5_SLEW_RATE_DESIRED       500
#endif
#ifndef AXIS5_ACCELERATION_TIME
#define AXIS5_ACCELERATION_TIME       2
#endif
#ifndef AXIS5_RAPID_STOP_TIME
#define AXIS5_RAPID_STOP_TIME         1
#endif
#ifndef AXIS5_BACKLASH_RATE
#define AXIS5_BACKLASH_RATE           (AXIS5_SLEW_RATE_DESIRED/4)
#endif
#ifndef AXIS5_LIMIT_MIN
#define AXIS5_LIMIT_MIN               0
#endif
#ifndef AXIS5_LIMIT_MAX
#define AXIS5_LIMIT_MAX               50
#endif
#ifndef AXIS5_SENSE_HOME
#define AXIS5_SENSE_HOME              OFF
#endif
#ifndef AXIS5_SENSE_HOME_INIT
#define AXIS5_SENSE_HOME_INIT         INPUT_PULLUP
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
#if AXIS5_DRIVER_MODEL >= DRIVER_FIRST && AXIS5_DRIVER_MODEL <= DRIVER_LAST
  #define AXIS5_DRIVER_PRESENT
  #if AXIS5_DRIVER_MODEL == TMC2130 || AXIS5_DRIVER_MODEL == TMC5160
  #define AXIS5_DRIVER_TMC_SPI
  #endif
  #ifndef AXIS5_STEP_STATE
  #define AXIS5_STEP_STATE              HIGH
  #endif
  #ifndef AXIS5_DRIVER_MICROSTEPS
  #define AXIS5_DRIVER_MICROSTEPS       OFF
  #endif
  #ifndef AXIS5_DRIVER_MICROSTEPS_GOTO
  #define AXIS5_DRIVER_MICROSTEPS_GOTO  OFF
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
  #define AXIS5_PARAMETER1              AXIS5_DRIVER_MICROSTEPS
  #define AXIS5_PARAMETER2              AXIS5_DRIVER_MICROSTEPS_GOTO
  #define AXIS5_PARAMETER3              AXIS5_DRIVER_IHOLD
  #define AXIS5_PARAMETER4              AXIS5_DRIVER_IRUN
  #define AXIS5_PARAMETER5              AXIS5_DRIVER_IGOTO
  #define AXIS5_PARAMETER6              OFF
#endif
#if AXIS5_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS5_SERVO_PRESENT
  #ifndef AXIS5_SERVO_P
  #define AXIS5_SERVO_P                 2.0
  #endif
  #ifndef AXIS5_SERVO_I
  #define AXIS5_SERVO_I                 5.0
  #endif
  #ifndef AXIS5_SERVO_D
  #define AXIS5_SERVO_D                 1.0
  #endif
  #ifndef AXIS5_SERVO_P_GOTO
  #define AXIS5_SERVO_P_GOTO            AXIS5_SERVO_P
  #endif
  #ifndef AXIS5_SERVO_I_GOTO
  #define AXIS5_SERVO_I_GOTO            AXIS5_SERVO_I
  #endif
  #ifndef AXIS5_SERVO_D_GOTO
  #define AXIS5_SERVO_D_GOTO            AXIS5_SERVO_D
  #endif
  #ifndef AXIS5_SERVO_ENCODER
  #define AXIS5_SERVO_ENCODER           ENC_AB
  #endif
  #ifndef AXIS5_SERVO_ENCODER_TRIGGER
  #define AXIS5_SERVO_ENCODER_TRIGGER   CHANGE
  #endif
  #ifndef AXIS5_SERVO_FEEDBACK
  #define AXIS5_SERVO_FEEDBACK          FB_PID
  #endif
  #ifndef AXIS5_SERVO_PH1_STATE
  #define AXIS5_SERVO_PH1_STATE         LOW
  #endif
  #ifndef AXIS5_SERVO_PH2_STATE
  #define AXIS5_SERVO_PH2_STATE         LOW
  #endif
  #define AXIS5_PARAMETER1              AXIS5_SERVO_P
  #define AXIS5_PARAMETER2              AXIS5_SERVO_I
  #define AXIS5_PARAMETER3              AXIS5_SERVO_D
  #define AXIS5_PARAMETER4              AXIS5_SERVO_P_GOTO
  #define AXIS5_PARAMETER5              AXIS5_SERVO_I_GOTO
  #define AXIS5_PARAMETER6              AXIS5_SERVO_D_GOTO
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
#ifndef AXIS6_ENABLE_STATE
#define AXIS6_ENABLE_STATE            LOW
#endif
#ifndef AXIS6_SLEW_RATE_MINIMUM
#define AXIS6_SLEW_RATE_MINIMUM       2
#endif
#ifndef AXIS6_SLEW_RATE_DESIRED
#define AXIS6_SLEW_RATE_DESIRED       500
#endif
#ifndef AXIS6_ACCELERATION_TIME
#define AXIS6_ACCELERATION_TIME       2
#endif
#ifndef AXIS6_RAPID_STOP_TIME
#define AXIS6_RAPID_STOP_TIME         1
#endif
#ifndef AXIS6_BACKLASH_RATE
#define AXIS6_BACKLASH_RATE           (AXIS6_SLEW_RATE_DESIRED/4)
#endif
#ifndef AXIS6_LIMIT_MIN
#define AXIS6_LIMIT_MIN               0
#endif
#ifndef AXIS6_LIMIT_MAX
#define AXIS6_LIMIT_MAX               50
#endif
#ifndef AXIS6_SENSE_HOME
#define AXIS6_SENSE_HOME              OFF
#endif
#ifndef AXIS6_SENSE_HOME_INIT
#define AXIS6_SENSE_HOME_INIT         INPUT_PULLUP
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
#if AXIS6_DRIVER_MODEL >= DRIVER_FIRST && AXIS6_DRIVER_MODEL <= DRIVER_LAST
  #define AXIS6_DRIVER_PRESENT
  #if AXIS6_DRIVER_MODEL == TMC2130 || AXIS6_DRIVER_MODEL == TMC5160
  #define AXIS6_DRIVER_TMC_SPI
  #endif
  #ifndef AXIS6_STEP_STATE
  #define AXIS6_STEP_STATE              HIGH
  #endif
  #ifndef AXIS6_DRIVER_MICROSTEPS
  #define AXIS6_DRIVER_MICROSTEPS       OFF
  #endif
  #ifndef AXIS6_DRIVER_MICROSTEPS_GOTO
  #define AXIS6_DRIVER_MICROSTEPS_GOTO  OFF
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
  #define AXIS6_PARAMETER1              AXIS6_DRIVER_MICROSTEPS
  #define AXIS6_PARAMETER2              AXIS6_DRIVER_MICROSTEPS_GOTO
  #define AXIS6_PARAMETER3              AXIS6_DRIVER_IHOLD
  #define AXIS6_PARAMETER4              AXIS6_DRIVER_IRUN
  #define AXIS6_PARAMETER5              AXIS6_DRIVER_IGOTO
  #define AXIS6_PARAMETER6              OFF
#endif
#if AXIS6_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS6_SERVO_PRESENT
  #ifndef AXIS6_SERVO_P
  #define AXIS6_SERVO_P                 2.0
  #endif
  #ifndef AXIS6_SERVO_I
  #define AXIS6_SERVO_I                 5.0
  #endif
  #ifndef AXIS6_SERVO_D
  #define AXIS6_SERVO_D                 1.0
  #endif
  #ifndef AXIS6_SERVO_P_GOTO
  #define AXIS6_SERVO_P_GOTO            AXIS6_SERVO_P
  #endif
  #ifndef AXIS6_SERVO_I_GOTO
  #define AXIS6_SERVO_I_GOTO            AXIS6_SERVO_I
  #endif
  #ifndef AXIS6_SERVO_D_GOTO
  #define AXIS6_SERVO_D_GOTO            AXIS6_SERVO_D
  #endif
  #ifndef AXIS6_SERVO_ENCODER
  #define AXIS6_SERVO_ENCODER           ENC_AB
  #endif
  #ifndef AXIS6_SERVO_ENCODER_TRIGGER
  #define AXIS6_SERVO_ENCODER_TRIGGER   CHANGE
  #endif
  #ifndef AXIS6_SERVO_FEEDBACK
  #define AXIS6_SERVO_FEEDBACK          FB_PID
  #endif
  #ifndef AXIS6_SERVO_PH1_STATE
  #define AXIS6_SERVO_PH1_STATE         LOW
  #endif
  #ifndef AXIS6_SERVO_PH2_STATE
  #define AXIS6_SERVO_PH2_STATE         LOW
  #endif
  #define AXIS6_PARAMETER1              AXIS6_SERVO_P
  #define AXIS6_PARAMETER2              AXIS6_SERVO_I
  #define AXIS6_PARAMETER3              AXIS6_SERVO_D
  #define AXIS6_PARAMETER4              AXIS6_SERVO_P_GOTO
  #define AXIS6_PARAMETER5              AXIS6_SERVO_I_GOTO
  #define AXIS6_PARAMETER6              AXIS6_SERVO_D_GOTO
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
#ifndef AXIS7_ENABLE_STATE
#define AXIS7_ENABLE_STATE            LOW
#endif
#ifndef AXIS7_SLEW_RATE_MINIMUM
#define AXIS7_SLEW_RATE_MINIMUM       2
#endif
#ifndef AXIS7_SLEW_RATE_DESIRED
#define AXIS7_SLEW_RATE_DESIRED       500
#endif
#ifndef AXIS7_ACCELERATION_TIME
#define AXIS7_ACCELERATION_TIME       2
#endif
#ifndef AXIS7_RAPID_STOP_TIME
#define AXIS7_RAPID_STOP_TIME         1
#endif
#ifndef AXIS7_BACKLASH_RATE
#define AXIS7_BACKLASH_RATE           (AXIS7_SLEW_RATE_DESIRED/4)
#endif
#ifndef AXIS7_LIMIT_MIN
#define AXIS7_LIMIT_MIN               0
#endif
#ifndef AXIS7_LIMIT_MAX
#define AXIS7_LIMIT_MAX               50
#endif
#ifndef AXIS7_SENSE_HOME
#define AXIS7_SENSE_HOME              OFF
#endif
#ifndef AXIS7_SENSE_HOME_INIT
#define AXIS7_SENSE_HOME_INIT         INPUT_PULLUP
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
#if AXIS7_DRIVER_MODEL >= DRIVER_FIRST && AXIS7_DRIVER_MODEL <= DRIVER_LAST
  #define AXIS7_DRIVER_PRESENT
  #if AXIS7_DRIVER_MODEL == TMC2130 || AXIS7_DRIVER_MODEL == TMC5160
  #define AXIS7_DRIVER_TMC_SPI
  #endif
  #ifndef AXIS7_STEP_STATE
  #define AXIS7_STEP_STATE              HIGH
  #endif
  #ifndef AXIS7_DRIVER_MICROSTEPS
  #define AXIS7_DRIVER_MICROSTEPS       OFF
  #endif
  #ifndef AXIS7_DRIVER_MICROSTEPS_GOTO
  #define AXIS7_DRIVER_MICROSTEPS_GOTO  OFF
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
  #define AXIS7_PARAMETER1              AXIS7_DRIVER_MICROSTEPS
  #define AXIS7_PARAMETER2              AXIS7_DRIVER_MICROSTEPS_GOTO
  #define AXIS7_PARAMETER3              AXIS7_DRIVER_IHOLD
  #define AXIS7_PARAMETER4              AXIS7_DRIVER_IRUN
  #define AXIS7_PARAMETER5              AXIS7_DRIVER_IGOTO
  #define AXIS7_PARAMETER6              OFF
#endif
#if AXIS7_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS7_SERVO_PRESENT
  #ifndef AXIS7_SERVO_P
  #define AXIS7_SERVO_P                 2.0
  #endif
  #ifndef AXIS7_SERVO_I
  #define AXIS7_SERVO_I                 5.0
  #endif
  #ifndef AXIS7_SERVO_D
  #define AXIS7_SERVO_D                 1.0
  #endif
  #ifndef AXIS7_SERVO_P_GOTO
  #define AXIS7_SERVO_P_GOTO            AXIS7_SERVO_P
  #endif
  #ifndef AXIS7_SERVO_I_GOTO
  #define AXIS7_SERVO_I_GOTO            AXIS7_SERVO_I
  #endif
  #ifndef AXIS7_SERVO_D_GOTO
  #define AXIS7_SERVO_D_GOTO            AXIS7_SERVO_D
  #endif
  #ifndef AXIS7_SERVO_ENCODER
  #define AXIS7_SERVO_ENCODER           ENC_AB
  #endif
  #ifndef AXIS7_SERVO_ENCODER_TRIGGER
  #define AXIS7_SERVO_ENCODER_TRIGGER   CHANGE
  #endif
  #ifndef AXIS7_SERVO_FEEDBACK
  #define AXIS7_SERVO_FEEDBACK          FB_PID
  #endif
  #ifndef AXIS7_SERVO_PH1_STATE
  #define AXIS7_SERVO_PH1_STATE         LOW
  #endif
  #ifndef AXIS7_SERVO_PH2_STATE
  #define AXIS7_SERVO_PH2_STATE         LOW
  #endif
  #define AXIS7_PARAMETER1              AXIS7_SERVO_P
  #define AXIS7_PARAMETER2              AXIS7_SERVO_I
  #define AXIS7_PARAMETER3              AXIS7_SERVO_D
  #define AXIS7_PARAMETER4              AXIS7_SERVO_P_GOTO
  #define AXIS7_PARAMETER5              AXIS7_SERVO_I_GOTO
  #define AXIS7_PARAMETER6              AXIS7_SERVO_D_GOTO
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
#ifndef AXIS8_ENABLE_STATE
#define AXIS8_ENABLE_STATE            LOW
#endif
#ifndef AXIS8_SLEW_RATE_MINIMUM
#define AXIS8_SLEW_RATE_MINIMUM       2
#endif
#ifndef AXIS8_SLEW_RATE_DESIRED
#define AXIS8_SLEW_RATE_DESIRED       500
#endif
#ifndef AXIS8_ACCELERATION_TIME
#define AXIS8_ACCELERATION_TIME       2
#endif
#ifndef AXIS8_RAPID_STOP_TIME
#define AXIS8_RAPID_STOP_TIME         1
#endif
#ifndef AXIS8_BACKLASH_RATE
#define AXIS8_BACKLASH_RATE           (AXIS8_SLEW_RATE_DESIRED/4)
#endif
#ifndef AXIS8_LIMIT_MIN
#define AXIS8_LIMIT_MIN               0
#endif
#ifndef AXIS8_LIMIT_MAX
#define AXIS8_LIMIT_MAX               50
#endif
#ifndef AXIS8_SENSE_HOME
#define AXIS8_SENSE_HOME              OFF
#endif
#ifndef AXIS8_SENSE_HOME_INIT
#define AXIS8_SENSE_HOME_INIT         INPUT_PULLUP
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
#if AXIS8_DRIVER_MODEL >= DRIVER_FIRST && AXIS8_DRIVER_MODEL <= DRIVER_LAST
  #define AXIS8_DRIVER_PRESENT
  #if AXIS8_DRIVER_MODEL == TMC2130 || AXIS8_DRIVER_MODEL == TMC5160
  #define AXIS8_DRIVER_TMC_SPI
  #endif
  #ifndef AXIS8_STEP_STATE
  #define AXIS8_STEP_STATE              HIGH
  #endif
  #ifndef AXIS8_DRIVER_MICROSTEPS
  #define AXIS8_DRIVER_MICROSTEPS       OFF
  #endif
  #ifndef AXIS8_DRIVER_MICROSTEPS_GOTO
  #define AXIS8_DRIVER_MICROSTEPS_GOTO  OFF
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
  #define AXIS8_PARAMETER1              AXIS8_DRIVER_MICROSTEPS
  #define AXIS8_PARAMETER2              AXIS8_DRIVER_MICROSTEPS_GOTO
  #define AXIS8_PARAMETER3              AXIS8_DRIVER_IHOLD
  #define AXIS8_PARAMETER4              AXIS8_DRIVER_IRUN
  #define AXIS8_PARAMETER5              AXIS8_DRIVER_IGOTO
  #define AXIS8_PARAMETER6              OFF
#endif
#if AXIS8_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS8_SERVO_PRESENT
  #ifndef AXIS8_SERVO_P
  #define AXIS8_SERVO_P                 2.0
  #endif
  #ifndef AXIS8_SERVO_I
  #define AXIS8_SERVO_I                 5.0
  #endif
  #ifndef AXIS8_SERVO_D
  #define AXIS8_SERVO_D                 1.0
  #endif
  #ifndef AXIS8_SERVO_P_GOTO
  #define AXIS8_SERVO_P_GOTO            AXIS8_SERVO_P
  #endif
  #ifndef AXIS8_SERVO_I_GOTO
  #define AXIS8_SERVO_I_GOTO            AXIS8_SERVO_I
  #endif
  #ifndef AXIS8_SERVO_D_GOTO
  #define AXIS8_SERVO_D_GOTO            AXIS8_SERVO_D
  #endif
  #ifndef AXIS8_SERVO_ENCODER
  #define AXIS8_SERVO_ENCODER           ENC_AB
  #endif
  #ifndef AXIS8_SERVO_ENCODER_TRIGGER
  #define AXIS8_SERVO_ENCODER_TRIGGER   CHANGE
  #endif
  #ifndef AXIS8_SERVO_FEEDBACK
  #define AXIS8_SERVO_FEEDBACK          FB_PID
  #endif
  #ifndef AXIS8_SERVO_PH1_STATE
  #define AXIS8_SERVO_PH1_STATE         LOW
  #endif
  #ifndef AXIS8_SERVO_PH2_STATE
  #define AXIS8_SERVO_PH2_STATE         LOW
  #endif
  #define AXIS8_PARAMETER1              AXIS8_SERVO_P
  #define AXIS8_PARAMETER2              AXIS8_SERVO_I
  #define AXIS8_PARAMETER3              AXIS8_SERVO_D
  #define AXIS8_PARAMETER4              AXIS8_SERVO_P_GOTO
  #define AXIS8_PARAMETER5              AXIS8_SERVO_I_GOTO
  #define AXIS8_PARAMETER6              AXIS8_SERVO_D_GOTO
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
#ifndef AXIS9_ENABLE_STATE
#define AXIS9_ENABLE_STATE            LOW
#endif
#ifndef AXIS9_SLEW_RATE_MINIMUM
#define AXIS9_SLEW_RATE_MINIMUM       2
#endif
#ifndef AXIS9_SLEW_RATE_DESIRED
#define AXIS9_SLEW_RATE_DESIRED       500
#endif
#ifndef AXIS9_ACCELERATION_TIME
#define AXIS9_ACCELERATION_TIME       2
#endif
#ifndef AXIS9_RAPID_STOP_TIME
#define AXIS9_RAPID_STOP_TIME         1
#endif
#ifndef AXIS9_BACKLASH_RATE
#define AXIS9_BACKLASH_RATE           (AXIS9_SLEW_RATE_DESIRED/4)
#endif
#ifndef AXIS9_LIMIT_MIN
#define AXIS9_LIMIT_MIN               0
#endif
#ifndef AXIS9_LIMIT_MAX
#define AXIS9_LIMIT_MAX               50
#endif
#ifndef AXIS9_SENSE_HOME
#define AXIS9_SENSE_HOME              OFF
#endif
#ifndef AXIS9_SENSE_HOME_INIT
#define AXIS9_SENSE_HOME_INIT         INPUT_PULLUP
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
#if AXIS9_DRIVER_MODEL >= DRIVER_FIRST && AXIS9_DRIVER_MODEL <= DRIVER_LAST
  #define AXIS9_DRIVER_PRESENT
  #if AXIS9_DRIVER_MODEL == TMC2130 || AXIS9_DRIVER_MODEL == TMC5160
  #define AXIS9_DRIVER_TMC_SPI
  #endif
  #ifndef AXIS9_STEP_STATE
  #define AXIS9_STEP_STATE              HIGH
  #endif
  #ifndef AXIS9_DRIVER_MICROSTEPS
  #define AXIS9_DRIVER_MICROSTEPS       OFF
  #endif
  #ifndef AXIS9_DRIVER_MICROSTEPS_GOTO
  #define AXIS9_DRIVER_MICROSTEPS_GOTO  OFF
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
  #define AXIS9_PARAMETER1              AXIS9_DRIVER_MICROSTEPS
  #define AXIS9_PARAMETER2              AXIS9_DRIVER_MICROSTEPS_GOTO
  #define AXIS9_PARAMETER3              AXIS9_DRIVER_IHOLD
  #define AXIS9_PARAMETER4              AXIS9_DRIVER_IRUN
  #define AXIS9_PARAMETER5              AXIS9_DRIVER_IGOTO
  #define AXIS9_PARAMETER6              OFF
#endif
#if AXIS9_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS9_SERVO_PRESENT
  #ifndef AXIS9_SERVO_P
  #define AXIS9_SERVO_P                 2.0
  #endif
  #ifndef AXIS9_SERVO_I
  #define AXIS9_SERVO_I                 5.0
  #endif
  #ifndef AXIS9_SERVO_D
  #define AXIS9_SERVO_D                 1.0
  #endif
  #ifndef AXIS9_SERVO_P_GOTO
  #define AXIS9_SERVO_P_GOTO            AXIS9_SERVO_P
  #endif
  #ifndef AXIS9_SERVO_I_GOTO
  #define AXIS9_SERVO_I_GOTO            AXIS9_SERVO_I
  #endif
  #ifndef AXIS9_SERVO_D_GOTO
  #define AXIS9_SERVO_D_GOTO            AXIS9_SERVO_D
  #endif
  #ifndef AXIS9_SERVO_ENCODER
  #define AXIS9_SERVO_ENCODER           ENC_AB
  #endif
  #ifndef AXIS9_SERVO_ENCODER_TRIGGER
  #define AXIS9_SERVO_ENCODER_TRIGGER   CHANGE
  #endif
  #ifndef AXIS9_SERVO_FEEDBACK
  #define AXIS9_SERVO_FEEDBACK          FB_PID
  #endif
  #ifndef AXIS9_SERVO_PH1_STATE
  #define AXIS9_SERVO_PH1_STATE         LOW
  #endif
  #ifndef AXIS9_SERVO_PH2_STATE
  #define AXIS9_SERVO_PH2_STATE         LOW
  #endif
  #define AXIS9_PARAMETER1              AXIS9_SERVO_P
  #define AXIS9_PARAMETER2              AXIS9_SERVO_I
  #define AXIS9_PARAMETER3              AXIS9_SERVO_D
  #define AXIS9_PARAMETER4              AXIS9_SERVO_P_GOTO
  #define AXIS9_PARAMETER5              AXIS9_SERVO_I_GOTO
  #define AXIS9_PARAMETER6              AXIS9_SERVO_D_GOTO
#endif

#ifndef FOCUSER_TEMPERATURE
#define FOCUSER_TEMPERATURE           OFF                         // activate and set focuser sensor type DS18B20, THERMISTOR, etc.
#endif
#ifndef FOCUSER_TEMPERATURE_PIN
#define FOCUSER_TEMPERATURE_PIN       OFF                         // for thermistors, analog pin
#endif

#if defined(AXIS1_DRIVER_PRESENT) || defined(AXIS2_DRIVER_PRESENT) || defined(AXIS3_DRIVER_PRESENT) || \
    defined(AXIS4_DRIVER_PRESENT) || defined(AXIS5_DRIVER_PRESENT) || defined(AXIS6_DRIVER_PRESENT) || \
    defined(AXIS7_DRIVER_PRESENT) || defined(AXIS8_DRIVER_PRESENT) || defined(AXIS9_DRIVER_PRESENT)
  #define STEP_DIR_MOTOR_PRESENT
#endif

#if defined(AXIS1_DRIVER_TMC_SPI) || defined(AXIS2_DRIVER_TMC_SPI) || defined(AXIS3_DRIVER_TMC_SPI) || \
    defined(AXIS4_DRIVER_TMC_SPI) || defined(AXIS5_DRIVER_TMC_SPI) || defined(AXIS6_DRIVER_TMC_SPI) || \
    defined(AXIS7_DRIVER_TMC_SPI) || defined(AXIS8_DRIVER_TMC_SPI) || defined(AXIS9_DRIVER_TMC_SPI)
  #define TMC_DRIVER_PRESENT
#endif

#if defined(AXIS1_SERVO_PRESENT) || defined(AXIS2_SERVO_PRESENT) || defined(AXIS3_SERVO_PRESENT) || \
    defined(AXIS4_SERVO_PRESENT) || defined(AXIS5_SERVO_PRESENT) || defined(AXIS6_SERVO_PRESENT) || \
    defined(AXIS7_SERVO_PRESENT) || defined(AXIS8_SERVO_PRESENT) || defined(AXIS9_SERVO_PRESENT)
  #define SERVO_MOTOR_PRESENT
#endif

#if defined(SERVO_MOTOR_PRESENT) || defined(STEP_DIR_MOTOR_PRESENT)
  #define MOTOR_PRESENT
#endif

// -----------------------------------------------------------------------------------
// auxiliary feature settings

#ifndef FEATURE1_PURPOSE
#define FEATURE1_PURPOSE              OFF                         // OFF or SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER
#endif
#ifndef FEATURE1_NAME
#define FEATURE1_NAME                "FEATURE1"
#endif
#ifndef FEATURE1_TEMP
#define FEATURE1_TEMP                 OFF                         // temperature sensor, thermistor or DS1820
#endif
#ifndef FEATURE1_TEMP_PIN
#define FEATURE1_TEMP_PIN             OFF                         // for thermistors, analog pin
#endif
#ifndef FEATURE1_PIN
#define FEATURE1_PIN                  OFF                         // OUTPUT control pin
#endif
#ifndef FEATURE1_DEFAULT_VALUE
#define FEATURE1_DEFAULT_VALUE        OFF                         // OUTPUT control pin default state
#endif
#ifndef FEATURE1_ACTIVE_STATE
#define FEATURE1_ACTIVE_STATE         HIGH                        // OUTPUT control pin active state
#endif

#ifndef FEATURE2_PURPOSE
#define FEATURE2_PURPOSE              OFF                         // OFF or SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER
#endif
#ifndef FEATURE2_NAME
#define FEATURE2_NAME                "FEATURE2"
#endif
#ifndef FEATURE2_TEMP
#define FEATURE2_TEMP                 OFF                         // temperature sensor, thermistor or DS1820
#endif
#ifndef FEATURE2_TEMP_PIN
#define FEATURE2_TEMP_PIN             OFF                         // for thermistors, analog pin
#endif
#ifndef FEATURE2_PIN
#define FEATURE2_PIN                  OFF                         // OUTPUT control pin
#endif
#ifndef FEATURE2_DEFAULT_VALUE
#define FEATURE2_DEFAULT_VALUE        OFF                         // OUTPUT control pin default state
#endif
#ifndef FEATURE2_ACTIVE_STATE
#define FEATURE2_ACTIVE_STATE         HIGH                        // OUTPUT control pin active state
#endif

#ifndef FEATURE3_PURPOSE
#define FEATURE3_PURPOSE              OFF                         // OFF or SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER
#endif
#ifndef FEATURE3_NAME
#define FEATURE3_NAME                "FEATURE3"
#endif
#ifndef FEATURE3_TEMP
#define FEATURE3_TEMP                 OFF                         // temperature sensor, thermistor or DS1820
#endif
#ifndef FEATURE3_TEMP_PIN
#define FEATURE3_TEMP_PIN             OFF                         // for thermistors, analog pin
#endif
#ifndef FEATURE3_PIN
#define FEATURE3_PIN                  OFF                         // OUTPUT control pin
#endif
#ifndef FEATURE3_DEFAULT_VALUE
#define FEATURE3_DEFAULT_VALUE        OFF                         // OUTPUT control pin default state
#endif
#ifndef FEATURE3_ACTIVE_STATE
#define FEATURE3_ACTIVE_STATE         HIGH                        // OUTPUT control pin active state
#endif

#ifndef FEATURE4_PURPOSE
#define FEATURE4_PURPOSE              OFF                         // OFF or SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER
#endif
#ifndef FEATURE4_NAME
#define FEATURE4_NAME                "FEATURE4"
#endif
#ifndef FEATURE4_TEMP
#define FEATURE4_TEMP                 OFF                         // temperature sensor, thermistor or DS1820
#endif
#ifndef FEATURE4_TEMP_PIN
#define FEATURE4_TEMP_PIN             OFF                         // for thermistors, analog pin
#endif
#ifndef FEATURE4_PIN
#define FEATURE4_PIN                  OFF                         // OUTPUT control pin
#endif
#ifndef FEATURE4_DEFAULT_VALUE
#define FEATURE4_DEFAULT_VALUE        OFF                         // OUTPUT control pin default state
#endif
#ifndef FEATURE4_ACTIVE_STATE
#define FEATURE4_ACTIVE_STATE         HIGH                        // OUTPUT control pin active state
#endif

#ifndef FEATURE5_PURPOSE
#define FEATURE5_PURPOSE              OFF                         // OFF or SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER
#endif
#ifndef FEATURE5_NAME
#define FEATURE5_NAME                "FEATURE5"
#endif
#ifndef FEATURE5_TEMP
#define FEATURE5_TEMP                 OFF                         // temperature sensor, thermistor or DS1820
#endif
#ifndef FEATURE5_TEMP_PIN
#define FEATURE5_TEMP_PIN             OFF                         // for thermistors, analog pin
#endif
#ifndef FEATURE5_PIN
#define FEATURE5_PIN                  OFF                         // OUTPUT control pin
#endif
#ifndef FEATURE5_DEFAULT_VALUE
#define FEATURE5_DEFAULT_VALUE        OFF                         // OUTPUT control pin default state
#endif
#ifndef FEATURE5_ACTIVE_STATE
#define FEATURE5_ACTIVE_STATE         HIGH                        // OUTPUT control pin active state
#endif

#ifndef FEATURE6_PURPOSE
#define FEATURE6_PURPOSE              OFF                         // OFF or SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER
#endif
#ifndef FEATURE6_NAME
#define FEATURE6_NAME                "FEATURE6"
#endif
#ifndef FEATURE6_TEMP
#define FEATURE6_TEMP                 OFF                         // temperature sensor, thermistor or DS1820
#endif
#ifndef FEATURE6_TEMP_PIN
#define FEATURE6_TEMP_PIN             OFF                         // for thermistors, analog pin
#endif
#ifndef FEATURE6_PIN
#define FEATURE6_PIN                  OFF                         // OUTPUT control pin
#endif
#ifndef FEATURE6_DEFAULT_VALUE
#define FEATURE6_DEFAULT_VALUE        OFF                         // OUTPUT control pin default state
#endif
#ifndef FEATURE6_ACTIVE_STATE
#define FEATURE6_ACTIVE_STATE         HIGH                        // OUTPUT control pin active state
#endif

#ifndef FEATURE7_PURPOSE
#define FEATURE7_PURPOSE              OFF                         // OFF or SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER
#endif
#ifndef FEATURE7_NAME
#define FEATURE7_NAME                "FEATURE7"
#endif
#ifndef FEATURE7_TEMP
#define FEATURE7_TEMP                 OFF                         // temperature sensor, thermistor or DS1820
#endif
#ifndef FEATURE7_TEMP_PIN
#define FEATURE7_TEMP_PIN             OFF                         // for thermistors, analog pin
#endif
#ifndef FEATURE7_PIN
#define FEATURE7_PIN                  OFF                         // OUTPUT control pin
#endif
#ifndef FEATURE7_DEFAULT_VALUE
#define FEATURE7_DEFAULT_VALUE        OFF                         // OUTPUT control pin default state
#endif
#ifndef FEATURE7_ACTIVE_STATE
#define FEATURE7_ACTIVE_STATE         HIGH                        // OUTPUT control pin active state
#endif

#ifndef FEATURE8_PURPOSE
#define FEATURE8_PURPOSE              OFF                         // OFF or SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER
#endif
#ifndef FEATURE8_NAME
#define FEATURE8_NAME                "FEATURE8"
#endif
#ifndef FEATURE8_TEMP
#define FEATURE8_TEMP                 OFF                         // temperature sensor, thermistor or DS1820
#endif
#ifndef FEATURE8_TEMP_PIN
#define FEATURE8_TEMP_PIN             OFF                         // for thermistors, analog pin
#endif
#ifndef FEATURE8_PIN
#define FEATURE8_PIN                  OFF                         // OUTPUT control pin
#endif
#ifndef FEATURE8_DEFAULT_VALUE
#define FEATURE8_DEFAULT_VALUE        OFF                         // OUTPUT control pin default state
#endif
#ifndef FEATURE8_ACTIVE_STATE
#define FEATURE8_ACTIVE_STATE         HIGH                        // OUTPUT control pin active state
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
