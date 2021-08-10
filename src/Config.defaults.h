// -----------------------------------------------------------------------------------
// controller settings
#pragma once

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

// ESP32 VIRTUAL SERIAL IP COMMAND CHANNEL (EXPERIMENTAL)
#ifndef SERIAL_IP_MODE
#define SERIAL_IP_MODE                 OFF //         OFF, Use ACCESS_POINT or STATION to enable the interface (ESP32 only.)
#endif

#ifndef STANDARD_COMMAND_CHANNEL
#define STANDARD_COMMAND_CHANNEL       ON //          ON, Wifi command channel for simultanious connections on port 9999.
#endif
#ifndef PERSISTENT_COMMAND_CHANNEL
#define PERSISTENT_COMMAND_CHANNEL     ON //          ON, Wifi command channel for a single connection on port 9998.
#endif

#ifndef AP_SSID
#define AP_SSID                 "ONSTEPX" //   "ONSTEPX", Wifi Access Point SSID.
#define AP_PASSWORD            "password" //  "password", Wifi Access Point password.
#define AP_CHANNEL                      7 //           7, Wifi Access Point channel.
#define AP_IP_ADDR          (192,168,0,1) // ..,168,0,1), Wifi Access Point IP Address.
#define AP_GW_ADDR          (192,168,0,1) // ..,168,0,1), Wifi Access Point GATEWAY Address.
#define AP_SN_MASK        (255,255,255,0) // ..55,255,0), Wifi Access Point SUBNET Mask.
#endif

#ifndef STA_SSID
#define STA_SSID                   "Home" //      "Home", Wifi Station SSID to connnect to.
#define STA_PASSWORD           "password" //  "password", Wifi Station mode password.
#define STA_IP_ADDR        (192,168,1,55) // ..168,1,55), Wifi Station IP Address.
#define STA_GW_ADDR         (192,168,1,1) // ..,168,1,1), Wifi Station GATEWAY Address.
#define STA_SN_MASK       (255,255,255,0) // ..55,255,0), Wifi Station SUBNET Mask.
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

// -----------------------------------------------------------------------------------
// mount settings

// axes
#ifndef AXIS1_STEPS_PER_DEGREE
#define AXIS1_STEPS_PER_DEGREE        12800.0
#endif
#ifndef AXIS1_REVERSE
#define AXIS1_REVERSE                 OFF
#endif
#ifndef AXIS1_POWER_DOWN
#define AXIS1_POWER_DOWN              OFF                         // motor power off if at standstill > 30 sec
#endif
#ifndef AXIS1_LIMIT_MIN
#define AXIS1_LIMIT_MIN               -180                        // in degrees
#endif
#ifndef AXIS1_LIMIT_MAX
#define AXIS1_LIMIT_MAX               180                         // in degrees
#endif
#ifndef AXIS1_SENSE_HOME
#define AXIS1_SENSE_HOME              OFF                         // HIGH/LOW to activate home sensing and set direction
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

#ifndef AXIS2_STEPS_PER_DEGREE
#define AXIS2_STEPS_PER_DEGREE        12800.0
#endif
#ifndef AXIS2_REVERSE
#define AXIS2_REVERSE                 OFF
#endif
#ifndef AXIS2_POWER_DOWN
#define AXIS2_POWER_DOWN              OFF
#endif
#ifndef AXIS2_TANGENT_ARM
#define AXIS2_TANGENT_ARM             OFF
#endif
#ifndef AXIS2_LIMIT_MIN
#define AXIS2_LIMIT_MIN               -90                         // in degrees
#endif
#ifndef AXIS2_LIMIT_MAX
#define AXIS2_LIMIT_MAX               90                          // in degrees
#endif
#ifndef AXIS2_SENSE_HOME
#define AXIS2_SENSE_HOME              OFF
#endif
#ifndef AXIS2_SENSE_HOME_INIT
#define AXIS2_SENSE_HOME_INIT         INPUT_PULLUP
#endif
#ifndef AXIS2_SENSE_LIMIT_MIN
#define AXIS2_SENSE_LIMIT_MIN         LIMIT_SENSE
#endif
#ifndef AXIS2_SENSE_LIMIT_MAX
#define AXIS2_SENSE_LIMIT_MAX         LIMIT_SENSE
#endif
#ifndef AXIS2_SENSE_LIMIT_INIT
#define AXIS2_SENSE_LIMIT_INIT        LIMIT_SENSE_INIT
#endif

