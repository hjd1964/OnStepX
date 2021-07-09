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
#define SERIAL_B_BAUD_DEFAULT         9600
#endif
#ifndef SERIAL_B_ESP_FLASHING
#define SERIAL_B_ESP_FLASHING         OFF
#endif
#ifndef SERIAL_C_BAUD_DEFAULT
#define SERIAL_C_BAUD_DEFAULT         OFF
#endif

// sensors
#ifndef WEATHER
#define WEATHER                       OFF
#endif
#ifndef FOCUSER_TEMPERATURE
#define FOCUSER_TEMPERATURE           OFF
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
#ifndef AXIS2_TANGENT_ARM
#define AXIS2_TANGENT_ARM             OFF
#endif
#ifndef AXIS2_STEPS_PER_DEGREE
#define AXIS2_STEPS_PER_DEGREE        12800.0
#endif

// mount type
#ifndef MOUNT_TYPE
#define MOUNT_TYPE                    GEM
#endif

// user feedback
#ifndef LED_STATUS
#define LED_STATUS                    ON
#endif
#ifndef LED_STATUS_ON_STATE
#define LED_STATUS_ON_STATE           LOW
#endif
#ifndef LED_MOUNT_STATUS
#define LED_MOUNT_STATUS              OFF
#endif
#ifndef LED_MOUNT_STATUS_ON_STATE
#define LED_MOUNT_STATUS_ON_STATE     LOW
#endif
#ifndef LED_ROTATOR_STATUS
#define LED_ROTATOR_STATUS            OFF
#endif
#ifndef LED_ROTATOR_STATUS_ON_STATE
#define LED_ROTATOR_STATUS_ON_STATE   LOW
#endif
#ifndef LED_FOCUSER_STATUS
#define LED_FOCUSER_STATUS            OFF
#endif
#ifndef LED_FOCUSER_STATUS_ON_STATE
#define LED_FOCUSER_STATUS_ON_STATE   LOW
#endif
#ifndef BUZZER
#define BUZZER                        OFF
#endif
#ifndef BUZZER_ON_STATE
#define BUZZER_ON_STATE               HIGH
#endif
#ifndef BUZZER_DEFAULT
#define BUZZER_DEFAULT                OFF
#endif
#ifndef BUZZER_MEMORY
#define BUZZER_MEMORY                 OFF
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
#define GUIDE_TIME_LIMIT              0
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
#ifndef AXIS3_SLEW_RATE_DESIRED
#define AXIS3_SLEW_RATE_DESIRED       1.0                         // in degrees/sec
#endif
#ifndef AXIS3_ACCELERATION_RATE
#define AXIS3_ACCELERATION_RATE       AXIS3_SLEW_RATE_DESIRED     // in degrees/sec/sec
#endif
#ifndef AXIS3_RAPID_STOP_RATE
#define AXIS3_RAPID_STOP_RATE         (AXIS3_ACCELERATION_RATE*2) // in degrees/sec/sec
#endif
#ifndef AXIS3_BACKLASH_RATE
#define AXIS3_BACKLASH_RATE           (AXIS3_ACCELERATION_RATE/4) // in degrees/sec
#endif

// -----------------------------------------------------------------------------------
// focuser settings, FOCUSER1
#ifndef AXIS4_DRIVER_DC_MODE
#define AXIS4_DRIVER_DC_MODE          OFF
#endif
#ifndef AXIS4_STEPS_PER_MICRON
#define AXIS4_STEPS_PER_MICRON        0.5
#endif
#ifndef AXIS4_SLEW_RATE_MINIMUM
#define AXIS4_SLEW_RATE_MINIMUM       50                          // in microns/sec
#endif
#ifndef AXIS4_SLEW_RATE_DESIRED
#define AXIS4_SLEW_RATE_DESIRED       500                         // in microns/sec
#endif
#ifndef AXIS4_ACCELERATION_RATE
#define AXIS4_ACCELERATION_RATE       AXIS4_SLEW_RATE_DESIRED     // in microns/sec/sec
#endif
#ifndef AXIS4_RAPID_STOP_RATE
#define AXIS4_RAPID_STOP_RATE         (AXIS4_ACCELERATION_RATE*2) // in microns/sec/sec
#endif
#ifndef AXIS4_BACKLASH_RATE
#define AXIS4_BACKLASH_RATE           (AXIS4_ACCELERATION_RATE/4) // in microns/sec
#endif

