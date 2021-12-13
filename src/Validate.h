// -------------------------------------------------------------------------------------------------
// Validate configuration
#pragma once
#include "Common.h"

#if FileVersionConfig != 5
  #error "Configuration (Config.h): FileVersionConfig (Config.h version) must be 5 for this OnStep."
#endif

// TELESCOPE -------------------------------------
#if PINMAP != OFF && (PINMAP < PINMAP_FIRST || PINMAP > PINMAP_LAST)
  #error "Configuration (Config.h): PINMAP must be set to a valid board (from Constants.h) or OFF (for user pin defs in Config.h)"
#endif

#if SERIAL_A_BAUD_DEFAULT != 9600 && SERIAL_A_BAUD_DEFAULT != 19200 && SERIAL_A_BAUD_DEFAULT != 38400 && SERIAL_A_BAUD_DEFAULT != 57600 && SERIAL_A_BAUD_DEFAULT != 115200
  #warning "Configuration (Config.h): Setting SERIAL_A_BAUD_DEFAULT unknown, use 9600, 19200, 38400, 57600 or 115200 (baud.)"
#endif

#if SERIAL_B_BAUD_DEFAULT != OFF && SERIAL_B_BAUD_DEFAULT != 9600 && SERIAL_B_BAUD_DEFAULT != 19200 && SERIAL_B_BAUD_DEFAULT != 38400 && SERIAL_B_BAUD_DEFAULT != 57600 && SERIAL_B_BAUD_DEFAULT != 115200
  #warning "Configuration (Config.h): Setting SERIAL_B_BAUD_DEFAULT unknown, use OFF or 9600, 19200, 38400, 57600 or 115200 (baud.)"
#endif

#if SERIAL_B_ESP_FLASHING != ON && SERIAL_B_ESP_FLASHING != OFF
  #error "Configuration (Config.h): Setting SERIAL_B_ESP_FLASHING unknown, use OFF or ON."
#endif

#if SERIAL_B_ESP_FLASHING == ON && SERIAL_B_BAUD_DEFAULT == OFF
  #error "Configuration (Config.h): Setting SERIAL_B_ESP_FLASHING only supported if SERIAL_B_BAUD_DEFAULT is defined."
#endif

#if SERIAL_B_ESP_FLASHING == ON && (ADDON_GPIO0_PIN == OFF || ADDON_RESET_PIN == OFF)
  #error "Configuration (Config.h): Setting SERIAL_B_ESP_FLASHING only supported if ADDON_GPIO0_PIN and ADDON_RESET_PIN are defined."
#endif

#if SERIAL_C_BAUD_DEFAULT != OFF && SERIAL_C_BAUD_DEFAULT != 9600 && SERIAL_C_BAUD_DEFAULT != 19200 && SERIAL_C_BAUD_DEFAULT != 38400 && SERIAL_C_BAUD_DEFAULT != 57600 && SERIAL_C_BAUD_DEFAULT != 115200
  #warning "Configuration (Config.h): Setting SERIAL_C_BAUD_DEFAULT unknown, use OFF or 9600, 19200, 38400, 57600 or 115200 (baud.)"
#endif

#if SERIAL_D_BAUD_DEFAULT != OFF && SERIAL_D_BAUD_DEFAULT != 9600 && SERIAL_D_BAUD_DEFAULT != 19200 && SERIAL_D_BAUD_DEFAULT != 38400 && SERIAL_D_BAUD_DEFAULT != 57600 && SERIAL_D_BAUD_DEFAULT != 115200
  #warning "Configuration (Config.h): Setting SERIAL_D_BAUD_DEFAULT unknown, use OFF or 9600, 19200, 38400, 57600 or 115200 (baud.)"
#endif

#if STATUS_LED != OFF && STATUS_LED != ON
  #error "Configuration (Config.h): Setting STATUS_LED unknown, use OFF or ON."
#endif

#if RETICLE_LED != OFF && (RETICLE_LED < 0 || RETICLE_LED > 255)
  #error "Configuration (Config.h): Setting RETICLE_LED unknown, use OFF or 0 to 255."
#endif

#if WEATHER != OFF && (WEATHER < WEATHER_FIRST || WEATHER > WEATHER_LAST)
  #error "Configuration (Config.h): Setting WEATHER unknown, use OFF or valid WEATHER SENSOR (from Constants.h)"
#endif

#if STEP_WAVE_FORM < STEP_WAVE_FORM_FIRST || STEP_WAVE_FORM > STEP_WAVE_FORM_LAST
  #error "Configuration (Config.h): Setting STEP_WAVE_FORM unknown, use a valid STEP WAVE FORM (from Constants.h)"
#endif

#if STEP_WAVE_FORM != SQUARE && (defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41))
  #error "Configuration (Config.h): Setting STEP_WAVE_FORM SQUARE is required for the Teensy4.0 and 4.1"
#endif

// MOUNT -----------------------------------------

#if (AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL == OFF) || \
    (AXIS1_DRIVER_MODEL == OFF && AXIS2_DRIVER_MODEL != OFF)
  #error "Configuration (Config.h): Settings AXIS1_DRIVER_MODEL and AXIS2_DRIVER_MODEL must both be OFF or set to a valid DRIVER (from Constants.h)"
#endif