// mount type
#ifndef MOUNT_TYPE
#define MOUNT_TYPE                    GEM
#endif

// user feedback
#ifndef LED_STATUS
#define LED_STATUS                    ON
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
#ifndef BUZZER_MEMORY
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
#define GUIDE_TIME_LIMIT              0              // in seconds
#endif
#ifndef GUIDE_HOME_TIME_LIMIT
#define GUIDE_HOME_TIME_LIMIT         5*60           // in seconds
#endif
#ifndef GUIDE_DISABLE_BACKLASH
#define GUIDE_DISABLE_BACKLASH        OFF
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

// non-volatile storage
#ifndef NV_DEFAULT
#define NV_DEFAULT                    ON
#endif

// ESP32 virtual serial bluetooth command channel
#ifndef SERIAL_BT_MODE
#define SERIAL_BT_MODE                OFF
#endif
#ifndef SERIAL_BT_NAME
#define SERIAL_BT_NAME                "OnStep"
#endif

// -----------------------------------------------------------------------------------
// rotator settings, ROTATOR
#ifndef AXIS3_STEPS_PER_DEGREE
#define AXIS3_STEPS_PER_DEGREE        64.0
#endif
#ifndef AXIS3_REVERSE
#define AXIS3_REVERSE                 OFF
#endif
#ifndef AXIS3_POWER_DOWN
#define AXIS3_POWER_DOWN              OFF
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

// -----------------------------------------------------------------------------------
// focuser settings, FOCUSER1
#ifndef AXIS4_STEPS_PER_MICRON
#define AXIS4_STEPS_PER_MICRON        0.5
#endif
#ifndef AXIS4_REVERSE
#define AXIS4_REVERSE                 OFF
#endif
#ifndef AXIS4_POWER_DOWN
#define AXIS4_POWER_DOWN              OFF
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

// focuser settings, FOCUSER2
#ifndef AXIS5_STEPS_PER_MICRON
#define AXIS5_STEPS_PER_MICRON        0.5
#endif
#ifndef AXIS5_REVERSE
#define AXIS5_REVERSE                 OFF
#endif
#ifndef AXIS5_POWER_DOWN
#define AXIS5_POWER_DOWN              OFF
#endif
#ifndef AXIS5_SLEW_RATE_MINIMUM
#define AXIS5_SLEW_RATE_MINIMUM       2                           // in microns/sec
#endif
#ifndef AXIS5_SLEW_RATE_DESIRED
#define AXIS5_SLEW_RATE_DESIRED       500                         // in microns/sec
#endif
#ifndef AXIS5_ACCELERATION_TIME
#define AXIS5_ACCELERATION_TIME       2                           // in seconds, to selected rate
#endif
#ifndef AXIS5_RAPID_STOP_TIME
#define AXIS5_RAPID_STOP_TIME         1                           // in seconds, to stop
#endif
#ifndef AXIS5_BACKLASH_RATE
#define AXIS5_BACKLASH_RATE           (AXIS5_SLEW_RATE_DESIRED/4) // in microns/sec
#endif
#ifndef AXIS5_LIMIT_MIN
#define AXIS5_LIMIT_MIN               0                           // in mm
#endif
#ifndef AXIS5_LIMIT_MAX
#define AXIS5_LIMIT_MAX               50                          // in mm
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