// focuser settings, FOCUSER2
#ifndef AXIS5_DRIVER_DC_MODE
#define AXIS5_DRIVER_DC_MODE          OFF
#endif
#ifndef AXIS5_STEPS_PER_MICRON
#define AXIS5_STEPS_PER_MICRON        0.5
#endif
#ifndef AXIS5_SLEW_RATE_MINIMUM
#define AXIS5_SLEW_RATE_MINIMUM       50                          // in microns/sec
#endif
#ifndef AXIS5_SLEW_RATE_DESIRED
#define AXIS5_SLEW_RATE_DESIRED       500                         // in microns/sec
#endif
#ifndef AXIS5_ACCELERATION_RATE
#define AXIS5_ACCELERATION_RATE       AXIS5_SLEW_RATE_DESIRED     // in microns/sec/sec
#endif
#ifndef AXIS5_RAPID_STOP_RATE
#define AXIS5_RAPID_STOP_RATE         (AXIS5_ACCELERATION_RATE*2) // in microns/sec/sec
#endif
#ifndef AXIS5_BACKLASH_RATE
#define AXIS5_BACKLASH_RATE           (AXIS5_ACCELERATION_RATE/4) // in microns/sec
#endif

// focuser settings, FOCUSER3
#ifndef AXIS6_DRIVER_DC_MODE
#define AXIS6_DRIVER_DC_MODE          OFF
#endif
#ifndef AXIS6_STEPS_PER_MICRON
#define AXIS6_STEPS_PER_MICRON        0.5
#endif
#ifndef AXIS6_SLEW_RATE_MINIMUM
#define AXIS6_SLEW_RATE_MINIMUM       50                          // in microns/sec
#endif
#ifndef AXIS6_SLEW_RATE_DESIRED
#define AXIS6_SLEW_RATE_DESIRED       500                         // in microns/sec
#endif
#ifndef AXIS6_ACCELERATION_RATE
#define AXIS6_ACCELERATION_RATE       AXIS6_SLEW_RATE_DESIRED     // in microns/sec/sec
#endif
#ifndef AXIS6_RAPID_STOP_RATE
#define AXIS6_RAPID_STOP_RATE         (AXIS6_ACCELERATION_RATE*2) // in microns/sec/sec
#endif
#ifndef AXIS6_BACKLASH_RATE
#define AXIS6_BACKLASH_RATE           (AXIS6_ACCELERATION_RATE/4) // in microns/sec
#endif

// focuser settings, FOCUSER4
#ifndef AXIS7_DRIVER_DC_MODE
#define AXIS7_DRIVER_DC_MODE          OFF
#endif
#ifndef AXIS7_STEPS_PER_MICRON
#define AXIS7_STEPS_PER_MICRON        0.5
#endif
#ifndef AXIS7_SLEW_RATE_MINIMUM
#define AXIS7_SLEW_RATE_MINIMUM       50                          // in microns/sec
#endif
#ifndef AXIS7_SLEW_RATE_DESIRED
#define AXIS7_SLEW_RATE_DESIRED       500                         // in microns/sec
#endif
#ifndef AXIS7_ACCELERATION_RATE
#define AXIS7_ACCELERATION_RATE       AXIS7_SLEW_RATE_DESIRED     // in microns/sec/sec
#endif
#ifndef AXIS7_RAPID_STOP_RATE
#define AXIS7_RAPID_STOP_RATE         (AXIS7_ACCELERATION_RATE*2) // in microns/sec/sec
#endif
#ifndef AXIS7_BACKLASH_RATE
#define AXIS7_BACKLASH_RATE           (AXIS7_ACCELERATION_RATE/4) // in microns/sec
#endif