// AXIS1 RA/AZM
#if AXIS1_DRIVER_MODEL != OFF && \
    (AXIS1_DRIVER_MODEL < DRIVER_FIRST || AXIS1_DRIVER_MODEL > DRIVER_LAST) && \
    (AXIS1_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS1_DRIVER_MODEL > SERVO_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS1_DRIVER_MODEL unknown, use OFF or a valid DRIVER (from Constants.h)"
#endif

#if AXIS1_DRIVER_STATUS != OFF && AXIS1_DRIVER_STATUS != ON && AXIS1_DRIVER_STATUS != HIGH && AXIS1_DRIVER_STATUS != LOW
  #error "Configuration (Config.h): Setting AXIS1_DRIVER_STATUS unknown, use OFF or a valid driver status."
#endif

#ifdef AXIS1_DRIVER_PRESENT
  #if AXIS1_DRIVER_MICROSTEPS != OFF && (AXIS1_DRIVER_MICROSTEPS < 1 || AXIS1_DRIVER_MICROSTEPS > 256)
    #error "Configuration (Config.h): Setting AXIS1_DRIVER_MICROSTEPS unknown, use OFF or a valid microstep setting (range 1 to 256x and supported by your driver/design.)"
    #if AXIS1_DRIVER_MICROSTEPS != 1 && AXIS1_DRIVER_MICROSTEPS != 2 && AXIS1_DRIVER_MICROSTEPS != 4 && \
        AXIS1_DRIVER_MICROSTEPS != 8 && AXIS1_DRIVER_MICROSTEPS != 16 && AXIS1_DRIVER_MICROSTEPS != 32 && \
        AXIS1_DRIVER_MICROSTEPS != 64 && AXIS1_DRIVER_MICROSTEPS != 128 && AXIS1_DRIVER_MICROSTEPS != 256
      #warning "Configuration (Config.h): Setting AXIS1_DRIVER_MICROSTEPS has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS1_DRIVER_MICROSTEPS_GOTO != OFF && (AXIS1_DRIVER_MICROSTEPS_GOTO < 1 || AXIS1_DRIVER_MICROSTEPS_GOTO >= AXIS1_DRIVER_MICROSTEPS)
    #error "Configuration (Config.h): Setting AXIS1_DRIVER_MICROSTEPS_GOTO unknown, use a valid microstep setting (range 1 to < AXIS1_DRIVER_MICROSTEPS and supported by your driver/design.)"
    #if AXIS1_DRIVER_MICROSTEPS_GOTO != 1 && AXIS1_DRIVER_MICROSTEPS_GOTO != 2 && AXIS1_DRIVER_MICROSTEPS_GOTO != 4 && \
        AXIS1_DRIVER_MICROSTEPS_GOTO != 8 && AXIS1_DRIVER_MICROSTEPS_GOTO != 16 && AXIS1_DRIVER_MICROSTEPS_GOTO != 32 && \
        AXIS1_DRIVER_MICROSTEPS_GOTO != 64 && AXIS1_DRIVER_MICROSTEPS_GOTO != 128 && AXIS1_DRIVER_MICROSTEPS_GOTO != 256
      #warning "Configuration (Config.h): Setting AXIS1_DRIVER_MICROSTEPS_GOTO has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS1_DRIVER_MICROSTEPS == OFF && AXIS1_DRIVER_MICROSTEPS_GOTO != OFF
    #error "Configuration (Config.h): Setting AXIS1_DRIVER_MICROSTEPS_GOTO can't be enabled without first enabling AXIS1_DRIVER_MICROSTEPS."
  #endif
  #if AXIS1_DRIVER_DECAY != OFF && (AXIS1_DRIVER_DECAY < DRIVER_DECAY_MODE_FIRST || AXIS1_DRIVER_DECAY > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS1_DRIVER_DECAY unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS1_DRIVER_DECAY_GOTO != OFF && (AXIS1_DRIVER_DECAY_GOTO < DRIVER_DECAY_MODE_FIRST || AXIS1_DRIVER_DECAY_GOTO > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS1_DRIVER_DECAY_GOTO unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS1_DRIVER_IRUN != OFF && (AXIS1_DRIVER_IRUN < 0 || AXIS1_DRIVER_IRUN > 3000)
    #error "Configuration (Config.h): Setting AXIS1_DRIVER_IRUN unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS1_DRIVER_IHOLD != HALF && (AXIS1_DRIVER_IHOLD < 0 || AXIS1_DRIVER_IHOLD > 3000)
    #error "Configuration (Config.h): Setting AXIS1_DRIVER_IHOLD unknown, use HALF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS1_DRIVER_IGOTO != SAME && (AXIS1_DRIVER_IGOTO < 0 || AXIS1_DRIVER_IGOTO > 3000)
    #error "Configuration (Config.h): Setting AXIS1_DRIVER_IGOTO unknown, use SAME or a value 0 to 3000 (mA.)"
  #endif
#endif

#ifdef AXIS1_SERVO_PRESENT
  #if AXIS1_SERVO_ENCODER < SERVO_ENCODER_FIRST || AXIS1_SERVO_ENCODER > SERVO_ENCODER_LAST
    #error "Configuration (Config.h): Setting AXIS1_SERVO_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
  #endif
#endif

#if AXIS1_REVERSE != ON && AXIS1_REVERSE != OFF
  #error "Configuration (Config.h): Setting AXIS1_REVERSE unknown, use OFF or ON."
#endif

#if AXIS1_POWER_DOWN != ON && AXIS1_POWER_DOWN != OFF
  #error "Configuration (Config.h): Setting AXIS1_POWER_DOWN unknown, use OFF or ON."
#endif

#if AXIS1_WRAP != ON && AXIS1_WRAP != OFF
  #error "Configuration (Config.h): Setting AXIS1_WRAP unknown, use OFF or ON."
#endif

#if AXIS1_LIMIT_MIN < -360 || AXIS1_LIMIT_MIN > -90
  #error "Configuration (Config.h): Setting AXIS1_LIMIT_MIN unknown, use value in the range -90 to -360."
#endif

#if AXIS1_LIMIT_MAX < 90 || AXIS1_LIMIT_MAX > 360
  #error "Configuration (Config.h): Setting AXIS1_LIMIT_MAX unknown, use value in the range 90 to 360."
#endif

#if AXIS1_SENSE_HOME != OFF && AXIS1_SENSE_HOME < 0
  #error "Configuration (Config.h): Setting AXIS1_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS1_SENSE_LIMIT_MIN != OFF && AXIS1_SENSE_LIMIT_MIN < 0
  #error "Configuration (Config.h): Setting AXIS1_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS1_SENSE_LIMIT_MAX != OFF && AXIS1_SENSE_LIMIT_MAX < 0
  #error "Configuration (Config.h): Setting AXIS1_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

// AXIS2 DEC/ALT
#if AXIS2_DRIVER_MODEL != OFF && \
    (AXIS2_DRIVER_MODEL < DRIVER_FIRST || AXIS2_DRIVER_MODEL > DRIVER_LAST) && \
    (AXIS2_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS2_DRIVER_MODEL > SERVO_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS2_DRIVER_MODEL unknown, use a valid DRIVER (from Constants.h)"
#endif

#if AXIS2_DRIVER_STATUS != OFF && AXIS2_DRIVER_STATUS != ON && AXIS2_DRIVER_STATUS != HIGH && AXIS2_DRIVER_STATUS != LOW
  #error "Configuration (Config.h): Setting AXIS2_DRIVER_STATUS unknown, use OFF or a valid driver status."
#endif

#ifdef AXIS2_DRIVER_PRESENT
  #if AXIS2_DRIVER_MICROSTEPS != OFF && (AXIS2_DRIVER_MICROSTEPS < 1 || AXIS2_DRIVER_MICROSTEPS > 256)
    #error "Configuration (Config.h): Setting AXIS2_DRIVER_MICROSTEPS unknown, use OFF or a valid microstep setting (range 1 to 256x and supported by your driver/design.)"
    #if AXIS2_DRIVER_MICROSTEPS != 1 && AXIS2_DRIVER_MICROSTEPS != 2 && AXIS2_DRIVER_MICROSTEPS != 4 && \
        AXIS2_DRIVER_MICROSTEPS != 8 && AXIS2_DRIVER_MICROSTEPS != 16 && AXIS2_DRIVER_MICROSTEPS != 32 && \
        AXIS2_DRIVER_MICROSTEPS != 64 && AXIS2_DRIVER_MICROSTEPS != 128 && AXIS2_DRIVER_MICROSTEPS != 256
      #warning "Configuration (Config.h): Setting AXIS2_DRIVER_MICROSTEPS has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS2_DRIVER_MICROSTEPS_GOTO != OFF && (AXIS2_DRIVER_MICROSTEPS_GOTO < 1 || AXIS2_DRIVER_MICROSTEPS_GOTO >= AXIS2_DRIVER_MICROSTEPS)
    #error "Configuration (Config.h): Setting AXIS2_DRIVER_MICROSTEPS_GOTO unknown, use OFF or a valid microstep setting (range 1 to < AXIS2_DRIVER_MICROSTEPS and supported by your driver/design.)"
    #if AXIS2_DRIVER_MICROSTEPS_GOTO != 1 && AXIS2_DRIVER_MICROSTEPS_GOTO != 2 && AXIS2_DRIVER_MICROSTEPS_GOTO != 4 && \
        AXIS2_DRIVER_MICROSTEPS_GOTO != 8 && AXIS2_DRIVER_MICROSTEPS_GOTO != 16 && AXIS2_DRIVER_MICROSTEPS_GOTO != 32 && \
        AXIS2_DRIVER_MICROSTEPS_GOTO != 64 && AXIS2_DRIVER_MICROSTEPS_GOTO != 128 && AXIS2_DRIVER_MICROSTEPS_GOTO != 256
      #warning "Configuration (Config.h): Setting AXIS2_DRIVER_MICROSTEPS_GOTO has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS2_DRIVER_MICROSTEPS == OFF && AXIS2_DRIVER_MICROSTEPS_GOTO != OFF
    #error "Configuration (Config.h): Setting AXIS2_DRIVER_MICROSTEPS_GOTO can't be enabled without first enabling AXIS2_DRIVER_MICROSTEPS."
  #endif
  #if AXIS2_DRIVER_DECAY != OFF && (AXIS2_DRIVER_DECAY < DRIVER_DECAY_MODE_FIRST || AXIS2_DRIVER_DECAY > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS2_DRIVER_DECAY unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS2_DRIVER_DECAY_GOTO != OFF && (AXIS2_DRIVER_DECAY_GOTO < DRIVER_DECAY_MODE_FIRST || AXIS2_DRIVER_DECAY_GOTO > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS2_DRIVER_DECAY_GOTO unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS2_DRIVER_IRUN != OFF && (AXIS2_DRIVER_IRUN < 0 || AXIS2_DRIVER_IRUN > 3000)
    #error "Configuration (Config.h): Setting AXIS2_DRIVER_IRUN unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS2_DRIVER_IHOLD != HALF && (AXIS2_DRIVER_IHOLD < 0 || AXIS2_DRIVER_IHOLD > 3000)
    #error "Configuration (Config.h): Setting AXIS2_DRIVER_IHOLD unknown, use HALF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS2_DRIVER_IGOTO != SAME && (AXIS2_DRIVER_IGOTO < 0 || AXIS2_DRIVER_IGOTO > 3000)
    #error "Configuration (Config.h): Setting AXIS2_DRIVER_IGOTO unknown, use SAME or a value 0 to 3000 (mA.)"
  #endif
#endif

#ifdef AXIS2_SERVO_PRESENT
  #if AXIS2_SERVO_ENCODER < SERVO_ENCODER_FIRST || AXIS2_SERVO_ENCODER > SERVO_ENCODER_LAST
    #error "Configuration (Config.h): Setting AXIS2_SERVO_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
  #endif
#endif

#if AXIS2_REVERSE != ON && AXIS2_REVERSE != OFF
  #error "Configuration (Config.h): Setting AXIS2_REVERSE unknown, use OFF or ON."
#endif

#if AXIS2_POWER_DOWN != ON && AXIS2_POWER_DOWN != OFF
  #error "Configuration (Config.h): Setting AXIS2_POWER_DOWN unknown, use OFF or ON."
#endif

#if AXIS2_LIMIT_MIN < -360 || AXIS2_LIMIT_MIN > -90
  #error "Configuration (Config.h): Setting AXIS2_LIMIT_MIN unknown, use value in the range -90 to -360."
#endif

#if AXIS2_LIMIT_MAX < 90 || AXIS2_LIMIT_MAX > 360
  #error "Configuration (Config.h): Setting AXIS2_LIMIT_MAX unknown, use value in the range 90 to 360."
#endif

#if AXIS2_SENSE_HOME != OFF && AXIS2_SENSE_HOME < 0
  #error "Configuration (Config.h): Setting AXIS2_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS2_SENSE_LIMIT_MIN != OFF && AXIS2_SENSE_LIMIT_MIN < 0
  #error "Configuration (Config.h): Setting AXIS2_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS2_SENSE_LIMIT_MAX != OFF && AXIS2_SENSE_LIMIT_MAX < 0
  #error "Configuration (Config.h): Setting AXIS2_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS2_TANGENT_ARM != ON && AXIS2_TANGENT_ARM != OFF
  #error "Configuration (Config.h): Setting AXIS2_TANGENT_ARM unknown, use OFF or ON."
#endif

#if AXIS2_TANGENT_ARM_CORRECTION != ON && AXIS2_TANGENT_ARM_CORRECTION != OFF
  #error "Configuration (Config.h): Setting AXIS2_TANGENT_ARM_CORRECTION unknown, use OFF or ON."
#endif

#if AXIS2_TANGENT_ARM != OFF && MOUNT_TYPE == ALTAZM
  #error "Configuration (Config.h): Setting AXIS2_TANGENT_ARM is not supported for default MOUNT_TYPE ALTAZM"
#endif

// MOUNT TYPE
#if MOUNT_TYPE < MOUNT_TYPE_FIRST && MOUNT_TYPE > MOUNT_TYPE_LAST
  #error "Configuration (Config.h): Setting MOUNT_TYPE unknown, use a valid MOUNT TYPE (from Constants.h)"
#endif

#if MOUNT_COORDS < MOUNT_COORDS_FIRST && MOUNT_COORDS > MOUNT_COORDS_LAST
  #error "Configuration (Config.h): Setting MOUNT_COORDS unknown, use a valid MOUNT COORDS (from Constants.h)"
#endif

#if ALIGN_MAX_STARS != AUTO && (ALIGN_MAX_STARS < 1 && ALIGN_MAX_STARS > 9)
  #error "Configuration (Config.h): Setting ALIGN_MAX_STARS unknown, use AUTO or a value from 1 to 9."
#endif

// TIME AND LOCATION
#if TIME_LOCATION_SOURCE < TLS_FIRST && TIME_LOCATION_SOURCE > TLS_LAST
  #error "Configuration (Config.h): Setting TIME_LOCATION_SOURCE unknown, use OFF or valid TIME LOCATION SOURCE (from Constants.h)"
#endif

#if TIME_LOCATION_PPS_SENSE != OFF && \
    TIME_LOCATION_PPS_SENSE != LOW && \
    TIME_LOCATION_PPS_SENSE != HIGH && \
    TIME_LOCATION_PPS_SENSE != BOTH
  #error "Configuration (Config.h): Setting TIME_LOCATION_PPS_SENSE unknown, use OFF or LOW or HIGH or BOTH."
#endif

// USER FEEDBACK
#if STATUS_MOUNT_LED != ON && STATUS_MOUNT_LED != OFF
  #error "Configuration (Config.h): Setting STATUS_MOUNT_LED unknown, use OFF or ON."
#endif

#if STATUS_BUZZER != ON && STATUS_BUZZER != OFF && (STATUS_BUZZER < 100 || STATUS_BUZZER > 6000)
  #error "Configuration (Config.h): Setting STATUS_BUZZER unknown, use OFF or ON or a value from 100 to 6000 (Hz.)"
#endif

#if STATUS_BUZZER_DEFAULT != ON && STATUS_BUZZER_DEFAULT != OFF
  #error "Configuration (Config.h): Setting STATUS_BUZZER_DEFAULT unknown, use OFF or ON."
#endif

#if STATUS_BUZZER_MEMORY != ON && STATUS_BUZZER_MEMORY != OFF
  #error "Configuration (Config.h): Setting STATUS_BUZZER_MEMORY unknown, use OFF or ON."
#endif

// ST4 INTERFACE
#if ST4_INTERFACE != ON && ST4_INTERFACE != OFF
  #error "Configuration (Config.h): Setting ST4_INTERFACE unknown, use OFF or ON."
#endif

#if ST4_HAND_CONTROL != ON && ST4_HAND_CONTROL != OFF
  #error "Configuration (Config.h): Setting ST4_HAND_CONTROL unknown, use OFF or ON."
#endif

#if ST4_HAND_CONTROL_FOCUSER != ON && ST4_HAND_CONTROL_FOCUSER != OFF
  #error "Configuration (Config.h): Setting ST4_HAND_CONTROL_FOCUSER unknown, use OFF or ON."
#endif

// GUIDING
#if GUIDE_TIME_LIMIT < 0 || GUIDE_TIME_LIMIT > 120
  #error "Configuration (Config.h): Setting GUIDE_TIME_LIMIT unknown, use the value 0 to disable or 1 to 120 (seconds.)"
#endif

#if GUIDE_DISABLE_BACKLASH != ON && GUIDE_DISABLE_BACKLASH != OFF
  #error "Configuration (Config.h): Setting GUIDE_DISABLE_BACKLASH unknown, use OFF or ON."
#endif

#if SEPARATE_PULSE_GUIDE_RATE != ON && SEPARATE_PULSE_GUIDE_RATE != OFF
  #error "Configuration (Config.h): Setting SEPARATE_PULSE_GUIDE_RATE unknown, use OFF or ON."
#endif

// SENSORS
#if LIMIT_SENSE != OFF && LIMIT_SENSE < 0
  #error "Configuration (Config.h): Setting LIMIT_SENSE unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

// PEC (max steps per degree 360000 * 360 degrees)
#if PEC_STEPS_PER_WORM_ROTATION < 0 || PEC_STEPS_PER_WORM_ROTATION > 129600000
  #error "Configuration (Config.h): Setting PEC_STEPS_PER_WORM_ROTATION unknown, use the value 0 to disable or 1 to 129600000 (steps.)"
#endif

#if PEC_SENSE != OFF && PEC_SENSE < 0
  #error "Configuration (Config.h): Setting PEC_SENSE unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if PEC_BUFFER_SIZE_LIMIT < 0 || PEC_BUFFER_SIZE_LIMIT > 86400
  #error "Configuration (Config.h): Setting PEC_BUFFER_SIZE_LIMIT unknown, use the value 0 to disable or 1 to 86400 (seconds.)"
#endif

// SLEWING BEHAVIOUR
#if SLEW_GOTO != ON && SLEW_GOTO != OFF
  #error "Configuration (Config.h): Setting SLEW_GOTO unknown, use OFF or ON."
#endif

#if SLEW_RATE_MEMORY != ON && SLEW_RATE_MEMORY != OFF
  #error "Configuration (Config.h): Setting SLEW_RATE_MEMORY unknown, use OFF or ON."
#endif

// TRACKING BEHAVIOUR
#if TRACK_AUTOSTART != ON && TRACK_AUTOSTART != OFF
  #error "Configuration (Config.h): Setting TRACK_AUTOSTART unknown, use OFF or ON."
#endif

#if TRACK_REFRACTION_RATE_DEFAULT != ON && TRACK_REFRACTION_RATE_DEFAULT != OFF
  #error "Configuration (Config.h): Setting TRACK_REFRACTION_RATE_DEFAULT unknown, use OFF or ON."
#endif

#if TRACK_BACKLASH_RATE < 2 && TRACK_BACKLASH_RATE > 100
  #error "Configuration (Config.h): Setting TRACK_BACKLASH_RATE unknown, use a value between 2 and 100 (x Sidereal.)"
#endif

// PIER SIDE BEHAVIOUR
#if MFLIP_SKIP_HOME != ON && MFLIP_SKIP_HOME != OFF
  #error "Configuration (Config.h): Setting MFLIP_SKIP_HOME unknown, use OFF or ON."
#endif

#if MFLIP_PAUSE_HOME_MEMORY != ON && MFLIP_PAUSE_HOME_MEMORY != OFF
  #error "Configuration (Config.h): Setting MFLIP_PAUSE_HOME_MEMORY unknown, use OFF or ON."
#endif

#if MFLIP_AUTOMATIC_MEMORY != ON && MFLIP_AUTOMATIC_MEMORY != OFF
  #error "Configuration (Config.h): Setting MFLIP_AUTOMATIC_MEMORY unknown, use OFF or ON."
#endif

#if PIER_SIDE_SYNC_CHANGE_SIDES != ON && PIER_SIDE_SYNC_CHANGE_SIDES != OFF
  #error "Configuration (Config.h): Setting PIER_SIDE_SYNC_CHANGE_SIDES unknown, use OFF or ON."
#endif

#if PIER_SIDE_PREFERRED_DEFAULT < PIER_SIDE_FIRST && PIER_SIDE_SYNC_CHANGE_SIDES > PIER_SIDE_LAST
  #error "Configuration (Config.h): Setting PIER_SIDE_SYNC_CHANGE_SIDES unknown, use EAST or WEST or BEST."
#endif

// PARKING BEHAVIOUR
#if STRICT_PARKING != ON && STRICT_PARKING != OFF
  #error "Configuration (Config.h): Setting STRICT_PARKING unknown, use OFF or ON."
#endif

// ROTATOR ---------------------------------------

// AXIS3 ROTATOR
#if AXIS3_DRIVER_MODEL != OFF && \
    (AXIS3_DRIVER_MODEL < DRIVER_FIRST || AXIS3_DRIVER_MODEL > DRIVER_LAST) && \
    (AXIS3_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS3_DRIVER_MODEL > SERVO_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS3_DRIVER_MODEL unknown, use OFF or a valid DRIVER (from Constants.h)"
#endif

#if AXIS3_DRIVER_STATUS != OFF && AXIS3_DRIVER_STATUS != ON && AXIS3_DRIVER_STATUS != HIGH && AXIS3_DRIVER_STATUS != LOW
  #error "Configuration (Config.h): Setting AXIS3_DRIVER_STATUS unknown, use OFF or valid driver status."
#endif

#ifdef AXIS3_DRIVER_PRESENT
  #if AXIS3_DRIVER_MICROSTEPS != OFF && (AXIS3_DRIVER_MICROSTEPS < 1 || AXIS3_DRIVER_MICROSTEPS > 256)
    #error "Configuration (Config.h): Setting AXIS3_DRIVER_MICROSTEPS unknown, use OFF or a valid microstep setting (range 1 to 256x and supported by your driver/design.)"
    #if AXIS3_DRIVER_MICROSTEPS != 1 && AXIS3_DRIVER_MICROSTEPS != 2 && AXIS3_DRIVER_MICROSTEPS != 4 && \
        AXIS3_DRIVER_MICROSTEPS != 8 && AXIS3_DRIVER_MICROSTEPS != 16 && AXIS3_DRIVER_MICROSTEPS != 32 && \
        AXIS3_DRIVER_MICROSTEPS != 64 && AXIS3_DRIVER_MICROSTEPS != 128 && AXIS3_DRIVER_MICROSTEPS != 256
      #warning "Configuration (Config.h): Setting AXIS3_DRIVER_MICROSTEPS has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS3_DRIVER_MICROSTEPS_GOTO != OFF && (AXIS3_DRIVER_MICROSTEPS_GOTO < 1 || AXIS3_DRIVER_MICROSTEPS_GOTO >= AXIS3_DRIVER_MICROSTEPS)
    #error "Configuration (Config.h): Setting AXIS3_DRIVER_MICROSTEPS_GOTO unknown, use OFF or a valid microstep setting (range 1 to < AXIS3_DRIVER_MICROSTEPS and supported by your driver/design.)"
    #if AXIS3_DRIVER_MICROSTEPS_GOTO != 1 && AXIS3_DRIVER_MICROSTEPS_GOTO != 2 && AXIS3_DRIVER_MICROSTEPS_GOTO != 4 && \
        AXIS3_DRIVER_MICROSTEPS_GOTO != 8 && AXIS3_DRIVER_MICROSTEPS_GOTO != 16 && AXIS3_DRIVER_MICROSTEPS_GOTO != 32 && \
        AXIS3_DRIVER_MICROSTEPS_GOTO != 64 && AXIS3_DRIVER_MICROSTEPS_GOTO != 128 && AXIS3_DRIVER_MICROSTEPS_GOTO != 256
      #warning "Configuration (Config.h): Setting AXIS3_DRIVER_MICROSTEPS_GOTO has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS3_DRIVER_MICROSTEPS == OFF && AXIS3_DRIVER_MICROSTEPS_GOTO != OFF
    #error "Configuration (Config.h): Setting AXIS3_DRIVER_MICROSTEPS_GOTO can't be enabled without first enabling AXIS3_DRIVER_MICROSTEPS."
  #endif
  #if AXIS3_DRIVER_DECAY != OFF && (AXIS3_DRIVER_DECAY < DRIVER_DECAY_MODE_FIRST || AXIS3_DRIVER_DECAY > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS3_DRIVER_DECAY unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS3_DRIVER_DECAY_GOTO != OFF && (AXIS3_DRIVER_DECAY_GOTO < DRIVER_DECAY_MODE_FIRST || AXIS3_DRIVER_DECAY_GOTO > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS3_DRIVER_DECAY_GOTO unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS3_DRIVER_IRUN != OFF && (AXIS3_DRIVER_IRUN < 0 || AXIS3_DRIVER_IRUN > 1000)
    #error "Configuration (Config.h): Setting AXIS3_DRIVER_IRUN unknown, use OFF or a value 0 to 1000 (mA.)"
  #endif
  #if AXIS3_DRIVER_IHOLD != HALF && (AXIS3_DRIVER_IHOLD < 0 || AXIS3_DRIVER_IHOLD > 1000)
    #error "Configuration (Config.h): Setting AXIS3_DRIVER_IHOLD unknown, use HALF or a value 0 to 1000 (mA.)"
  #endif
  #if AXIS3_DRIVER_IGOTO != SAME && (AXIS3_DRIVER_IGOTO < 0 || AXIS3_DRIVER_IGOTO > 1000)
    #error "Configuration (Config.h): Setting AXIS3_DRIVER_IGOTO unknown, use SAME or a value 0 to 1000 (mA.)"
  #endif
#endif

#ifdef AXIS3_SERVO_PRESENT
  #if AXIS3_SERVO_ENCODER != SAME && (AXIS3_SERVO_ENCODER < SERVO_ENCODER_FIRST || AXIS3_SERVO_ENCODER > SERVO_ENCODER_LAST)
    #error "Configuration (Config.h): Setting AXIS3_SERVO_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
  #endif
#endif

#if AXIS3_REVERSE != ON && AXIS3_REVERSE != OFF
  #error "Configuration (Config.h): Setting AXIS3_REVERSE unknown, use OFF or ON."
#endif

#if AXIS3_POWER_DOWN != ON && AXIS3_POWER_DOWN != OFF
  #error "Configuration (Config.h): Setting AXIS3_POWER_DOWN unknown, use OFF or ON."
#endif

#if AXIS3_LIMIT_MIN < -360 || AXIS3_LIMIT_MIN > 0
  #error "Configuration (Config.h): Setting AXIS3_LIMIT_MIN unknown, use value in the range 0 to -360."
#endif

#if AXIS3_LIMIT_MAX < 0 || AXIS3_LIMIT_MAX > 360
  #error "Configuration (Config.h): Setting AXIS3_LIMIT_MAX unknown, use value in the range 0 to 360."
#endif

#if AXIS3_SENSE_HOME != OFF && AXIS3_SENSE_HOME < 0
  #error "Configuration (Config.h): Setting AXIS3_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS3_SENSE_LIMIT_MIN != OFF && AXIS3_SENSE_LIMIT_MIN < 0
  #error "Configuration (Config.h): Setting AXIS3_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS3_SENSE_LIMIT_MAX != OFF && AXIS3_SENSE_LIMIT_MAX < 0
  #error "Configuration (Config.h): Setting AXIS3_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

// USER FEEDBACK
#if STATUS_ROTATOR_LED != ON && STATUS_ROTATOR_LED != OFF
  #error "Configuration (Config.h): Setting STATUS_ROTATOR_LED unknown, use OFF or ON."
#endif

// FOCUSER ---------------------------------------

// AXIS4 FOCUSER
#if AXIS4_DRIVER_MODEL != OFF && \
    (AXIS4_DRIVER_MODEL < DRIVER_FIRST || AXIS4_DRIVER_MODEL > DRIVER_LAST) && \
    (AXIS4_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS4_DRIVER_MODEL > SERVO_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS4_DRIVER_MODEL unknown, use OFF or a valid DRIVER (from Constants.h)"
#endif

#if AXIS4_DRIVER_STATUS != OFF && AXIS4_DRIVER_STATUS != ON && AXIS4_DRIVER_STATUS != HIGH && AXIS4_DRIVER_STATUS != LOW
  #error "Configuration (Config.h): Setting AXIS4_DRIVER_STATUS unknown, use OFF or valid driver status."
#endif

#if AXIS4_REVERSE != ON && AXIS4_REVERSE != OFF
  #error "Configuration (Config.h): Setting AXIS4_REVERSE unknown, use OFF or ON."
#endif

#if AXIS4_POWER_DOWN != ON && AXIS4_POWER_DOWN != OFF
  #error "Configuration (Config.h): Setting AXIS4_POWER_DOWN unknown, use OFF or ON."
#endif

#if AXIS4_LIMIT_MIN < 0 || AXIS4_LIMIT_MIN > 500
  #error "Configuration (Config.h): Setting AXIS4_LIMIT_MIN unknown, use value in the range 0 to 500 (mm.)"
#endif

#if AXIS4_LIMIT_MAX < AXIS4_LIMIT_MIN || AXIS4_LIMIT_MAX > 500
  #error "Configuration (Config.h): Setting AXIS4_LIMIT_MAX unknown, use value in the range AXIS4_LIMIT_MIN to 500 (mm.)"
#endif

#if AXIS4_SENSE_HOME != OFF && AXIS4_SENSE_HOME < 0
  #error "Configuration (Config.h): Setting AXIS4_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS4_SENSE_LIMIT_MIN != OFF && AXIS4_SENSE_LIMIT_MIN < 0
  #error "Configuration (Config.h): Setting AXIS4_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS4_SENSE_LIMIT_MAX != OFF && AXIS4_SENSE_LIMIT_MAX < 0
  #error "Configuration (Config.h): Setting AXIS4_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#ifdef AXIS4_DRIVER_PRESENT
  #if AXIS4_DRIVER_MICROSTEPS != OFF && (AXIS4_DRIVER_MICROSTEPS < 1 || AXIS4_DRIVER_MICROSTEPS > 256)
    #error "Configuration (Config.h): Setting AXIS4_DRIVER_MICROSTEPS unknown, use OFF or a valid microstep setting (range 1 to 256x and supported by your driver/design.)"
    #if AXIS4_DRIVER_MICROSTEPS != 1 && AXIS4_DRIVER_MICROSTEPS != 2 && AXIS4_DRIVER_MICROSTEPS != 4 && \
        AXIS4_DRIVER_MICROSTEPS != 8 && AXIS4_DRIVER_MICROSTEPS != 16 && AXIS4_DRIVER_MICROSTEPS != 32 && \
        AXIS4_DRIVER_MICROSTEPS != 64 && AXIS4_DRIVER_MICROSTEPS != 128 && AXIS4_DRIVER_MICROSTEPS != 256
      #warning "Configuration (Config.h): Setting AXIS4_DRIVER_MICROSTEPS has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS4_DRIVER_MICROSTEPS_GOTO != OFF && (AXIS4_DRIVER_MICROSTEPS_GOTO < 1 || AXIS4_DRIVER_MICROSTEPS_GOTO >= AXIS4_DRIVER_MICROSTEPS)
    #error "Configuration (Config.h): Setting AXIS4_DRIVER_MICROSTEPS_GOTO unknown, use OFF or a valid microstep setting (range 1 to < AXIS4_DRIVER_MICROSTEPS and supported by your driver/design.)"
    #if AXIS4_DRIVER_MICROSTEPS_GOTO != 1 && AXIS4_DRIVER_MICROSTEPS_GOTO != 2 && AXIS4_DRIVER_MICROSTEPS_GOTO != 4 && \
        AXIS4_DRIVER_MICROSTEPS_GOTO != 8 && AXIS4_DRIVER_MICROSTEPS_GOTO != 16 && AXIS4_DRIVER_MICROSTEPS_GOTO != 32 && \
        AXIS4_DRIVER_MICROSTEPS_GOTO != 64 && AXIS4_DRIVER_MICROSTEPS_GOTO != 128 && AXIS4_DRIVER_MICROSTEPS_GOTO != 256
      #warning "Configuration (Config.h): Setting AXIS4_DRIVER_MICROSTEPS_GOTO has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS4_DRIVER_MICROSTEPS == OFF && AXIS4_DRIVER_MICROSTEPS_GOTO != OFF
    #error "Configuration (Config.h): Setting AXIS4_DRIVER_MICROSTEPS_GOTO can't be enabled without first enabling AXIS4_DRIVER_MICROSTEPS."
  #endif
  #if AXIS4_DRIVER_DECAY != OFF && (AXIS4_DRIVER_DECAY < DRIVER_DECAY_MODE_FIRST || AXIS4_DRIVER_DECAY > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS4_DRIVER_DECAY unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS4_DRIVER_DECAY_GOTO != OFF && (AXIS4_DRIVER_DECAY_GOTO < DRIVER_DECAY_MODE_FIRST || AXIS4_DRIVER_DECAY_GOTO > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS4_DRIVER_DECAY_GOTO unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS4_DRIVER_IRUN != OFF && (AXIS4_DRIVER_IRUN < 0 || AXIS4_DRIVER_IRUN > 1000)
    #error "Configuration (Config.h): Setting AXIS4_DRIVER_IRUN unknown, use OFF or a value 0 to 1000 (mA.)"
  #endif
  #if AXIS4_DRIVER_IHOLD != HALF && (AXIS4_DRIVER_IHOLD < 0 || AXIS4_DRIVER_IHOLD > 1000)
    #error "Configuration (Config.h): Setting AXIS4_DRIVER_IHOLD unknown, use HALF or a value 0 to 1000 (mA.)"
  #endif
  #if AXIS4_DRIVER_IGOTO != SAME && (AXIS4_DRIVER_IGOTO < 0 || AXIS4_DRIVER_IGOTO > 1000)
    #error "Configuration (Config.h): Setting AXIS4_DRIVER_IGOTO unknown, use SAME or a value 0 to 1000 (mA.)"
  #endif
#endif

#ifdef AXIS4_SERVO_PRESENT
  #if AXIS4_SERVO_ENCODER < SERVO_ENCODER_FIRST || AXIS4_SERVO_ENCODER > SERVO_ENCODER_LAST
    #error "Configuration (Config.h): Setting AXIS4_SERVO_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
  #endif
#endif

// AXIS5 FOCUSER
#if AXIS5_DRIVER_MODEL != OFF && \
    (AXIS5_DRIVER_MODEL < DRIVER_FIRST || AXIS5_DRIVER_MODEL > DRIVER_LAST) && \
    (AXIS5_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS5_DRIVER_MODEL > SERVO_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS5_DRIVER_MODEL unknown, use OFF or a valid DRIVER (from Constants.h)"
#endif

#if AXIS5_DRIVER_STATUS != OFF && AXIS5_DRIVER_STATUS != ON && AXIS5_DRIVER_STATUS != HIGH && AXIS5_DRIVER_STATUS != LOW
  #error "Configuration (Config.h): Setting AXIS5_DRIVER_STATUS unknown, use OFF or valid driver status."
#endif

#if AXIS5_REVERSE != ON && AXIS5_REVERSE != OFF
  #error "Configuration (Config.h): Setting AXIS5_REVERSE unknown, use OFF or ON."
#endif

#if AXIS5_POWER_DOWN != ON && AXIS5_POWER_DOWN != OFF
  #error "Configuration (Config.h): Setting AXIS5_POWER_DOWN unknown, use OFF or ON."
#endif

#if AXIS5_LIMIT_MIN < 0 || AXIS5_LIMIT_MIN > 500
  #error "Configuration (Config.h): Setting AXIS5_LIMIT_MIN unknown, use value in the range 0 to 500 (mm.)"
#endif

#if AXIS5_LIMIT_MAX < AXIS5_LIMIT_MIN || AXIS5_LIMIT_MAX > 500
  #error "Configuration (Config.h): Setting AXIS5_LIMIT_MAX unknown, use value in the range AXIS5_LIMIT_MIN to 500 (mm.)"
#endif

#if AXIS5_SENSE_HOME != OFF && AXIS5_SENSE_HOME < 0
  #error "Configuration (Config.h): Setting AXIS5_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS5_SENSE_LIMIT_MIN != OFF && AXIS5_SENSE_LIMIT_MIN < 0
  #error "Configuration (Config.h): Setting AXIS5_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS5_SENSE_LIMIT_MAX != OFF && AXIS5_SENSE_LIMIT_MAX < 0
  #error "Configuration (Config.h): Setting AXIS5_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#ifdef AXIS5_DRIVER_PRESENT
  #if AXIS5_DRIVER_MICROSTEPS != OFF && (AXIS5_DRIVER_MICROSTEPS < 1 || AXIS5_DRIVER_MICROSTEPS > 256)
    #error "Configuration (Config.h): Setting AXIS5_DRIVER_MICROSTEPS unknown, use OFF or a valid microstep setting (range 1 to 256x and supported by your driver/design.)"
    #if AXIS5_DRIVER_MICROSTEPS != 1 && AXIS5_DRIVER_MICROSTEPS != 2 && AXIS5_DRIVER_MICROSTEPS != 4 && \
        AXIS5_DRIVER_MICROSTEPS != 8 && AXIS5_DRIVER_MICROSTEPS != 16 && AXIS5_DRIVER_MICROSTEPS != 32 && \
        AXIS5_DRIVER_MICROSTEPS != 64 && AXIS5_DRIVER_MICROSTEPS != 128 && AXIS5_DRIVER_MICROSTEPS != 256
      #warning "Configuration (Config.h): Setting AXIS5_DRIVER_MICROSTEPS has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS5_DRIVER_MICROSTEPS_GOTO != OFF && (AXIS5_DRIVER_MICROSTEPS_GOTO < 1 || AXIS5_DRIVER_MICROSTEPS_GOTO >= AXIS5_DRIVER_MICROSTEPS)
    #error "Configuration (Config.h): Setting AXIS5_DRIVER_MICROSTEPS_GOTO unknown, use OFF or a valid microstep setting (range 1 to < AXIS5_DRIVER_MICROSTEPS and supported by your driver/design.)"
    #if AXIS5_DRIVER_MICROSTEPS_GOTO != 1 && AXIS5_DRIVER_MICROSTEPS_GOTO != 2 && AXIS5_DRIVER_MICROSTEPS_GOTO != 4 && \
        AXIS5_DRIVER_MICROSTEPS_GOTO != 8 && AXIS5_DRIVER_MICROSTEPS_GOTO != 16 && AXIS5_DRIVER_MICROSTEPS_GOTO != 32 && \
        AXIS5_DRIVER_MICROSTEPS_GOTO != 64 && AXIS5_DRIVER_MICROSTEPS_GOTO != 128 && AXIS5_DRIVER_MICROSTEPS_GOTO != 256
      #warning "Configuration (Config.h): Setting AXIS5_DRIVER_MICROSTEPS_GOTO has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS5_DRIVER_MICROSTEPS == OFF && AXIS5_DRIVER_MICROSTEPS_GOTO != OFF
    #error "Configuration (Config.h): Setting AXIS5_DRIVER_MICROSTEPS_GOTO can't be enabled without first enabling AXIS5_DRIVER_MICROSTEPS."
  #endif
  #if AXIS5_DRIVER_DECAY != OFF && (AXIS5_DRIVER_DECAY < DRIVER_DECAY_MODE_FIRST || AXIS5_DRIVER_DECAY > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS5_DRIVER_DECAY unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS5_DRIVER_DECAY_GOTO != OFF && (AXIS5_DRIVER_DECAY_GOTO < DRIVER_DECAY_MODE_FIRST || AXIS5_DRIVER_DECAY_GOTO > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS5_DRIVER_DECAY_GOTO unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS5_DRIVER_IRUN != OFF && (AXIS5_DRIVER_IRUN < 0 || AXIS5_DRIVER_IRUN > 1000)
    #error "Configuration (Config.h): Setting AXIS5_DRIVER_IRUN unknown, use OFF or a value 0 to 1000 (mA.)"
  #endif
  #if AXIS5_DRIVER_IHOLD != HALF && (AXIS5_DRIVER_IHOLD < 0 || AXIS5_DRIVER_IHOLD > 1000)
    #error "Configuration (Config.h): Setting AXIS5_DRIVER_IHOLD unknown, use HALF or a value 0 to 1000 (mA.)"
  #endif
  #if AXIS5_DRIVER_IGOTO != SAME && (AXIS5_DRIVER_IGOTO < 0 || AXIS5_DRIVER_IGOTO > 1000)
    #error "Configuration (Config.h): Setting AXIS5_DRIVER_IGOTO unknown, use SAME or a value 0 to 1000 (mA.)"
  #endif
#endif

#ifdef AXIS5_SERVO_PRESENT
  #if AXIS5_SERVO_ENCODER < SERVO_ENCODER_FIRST || AXIS5_SERVO_ENCODER > SERVO_ENCODER_LAST
    #error "Configuration (Config.h): Setting AXIS5_SERVO_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
  #endif
#endif

// AXIS6 FOCUSER
#if AXIS6_DRIVER_MODEL != OFF && \
    (AXIS6_DRIVER_MODEL < DRIVER_FIRST || AXIS6_DRIVER_MODEL > DRIVER_LAST) && \
    (AXIS6_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS6_DRIVER_MODEL > SERVO_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS6_DRIVER_MODEL unknown, use OFF or a valid DRIVER (from Constants.h)"
#endif

#if AXIS6_DRIVER_STATUS != OFF && AXIS6_DRIVER_STATUS != ON && AXIS6_DRIVER_STATUS != HIGH && AXIS6_DRIVER_STATUS != LOW
  #error "Configuration (Config.h): Setting AXIS6_DRIVER_STATUS unknown, use OFF or valid driver status."
#endif

#if AXIS6_REVERSE != ON && AXIS6_REVERSE != OFF
  #error "Configuration (Config.h): Setting AXIS6_REVERSE unknown, use OFF or ON."
#endif

#if AXIS6_POWER_DOWN != ON && AXIS6_POWER_DOWN != OFF
  #error "Configuration (Config.h): Setting AXIS6_POWER_DOWN unknown, use OFF or ON."
#endif

#if AXIS6_LIMIT_MIN < 0 || AXIS6_LIMIT_MIN > 500
  #error "Configuration (Config.h): Setting AXIS6_LIMIT_MIN unknown, use value in the range 0 to 500 (mm.)"
#endif

#if AXIS6_LIMIT_MAX < AXIS6_LIMIT_MIN || AXIS6_LIMIT_MAX > 500
  #error "Configuration (Config.h): Setting AXIS6_LIMIT_MAX unknown, use value in the range AXIS6_LIMIT_MIN to 500 (mm.)"
#endif

#if AXIS6_SENSE_HOME != OFF && AXIS6_SENSE_HOME < 0
  #error "Configuration (Config.h): Setting AXIS6_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS6_SENSE_LIMIT_MIN != OFF && AXIS6_SENSE_LIMIT_MIN < 0
  #error "Configuration (Config.h): Setting AXIS6_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS6_SENSE_LIMIT_MAX != OFF && AXIS6_SENSE_LIMIT_MAX < 0
  #error "Configuration (Config.h): Setting AXIS6_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#ifdef AXIS6_DRIVER_PRESENT
  #if AXIS6_DRIVER_MICROSTEPS != OFF && (AXIS6_DRIVER_MICROSTEPS < 1 || AXIS6_DRIVER_MICROSTEPS > 256)
    #error "Configuration (Config.h): Setting AXIS6_DRIVER_MICROSTEPS unknown, use OFF or a valid microstep setting (range 1 to 256x and supported by your driver/design.)"
    #if AXIS6_DRIVER_MICROSTEPS != 1 && AXIS6_DRIVER_MICROSTEPS != 2 && AXIS6_DRIVER_MICROSTEPS != 4 && \
        AXIS6_DRIVER_MICROSTEPS != 8 && AXIS6_DRIVER_MICROSTEPS != 16 && AXIS6_DRIVER_MICROSTEPS != 32 && \
        AXIS6_DRIVER_MICROSTEPS != 64 && AXIS6_DRIVER_MICROSTEPS != 128 && AXIS6_DRIVER_MICROSTEPS != 256
      #warning "Configuration (Config.h): Setting AXIS6_DRIVER_MICROSTEPS has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS6_DRIVER_MICROSTEPS_GOTO != OFF && (AXIS6_DRIVER_MICROSTEPS_GOTO < 1 || AXIS6_DRIVER_MICROSTEPS_GOTO >= AXIS6_DRIVER_MICROSTEPS)
    #error "Configuration (Config.h): Setting AXIS6_DRIVER_MICROSTEPS_GOTO unknown, use OFF or a valid microstep setting (range 1 to < AXIS6_DRIVER_MICROSTEPS and supported by your driver/design.)"
    #if AXIS6_DRIVER_MICROSTEPS_GOTO != 1 && AXIS6_DRIVER_MICROSTEPS_GOTO != 2 && AXIS6_DRIVER_MICROSTEPS_GOTO != 4 && \
        AXIS6_DRIVER_MICROSTEPS_GOTO != 8 && AXIS6_DRIVER_MICROSTEPS_GOTO != 16 && AXIS6_DRIVER_MICROSTEPS_GOTO != 32 && \
        AXIS6_DRIVER_MICROSTEPS_GOTO != 64 && AXIS6_DRIVER_MICROSTEPS_GOTO != 128 && AXIS6_DRIVER_MICROSTEPS_GOTO != 256
      #warning "Configuration (Config.h): Setting AXIS6_DRIVER_MICROSTEPS_GOTO has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS6_DRIVER_MICROSTEPS == OFF && AXIS6_DRIVER_MICROSTEPS_GOTO != OFF
    #error "Configuration (Config.h): Setting AXIS6_DRIVER_MICROSTEPS_GOTO can't be enabled without first enabling AXIS6_DRIVER_MICROSTEPS."
  #endif
  #if AXIS6_DRIVER_DECAY != OFF && (AXIS6_DRIVER_DECAY < DRIVER_DECAY_MODE_FIRST || AXIS6_DRIVER_DECAY > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS6_DRIVER_DECAY unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS6_DRIVER_DECAY_GOTO != OFF && (AXIS6_DRIVER_DECAY_GOTO < DRIVER_DECAY_MODE_FIRST || AXIS6_DRIVER_DECAY_GOTO > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS6_DRIVER_DECAY_GOTO unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS6_DRIVER_IRUN != OFF && (AXIS6_DRIVER_IRUN < 0 || AXIS6_DRIVER_IRUN > 1000)
    #error "Configuration (Config.h): Setting AXIS6_DRIVER_IRUN unknown, use OFF or a value 0 to 1000 (mA.)"
  #endif
  #if AXIS6_DRIVER_IHOLD != HALF && (AXIS6_DRIVER_IHOLD < 0 || AXIS6_DRIVER_IHOLD > 1000)
    #error "Configuration (Config.h): Setting AXIS6_DRIVER_IHOLD unknown, use HALF or a value 0 to 1000 (mA.)"
  #endif
  #if AXIS6_DRIVER_IGOTO != SAME && (AXIS6_DRIVER_IGOTO < 0 || AXIS6_DRIVER_IGOTO > 1000)
    #error "Configuration (Config.h): Setting AXIS6_DRIVER_IGOTO unknown, use SAME or a value 0 to 1000 (mA.)"
  #endif
#endif

#ifdef AXIS6_SERVO_PRESENT
  #if AXIS6_SERVO_ENCODER < SERVO_ENCODER_FIRST || AXIS6_SERVO_ENCODER > SERVO_ENCODER_LAST
    #error "Configuration (Config.h): Setting AXIS6_SERVO_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
  #endif
#endif

// AXIS7 FOCUSER
#if AXIS7_DRIVER_MODEL != OFF && \
    (AXIS7_DRIVER_MODEL < DRIVER_FIRST || AXIS7_DRIVER_MODEL > DRIVER_LAST) && \
    (AXIS7_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS7_DRIVER_MODEL > SERVO_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS7_DRIVER_MODEL unknown, use OFF or a valid DRIVER (from Constants.h)"
#endif

#if AXIS7_DRIVER_STATUS != OFF && AXIS7_DRIVER_STATUS != ON && AXIS7_DRIVER_STATUS != HIGH && AXIS7_DRIVER_STATUS != LOW
  #error "Configuration (Config.h): Setting AXIS7_DRIVER_STATUS unknown, use OFF or valid driver status."
#endif

#if AXIS7_REVERSE != ON && AXIS7_REVERSE != OFF
  #error "Configuration (Config.h): Setting AXIS7_REVERSE unknown, use OFF or ON."
#endif

#if AXIS7_POWER_DOWN != ON && AXIS7_POWER_DOWN != OFF
  #error "Configuration (Config.h): Setting AXIS7_POWER_DOWN unknown, use OFF or ON."
#endif

#if AXIS7_LIMIT_MIN < 0 || AXIS7_LIMIT_MIN > 500
  #error "Configuration (Config.h): Setting AXIS7_LIMIT_MIN unknown, use value in the range 0 to 500 (mm.)"
#endif

#if AXIS7_LIMIT_MAX < AXIS7_LIMIT_MIN || AXIS7_LIMIT_MAX > 500
  #error "Configuration (Config.h): Setting AXIS7_LIMIT_MAX unknown, use value in the range AXIS7_LIMIT_MIN to 500 (mm.)"
#endif

#if AXIS7_SENSE_HOME != OFF && AXIS7_SENSE_HOME < 0
  #error "Configuration (Config.h): Setting AXIS7_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS7_SENSE_LIMIT_MIN != OFF && AXIS7_SENSE_LIMIT_MIN < 0
  #error "Configuration (Config.h): Setting AXIS7_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS7_SENSE_LIMIT_MAX != OFF && AXIS7_SENSE_LIMIT_MAX < 0
  #error "Configuration (Config.h): Setting AXIS7_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#ifdef AXIS7_DRIVER_PRESENT
  #if AXIS7_DRIVER_MICROSTEPS != OFF && (AXIS7_DRIVER_MICROSTEPS < 1 || AXIS7_DRIVER_MICROSTEPS > 256)
    #error "Configuration (Config.h): Setting AXIS7_DRIVER_MICROSTEPS unknown, use OFF or a valid microstep setting (range 1 to 256x and supported by your driver/design.)"
    #if AXIS7_DRIVER_MICROSTEPS != 1 && AXIS7_DRIVER_MICROSTEPS != 2 && AXIS7_DRIVER_MICROSTEPS != 4 && \
        AXIS7_DRIVER_MICROSTEPS != 8 && AXIS7_DRIVER_MICROSTEPS != 16 && AXIS7_DRIVER_MICROSTEPS != 32 && \
        AXIS7_DRIVER_MICROSTEPS != 64 && AXIS7_DRIVER_MICROSTEPS != 128 && AXIS7_DRIVER_MICROSTEPS != 256
      #warning "Configuration (Config.h): Setting AXIS7_DRIVER_MICROSTEPS has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS7_DRIVER_MICROSTEPS_GOTO != OFF && (AXIS7_DRIVER_MICROSTEPS_GOTO < 1 || AXIS7_DRIVER_MICROSTEPS_GOTO >= AXIS7_DRIVER_MICROSTEPS)
    #error "Configuration (Config.h): Setting AXIS7_DRIVER_MICROSTEPS_GOTO unknown, use OFF or a valid microstep setting (range 1 to < AXIS7_DRIVER_MICROSTEPS and supported by your driver/design.)"
    #if AXIS7_DRIVER_MICROSTEPS_GOTO != 1 && AXIS7_DRIVER_MICROSTEPS_GOTO != 2 && AXIS7_DRIVER_MICROSTEPS_GOTO != 4 && \
        AXIS7_DRIVER_MICROSTEPS_GOTO != 8 && AXIS7_DRIVER_MICROSTEPS_GOTO != 16 && AXIS7_DRIVER_MICROSTEPS_GOTO != 32 && \
        AXIS7_DRIVER_MICROSTEPS_GOTO != 64 && AXIS7_DRIVER_MICROSTEPS_GOTO != 128 && AXIS7_DRIVER_MICROSTEPS_GOTO != 256
      #warning "Configuration (Config.h): Setting AXIS7_DRIVER_MICROSTEPS_GOTO has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS7_DRIVER_MICROSTEPS == OFF && AXIS7_DRIVER_MICROSTEPS_GOTO != OFF
    #error "Configuration (Config.h): Setting AXIS7_DRIVER_MICROSTEPS_GOTO can't be enabled without first enabling AXIS7_DRIVER_MICROSTEPS."
  #endif
  #if AXIS7_DRIVER_DECAY != OFF && (AXIS7_DRIVER_DECAY < DRIVER_DECAY_MODE_FIRST || AXIS7_DRIVER_DECAY > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS7_DRIVER_DECAY unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS7_DRIVER_DECAY_GOTO != OFF && (AXIS7_DRIVER_DECAY_GOTO < DRIVER_DECAY_MODE_FIRST || AXIS7_DRIVER_DECAY_GOTO > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS7_DRIVER_DECAY_GOTO unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS7_DRIVER_IRUN != OFF && (AXIS7_DRIVER_IRUN < 0 || AXIS7_DRIVER_IRUN > 1000)
    #error "Configuration (Config.h): Setting AXIS7_DRIVER_IRUN unknown, use OFF or a value 0 to 1000 (mA.)"
  #endif
  #if AXIS7_DRIVER_IHOLD != HALF && (AXIS7_DRIVER_IHOLD < 0 || AXIS7_DRIVER_IHOLD > 1000)
    #error "Configuration (Config.h): Setting AXIS7_DRIVER_IHOLD unknown, use HALF or a value 0 to 1000 (mA.)"
  #endif
  #if AXIS7_DRIVER_IGOTO != SAME && (AXIS7_DRIVER_IGOTO < 0 || AXIS7_DRIVER_IGOTO > 1000)
    #error "Configuration (Config.h): Setting AXIS7_DRIVER_IGOTO unknown, use SAME or a value 0 to 1000 (mA.)"
  #endif
#endif

#ifdef AXIS7_SERVO_PRESENT
  #if AXIS7_SERVO_ENCODER < SERVO_ENCODER_FIRST || AXIS7_SERVO_ENCODER > SERVO_ENCODER_LAST
    #error "Configuration (Config.h): Setting AXIS7_SERVO_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
  #endif
#endif

// AXIS8 FOCUSER
#if AXIS8_DRIVER_MODEL != OFF && \
    (AXIS8_DRIVER_MODEL < DRIVER_FIRST || AXIS8_DRIVER_MODEL > DRIVER_LAST) && \
    (AXIS8_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS8_DRIVER_MODEL > SERVO_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS8_DRIVER_MODEL unknown, use OFF or a valid DRIVER (from Constants.h)"
#endif

#if AXIS8_DRIVER_STATUS != OFF && AXIS8_DRIVER_STATUS != ON && AXIS8_DRIVER_STATUS != HIGH && AXIS8_DRIVER_STATUS != LOW
  #error "Configuration (Config.h): Setting AXIS8_DRIVER_STATUS unknown, use OFF or valid driver status."
#endif

#if AXIS8_REVERSE != ON && AXIS8_REVERSE != OFF
  #error "Configuration (Config.h): Setting AXIS8_REVERSE unknown, use OFF or ON."
#endif

#if AXIS8_POWER_DOWN != ON && AXIS8_POWER_DOWN != OFF
  #error "Configuration (Config.h): Setting AXIS8_POWER_DOWN unknown, use OFF or ON."
#endif

#if AXIS8_LIMIT_MIN < 0 || AXIS8_LIMIT_MIN > 500
  #error "Configuration (Config.h): Setting AXIS8_LIMIT_MIN unknown, use value in the range 0 to 500 (mm.)"
#endif

#if AXIS8_LIMIT_MAX < AXIS8_LIMIT_MIN || AXIS8_LIMIT_MAX > 500
  #error "Configuration (Config.h): Setting AXIS8_LIMIT_MAX unknown, use value in the range AXIS8_LIMIT_MIN to 500 (mm.)"
#endif

#if AXIS8_SENSE_HOME != OFF && AXIS8_SENSE_HOME < 0
  #error "Configuration (Config.h): Setting AXIS8_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS8_SENSE_LIMIT_MIN != OFF && AXIS8_SENSE_LIMIT_MIN < 0
  #error "Configuration (Config.h): Setting AXIS8_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS8_SENSE_LIMIT_MAX != OFF && AXIS8_SENSE_LIMIT_MAX < 0
  #error "Configuration (Config.h): Setting AXIS8_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#ifdef AXIS8_DRIVER_PRESENT
  #if AXIS8_DRIVER_MICROSTEPS != OFF && (AXIS8_DRIVER_MICROSTEPS < 1 || AXIS8_DRIVER_MICROSTEPS > 256)
    #error "Configuration (Config.h): Setting AXIS8_DRIVER_MICROSTEPS unknown, use OFF or a valid microstep setting (range 1 to 256x and supported by your driver/design.)"
    #if AXIS8_DRIVER_MICROSTEPS != 1 && AXIS8_DRIVER_MICROSTEPS != 2 && AXIS8_DRIVER_MICROSTEPS != 4 && \
        AXIS8_DRIVER_MICROSTEPS != 8 && AXIS8_DRIVER_MICROSTEPS != 16 && AXIS8_DRIVER_MICROSTEPS != 32 && \
        AXIS8_DRIVER_MICROSTEPS != 64 && AXIS8_DRIVER_MICROSTEPS != 128 && AXIS8_DRIVER_MICROSTEPS != 256
      #warning "Configuration (Config.h): Setting AXIS8_DRIVER_MICROSTEPS has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS8_DRIVER_MICROSTEPS_GOTO != OFF && (AXIS8_DRIVER_MICROSTEPS_GOTO < 1 || AXIS8_DRIVER_MICROSTEPS_GOTO >= AXIS8_DRIVER_MICROSTEPS)
    #error "Configuration (Config.h): Setting AXIS8_DRIVER_MICROSTEPS_GOTO unknown, use OFF or a valid microstep setting (range 1 to < AXIS8_DRIVER_MICROSTEPS and supported by your driver/design.)"
    #if AXIS8_DRIVER_MICROSTEPS_GOTO != 1 && AXIS8_DRIVER_MICROSTEPS_GOTO != 2 && AXIS8_DRIVER_MICROSTEPS_GOTO != 4 && \
        AXIS8_DRIVER_MICROSTEPS_GOTO != 8 && AXIS8_DRIVER_MICROSTEPS_GOTO != 16 && AXIS8_DRIVER_MICROSTEPS_GOTO != 32 && \
        AXIS8_DRIVER_MICROSTEPS_GOTO != 64 && AXIS8_DRIVER_MICROSTEPS_GOTO != 128 && AXIS8_DRIVER_MICROSTEPS_GOTO != 256
      #warning "Configuration (Config.h): Setting AXIS8_DRIVER_MICROSTEPS_GOTO has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS8_DRIVER_MICROSTEPS == OFF && AXIS8_DRIVER_MICROSTEPS_GOTO != OFF
    #error "Configuration (Config.h): Setting AXIS8_DRIVER_MICROSTEPS_GOTO can't be enabled without first enabling AXIS8_DRIVER_MICROSTEPS."
  #endif
  #if AXIS8_DRIVER_DECAY != OFF && (AXIS8_DRIVER_DECAY < DRIVER_DECAY_MODE_FIRST || AXIS8_DRIVER_DECAY > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS8_DRIVER_DECAY unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS8_DRIVER_DECAY_GOTO != OFF && (AXIS8_DRIVER_DECAY_GOTO < DRIVER_DECAY_MODE_FIRST || AXIS8_DRIVER_DECAY_GOTO > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS8_DRIVER_DECAY_GOTO unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS8_DRIVER_IRUN != OFF && (AXIS8_DRIVER_IRUN < 0 || AXIS8_DRIVER_IRUN > 1000)
    #error "Configuration (Config.h): Setting AXIS8_DRIVER_IRUN unknown, use OFF or a value 0 to 1000 (mA.)"
  #endif
  #if AXIS8_DRIVER_IHOLD != HALF && (AXIS8_DRIVER_IHOLD < 0 || AXIS8_DRIVER_IHOLD > 1000)
    #error "Configuration (Config.h): Setting AXIS8_DRIVER_IHOLD unknown, use HALF or a value 0 to 1000 (mA.)"
  #endif
  #if AXIS8_DRIVER_IGOTO != SAME && (AXIS8_DRIVER_IGOTO < 0 || AXIS8_DRIVER_IGOTO > 1000)
    #error "Configuration (Config.h): Setting AXIS8_DRIVER_IGOTO unknown, use SAME or a value 0 to 1000 (mA.)"
  #endif
#endif

#ifdef AXIS8_SERVO_PRESENT
  #if AXIS8_SERVO_ENCODER < SERVO_ENCODER_FIRST || AXIS8_SERVO_ENCODER > SERVO_ENCODER_LAST
    #error "Configuration (Config.h): Setting AXIS8_SERVO_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
  #endif
#endif

// AXIS9 FOCUSER
#if AXIS9_DRIVER_MODEL != OFF && \
    (AXIS9_DRIVER_MODEL < DRIVER_FIRST || AXIS9_DRIVER_MODEL > DRIVER_LAST) && \
    (AXIS9_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS9_DRIVER_MODEL > SERVO_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS9_DRIVER_MODEL unknown, use OFF or a valid DRIVER (from Constants.h)"
#endif

#if AXIS9_DRIVER_STATUS != OFF && AXIS9_DRIVER_STATUS != ON && AXIS9_DRIVER_STATUS != HIGH && AXIS9_DRIVER_STATUS != LOW
  #error "Configuration (Config.h): Setting AXIS9_DRIVER_STATUS unknown, use OFF or valid driver status."
#endif

#if AXIS9_REVERSE != ON && AXIS9_REVERSE != OFF
  #error "Configuration (Config.h): Setting AXIS9_REVERSE unknown, use OFF or ON."
#endif

#if AXIS9_POWER_DOWN != ON && AXIS9_POWER_DOWN != OFF
  #error "Configuration (Config.h): Setting AXIS9_POWER_DOWN unknown, use OFF or ON."
#endif

#if AXIS9_LIMIT_MIN < 0 || AXIS9_LIMIT_MIN > 500
  #error "Configuration (Config.h): Setting AXIS9_LIMIT_MIN unknown, use value in the range 0 to 500 (mm.)"
#endif

#if AXIS9_LIMIT_MAX < AXIS9_LIMIT_MIN || AXIS9_LIMIT_MAX > 500
  #error "Configuration (Config.h): Setting AXIS9_LIMIT_MAX unknown, use value in the range AXIS9_LIMIT_MIN to 500 (mm.)"
#endif

#if AXIS9_SENSE_HOME != OFF && AXIS9_SENSE_HOME < 0
  #error "Configuration (Config.h): Setting AXIS9_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS9_SENSE_LIMIT_MIN != OFF && AXIS9_SENSE_LIMIT_MIN < 0
  #error "Configuration (Config.h): Setting AXIS9_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if AXIS9_SENSE_LIMIT_MAX != OFF && AXIS9_SENSE_LIMIT_MAX < 0
  #error "Configuration (Config.h): Setting AXIS9_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#ifdef AXIS9_DRIVER_PRESENT
  #if AXIS9_DRIVER_MICROSTEPS != OFF && (AXIS9_DRIVER_MICROSTEPS < 1 || AXIS9_DRIVER_MICROSTEPS > 256)
    #error "Configuration (Config.h): Setting AXIS9_DRIVER_MICROSTEPS unknown, use OFF or a valid microstep setting (range 1 to 256x and supported by your driver/design.)"
    #if AXIS9_DRIVER_MICROSTEPS != 1 && AXIS9_DRIVER_MICROSTEPS != 2 && AXIS9_DRIVER_MICROSTEPS != 4 && \
        AXIS9_DRIVER_MICROSTEPS != 8 && AXIS9_DRIVER_MICROSTEPS != 16 && AXIS9_DRIVER_MICROSTEPS != 32 && \
        AXIS9_DRIVER_MICROSTEPS != 64 && AXIS9_DRIVER_MICROSTEPS != 128 && AXIS9_DRIVER_MICROSTEPS != 256
      #warning "Configuration (Config.h): Setting AXIS9_DRIVER_MICROSTEPS has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS9_DRIVER_MICROSTEPS_GOTO != OFF && (AXIS9_DRIVER_MICROSTEPS_GOTO < 1 || AXIS9_DRIVER_MICROSTEPS_GOTO >= AXIS9_DRIVER_MICROSTEPS)
    #error "Configuration (Config.h): Setting AXIS9_DRIVER_MICROSTEPS_GOTO unknown, use OFF or a valid microstep setting (range 1 to < AXIS9_DRIVER_MICROSTEPS and supported by your driver/design.)"
    #if AXIS9_DRIVER_MICROSTEPS_GOTO != 1 && AXIS9_DRIVER_MICROSTEPS_GOTO != 2 && AXIS9_DRIVER_MICROSTEPS_GOTO != 4 && \
        AXIS9_DRIVER_MICROSTEPS_GOTO != 8 && AXIS9_DRIVER_MICROSTEPS_GOTO != 16 && AXIS9_DRIVER_MICROSTEPS_GOTO != 32 && \
        AXIS9_DRIVER_MICROSTEPS_GOTO != 64 && AXIS9_DRIVER_MICROSTEPS_GOTO != 128 && AXIS9_DRIVER_MICROSTEPS_GOTO != 256
      #warning "Configuration (Config.h): Setting AXIS9_DRIVER_MICROSTEPS_GOTO has an unusual value (not a power of 2!)"
    #endif
  #endif
  #if AXIS9_DRIVER_MICROSTEPS == OFF && AXIS9_DRIVER_MICROSTEPS_GOTO != OFF
    #error "Configuration (Config.h): Setting AXIS9_DRIVER_MICROSTEPS_GOTO can't be enabled without first enabling AXIS9_DRIVER_MICROSTEPS."
  #endif
  #if AXIS9_DRIVER_DECAY != OFF && (AXIS9_DRIVER_DECAY < DRIVER_DECAY_MODE_FIRST || AXIS9_DRIVER_DECAY > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS9_DRIVER_DECAY unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS9_DRIVER_DECAY_GOTO != OFF && (AXIS9_DRIVER_DECAY_GOTO < DRIVER_DECAY_MODE_FIRST || AXIS9_DRIVER_DECAY_GOTO > DRIVER_DECAY_MODE_LAST)
    #error "Configuration (Config.h): Setting AXIS9_DRIVER_DECAY_GOTO unknown, use a valid DRIVER DECAY MODE (from Constants.h)"
  #endif
  #if AXIS9_DRIVER_IRUN != OFF && (AXIS9_DRIVER_IRUN < 0 || AXIS9_DRIVER_IRUN > 1000)
    #error "Configuration (Config.h): Setting AXIS9_DRIVER_IRUN unknown, use OFF or a value 0 to 1000 (mA.)"
  #endif
  #if AXIS9_DRIVER_IHOLD != HALF && (AXIS9_DRIVER_IHOLD < 0 || AXIS9_DRIVER_IHOLD > 1000)
    #error "Configuration (Config.h): Setting AXIS9_DRIVER_IHOLD unknown, use HALF or a value 0 to 1000 (mA.)"
  #endif
  #if AXIS9_DRIVER_IGOTO != SAME && (AXIS9_DRIVER_IGOTO < 0 || AXIS9_DRIVER_IGOTO > 1000)
    #error "Configuration (Config.h): Setting AXIS9_DRIVER_IGOTO unknown, use SAME or a value 0 to 1000 (mA.)"
  #endif
#endif

#ifdef AXIS9_SERVO_PRESENT
  #if AXIS9_SERVO_ENCODER < SERVO_ENCODER_FIRST || AXIS9_SERVO_ENCODER > SERVO_ENCODER_LAST
    #error "Configuration (Config.h): Setting AXIS9_SERVO_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
  #endif
#endif

// USER FEEDBACK
#if STATUS_FOCUSER_LED != ON && STATUS_FOCUSER_LED != OFF
  #error "Configuration (Config.h): Setting STATUS_FOCUSER_LED unknown, use OFF or ON."
#endif

// FOCUSER TEMPERATURE
#if FOCUSER_TEMPERATURE != OFF && \
    FOCUSER_TEMPERATURE & DS_MASK != DS18B20 && \
    FOCUSER_TEMPERATURE & DS_MASK != DS18S20 && \
    (FOCUSER_TEMPERATURE < TEMPERATURE_FIRST || FOCUSER_TEMPERATURE > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FOCUSER_TEMPERATURE unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif

// AUXILIARY FEATURE -----------------------------

#if FEATURE1_PURPOSE != OFF && (FEATURE1_PURPOSE < AUX_FEATURE_PURPOSE_FIRST || FEATURE1_PURPOSE > AUX_FEATURE_PURPOSE_LAST)
  #error "Configuration (Config.h): Setting FEATURE1_PURPOSE unknown, use OFF or AUXILIARY FEATURE purpose (from Constants.h)"
#endif
#if FEATURE2_PURPOSE != OFF && (FEATURE2_PURPOSE < AUX_FEATURE_PURPOSE_FIRST || FEATURE2_PURPOSE > AUX_FEATURE_PURPOSE_LAST)
  #error "Configuration (Config.h): Setting FEATURE2_PURPOSE unknown, use OFF or AUXILIARY FEATURE purpose (from Constants.h)"
#endif
#if FEATURE3_PURPOSE != OFF && (FEATURE3_PURPOSE < AUX_FEATURE_PURPOSE_FIRST || FEATURE3_PURPOSE > AUX_FEATURE_PURPOSE_LAST)
  #error "Configuration (Config.h): Setting FEATURE3_PURPOSE unknown, use OFF or AUXILIARY FEATURE purpose (from Constants.h)"
#endif
#if FEATURE4_PURPOSE != OFF && (FEATURE4_PURPOSE < AUX_FEATURE_PURPOSE_FIRST || FEATURE4_PURPOSE > AUX_FEATURE_PURPOSE_LAST)
  #error "Configuration (Config.h): Setting FEATURE4_PURPOSE unknown, use OFF or AUXILIARY FEATURE purpose (from Constants.h)"
#endif
#if FEATURE5_PURPOSE != OFF && (FEATURE5_PURPOSE < AUX_FEATURE_PURPOSE_FIRST || FEATURE5_PURPOSE > AUX_FEATURE_PURPOSE_LAST)
  #error "Configuration (Config.h): Setting FEATURE5_PURPOSE unknown, use OFF or AUXILIARY FEATURE purpose (from Constants.h)"
#endif
#if FEATURE6_PURPOSE != OFF && (FEATURE6_PURPOSE < AUX_FEATURE_PURPOSE_FIRST || FEATURE6_PURPOSE > AUX_FEATURE_PURPOSE_LAST)
  #error "Configuration (Config.h): Setting FEATURE6_PURPOSE unknown, use OFF or AUXILIARY FEATURE purpose (from Constants.h)"
#endif
#if FEATURE7_PURPOSE != OFF && (FEATURE7_PURPOSE < AUX_FEATURE_PURPOSE_FIRST || FEATURE7_PURPOSE > AUX_FEATURE_PURPOSE_LAST)
  #error "Configuration (Config.h): Setting FEATURE7_PURPOSE unknown, use OFF or AUXILIARY FEATURE purpose (from Constants.h)"
#endif
#if FEATURE8_PURPOSE != OFF && (FEATURE8_PURPOSE < AUX_FEATURE_PURPOSE_FIRST || FEATURE8_PURPOSE > AUX_FEATURE_PURPOSE_LAST)
  #error "Configuration (Config.h): Setting FEATURE8_PURPOSE unknown, use OFF or AUXILIARY FEATURE purpose (from Constants.h)"
#endif

#if FEATURE1_TEMP != OFF && \
    FEATURE1_TEMP & DS_MASK != DS18B20 && \
    FEATURE1_TEMP & DS_MASK != DS18S20 && \
    (FEATURE1_TEMP < TEMPERATURE_FIRST || FEATURE1_TEMP > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FEATURE1_TEMP unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif
#if FEATURE2_TEMP != OFF && \
    FEATURE2_TEMP & DS_MASK != DS18B20 && \
    FEATURE2_TEMP & DS_MASK != DS18S20 && \
    (FEATURE2_TEMP < TEMPERATURE_FIRST || FEATURE2_TEMP > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FEATURE2_TEMP unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif
#if FEATURE3_TEMP != OFF && \
    FEATURE3_TEMP & DS_MASK != DS18B20 && \
    FEATURE3_TEMP & DS_MASK != DS18S20 && \
    (FEATURE3_TEMP < TEMPERATURE_FIRST || FEATURE3_TEMP > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FEATURE3_TEMP unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif
#if FEATURE4_TEMP != OFF && \
    FEATURE4_TEMP & DS_MASK != DS18B20 && \
    FEATURE4_TEMP & DS_MASK != DS18S20 && \
    (FEATURE4_TEMP < TEMPERATURE_FIRST || FEATURE4_TEMP > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FEATURE4_TEMP unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif
#if FEATURE5_TEMP != OFF && \
    FEATURE5_TEMP & DS_MASK != DS18B20 && \
    FEATURE5_TEMP & DS_MASK != DS18S20 && \
    (FEATURE5_TEMP < TEMPERATURE_FIRST || FEATURE5_TEMP > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FEATURE5_TEMP unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif
#if FEATURE6_TEMP != OFF && \
    FEATURE6_TEMP & DS_MASK != DS18B20 && \
    FEATURE6_TEMP & DS_MASK != DS18S20 && \
    (FEATURE6_TEMP < TEMPERATURE_FIRST || FEATURE6_TEMP > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FEATURE6_TEMP unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif
#if FEATURE7_TEMP != OFF && \
    FEATURE7_TEMP & DS_MASK != DS18B20 && \
    FEATURE7_TEMP & DS_MASK != DS18S20 && \
    (FEATURE7_TEMP < TEMPERATURE_FIRST || FEATURE7_TEMP > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FEATURE7_TEMP unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif
#if FEATURE8_TEMP != OFF && \
    FEATURE8_TEMP & DS_MASK != DS18B20 && \
    FEATURE8_TEMP & DS_MASK != DS18S20 && \
    (FEATURE8_TEMP < TEMPERATURE_FIRST || FEATURE8_TEMP > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FEATURE8_TEMP unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif

#if FEATURE1_PIN != OFF && FEATURE1_PIN < 0
  #error "Configuration (Config.h): Setting FEATURE1_PIN unknown, use OFF or a valid microcontroller pin designation."
#endif
#if FEATURE2_PIN != OFF && FEATURE2_PIN < 0
  #error "Configuration (Config.h): Setting FEATURE3_PIN unknown, use OFF or a valid microcontroller pin designation."
#endif
#if FEATURE3_PIN != OFF && FEATURE3_PIN < 0
  #error "Configuration (Config.h): Setting FEATURE4_PIN unknown, use OFF or a valid microcontroller pin designation."
#endif
#if FEATURE4_PIN != OFF && FEATURE4_PIN < 0
  #error "Configuration (Config.h): Setting FEATURE4_PIN unknown, use OFF or a valid microcontroller pin designation."
#endif
#if FEATURE5_PIN != OFF && FEATURE5_PIN < 0
  #error "Configuration (Config.h): Setting FEATURE5_PIN unknown, use OFF or a valid microcontroller pin designation."
#endif
#if FEATURE6_PIN != OFF && FEATURE6_PIN < 0
  #error "Configuration (Config.h): Setting FEATURE6_PIN unknown, use OFF or a valid microcontroller pin designation."
#endif
#if FEATURE7_PIN != OFF && FEATURE7_PIN < 0
  #error "Configuration (Config.h): Setting FEATURE7_PIN unknown, use OFF or a valid microcontroller pin designation."
#endif
#if FEATURE8_PIN != OFF && FEATURE8_PIN < 0
  #error "Configuration (Config.h): Setting FEATURE8_PIN unknown, use OFF or a valid microcontroller pin designation."
#endif

#if FEATURE1_DEFAULT_VALUE != OFF && FEATURE1_DEFAULT_VALUE != ON && (FEATURE1_DEFAULT_VALUE < 0 || FEATURE1_DEFAULT_VALUE > 255)
  #error "Configuration (Config.h): Setting FEATURE1_DEFAULT_VALUE unknown, use OFF, ON or a value from 0 to 255 (for ANALOG_OUT.)"
#endif
#if FEATURE2_DEFAULT_VALUE != OFF && FEATURE1_DEFAULT_VALUE != ON && (FEATURE2_DEFAULT_VALUE < 0 || FEATURE2_DEFAULT_VALUE > 255)
  #error "Configuration (Config.h): Setting FEATURE2_DEFAULT_VALUE unknown, use OFF, ON or a value from 0 to 255 (for ANALOG_OUT.)"
#endif
#if FEATURE3_DEFAULT_VALUE != OFF && FEATURE1_DEFAULT_VALUE != ON && (FEATURE3_DEFAULT_VALUE < 0 || FEATURE3_DEFAULT_VALUE > 255)
  #error "Configuration (Config.h): Setting FEATURE3_DEFAULT_VALUE unknown, use OFF, ON or a value from 0 to 255 (for ANALOG_OUT.)"
#endif
#if FEATURE4_DEFAULT_VALUE != OFF && FEATURE1_DEFAULT_VALUE != ON && (FEATURE4_DEFAULT_VALUE < 0 || FEATURE4_DEFAULT_VALUE > 255)
  #error "Configuration (Config.h): Setting FEATURE4_DEFAULT_VALUE unknown, use OFF, ON or a value from 0 to 255 (for ANALOG_OUT.)"
#endif
#if FEATURE5_DEFAULT_VALUE != OFF && FEATURE1_DEFAULT_VALUE != ON && (FEATURE5_DEFAULT_VALUE < 0 || FEATURE5_DEFAULT_VALUE > 255)
  #error "Configuration (Config.h): Setting FEATURE5_DEFAULT_VALUE unknown, use OFF, ON or a value from 0 to 255 (for ANALOG_OUT.)"
#endif
#if FEATURE6_DEFAULT_VALUE != OFF && FEATURE1_DEFAULT_VALUE != ON && (FEATURE6_DEFAULT_VALUE < 0 || FEATURE6_DEFAULT_VALUE > 255)
  #error "Configuration (Config.h): Setting FEATURE6_DEFAULT_VALUE unknown, use OFF, ON or a value from 0 to 255 (for ANALOG_OUT.)"
#endif
#if FEATURE7_DEFAULT_VALUE != OFF && FEATURE1_DEFAULT_VALUE != ON && (FEATURE7_DEFAULT_VALUE < 0 || FEATURE7_DEFAULT_VALUE > 255)
  #error "Configuration (Config.h): Setting FEATURE7_DEFAULT_VALUE unknown, use OFF, ON or a value from 0 to 255 (for ANALOG_OUT.)"
#endif
#if FEATURE8_DEFAULT_VALUE != OFF && FEATURE1_DEFAULT_VALUE != ON && (FEATURE8_DEFAULT_VALUE < 0 || FEATURE8_DEFAULT_VALUE > 255)
  #error "Configuration (Config.h): Setting FEATURE8_DEFAULT_VALUE unknown, use OFF, ON or a value from 0 to 255 (for ANALOG_OUT.)"
#endif

#if FEATURE1_ACTIVE_STATE != HIGH && FEATURE1_ACTIVE_STATE != LOW
  #error "Configuration (Config.h): Setting FEATURE1_ACTIVE_STATE unknown, use HIGH or LOW."
#endif
#if FEATURE2_ACTIVE_STATE != HIGH && FEATURE2_ACTIVE_STATE != LOW
  #error "Configuration (Config.h): Setting FEATURE2_ACTIVE_STATE unknown, use HIGH or LOW."
#endif
#if FEATURE3_ACTIVE_STATE != HIGH && FEATURE3_ACTIVE_STATE != LOW
  #error "Configuration (Config.h): Setting FEATURE3_ACTIVE_STATE unknown, use HIGH or LOW."
#endif
#if FEATURE4_ACTIVE_STATE != HIGH && FEATURE4_ACTIVE_STATE != LOW
  #error "Configuration (Config.h): Setting FEATURE4_ACTIVE_STATE unknown, use HIGH or LOW."
#endif
#if FEATURE5_ACTIVE_STATE != HIGH && FEATURE5_ACTIVE_STATE != LOW
  #error "Configuration (Config.h): Setting FEATURE5_ACTIVE_STATE unknown, use HIGH or LOW."
#endif
#if FEATURE6_ACTIVE_STATE != HIGH && FEATURE6_ACTIVE_STATE != LOW
  #error "Configuration (Config.h): Setting FEATURE6_ACTIVE_STATE unknown, use HIGH or LOW."
#endif
#if FEATURE7_ACTIVE_STATE != HIGH && FEATURE7_ACTIVE_STATE != LOW
  #error "Configuration (Config.h): Setting FEATURE7_ACTIVE_STATE unknown, use HIGH or LOW."
#endif
#if FEATURE8_ACTIVE_STATE != HIGH && FEATURE8_ACTIVE_STATE != LOW
  #error "Configuration (Config.h): Setting FEATURE8_ACTIVE_STATE unknown, use HIGH or LOW."
#endif