// focuser settings, FOCUSER3
#ifndef AXIS6_STEPS_PER_MICRON
#define AXIS6_STEPS_PER_MICRON        0.5
#endif
#ifndef AXIS6_REVERSE
#define AXIS6_REVERSE                 OFF
#endif
#ifndef AXIS6_POWER_DOWN
#define AXIS6_POWER_DOWN              OFF
#endif
#ifndef AXIS6_SLEW_RATE_MINIMUM
#define AXIS6_SLEW_RATE_MINIMUM       2                           // in microns/sec
#endif
#ifndef AXIS6_SLEW_RATE_DESIRED
#define AXIS6_SLEW_RATE_DESIRED       500                         // in microns/sec
#endif
#ifndef AXIS6_ACCELERATION_TIME
#define AXIS6_ACCELERATION_TIME       2                           // in seconds, to selected rate
#endif
#ifndef AXIS6_RAPID_STOP_TIME
#define AXIS6_RAPID_STOP_TIME         1                           // in seconds, to stop
#endif
#ifndef AXIS6_BACKLASH_RATE
#define AXIS6_BACKLASH_RATE           (AXIS6_SLEW_RATE_DESIRED/4) // in microns/sec
#endif
#ifndef AXIS6_LIMIT_MIN
#define AXIS6_LIMIT_MIN               0                           // in mm
#endif
#ifndef AXIS6_LIMIT_MAX
#define AXIS6_LIMIT_MAX               50                          // in mm
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

// focuser settings, FOCUSER4
#ifndef AXIS7_STEPS_PER_MICRON
#define AXIS7_STEPS_PER_MICRON        0.5
#endif
#ifndef AXIS7_REVERSE
#define AXIS7_REVERSE                 OFF
#endif
#ifndef AXIS7_POWER_DOWN
#define AXIS7_POWER_DOWN              OFF
#endif
#ifndef AXIS7_SLEW_RATE_MINIMUM
#define AXIS7_SLEW_RATE_MINIMUM       2                           // in microns/sec
#endif
#ifndef AXIS7_SLEW_RATE_DESIRED
#define AXIS7_SLEW_RATE_DESIRED       500                         // in microns/sec
#endif
#ifndef AXIS7_ACCELERATION_TIME
#define AXIS7_ACCELERATION_TIME       2                           // in seconds, to selected rate
#endif
#ifndef AXIS7_RAPID_STOP_TIME
#define AXIS7_RAPID_STOP_TIME         1                           // in seconds, to stop
#endif
#ifndef AXIS7_BACKLASH_RATE
#define AXIS7_BACKLASH_RATE           (AXIS7_SLEW_RATE_DESIRED/4) // in microns/sec
#endif
#ifndef AXIS7_LIMIT_MIN
#define AXIS7_LIMIT_MIN               0                           // in mm
#endif
#ifndef AXIS7_LIMIT_MAX
#define AXIS7_LIMIT_MAX               50                          // in mm
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

// focuser settings, FOCUSER5
#ifndef AXIS8_STEPS_PER_MICRON
#define AXIS8_STEPS_PER_MICRON        0.5
#endif
#ifndef AXIS8_REVERSE
#define AXIS8_REVERSE                 OFF
#endif
#ifndef AXIS8_POWER_DOWN
#define AXIS8_POWER_DOWN              OFF
#endif
#ifndef AXIS8_SLEW_RATE_MINIMUM
#define AXIS8_SLEW_RATE_MINIMUM       2                           // in microns/sec
#endif
#ifndef AXIS8_SLEW_RATE_DESIRED
#define AXIS8_SLEW_RATE_DESIRED       500                         // in microns/sec
#endif
#ifndef AXIS8_ACCELERATION_TIME
#define AXIS8_ACCELERATION_TIME       2                           // in seconds, to selected rate
#endif
#ifndef AXIS8_RAPID_STOP_TIME
#define AXIS8_RAPID_STOP_TIME         1                           // in seconds, to stop
#endif
#ifndef AXIS8_BACKLASH_RATE
#define AXIS8_BACKLASH_RATE           (AXIS8_SLEW_RATE_DESIRED/4) // in microns/sec
#endif
#ifndef AXIS8_LIMIT_MIN
#define AXIS8_LIMIT_MIN               0                           // in mm
#endif
#ifndef AXIS8_LIMIT_MAX
#define AXIS8_LIMIT_MAX               50                          // in mm
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