// focuser settings, FOCUSER5
#ifndef AXIS8_DRIVER_DC_MODE
#define AXIS8_DRIVER_DC_MODE          OFF
#endif
#ifndef AXIS8_STEPS_PER_MICRON
#define AXIS8_STEPS_PER_MICRON        0.5
#endif
#ifndef AXIS8_SLEW_RATE_MINIMUM
#define AXIS8_SLEW_RATE_MINIMUM       50                          // in microns/sec
#endif
#ifndef AXIS8_SLEW_RATE_DESIRED
#define AXIS8_SLEW_RATE_DESIRED       500                         // in microns/sec
#endif
#ifndef AXIS8_ACCELERATION_RATE
#define AXIS8_ACCELERATION_RATE       AXIS8_SLEW_RATE_DESIRED     // in microns/sec/sec
#endif
#ifndef AXIS8_RAPID_STOP_RATE
#define AXIS8_RAPID_STOP_RATE         (AXIS8_ACCELERATION_RATE*2) // in microns/sec/sec
#endif
#ifndef AXIS8_BACKLASH_RATE
#define AXIS8_BACKLASH_RATE           (AXIS8_ACCELERATION_RATE/4) // in microns/sec
#endif

// focuser settings, FOCUSER6
#ifndef AXIS9_DRIVER_DC_MODE
#define AXIS9_DRIVER_DC_MODE          OFF
#endif
#ifndef AXIS9_STEPS_PER_MICRON
#define AXIS9_STEPS_PER_MICRON        0.5
#endif
#ifndef AXIS9_SLEW_RATE_MINIMUM
#define AXIS9_SLEW_RATE_MINIMUM       50                          // in microns/sec
#endif
#ifndef AXIS9_SLEW_RATE_DESIRED
#define AXIS9_SLEW_RATE_DESIRED       500                         // in microns/sec
#endif
#ifndef AXIS9_ACCELERATION_RATE
#define AXIS9_ACCELERATION_RATE       AXIS9_SLEW_RATE_DESIRED     // in microns/sec/sec
#endif
#ifndef AXIS9_RAPID_STOP_RATE
#define AXIS9_RAPID_STOP_RATE         (AXIS9_ACCELERATION_RATE*2) // in microns/sec/sec
#endif
#ifndef AXIS9_BACKLASH_RATE
#define AXIS9_BACKLASH_RATE           (AXIS9_ACCELERATION_RATE/4) // in microns/sec
#endif

// -----------------------------------------------------------------------------------
// common axis settings, RA/AZM
#ifndef AXIS1_DRIVER_MODEL
#define AXIS1_DRIVER_MODEL            OFF // axis 1/2 combination enables this feature
#endif
#ifndef AXIS1_DRIVER_DECAY
#define AXIS1_DRIVER_DECAY            OFF // OFF for default, TMC STEALTHCHOP
#endif
#ifndef AXIS1_DRIVER_DECAY_GOTO
#define AXIS1_DRIVER_DECAY_GOTO       OFF // OFF for default, TMC SPREADCYCLE
#endif
#ifndef AXIS1_DRIVER_MICROSTEPS
#define AXIS1_DRIVER_MICROSTEPS       OFF
#endif
#ifndef AXIS1_DRIVER_MICROSTEPS_GOTO
#define AXIS1_DRIVER_MICROSTEPS_GOTO  OFF
#endif
#ifndef AXIS1_DRIVER_IHOLD
#define AXIS1_DRIVER_IHOLD            HALF
#endif
#ifndef AXIS1_DRIVER_IRUN
#define AXIS1_DRIVER_IRUN             OFF
#endif
#ifndef AXIS1_DRIVER_IGOTO
#define AXIS1_DRIVER_IGOTO            SAME
#endif
#ifndef AXIS1_DRIVER_POWER_DOWN
#define AXIS1_DRIVER_POWER_DOWN       OFF
#endif
#ifndef AXIS1_DRIVER_REVERSE
#define AXIS1_DRIVER_REVERSE          OFF
#endif
#ifndef AXIS1_DRIVER_STATUS
#define AXIS1_DRIVER_STATUS           OFF
#endif
#ifndef AXIS1_LIMIT_MIN
#define AXIS1_LIMIT_MIN               -180
#endif
#ifndef AXIS1_LIMIT_MAX
#define AXIS1_LIMIT_MAX               180
#endif
#ifndef AXIS1_SENSE_HOME
#define AXIS1_SENSE_HOME              OFF
#endif
#ifndef AXIS1_SENSE_HOME_INIT
#define AXIS1_SENSE_HOME_INIT         INPUT_PULLUP
#endif
#ifndef AXIS1_SENSE_LIMIT_MIN
#define AXIS1_SENSE_LIMIT_MIN         LIMIT_SENSE
#endif
#ifndef AXIS1_SENSE_LIMIT_MAX
#define AXIS1_SENSE_LIMIT_MAX         LIMIT_SENSE
#endif
#ifndef AXIS1_SENSE_LIMIT_INIT
#define AXIS1_SENSE_LIMIT_INIT        LIMIT_SENSE_INIT
#endif

// common axis settings, DEC/ALT
#ifndef AXIS2_DRIVER_MODEL
#define AXIS2_DRIVER_MODEL            OFF // axis 1/2 combination enables this feature
#endif
#ifndef AXIS2_DRIVER_DECAY
#define AXIS2_DRIVER_DECAY            OFF // OFF for default, TMC STEALTHCHOP
#endif
#ifndef AXIS2_DRIVER_DECAY_GOTO
#define AXIS2_DRIVER_DECAY_GOTO       OFF // OFF for default, TMC SPREADCYCLE
#endif
#ifndef AXIS2_DRIVER_MICROSTEPS
#define AXIS2_DRIVER_MICROSTEPS       OFF
#endif
#ifndef AXIS2_DRIVER_MICROSTEPS_GOTO
#define AXIS2_DRIVER_MICROSTEPS_GOTO  OFF
#endif
#ifndef AXIS2_DRIVER_IHOLD
#define AXIS2_DRIVER_IHOLD            HALF
#endif
#ifndef AXIS2_DRIVER_IRUN
#define AXIS2_DRIVER_IRUN             OFF
#endif
#ifndef AXIS2_DRIVER_IGOTO
#define AXIS2_DRIVER_IGOTO            SAME
#endif
#ifndef AXIS2_DRIVER_POWER_DOWN
#define AXIS2_DRIVER_POWER_DOWN       OFF
#endif
#ifndef AXIS2_DRIVER_REVERSE
#define AXIS2_DRIVER_REVERSE          OFF
#endif
#ifndef AXIS2_DRIVER_STATUS
#define AXIS2_DRIVER_STATUS           OFF
#endif
#ifndef AXIS2_LIMIT_MIN
#define AXIS2_LIMIT_MIN               -90
#endif
#ifndef AXIS2_LIMIT_MAX
#define AXIS2_LIMIT_MAX               90
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

// common axis settings, ROTATOR
#ifndef AXIS3_DRIVER_MODEL
#define AXIS3_DRIVER_MODEL            OFF // axis 1/2 combination enables this feature
#endif
#ifndef AXIS3_DRIVER_DECAY
#define AXIS3_DRIVER_DECAY            OFF // OFF for default, TMC STEALTHCHOP
#endif
#ifndef AXIS3_DRIVER_DECAY_GOTO
#define AXIS3_DRIVER_DECAY_GOTO       OFF // OFF for default, TMC SPREADCYCLE
#endif
#ifndef AXIS3_DRIVER_MICROSTEPS
#define AXIS3_DRIVER_MICROSTEPS       OFF
#endif
#ifndef AXIS3_DRIVER_MICROSTEPS_GOTO
#define AXIS3_DRIVER_MICROSTEPS_GOTO  OFF
#endif
#ifndef AXIS3_DRIVER_IHOLD
#define AXIS3_DRIVER_IHOLD            HALF
#endif
#ifndef AXIS3_DRIVER_IRUN
#define AXIS3_DRIVER_IRUN             OFF
#endif
#ifndef AXIS3_DRIVER_IGOTO
#define AXIS3_DRIVER_IGOTO            SAME
#endif
#ifndef AXIS3_DRIVER_POWER_DOWN
#define AXIS3_DRIVER_POWER_DOWN       OFF
#endif
#ifndef AXIS3_DRIVER_REVERSE
#define AXIS3_DRIVER_REVERSE          OFF
#endif
#ifndef AXIS3_DRIVER_STATUS
#define AXIS3_DRIVER_STATUS           OFF
#endif
#ifndef AXIS3_LIMIT_MIN
#define AXIS3_LIMIT_MIN               -180
#endif
#ifndef AXIS3_LIMIT_MAX
#define AXIS3_LIMIT_MAX               180
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