// focuser settings, FOCUSER6
#ifndef AXIS9_STEPS_PER_MICRON
#define AXIS9_STEPS_PER_MICRON        0.5
#endif
#ifndef AXIS9_REVERSE
#define AXIS9_REVERSE                 OFF
#endif
#ifndef AXIS9_POWER_DOWN
#define AXIS9_POWER_DOWN              OFF
#endif
#ifndef AXIS9_SLEW_RATE_MINIMUM
#define AXIS9_SLEW_RATE_MINIMUM       2                           // in microns/sec
#endif
#ifndef AXIS9_SLEW_RATE_DESIRED
#define AXIS9_SLEW_RATE_DESIRED       500                         // in microns/sec
#endif
#ifndef AXIS9_ACCELERATION_TIME
#define AXIS9_ACCELERATION_TIME       2                           // in seconds, to selected rate
#endif
#ifndef AXIS9_RAPID_STOP_TIME
#define AXIS9_RAPID_STOP_TIME         1                           // in seconds, to stop
#endif
#ifndef AXIS9_BACKLASH_RATE
#define AXIS9_BACKLASH_RATE           (AXIS9_SLEW_RATE_DESIRED/4) // in microns/sec
#endif
#ifndef AXIS9_LIMIT_MIN
#define AXIS9_LIMIT_MIN               0                           // in mm
#endif
#ifndef AXIS9_LIMIT_MAX
#define AXIS9_LIMIT_MAX               50                          // in mm
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

#ifndef FOCUSER_TEMPERATURE
#define FOCUSER_TEMPERATURE           OFF // activate and set focuser sensor type DS18B20, THERMISTOR, etc.
#endif
#ifndef FOCUSER_TEMPERATURE_PIN
#define FOCUSER_TEMPERATURE_PIN       OFF // for thermistors, analog pin
#endif

// -----------------------------------------------------------------------------------
// auxiliary feature settings

#ifndef FEATURE1_PURPOSE
#define FEATURE1_PURPOSE              OFF
#endif
#ifndef FEATURE1_NAME
#define FEATURE1_NAME                "FEATURE1"
#endif
#ifndef FEATURE1_TEMP
#define FEATURE1_TEMP                 OFF   // temperature sensor, thermistor or DS1820
#endif
#ifndef FEATURE1_TEMP_PIN
#define FEATURE1_TEMP_PIN             OFF   // for thermistors, analog pin
#endif
#ifndef FEATURE1_PIN
#define FEATURE1_PIN                  OFF   // OUTPUT control pin
#endif
#ifndef FEATURE1_DEFAULT_VALUE
#define FEATURE1_DEFAULT_VALUE        OFF   // OUTPUT control pin default state
#endif
#ifndef FEATURE1_ACTIVE_STATE
#define FEATURE1_ACTIVE_STATE         HIGH  // OUTPUT control pin active state
#endif

#ifndef FEATURE2_PURPOSE
#define FEATURE2_PURPOSE              OFF
#endif
#ifndef FEATURE2_NAME
#define FEATURE2_NAME                "FEATURE2"
#endif
#ifndef FEATURE2_TEMP
#define FEATURE2_TEMP                 OFF   // temperature sensor, thermistor or DS1820
#endif
#ifndef FEATURE2_TEMP_PIN
#define FEATURE2_TEMP_PIN             OFF   // for thermistors, analog pin
#endif
#ifndef FEATURE2_PIN
#define FEATURE2_PIN                  OFF   // OUTPUT control pin
#endif
#ifndef FEATURE2_DEFAULT_VALUE
#define FEATURE2_DEFAULT_VALUE        OFF   // OUTPUT control pin default state
#endif
#ifndef FEATURE2_ACTIVE_STATE
#define FEATURE2_ACTIVE_STATE         HIGH  // OUTPUT control pin active state
#endif