// common axis settings, FOCUSER1
#ifndef AXIS4_DRIVER_MODEL
#define AXIS4_DRIVER_MODEL            OFF // axis 1/2 combination enables this feature
#endif
#ifndef AXIS4_DRIVER_DECAY
#define AXIS4_DRIVER_DECAY            OFF // OFF for default, TMC STEALTHCHOP
#endif
#ifndef AXIS4_DRIVER_DECAY_GOTO
#define AXIS4_DRIVER_DECAY_GOTO       OFF // OFF for default, TMC SPREADCYCLE
#endif
#ifndef AXIS4_DRIVER_MICROSTEPS
#define AXIS4_DRIVER_MICROSTEPS       OFF
#endif
#ifndef AXIS4_DRIVER_MICROSTEPS_GOTO
#define AXIS4_DRIVER_MICROSTEPS_GOTO  OFF
#endif
#ifndef AXIS4_DRIVER_IHOLD
#define AXIS4_DRIVER_IHOLD            HALF
#endif
#ifndef AXIS4_DRIVER_IRUN
#define AXIS4_DRIVER_IRUN             OFF
#endif
#ifndef AXIS4_DRIVER_IGOTO
#define AXIS4_DRIVER_IGOTO            SAME
#endif
#ifndef AXIS4_DRIVER_POWER_DOWN
#define AXIS4_DRIVER_POWER_DOWN       OFF
#endif
#ifndef AXIS4_DRIVER_REVERSE
#define AXIS4_DRIVER_REVERSE          OFF
#endif
#ifndef AXIS4_DRIVER_STATUS
#define AXIS4_DRIVER_STATUS           OFF
#endif
#ifndef AXIS4_LIMIT_MIN
#define AXIS4_LIMIT_MIN               0
#endif
#ifndef AXIS4_LIMIT_MAX
#define AXIS4_LIMIT_MAX               50
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

// common axis settings, FOCUSER2
#ifndef AXIS5_DRIVER_MODEL
#define AXIS5_DRIVER_MODEL            OFF // axis 1/2 combination enables this feature
#endif
#ifndef AXIS5_DRIVER_DECAY
#define AXIS5_DRIVER_DECAY            OFF // OFF for default, TMC STEALTHCHOP
#endif
#ifndef AXIS5_DRIVER_DECAY_GOTO
#define AXIS5_DRIVER_DECAY_GOTO       OFF // OFF for default, TMC SPREADCYCLE
#endif
#ifndef AXIS5_DRIVER_MICROSTEPS
#define AXIS5_DRIVER_MICROSTEPS       OFF
#endif
#ifndef AXIS5_DRIVER_MICROSTEPS_GOTO
#define AXIS5_DRIVER_MICROSTEPS_GOTO  OFF
#endif
#ifndef AXIS5_DRIVER_IHOLD
#define AXIS5_DRIVER_IHOLD            HALF
#endif
#ifndef AXIS5_DRIVER_IRUN
#define AXIS5_DRIVER_IRUN             OFF
#endif
#ifndef AXIS5_DRIVER_IGOTO
#define AXIS5_DRIVER_IGOTO            SAME
#endif
#ifndef AXIS5_DRIVER_POWER_DOWN
#define AXIS5_DRIVER_POWER_DOWN       OFF
#endif
#ifndef AXIS5_DRIVER_REVERSE
#define AXIS5_DRIVER_REVERSE          OFF
#endif
#ifndef AXIS5_DRIVER_STATUS
#define AXIS5_DRIVER_STATUS           OFF
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

// common axis settings, FOCUSER3
#ifndef AXIS6_DRIVER_MODEL
#define AXIS6_DRIVER_MODEL            OFF // axis 1/2 combination enables this feature
#endif
#ifndef AXIS6_DRIVER_DECAY
#define AXIS6_DRIVER_DECAY            OFF // OFF for default, TMC STEALTHCHOP
#endif
#ifndef AXIS6_DRIVER_DECAY_GOTO
#define AXIS6_DRIVER_DECAY_GOTO       OFF // OFF for default, TMC SPREADCYCLE
#endif
#ifndef AXIS6_DRIVER_MICROSTEPS
#define AXIS6_DRIVER_MICROSTEPS       OFF
#endif
#ifndef AXIS6_DRIVER_MICROSTEPS_GOTO
#define AXIS6_DRIVER_MICROSTEPS_GOTO  OFF
#endif
#ifndef AXIS6_DRIVER_IHOLD
#define AXIS6_DRIVER_IHOLD            HALF
#endif
#ifndef AXIS6_DRIVER_IRUN
#define AXIS6_DRIVER_IRUN             OFF
#endif
#ifndef AXIS6_DRIVER_IGOTO
#define AXIS6_DRIVER_IGOTO            SAME
#endif
#ifndef AXIS6_DRIVER_POWER_DOWN
#define AXIS6_DRIVER_POWER_DOWN       OFF
#endif
#ifndef AXIS6_DRIVER_REVERSE
#define AXIS6_DRIVER_REVERSE          OFF
#endif
#ifndef AXIS6_DRIVER_STATUS
#define AXIS6_DRIVER_STATUS           OFF
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

// common axis settings, FOCUSER4
#ifndef AXIS7_DRIVER_MODEL
#define AXIS7_DRIVER_MODEL            OFF // axis 1/2 combination enables this feature
#endif
#ifndef AXIS7_DRIVER_DECAY
#define AXIS7_DRIVER_DECAY            OFF // OFF for default, TMC STEALTHCHOP
#endif
#ifndef AXIS7_DRIVER_DECAY_GOTO
#define AXIS7_DRIVER_DECAY_GOTO       OFF // OFF for default, TMC SPREADCYCLE
#endif
#ifndef AXIS7_DRIVER_MICROSTEPS
#define AXIS7_DRIVER_MICROSTEPS       OFF
#endif
#ifndef AXIS7_DRIVER_MICROSTEPS_GOTO
#define AXIS7_DRIVER_MICROSTEPS_GOTO  OFF
#endif
#ifndef AXIS7_DRIVER_IHOLD
#define AXIS7_DRIVER_IHOLD            HALF
#endif
#ifndef AXIS7_DRIVER_IRUN
#define AXIS7_DRIVER_IRUN             OFF
#endif
#ifndef AXIS7_DRIVER_IGOTO
#define AXIS7_DRIVER_IGOTO            SAME
#endif
#ifndef AXIS7_DRIVER_POWER_DOWN
#define AXIS7_DRIVER_POWER_DOWN       OFF
#endif
#ifndef AXIS7_DRIVER_REVERSE
#define AXIS7_DRIVER_REVERSE          OFF
#endif
#ifndef AXIS7_DRIVER_STATUS
#define AXIS7_DRIVER_STATUS           OFF
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

// common axis settings, FOCUSER5
#ifndef AXIS8_DRIVER_MODEL
#define AXIS8_DRIVER_MODEL            OFF // axis 1/2 combination enables this feature
#endif
#ifndef AXIS8_DRIVER_DECAY
#define AXIS8_DRIVER_DECAY            OFF // OFF for default, TMC STEALTHCHOP
#endif
#ifndef AXIS8_DRIVER_DECAY_GOTO
#define AXIS8_DRIVER_DECAY_GOTO       OFF // OFF for default, TMC SPREADCYCLE
#endif
#ifndef AXIS8_DRIVER_MICROSTEPS
#define AXIS8_DRIVER_MICROSTEPS       OFF
#endif
#ifndef AXIS8_DRIVER_MICROSTEPS_GOTO
#define AXIS8_DRIVER_MICROSTEPS_GOTO  OFF
#endif
#ifndef AXIS8_DRIVER_IHOLD
#define AXIS8_DRIVER_IHOLD            HALF
#endif
#ifndef AXIS8_DRIVER_IRUN
#define AXIS8_DRIVER_IRUN             OFF
#endif
#ifndef AXIS8_DRIVER_IGOTO
#define AXIS8_DRIVER_IGOTO            SAME
#endif
#ifndef AXIS8_DRIVER_POWER_DOWN
#define AXIS8_DRIVER_POWER_DOWN       OFF
#endif
#ifndef AXIS8_DRIVER_REVERSE
#define AXIS8_DRIVER_REVERSE          OFF
#endif
#ifndef AXIS8_DRIVER_STATUS
#define AXIS8_DRIVER_STATUS           OFF
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