#ifndef FEATURE3_PURPOSE
#define FEATURE3_PURPOSE              OFF
#endif
#ifndef FEATURE3_NAME
#define FEATURE3_NAME                "FEATURE3"
#endif
#ifndef FEATURE3_TEMP
#define FEATURE3_TEMP                 OFF   // temperature sensor, thermistor or DS1820
#endif
#ifndef FEATURE3_TEMP_PIN
#define FEATURE3_TEMP_PIN             OFF   // for thermistors, analog pin
#endif
#ifndef FEATURE3_PIN
#define FEATURE3_PIN                  OFF   // OUTPUT control pin
#endif
#ifndef FEATURE3_DEFAULT_VALUE
#define FEATURE3_DEFAULT_VALUE        OFF   // OUTPUT control pin default state
#endif
#ifndef FEATURE3_ACTIVE_STATE
#define FEATURE3_ACTIVE_STATE         HIGH  // OUTPUT control pin active state
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
#ifndef FEATURE4_TEMP_PIN
#define FEATURE4_TEMP_PIN             OFF   // for thermistors, analog pin
#endif
#ifndef FEATURE4_PIN
#define FEATURE4_PIN                  OFF   // OUTPUT control pin
#endif
#ifndef FEATURE4_DEFAULT_VALUE
#define FEATURE4_DEFAULT_VALUE        OFF   // OUTPUT control pin default state
#endif
#ifndef FEATURE4_ACTIVE_STATE
#define FEATURE4_ACTIVE_STATE         HIGH  // OUTPUT control pin active state
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
#ifndef FEATURE5_TEMP_PIN
#define FEATURE5_TEMP_PIN             OFF   // for thermistors, analog pin
#endif
#ifndef FEATURE5_PIN
#define FEATURE5_PIN                  OFF   // OUTPUT control pin
#endif
#ifndef FEATURE5_DEFAULT_VALUE
#define FEATURE5_DEFAULT_VALUE        OFF   // OUTPUT control pin default state
#endif
#ifndef FEATURE5_ACTIVE_STATE
#define FEATURE5_ACTIVE_STATE         HIGH  // OUTPUT control pin active state
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
#ifndef FEATURE6_TEMP_PIN
#define FEATURE6_TEMP_PIN             OFF   // for thermistors, analog pin
#endif
#ifndef FEATURE6_PIN
#define FEATURE6_PIN                  OFF   // OUTPUT control pin
#endif
#ifndef FEATURE6_DEFAULT_VALUE
#define FEATURE6_DEFAULT_VALUE        OFF   // OUTPUT control pin default state
#endif
#ifndef FEATURE6_ACTIVE_STATE
#define FEATURE6_ACTIVE_STATE         HIGH  // OUTPUT control pin active state
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
#ifndef FEATURE7_TEMP_PIN
#define FEATURE7_TEMP_PIN             OFF   // for thermistors, analog pin
#endif
#ifndef FEATURE7_PIN
#define FEATURE7_PIN                  OFF   // OUTPUT control pin
#endif
#ifndef FEATURE7_DEFAULT_VALUE
#define FEATURE7_DEFAULT_VALUE        OFF   // OUTPUT control pin default state
#endif
#ifndef FEATURE7_ACTIVE_STATE
#define FEATURE7_ACTIVE_STATE         HIGH  // OUTPUT control pin active state
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
#ifndef FEATURE8_TEMP_PIN
#define FEATURE8_TEMP_PIN             OFF   // for thermistors, analog pin
#endif
#ifndef FEATURE8_PIN
#define FEATURE8_PIN                  OFF   // OUTPUT control pin
#endif
#ifndef FEATURE8_DEFAULT_VALUE
#define FEATURE8_DEFAULT_VALUE        OFF   // OUTPUT control pin default state
#endif
#ifndef FEATURE8_ACTIVE_STATE
#define FEATURE8_ACTIVE_STATE         HIGH  // OUTPUT control pin active state
#endif

// thermistor configuration settings to support two types

#ifndef THERMISTOR1_TNOM
#define THERMISTOR1_TNOM              25    // nominal temperature (Celsius)
#endif
#ifndef THERMISTOR1_RNOM
#define THERMISTOR1_RNOM              10000 // nominal resistance (Ohms) at nominal temperature
#endif
#ifndef THERMISTOR1_BETA
#define THERMISTOR1_BETA              3950  // beta coefficient
#endif
#ifndef THERMISTOR1_RSERIES
#define THERMISTOR1_RSERIES           4700  // series resistor value (Ohms)
#endif

#ifndef THERMISTOR2_TNOM
#define THERMISTOR2_TNOM              25    // nominal temperature (Celsius)
#endif
#ifndef THERMISTOR2_RNOM
#define THERMISTOR2_RNOM              10000 // nominal resistance (Ohms) at nominal temperature
#endif
#ifndef THERMISTOR2_BETA
#define THERMISTOR2_BETA              3950  // beta coefficient
#endif
#ifndef THERMISTOR2_RSERIES
#define THERMISTOR2_RSERIES           4700  // series resistor value (Ohms)
#endif