// common axis settings, FOCUSER6
#ifndef AXIS9_DRIVER_MODEL
#define AXIS9_DRIVER_MODEL            OFF // axis 1/2 combination enables this feature
#endif
#ifndef AXIS9_DRIVER_DECAY
#define AXIS9_DRIVER_DECAY            OFF // OFF for default, TMC STEALTHCHOP
#endif
#ifndef AXIS9_DRIVER_DECAY_GOTO
#define AXIS9_DRIVER_DECAY_GOTO       OFF // OFF for default, TMC SPREADCYCLE
#endif
#ifndef AXIS9_DRIVER_MICROSTEPS
#define AXIS9_DRIVER_MICROSTEPS       OFF
#endif
#ifndef AXIS9_DRIVER_MICROSTEPS_GOTO
#define AXIS9_DRIVER_MICROSTEPS_GOTO  OFF
#endif
#ifndef AXIS9_DRIVER_IHOLD
#define AXIS9_DRIVER_IHOLD            HALF
#endif
#ifndef AXIS9_DRIVER_IRUN
#define AXIS9_DRIVER_IRUN             OFF
#endif
#ifndef AXIS9_DRIVER_IGOTO
#define AXIS9_DRIVER_IGOTO            SAME
#endif
#ifndef AXIS9_DRIVER_POWER_DOWN
#define AXIS9_DRIVER_POWER_DOWN       OFF
#endif
#ifndef AXIS9_DRIVER_REVERSE
#define AXIS9_DRIVER_REVERSE          OFF
#endif
#ifndef AXIS9_DRIVER_STATUS
#define AXIS9_DRIVER_STATUS           OFF
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

// -----------------------------------------------------------------------------------
// auxiliary feature settings

#ifndef FEATURE1_PURPOSE
#define FEATURE1_PURPOSE              OFF
#endif
#ifndef FEATURE1_NAME
#define FEATURE1_NAME                "FEATURE1"
#endif
#ifndef FEATURE1_TEMP
#define FEATURE1_TEMP                 OFF
#endif
#ifndef FEATURE1_PIN
#define FEATURE1_PIN                  OFF
#endif
#ifndef FEATURE1_DEFAULT_VALUE
#define FEATURE1_DEFAULT_VALUE        OFF
#endif
#ifndef FEATURE1_ACTIVE_STATE
#define FEATURE1_ACTIVE_STATE         HIGH
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
#ifndef FEATURE2_DEFAULT_VALUE
#define FEATURE2_DEFAULT_VALUE        OFF
#endif
#ifndef FEATURE2_ACTIVE_STATE
#define FEATURE2_ACTIVE_STATE         HIGH
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
#ifndef FEATURE3_DEFAULT_VALUE
#define FEATURE3_DEFAULT_VALUE        OFF
#endif
#ifndef FEATURE3_ACTIVE_STATE
#define FEATURE3_ACTIVE_STATE         HIGH
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
#ifndef FEATURE4_DEFAULT_VALUE
#define FEATURE4_DEFAULT_VALUE        OFF
#endif
#ifndef FEATURE4_ACTIVE_STATE
#define FEATURE4_ACTIVE_STATE         HIGH
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
#ifndef FEATURE5_DEFAULT_VALUE
#define FEATURE5_DEFAULT_VALUE        OFF
#endif
#ifndef FEATURE5_ACTIVE_STATE
#define FEATURE5_ACTIVE_STATE         HIGH
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
#ifndef FEATURE6_DEFAULT_VALUE
#define FEATURE6_DEFAULT_VALUE        OFF
#endif
#ifndef FEATURE6_ACTIVE_STATE
#define FEATURE6_ACTIVE_STATE         HIGH
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
#ifndef FEATURE7_DEFAULT_VALUE
#define FEATURE7_DEFAULT_VALUE        OFF
#endif
#ifndef FEATURE7_ACTIVE_STATE
#define FEATURE7_ACTIVE_STATE         HIGH
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
#ifndef FEATURE8_DEFAULT_VALUE
#define FEATURE8_DEFAULT_VALUE        OFF
#endif
#ifndef FEATURE8_ACTIVE_STATE
#define FEATURE8_ACTIVE_STATE         HIGH
#endif
