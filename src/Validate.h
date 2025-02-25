// -------------------------------------------------------------------------------------------------
// Validate configuration
#pragma once
#include "Common.h"

#if !defined(FileVersionConfig) || FileVersionConfig != 6 // per FirmwareVersionConfig
  #error "Configuration (Config.h): FileVersionConfig (Config.h version) must be 6 for this OnStep."
#endif

// BACKWARDS COMPATABILITY ------------------------
#ifdef AXIS3_SLEW_RATE_DESIRED
  #warning "Configuration (Config.h): AXIS3_SLEW_RATE_DESIRED has been replaced with AXIS3_SLEW_RATE_BASE_DESIRED please update"
  #define AXIS3_SLEW_RATE_BASE_DESIRED AXIS3_SLEW_RATE_DESIRED
#endif
#ifdef AXIS4_SLEW_RATE_DESIRED
  #warning "Configuration (Config.h): AXIS4_SLEW_RATE_DESIRED has been replaced with AXIS4_SLEW_RATE_BASE_DESIRED please update"
  #define AXIS4_SLEW_RATE_BASE_DESIRED AXIS4_SLEW_RATE_DESIRED
#endif
#ifdef AXIS5_SLEW_RATE_DESIRED
  #warning "Configuration (Config.h): AXIS5_SLEW_RATE_DESIRED has been replaced with AXIS5_SLEW_RATE_BASE_DESIRED please update"
  #define AXIS5_SLEW_RATE_BASE_DESIRED AXIS5_SLEW_RATE_DESIRED
#endif

// GENERAL ---------------------------------------
#if defined(STEP_DIR_TMC_UART_PRESENT) && (!defined(SERIAL_TMC) || !defined(SERIAL_TMC_BAUD))
  #error "Configuration (Config.h): This PINMAP doesn't support TMC UART mode drivers"
#endif

// TELESCOPE -------------------------------------
#if PINMAP != OFF && (PINMAP < PINMAP_FIRST || PINMAP > PINMAP_LAST)
  #error "Configuration (Config.h): PINMAP must be set to a valid board (from Constants.h) or OFF (for user pin defs in Config.h)"
#endif

#if SERIAL_A_BAUD_DEFAULT != 9600 && SERIAL_A_BAUD_DEFAULT != 19200 && SERIAL_A_BAUD_DEFAULT != 38400 && \
    SERIAL_A_BAUD_DEFAULT != 57600 && SERIAL_A_BAUD_DEFAULT != 115200 && SERIAL_A_BAUD_DEFAULT != 230400 && \
    SERIAL_A_BAUD_DEFAULT != 460800 && SERIAL_A_BAUD_DEFAULT != 921600 && SERIAL_A_BAUD_DEFAULT != OFF
  #warning "Configuration (Config.h): Setting SERIAL_A_BAUD_DEFAULT unknown, use 9600, 19200, 38400, 57600, 115200, 230400, 460800, or 921600 (baud.)"
#endif

#if SERIAL_B_BAUD_DEFAULT != 9600 && SERIAL_B_BAUD_DEFAULT != 19200 && SERIAL_B_BAUD_DEFAULT != 38400 && \
    SERIAL_B_BAUD_DEFAULT != 57600 && SERIAL_B_BAUD_DEFAULT != 115200 && SERIAL_B_BAUD_DEFAULT != 230400 && \
    SERIAL_B_BAUD_DEFAULT != 460800 && SERIAL_B_BAUD_DEFAULT != 921600 && SERIAL_B_BAUD_DEFAULT != OFF
  #warning "Configuration (Config.h): Setting SERIAL_B_BAUD_DEFAULT unknown, use OFF, 9600, 19200, 38400, 57600, 115200, 230400, 460800, or 921600 (baud.)"
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

#if SERIAL_C_BAUD_DEFAULT != 9600 && SERIAL_C_BAUD_DEFAULT != 19200 && SERIAL_C_BAUD_DEFAULT != 38400 && \
    SERIAL_C_BAUD_DEFAULT != 57600 && SERIAL_C_BAUD_DEFAULT != 115200 && SERIAL_C_BAUD_DEFAULT != 230400 && \
    SERIAL_C_BAUD_DEFAULT != 460800 && SERIAL_C_BAUD_DEFAULT != 921600 && SERIAL_C_BAUD_DEFAULT != OFF
  #warning "Configuration (Config.h): Setting SERIAL_C_BAUD_DEFAULT unknown, use OFF, 9600, 19200, 38400, 57600, 115200, 230400, 460800, or 921600 (baud.)"
#endif

#if SERIAL_D_BAUD_DEFAULT != 9600 && SERIAL_D_BAUD_DEFAULT != 19200 && SERIAL_D_BAUD_DEFAULT != 38400 && \
    SERIAL_D_BAUD_DEFAULT != 57600 && SERIAL_D_BAUD_DEFAULT != 115200 && SERIAL_D_BAUD_DEFAULT != 230400 && \
    SERIAL_D_BAUD_DEFAULT != 460800 && SERIAL_D_BAUD_DEFAULT != 921600 && SERIAL_D_BAUD_DEFAULT != OFF
  #warning "Configuration (Config.h): Setting SERIAL_D_BAUD_DEFAULT unknown, use OFF, 9600, 19200, 38400, 57600, 115200, 230400, 460800, or 921600 (baud.)"
#endif

#if SERIAL_E_BAUD_DEFAULT != 9600 && SERIAL_E_BAUD_DEFAULT != 19200 && SERIAL_E_BAUD_DEFAULT != 38400 && \
    SERIAL_E_BAUD_DEFAULT != 57600 && SERIAL_E_BAUD_DEFAULT != 115200 && SERIAL_E_BAUD_DEFAULT != 230400 && \
    SERIAL_E_BAUD_DEFAULT != 460800 && SERIAL_E_BAUD_DEFAULT != 921600 && SERIAL_E_BAUD_DEFAULT != OFF
  #warning "Configuration (Config.h): Setting SERIAL_E_BAUD_DEFAULT unknown, use OFF, 9600, 19200, 38400, 57600, 115200, 230400, 460800, or 921600 (baud.)"
#endif

#if SERIAL_RADIO != OFF && SERIAL_RADIO != BLUETOOTH && SERIAL_RADIO != WIFI_ACCESS_POINT && SERIAL_RADIO != WIFI_STATION
  #warning "Configuration (Config.h): Setting SERIAL_RADIO unknown, use OFF, BLUETOOTH, WIFI_ACCESS_POINT, or WIFI_STATION"
#endif

#if STATUS_LED != OFF && STATUS_LED != ON
  #error "Configuration (Config.h): Setting STATUS_LED unknown, use OFF or ON."
#endif

#if RETICLE_LED_DEFAULT != OFF && (RETICLE_LED_DEFAULT < 0 || RETICLE_LED_DEFAULT > 255)
  #error "Configuration (Config.h): Setting RETICLE_LED_DEFAULT unknown, use OFF or 0 to 255."
#endif

#if RETICLE_LED_MEMORY != OFF && RETICLE_LED_MEMORY != ON
  #error "Configuration (Config.h): Setting RETICLE_LED_MEMORY unknown, use OFF or ON."
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
    (AXIS1_DRIVER_MODEL < STEP_DIR_DRIVER_FIRST || AXIS1_DRIVER_MODEL > STEP_DIR_DRIVER_LAST) && \
    (AXIS1_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS1_DRIVER_MODEL > SERVO_DRIVER_LAST) && \
    (AXIS1_DRIVER_MODEL < MOTOR_DRIVER_FIRST || AXIS1_DRIVER_MODEL > MOTOR_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS1_DRIVER_MODEL unknown, use OFF or a valid DRIVER (from Constants.h)"
#endif

#if AXIS1_DRIVER_STATUS != OFF && AXIS1_DRIVER_STATUS != ON && AXIS1_DRIVER_STATUS != HIGH && AXIS1_DRIVER_STATUS != LOW
  #error "Configuration (Config.h): Setting AXIS1_DRIVER_STATUS unknown, use OFF or a valid driver status."
#endif

#ifdef AXIS1_STEP_DIR_PRESENT
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
  #if AXIS1_DRIVER_IHOLD != OFF && (AXIS1_DRIVER_IHOLD < 0 || AXIS1_DRIVER_IHOLD > 3000)
    #error "Configuration (Config.h): Setting AXIS1_DRIVER_IHOLD unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS1_DRIVER_IGOTO != OFF && (AXIS1_DRIVER_IGOTO < 0 || AXIS1_DRIVER_IGOTO > 3000)
    #error "Configuration (Config.h): Setting AXIS1_DRIVER_IGOTO unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
#endif

#ifdef AXIS1_SERVO_PRESENT
  #if AXIS1_ENCODER < ENC_FIRST || AXIS1_ENCODER > ENC_LAST
    #error "Configuration (Config.h): Setting AXIS1_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
  #endif
#endif

#if AXIS1_REVERSE != ON && AXIS1_REVERSE != OFF
  #error "Configuration (Config.h): Setting AXIS1_REVERSE unknown, use OFF or ON."
#endif

#if AXIS1_POWER_DOWN != ON && AXIS1_POWER_DOWN != OFF
  #error "Configuration (Config.h): Setting AXIS1_POWER_DOWN unknown, use OFF or ON."
#endif

#if AXIS1_SECTOR_GEAR != ON && AXIS1_SECTOR_GEAR != OFF
  #error "Configuration (Config.h): Setting AXIS1_SECTOR_GEAR unknown, use OFF or ON."
#endif

#if AXIS1_SECTOR_GEAR == ON
  #if MOUNT_SUBTYPE != GEM && MOUNT_SUBTYPE != FORK
    #error "Configuration (Config.h): Setting MOUNT_TYPE is not compatible with AXIS1_SECTOR_GEAR ON  (from Constants.h)"
  #endif
  #if (AXIS1_SENSE_HOME) != OFF && (AXIS1_SENSE_HOME_OFFSET) != 0
    #error "Configuration (Config.h): Enabling AXIS1_SECTOR_GEAR and AXIS1_SENSE_HOME requires an AXIS1_SENSE_HOME_OFFSET of 0."
  #endif
  #if (AXIS2_SENSE_HOME) != OFF && AXIS2_TANGENT_ARM == OFF
    #error "Configuration (Config.h): Enabling AXIS1_SECTOR_GEAR requires AXIS2_SENSE_HOME to be OFF (except for tangent arm Dec mounts.)"
  #endif
#else
  #if AXIS2_TANGENT_ARM == OFF && ((AXIS1_SENSE_HOME) == OFF && (AXIS2_SENSE_HOME) != OFF)
    #error "Configuration (Config.h): Enabling AXIS2_SENSE_HOME requires enabling AXIS1_SENSE_HOME or AXIS2_TANGENT_ARM."
  #endif
  #if AXIS1_LIMIT_MIN < -360 || AXIS1_LIMIT_MIN > -90
    #error "Configuration (Config.h): Setting AXIS1_LIMIT_MIN unknown, use value in the range -90 to -360."
  #endif
  #if AXIS1_LIMIT_MAX < 90 || AXIS1_LIMIT_MAX > 360
    #error "Configuration (Config.h): Setting AXIS1_LIMIT_MAX unknown, use value in the range 90 to 360."
  #endif
#endif

#if (AXIS1_SENSE_HOME) != OFF && (AXIS1_SENSE_HOME) < 0
  #error "Configuration (Config.h): Setting AXIS1_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS1_SENSE_LIMIT_MIN) != OFF && (AXIS1_SENSE_LIMIT_MIN) < 0
  #error "Configuration (Config.h): Setting AXIS1_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS1_SENSE_LIMIT_MAX) != OFF && (AXIS1_SENSE_LIMIT_MAX) < 0
  #error "Configuration (Config.h): Setting AXIS1_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

// AXIS2 DEC/ALT
#if AXIS2_DRIVER_MODEL != OFF && \
    (AXIS2_DRIVER_MODEL < STEP_DIR_DRIVER_FIRST || AXIS2_DRIVER_MODEL > STEP_DIR_DRIVER_LAST) && \
    (AXIS2_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS2_DRIVER_MODEL > SERVO_DRIVER_LAST) && \
    (AXIS2_DRIVER_MODEL < MOTOR_DRIVER_FIRST || AXIS2_DRIVER_MODEL > MOTOR_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS2_DRIVER_MODEL unknown, use a valid DRIVER (from Constants.h)"
#endif

#if AXIS2_DRIVER_STATUS != OFF && AXIS2_DRIVER_STATUS != ON && AXIS2_DRIVER_STATUS != HIGH && AXIS2_DRIVER_STATUS != LOW
  #error "Configuration (Config.h): Setting AXIS2_DRIVER_STATUS unknown, use OFF or a valid driver status."
#endif

#ifdef AXIS2_STEP_DIR_PRESENT
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
  #if AXIS2_DRIVER_IHOLD != OFF && (AXIS2_DRIVER_IHOLD < 0 || AXIS2_DRIVER_IHOLD > 3000)
    #error "Configuration (Config.h): Setting AXIS2_DRIVER_IHOLD unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS2_DRIVER_IGOTO != OFF && (AXIS2_DRIVER_IGOTO < 0 || AXIS2_DRIVER_IGOTO > 3000)
    #error "Configuration (Config.h): Setting AXIS2_DRIVER_IGOTO unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
#endif

#ifdef AXIS2_SERVO_PRESENT
  #if AXIS2_ENCODER < ENC_FIRST || AXIS2_ENCODER > ENC_LAST
    #error "Configuration (Config.h): Setting AXIS2_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
  #endif
#endif

#if AXIS2_REVERSE != ON && AXIS2_REVERSE != OFF
  #error "Configuration (Config.h): Setting AXIS2_REVERSE unknown, use OFF or ON."
#endif

#if AXIS2_POWER_DOWN != ON && AXIS2_POWER_DOWN != OFF
  #error "Configuration (Config.h): Setting AXIS2_POWER_DOWN unknown, use OFF or ON."
#endif

#if AXIS2_LIMIT_MIN < -90 || AXIS2_LIMIT_MIN > 0
  #error "Configuration (Config.h): Setting AXIS2_LIMIT_MIN unknown, use value in the range -90 to 0."
#endif

#if AXIS2_LIMIT_MAX < 0 || AXIS2_LIMIT_MAX > 90
  #error "Configuration (Config.h): Setting AXIS2_LIMIT_MAX unknown, use value in the range 0 to 90."
#endif

#if (AXIS2_SENSE_HOME) != OFF && (AXIS2_SENSE_HOME) < 0
  #error "Configuration (Config.h): Setting AXIS2_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS2_SENSE_LIMIT_MIN) != OFF && (AXIS2_SENSE_LIMIT_MIN) < 0
  #error "Configuration (Config.h): Setting AXIS2_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS2_SENSE_LIMIT_MAX) != OFF && (AXIS2_SENSE_LIMIT_MAX) < 0
  #error "Configuration (Config.h): Setting AXIS2_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if MOUNT_SUBTYPE < MOUNT_SUBTYPE_FIRST || MOUNT_SUBTYPE > MOUNT_SUBTYPE_LAST
  #error "Configuration (Config.h): Setting MOUNT_TYPE unknown, use a valid MOUNT TYPE (from Constants.h)"
#endif

#if AXIS2_TANGENT_ARM != ON && AXIS2_TANGENT_ARM != OFF
  #error "Configuration (Config.h): Setting AXIS2_TANGENT_ARM unknown, use OFF or ON."
#endif

#if AXIS2_TANGENT_ARM == ON
  #if MOUNT_SUBTYPE != GEM && MOUNT_SUBTYPE != FORK
    #error "Configuration (Config.h): Setting MOUNT_TYPE is not compatible with AXIS2_TANGENT_ARM ON  (from Constants.h)"
  #endif
  #if (AXIS2_SENSE_HOME) != OFF && (AXIS2_SENSE_HOME_OFFSET) != 0
    #error "Configuration (Config.h): Enabling AXIS2_TANGENT_ARM and AXIS2_SENSE_HOME requires an AXIS2_SENSE_HOME_OFFSET of 0."
  #endif
  #if (AXIS1_SENSE_HOME) != OFF && AXIS1_SECTOR_GEAR == OFF
    #error "Configuration (Config.h): Enabling AXIS2_TANGENT_ARM requires AXIS1_SENSE_HOME to be OFF (except for sector gear RA mounts.)"
  #endif
#else
  #if (AXIS2_SENSE_HOME) != OFF && (AXIS1_SENSE_HOME) == OFF
    #error "Configuration (Config.h): Enabling AXIS2_SENSE_HOME requires enabling AXIS1_SENSE_HOME or AXIS2_TANGENT_ARM."
  #endif
#endif

#if AXIS2_TANGENT_ARM_CORRECTION != ON && AXIS2_TANGENT_ARM_CORRECTION != OFF
  #error "Configuration (Config.h): Setting AXIS2_TANGENT_ARM_CORRECTION unknown, use OFF or ON."
#endif

#if MOUNT_ALTERNATE_ORIENTATION != OFF && MOUNT_ALTERNATE_ORIENTATION != ON
  #error "Configuration (Config.h): Setting MOUNT_ALTERNATE_ORIENTATION unknown, use ON or OFF"
#endif

#if MOUNT_COORDS < MOUNT_COORDS_FIRST && MOUNT_COORDS > MOUNT_COORDS_LAST
  #error "Configuration (Config.h): Setting MOUNT_COORDS unknown, use a valid MOUNT COORDS (from Constants.h)"
#endif

#if MOUNT_COORDS_MEMORY != ON && MOUNT_COORDS_MEMORY != OFF
  #error "Configuration (Config.h): Setting MOUNT_COORDS_MEMORY unknown, use ON or OFF"
#endif

#if MOUNT_ENABLE_IN_STANDBY != ON && MOUNT_ENABLE_IN_STANDBY != OFF
  #error "Configuration (Config.h): Setting MOUNT_ENABLE_IN_STANDBY unknown, use ON or OFF"
#endif

#if ALIGN_MAX_STARS != AUTO && (ALIGN_MAX_STARS < 1 && ALIGN_MAX_STARS > 9)
  #error "Configuration (Config.h): Setting ALIGN_MAX_STARS unknown, use AUTO or a value from 1 to 9."
#endif

// TIME AND LOCATION
#if TIME_LOCATION_SOURCE < TLS_FIRST && TIME_LOCATION_SOURCE > TLS_LAST
  #error "Configuration (Config.h): Setting TIME_LOCATION_SOURCE unknown, use OFF or valid TIME LOCATION SOURCE (from Constants.h)"
#endif

#if TIME_LOCATION_SOURCE_FALLBACK != OFF
  #if (TIME_LOCATION_SOURCE_FALLBACK < TLS_FIRST && TIME_LOCATION_SOURCE_FALLBACK > TLS_LAST) || \
      TIME_LOCATION_SOURCE_FALLBACK == GPS || TIME_LOCATION_SOURCE_FALLBACK == NTP || \
      TIME_LOCATION_SOURCE_FALLBACK == TIME_LOCATION_SOURCE
    #error "Configuration (Config.h): Setting TIME_LOCATION_SOURCE_FALLBACK unknown, use OFF or valid alternate TIME LOCATION SOURCE (except GPS or NTP, from Constants.h)"
  #endif
#endif

#if (TIME_LOCATION_PPS_SENSE) != OFF && \
    (TIME_LOCATION_PPS_SENSE) != LOW && \
    (TIME_LOCATION_PPS_SENSE) != HIGH && \
    (TIME_LOCATION_PPS_SENSE) != BOTH
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

#if GUIDE_SEPARATE_PULSE_RATE != ON && GUIDE_SEPARATE_PULSE_RATE != OFF
  #error "Configuration (Config.h): Setting GUIDE_SEPARATE_PULSE_RATE unknown, use OFF or ON."
#endif

// SENSORS
#if (LIMIT_SENSE) != OFF && (LIMIT_SENSE) < 0
  #error "Configuration (Config.h): Setting LIMIT_SENSE unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

// PEC (max steps per degree 360000 * 360 degrees)
#if PEC_STEPS_PER_WORM_ROTATION < 0 || PEC_STEPS_PER_WORM_ROTATION > 129600000
  #error "Configuration (Config.h): Setting PEC_STEPS_PER_WORM_ROTATION unknown, use the value 0 to disable or 1 to 129600000 (steps.)"
#endif

#if (PEC_SENSE) != OFF && (PEC_SENSE) < 0
  #error "Configuration (Config.h): Setting PEC_SENSE unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if PEC_BUFFER_SIZE_LIMIT < 0 || PEC_BUFFER_SIZE_LIMIT > 30000
  #error "Configuration (Config.h): Setting PEC_BUFFER_SIZE_LIMIT unknown, use the value 0 to disable or 1 to 30000 (seconds.)"
#endif

// SLEWING BEHAVIOUR
#if GOTO_FEATURE != ON && GOTO_FEATURE != OFF
  #error "Configuration (Config.h): Setting GOTO_FEATURE unknown, use OFF or ON."
#endif

#if SLEW_RATE_MEMORY != ON && SLEW_RATE_MEMORY != OFF
  #error "Configuration (Config.h): Setting SLEW_RATE_MEMORY unknown, use OFF or ON."
#endif

// TRACKING BEHAVIOUR
#if TRACK_AUTOSTART != ON && TRACK_AUTOSTART != OFF
  #error "Configuration (Config.h): Setting TRACK_AUTOSTART unknown, use OFF or ON."
#endif

#if TRACK_COMPENSATION_DEFAULT != OFF && (TRACK_COMPENSATION_DEFAULT < COMPENSATED_TRACKING_FIRST || TRACK_COMPENSATION_DEFAULT > COMPENSATED_TRACKING_LAST)
  #error "Configuration (Config.h): Setting TRACK_COMPENSATION_DEFAULT unknown, use OFF or REFRACTION or REFRACTION_DUAL or MODEL or MODEL_DUAL."
#endif

#if TRACK_COMPENSATION_MEMORY != ON && TRACK_COMPENSATION_MEMORY != OFF
  #error "Configuration (Config.h): Setting TRACK_COMPENSATION_MEMORY unknown, use OFF or ON."
#endif

#if TRACK_BACKLASH_RATE < 2 && TRACK_BACKLASH_RATE > 100
  #error "Configuration (Config.h): Setting TRACK_BACKLASH_RATE unknown, use a value between 2 and 100 (x Sidereal.)"
#endif

// GOTO_FEATURE CHECKS
#if GOTO_FEATURE == OFF && TRACK_BACKLASH_RATE > 20
  #error "Configuration (Config.h): Setting TRACK_BACKLASH_RATE must be <= 20 when GOTO_FEATURE is OFF."
#endif

#if GOTO_FEATURE == OFF && (MOUNT_TYPE == GEM_TA || MOUNT_TYPE == GEM_TAC)
  #error "Configuration (Config.h): Setting GEM_TA/GEM_TAC not available when GOTO_FEATURE is OFF."
#endif

#if GOTO_FEATURE == OFF && (MOUNT_TYPE == FORK_TA || MOUNT_TYPE == FORK_TAC)
  #error "Configuration (Config.h): Setting FORK_TA/FORK_TAC not available when GOTO_FEATURE is OFF."
#endif

// PIER SIDE BEHAVIOUR
#if MFLIP_SKIP_HOME != ON && MFLIP_SKIP_HOME != OFF
  #error "Configuration (Config.h): Setting MFLIP_SKIP_HOME unknown, use OFF or ON."
#endif

#if MFLIP_PAUSE_HOME_DEFAULT != ON && MFLIP_PAUSE_HOME_DEFAULT != OFF
  #error "Configuration (Config.h): Setting MFLIP_PAUSE_HOME_DEFAULT unknown, use OFF or ON."
#endif

#if MFLIP_PAUSE_HOME_MEMORY != ON && MFLIP_PAUSE_HOME_MEMORY != OFF
  #error "Configuration (Config.h): Setting MFLIP_PAUSE_HOME_MEMORY unknown, use OFF or ON."
#endif

#if MFLIP_AUTOMATIC_DEFAULT != ON && MFLIP_AUTOMATIC_DEFAULT != OFF
  #error "Configuration (Config.h): Setting MFLIP_AUTOMATIC_DEFAULT unknown, use OFF or ON."
#endif

#if MFLIP_AUTOMATIC_MEMORY != ON && MFLIP_AUTOMATIC_MEMORY != OFF
  #error "Configuration (Config.h): Setting MFLIP_AUTOMATIC_MEMORY unknown, use OFF or ON."
#endif

#if PIER_SIDE_SYNC_CHANGE_SIDES != ON && PIER_SIDE_SYNC_CHANGE_SIDES != OFF
  #error "Configuration (Config.h): Setting PIER_SIDE_SYNC_CHANGE_SIDES unknown, use OFF or ON."
#endif

#if PIER_SIDE_PREFERRED_DEFAULT < PIER_SIDE_FIRST && PIER_SIDE_PREFERRED_DEFAULT > PIER_SIDE_LAST
  #error "Configuration (Config.h): Setting PIER_SIDE_PREFERRED_DEFAULT unknown, use EAST or WEST or BEST."
#endif

#if PIER_SIDE_PREFERRED_MEMORY != OFF && PIER_SIDE_PREFERRED_MEMORY != ON
  #error "Configuration (Config.h): Setting PIER_SIDE_PREFERRED_MEMORY unknown, use ON or OFF."
#endif

// PARKING BEHAVIOUR
#if PARK_STRICT != ON && PARK_STRICT != OFF
  #error "Configuration (Config.h): Setting PARK_STRICT unknown, use OFF or ON."
#endif

// ROTATOR ---------------------------------------

// AXIS3 ROTATOR
#if AXIS3_DRIVER_MODEL != OFF && \
    (AXIS3_DRIVER_MODEL < STEP_DIR_DRIVER_FIRST || AXIS3_DRIVER_MODEL > STEP_DIR_DRIVER_LAST) && \
    (AXIS3_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS3_DRIVER_MODEL > SERVO_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS3_DRIVER_MODEL unknown, use OFF or a valid DRIVER (from Constants.h)"
#endif

#if AXIS3_DRIVER_STATUS != OFF && AXIS3_DRIVER_STATUS != ON && AXIS3_DRIVER_STATUS != HIGH && AXIS3_DRIVER_STATUS != LOW
  #error "Configuration (Config.h): Setting AXIS3_DRIVER_STATUS unknown, use OFF or valid driver status."
#endif

#ifdef AXIS3_STEP_DIR_PRESENT
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
  #if AXIS3_DRIVER_IRUN != OFF && (AXIS3_DRIVER_IRUN < 0 || AXIS3_DRIVER_IRUN > 3000)
    #error "Configuration (Config.h): Setting AXIS3_DRIVER_IRUN unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS3_DRIVER_IHOLD != OFF && (AXIS3_DRIVER_IHOLD < 0 || AXIS3_DRIVER_IHOLD > 3000)
    #error "Configuration (Config.h): Setting AXIS3_DRIVER_IHOLD unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS3_DRIVER_IGOTO != OFF && (AXIS3_DRIVER_IGOTO < 0 || AXIS3_DRIVER_IGOTO > 3000)
    #error "Configuration (Config.h): Setting AXIS3_DRIVER_IGOTO unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if (AXIS3_DRIVER_IRUN > 1000 || AXIS3_DRIVER_IHOLD > 1000 || AXIS3_DRIVER_IGOTO > 1000)
    #warning "Configuration (Config.h): Setting AXIS3_DRIVER_IHOLD or _IRUN or _IGOTO > 1000 (mA) this Axis on many boards is not designed to operate at high current"
  #endif
#endif

#ifdef AXIS3_SERVO_PRESENT
  #if AXIS3_ENCODER != OFF && (AXIS3_ENCODER < ENC_FIRST || AXIS3_ENCODER > ENC_LAST)
    #error "Configuration (Config.h): Setting AXIS3_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
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

#if (AXIS3_SENSE_HOME) != OFF && (AXIS3_SENSE_HOME) < 0
  #error "Configuration (Config.h): Setting AXIS3_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS3_SENSE_LIMIT_MIN) != OFF && (AXIS3_SENSE_LIMIT_MIN) < 0
  #error "Configuration (Config.h): Setting AXIS3_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS3_SENSE_LIMIT_MAX) != OFF && (AXIS3_SENSE_LIMIT_MAX) < 0
  #error "Configuration (Config.h): Setting AXIS3_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

// FOCUSER ---------------------------------------

// AXIS4 FOCUSER
#if AXIS4_DRIVER_MODEL != OFF && \
    (AXIS4_DRIVER_MODEL < STEP_DIR_DRIVER_FIRST || AXIS4_DRIVER_MODEL > STEP_DIR_DRIVER_LAST) && \
    (AXIS4_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS4_DRIVER_MODEL > SERVO_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS4_DRIVER_MODEL unknown, use OFF or a valid DRIVER (from Constants.h)"
#endif

#if AXIS4_SLEW_RATE_MINIMUM < 5 || AXIS4_SLEW_RATE_MINIMUM > 200
  #error "Configuration (Config.h): Setting AXIS4_SLEW_RATE_MINIMUM out of range, use a value between 5 and 200"
#endif

#if AXIS4_SLEW_RATE_BASE_DESIRED < 200 || AXIS4_SLEW_RATE_BASE_DESIRED > 5000
  #error "Configuration (Config.h): Setting AXIS4_SLEW_RATE_BASE_DESIRED out of range, use a value between 200 and 5000"
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

#if AXIS4_HOME_DEFAULT != MINIMUM && AXIS4_HOME_DEFAULT != MIDDLE && AXIS4_HOME_DEFAULT != MAXIMUM && (AXIS4_HOME_DEFAULT < 0 || AXIS4_HOME_DEFAULT > 500000)
  #error "Configuration (Config.h): Setting AXIS4_HOME_DEFAULT unknown, use MINIMUM (zero) or MIDDLE (half travel) or MAXIMUM (full travel) or the position in microns (0 to 500000.)"
#endif 

#if (AXIS4_SENSE_HOME) != OFF && (AXIS4_SENSE_HOME) < 0
  #error "Configuration (Config.h): Setting AXIS4_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS4_SENSE_LIMIT_MIN) != OFF && (AXIS4_SENSE_LIMIT_MIN) < 0
  #error "Configuration (Config.h): Setting AXIS4_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS4_SENSE_LIMIT_MAX) != OFF && (AXIS4_SENSE_LIMIT_MAX) < 0
  #error "Configuration (Config.h): Setting AXIS4_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#ifdef AXIS4_STEP_DIR_PRESENT
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
  #if AXIS4_DRIVER_IRUN != OFF && (AXIS4_DRIVER_IRUN < 0 || AXIS4_DRIVER_IRUN > 3000)
    #error "Configuration (Config.h): Setting AXIS4_DRIVER_IRUN unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS4_DRIVER_IHOLD != OFF && (AXIS4_DRIVER_IHOLD < 0 || AXIS4_DRIVER_IHOLD > 3000)
    #error "Configuration (Config.h): Setting AXIS4_DRIVER_IHOLD unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS4_DRIVER_IGOTO != OFF && (AXIS4_DRIVER_IGOTO < 0 || AXIS4_DRIVER_IGOTO > 3000)
    #error "Configuration (Config.h): Setting AXIS4_DRIVER_IGOTO unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if (AXIS4_DRIVER_IRUN > 1000 || AXIS4_DRIVER_IHOLD > 1000 || AXIS4_DRIVER_IGOTO > 1000)
    #warning "Configuration (Config.h): Setting AXIS4_DRIVER_IHOLD or _IRUN or _IGOTO > 1000 (mA) this Axis on many boards is not designed to operate at high current"
  #endif
#endif

#ifdef AXIS4_SERVO_PRESENT
  #if AXIS4_ENCODER < ENC_FIRST || AXIS4_ENCODER > ENC_LAST
    #error "Configuration (Config.h): Setting AXIS4_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
  #endif
#endif

// AXIS5 FOCUSER
#if AXIS5_DRIVER_MODEL != OFF && \
    (AXIS5_DRIVER_MODEL < STEP_DIR_DRIVER_FIRST || AXIS5_DRIVER_MODEL > STEP_DIR_DRIVER_LAST) && \
    (AXIS5_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS5_DRIVER_MODEL > SERVO_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS5_DRIVER_MODEL unknown, use OFF or a valid DRIVER (from Constants.h)"
#endif

#if AXIS5_SLEW_RATE_MINIMUM < 5 || AXIS5_SLEW_RATE_MINIMUM > 200
  #error "Configuration (Config.h): Setting AXIS5_SLEW_RATE_MINIMUM out of range, use a value between 5 and 200"
#endif

#if AXIS5_SLEW_RATE_BASE_DESIRED < 200 || AXIS5_SLEW_RATE_BASE_DESIRED > 5000
  #error "Configuration (Config.h): Setting AXIS5_SLEW_RATE_BASE_DESIRED out of range, use a value between 200 and 5000"
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

#if AXIS5_HOME_DEFAULT != MINIMUM && AXIS5_HOME_DEFAULT != MIDDLE && AXIS5_HOME_DEFAULT != MAXIMUM && (AXIS5_HOME_DEFAULT < 0 || AXIS5_HOME_DEFAULT > 500000)
  #error "Configuration (Config.h): Setting AXIS5_HOME_DEFAULT unknown, use MINIMUM (zero) or MIDDLE (half travel) or MAXIMUM (full travel) or the position in microns (0 to 500000.)"
#endif 

#if (AXIS5_SENSE_HOME) != OFF && (AXIS5_SENSE_HOME) < 0
  #error "Configuration (Config.h): Setting AXIS5_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS5_SENSE_LIMIT_MIN) != OFF && (AXIS5_SENSE_LIMIT_MIN) < 0
  #error "Configuration (Config.h): Setting AXIS5_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS5_SENSE_LIMIT_MAX) != OFF && (AXIS5_SENSE_LIMIT_MAX) < 0
  #error "Configuration (Config.h): Setting AXIS5_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#ifdef AXIS5_STEP_DIR_PRESENT
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
  #if AXIS5_DRIVER_IRUN != OFF && (AXIS5_DRIVER_IRUN < 0 || AXIS5_DRIVER_IRUN > 3000)
    #error "Configuration (Config.h): Setting AXIS5_DRIVER_IRUN unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS5_DRIVER_IHOLD != OFF && (AXIS5_DRIVER_IHOLD < 0 || AXIS5_DRIVER_IHOLD > 3000)
    #error "Configuration (Config.h): Setting AXIS5_DRIVER_IHOLD unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS5_DRIVER_IGOTO != OFF && (AXIS5_DRIVER_IGOTO < 0 || AXIS5_DRIVER_IGOTO > 3000)
    #error "Configuration (Config.h): Setting AXIS5_DRIVER_IGOTO unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if (AXIS5_DRIVER_IRUN > 1000 || AXIS5_DRIVER_IHOLD > 1000 || AXIS5_DRIVER_IGOTO > 1000)
    #warning "Configuration (Config.h): Setting AXIS5_DRIVER_IHOLD or _IRUN or _IGOTO > 1000 (mA) this Axis on many boards is not designed to operate at high current"
  #endif
#endif

#ifdef AXIS5_SERVO_PRESENT
  #if AXIS5_ENCODER < ENC_FIRST || AXIS5_ENCODER > ENC_LAST
    #error "Configuration (Config.h): Setting AXIS5_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
  #endif
#endif

// AXIS6 FOCUSER
#if AXIS6_DRIVER_MODEL != OFF && \
    (AXIS6_DRIVER_MODEL < STEP_DIR_DRIVER_FIRST || AXIS6_DRIVER_MODEL > STEP_DIR_DRIVER_LAST) && \
    (AXIS6_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS6_DRIVER_MODEL > SERVO_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS6_DRIVER_MODEL unknown, use OFF or a valid DRIVER (from Constants.h)"
#endif

#if AXIS6_SLEW_RATE_MINIMUM < 5 || AXIS6_SLEW_RATE_MINIMUM > 200
  #error "Configuration (Config.h): Setting AXIS6_SLEW_RATE_MINIMUM out of range, use a value between 5 and 200"
#endif

#if AXIS6_SLEW_RATE_BASE_DESIRED < 200 || AXIS6_SLEW_RATE_BASE_DESIRED > 5000
  #error "Configuration (Config.h): Setting AXIS6_SLEW_RATE_BASE_DESIRED out of range, use a value between 200 and 5000"
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

#if AXIS6_HOME_DEFAULT != MINIMUM && AXIS6_HOME_DEFAULT != MIDDLE && AXIS6_HOME_DEFAULT != MAXIMUM && (AXIS6_HOME_DEFAULT < 0 || AXIS6_HOME_DEFAULT > 500000)
  #error "Configuration (Config.h): Setting AXIS6_HOME_DEFAULT unknown, use MINIMUM (zero) or MIDDLE (half travel) or MAXIMUM (full travel) or the position in microns (0 to 500000.)"
#endif 

#if (AXIS6_SENSE_HOME) != OFF && (AXIS6_SENSE_HOME) < 0
  #error "Configuration (Config.h): Setting AXIS6_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS6_SENSE_LIMIT_MIN) != OFF && (AXIS6_SENSE_LIMIT_MIN) < 0
  #error "Configuration (Config.h): Setting AXIS6_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS6_SENSE_LIMIT_MAX) != OFF && (AXIS6_SENSE_LIMIT_MAX) < 0
  #error "Configuration (Config.h): Setting AXIS6_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#ifdef AXIS6_STEP_DIR_PRESENT
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
  #if AXIS6_DRIVER_IRUN != OFF && (AXIS6_DRIVER_IRUN < 0 || AXIS6_DRIVER_IRUN > 3000)
    #error "Configuration (Config.h): Setting AXIS6_DRIVER_IRUN unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS6_DRIVER_IHOLD != OFF && (AXIS6_DRIVER_IHOLD < 0 || AXIS6_DRIVER_IHOLD > 3000)
    #error "Configuration (Config.h): Setting AXIS6_DRIVER_IHOLD unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS6_DRIVER_IGOTO != OFF && (AXIS6_DRIVER_IGOTO < 0 || AXIS6_DRIVER_IGOTO > 3000)
    #error "Configuration (Config.h): Setting AXIS6_DRIVER_IGOTO unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
#endif

#ifdef AXIS6_SERVO_PRESENT
  #if AXIS6_ENCODER < ENC_FIRST || AXIS6_ENCODER > ENC_LAST
    #error "Configuration (Config.h): Setting AXIS6_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
  #endif
#endif

// AXIS7 FOCUSER
#if AXIS7_DRIVER_MODEL != OFF && \
    (AXIS7_DRIVER_MODEL < STEP_DIR_DRIVER_FIRST || AXIS7_DRIVER_MODEL > STEP_DIR_DRIVER_LAST) && \
    (AXIS7_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS7_DRIVER_MODEL > SERVO_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS7_DRIVER_MODEL unknown, use OFF or a valid DRIVER (from Constants.h)"
#endif

#if AXIS7_SLEW_RATE_MINIMUM < 5 || AXIS7_SLEW_RATE_MINIMUM > 200
  #error "Configuration (Config.h): Setting AXIS7_SLEW_RATE_MINIMUM out of range, use a value between 5 and 200"
#endif

#if AXIS7_SLEW_RATE_BASE_DESIRED < 200 || AXIS7_SLEW_RATE_BASE_DESIRED > 5000
  #error "Configuration (Config.h): Setting AXIS7_SLEW_RATE_BASE_DESIRED out of range, use a value between 200 and 5000"
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

#if AXIS7_HOME_DEFAULT != MINIMUM && AXIS7_HOME_DEFAULT != MIDDLE && AXIS7_HOME_DEFAULT != MAXIMUM && (AXIS7_HOME_DEFAULT < 0 || AXIS7_HOME_DEFAULT > 500000)
  #error "Configuration (Config.h): Setting AXIS7_HOME_DEFAULT unknown, use MINIMUM (zero) or MIDDLE (half travel) or MAXIMUM (full travel) or the position in microns (0 to 500000.)"
#endif 

#if (AXIS7_SENSE_HOME) != OFF && (AXIS7_SENSE_HOME) < 0
  #error "Configuration (Config.h): Setting AXIS7_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS7_SENSE_LIMIT_MIN) != OFF && (AXIS7_SENSE_LIMIT_MIN) < 0
  #error "Configuration (Config.h): Setting AXIS7_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS7_SENSE_LIMIT_MAX) != OFF && (AXIS7_SENSE_LIMIT_MAX) < 0
  #error "Configuration (Config.h): Setting AXIS7_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#ifdef AXIS7_STEP_DIR_PRESENT
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
  #if AXIS7_DRIVER_IRUN != OFF && (AXIS7_DRIVER_IRUN < 0 || AXIS7_DRIVER_IRUN > 3000)
    #error "Configuration (Config.h): Setting AXIS7_DRIVER_IRUN unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS7_DRIVER_IHOLD != OFF && (AXIS7_DRIVER_IHOLD < 0 || AXIS7_DRIVER_IHOLD > 3000)
    #error "Configuration (Config.h): Setting AXIS7_DRIVER_IHOLD unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS7_DRIVER_IGOTO != OFF && (AXIS7_DRIVER_IGOTO < 0 || AXIS7_DRIVER_IGOTO > 3000)
    #error "Configuration (Config.h): Setting AXIS7_DRIVER_IGOTO unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
#endif

#ifdef AXIS7_SERVO_PRESENT
  #if AXIS7_ENCODER < ENC_FIRST || AXIS7_ENCODER > ENC_LAST
    #error "Configuration (Config.h): Setting AXIS7_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
  #endif
#endif

// AXIS8 FOCUSER
#if AXIS8_DRIVER_MODEL != OFF && \
    (AXIS8_DRIVER_MODEL < STEP_DIR_DRIVER_FIRST || AXIS8_DRIVER_MODEL > STEP_DIR_DRIVER_LAST) && \
    (AXIS8_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS8_DRIVER_MODEL > SERVO_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS8_DRIVER_MODEL unknown, use OFF or a valid DRIVER (from Constants.h)"
#endif

#if AXIS8_SLEW_RATE_MINIMUM < 5 || AXIS8_SLEW_RATE_MINIMUM > 200
  #error "Configuration (Config.h): Setting AXIS8_SLEW_RATE_MINIMUM out of range, use a value between 5 and 200"
#endif

#if AXIS8_SLEW_RATE_BASE_DESIRED < 200 || AXIS8_SLEW_RATE_BASE_DESIRED > 5000
  #error "Configuration (Config.h): Setting AXIS8_SLEW_RATE_BASE_DESIRED out of range, use a value between 200 and 5000"
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

#if AXIS8_HOME_DEFAULT != MINIMUM && AXIS8_HOME_DEFAULT != MIDDLE && AXIS8_HOME_DEFAULT != MAXIMUM && (AXIS8_HOME_DEFAULT < 0 || AXIS8_HOME_DEFAULT > 500000)
  #error "Configuration (Config.h): Setting AXIS8_HOME_DEFAULT unknown, use MINIMUM (zero) or MIDDLE (half travel) or MAXIMUM (full travel) or the position in microns (0 to 500000.)"
#endif 

#if (AXIS8_SENSE_HOME) != OFF && (AXIS8_SENSE_HOME) < 0
  #error "Configuration (Config.h): Setting AXIS8_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS8_SENSE_LIMIT_MIN) != OFF && (AXIS8_SENSE_LIMIT_MIN) < 0
  #error "Configuration (Config.h): Setting AXIS8_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS8_SENSE_LIMIT_MAX) != OFF && (AXIS8_SENSE_LIMIT_MAX) < 0
  #error "Configuration (Config.h): Setting AXIS8_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#ifdef AXIS8_STEP_DIR_PRESENT
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
  #if AXIS8_DRIVER_IRUN != OFF && (AXIS8_DRIVER_IRUN < 0 || AXIS8_DRIVER_IRUN > 3000)
    #error "Configuration (Config.h): Setting AXIS8_DRIVER_IRUN unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS8_DRIVER_IHOLD != OFF && (AXIS8_DRIVER_IHOLD < 0 || AXIS8_DRIVER_IHOLD > 3000)
    #error "Configuration (Config.h): Setting AXIS8_DRIVER_IHOLD unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS8_DRIVER_IGOTO != OFF && (AXIS8_DRIVER_IGOTO < 0 || AXIS8_DRIVER_IGOTO > 3000)
    #error "Configuration (Config.h): Setting AXIS8_DRIVER_IGOTO unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
#endif

#ifdef AXIS8_SERVO_PRESENT
  #if AXIS8_ENCODER < ENC_FIRST || AXIS8_ENCODER > ENC_LAST
    #error "Configuration (Config.h): Setting AXIS8_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
  #endif
#endif

// AXIS9 FOCUSER
#if AXIS9_DRIVER_MODEL != OFF && \
    (AXIS9_DRIVER_MODEL < STEP_DIR_DRIVER_FIRST || AXIS9_DRIVER_MODEL > STEP_DIR_DRIVER_LAST) && \
    (AXIS9_DRIVER_MODEL < SERVO_DRIVER_FIRST || AXIS9_DRIVER_MODEL > SERVO_DRIVER_LAST)
  #error "Configuration (Config.h): Setting AXIS9_DRIVER_MODEL unknown, use OFF or a valid DRIVER (from Constants.h)"
#endif

#if AXIS9_SLEW_RATE_MINIMUM < 5 || AXIS9_SLEW_RATE_MINIMUM > 200
  #error "Configuration (Config.h): Setting AXIS9_SLEW_RATE_MINIMUM out of range, use a value between 5 and 200"
#endif

#if AXIS9_SLEW_RATE_BASE_DESIRED < 200 || AXIS9_SLEW_RATE_BASE_DESIRED > 5000
  #error "Configuration (Config.h): Setting AXIS9_SLEW_RATE_BASE_DESIRED out of range, use a value between 200 and 5000"
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

#if AXIS9_HOME_DEFAULT != MINIMUM && AXIS9_HOME_DEFAULT != MIDDLE && AXIS9_HOME_DEFAULT != MAXIMUM && (AXIS9_HOME_DEFAULT < 0 || AXIS9_HOME_DEFAULT > 500000)
  #error "Configuration (Config.h): Setting AXIS9_HOME_DEFAULT unknown, use MINIMUM (zero) or MIDDLE (half travel) or MAXIMUM (full travel) or the position in microns (0 to 500000.)"
#endif

#if (AXIS9_SENSE_HOME) != OFF && (AXIS9_SENSE_HOME) < 0
  #error "Configuration (Config.h): Setting AXIS9_SENSE_HOME unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS9_SENSE_LIMIT_MIN) != OFF && (AXIS9_SENSE_LIMIT_MIN) < 0
  #error "Configuration (Config.h): Setting AXIS9_SENSE_LIMIT_MIN unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#if (AXIS9_SENSE_LIMIT_MAX) != OFF && (AXIS9_SENSE_LIMIT_MAX) < 0
  #error "Configuration (Config.h): Setting AXIS9_SENSE_LIMIT_MAX unknown, use OFF or HIGH/LOW and HYST() and/or THLD() as described in comments."
#endif

#ifdef AXIS9_STEP_DIR_PRESENT
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
  #if AXIS9_DRIVER_IRUN != OFF && (AXIS9_DRIVER_IRUN < 0 || AXIS9_DRIVER_IRUN > 3000)
    #error "Configuration (Config.h): Setting AXIS9_DRIVER_IRUN unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS9_DRIVER_IHOLD != OFF && (AXIS9_DRIVER_IHOLD < 0 || AXIS9_DRIVER_IHOLD > 3000)
    #error "Configuration (Config.h): Setting AXIS9_DRIVER_IHOLD unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
  #if AXIS9_DRIVER_IGOTO != OFF && (AXIS9_DRIVER_IGOTO < 0 || AXIS9_DRIVER_IGOTO > 3000)
    #error "Configuration (Config.h): Setting AXIS9_DRIVER_IGOTO unknown, use OFF or a value 0 to 3000 (mA.)"
  #endif
#endif

#ifdef AXIS9_SERVO_PRESENT
  #if AXIS9_ENCODER < ENC_FIRST || AXIS9_ENCODER > ENC_LAST
    #error "Configuration (Config.h): Setting AXIS9_ENCODER unknown, use a valid SERVO ENCODER (from Constants.h)"
  #endif
#endif

// GENERAL TEMPERATURE ---------------------------
#if defined(DS1820_DEVICES_PRESENT) && defined(THERMISTOR_DEVICES_PRESENT)
  #error "Configuration (Config.h): Setting DS18B20 devices and THERMISTOR devices can not both be used at the same time, use one or the other"
#endif

// FOCUSER TEMPERATURE ---------------------------
#if FOCUSER_TEMPERATURE != OFF && \
    (FOCUSER_TEMPERATURE & DS_MASK) != DS18B20 && \
    (FOCUSER_TEMPERATURE & DS_MASK) != DS18S20 && \
    (FOCUSER_TEMPERATURE < TEMPERATURE_FIRST || FOCUSER_TEMPERATURE > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FOCUSER_TEMPERATURE unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif

// AUXILIARY FEATURE -----------------------------

#if FEATURE1_PURPOSE != OFF && (FEATURE1_PURPOSE < AUX_FEATURE_PURPOSE_FIRST || FEATURE1_PURPOSE > AUX_FEATURE_PURPOSE_LAST)
  #error "Configuration (Config.h): Setting FEATURE1_PURPOSE unknown, use OFF or AUXILIARY FEATURE purpose (from Constants.h)"
#endif
#if FEATURE1_VALUE_DEFAULT != OFF && FEATURE1_VALUE_DEFAULT != ON && (FEATURE1_VALUE_DEFAULT < 0 || FEATURE1_VALUE_DEFAULT > 255)
  #error "Configuration (Config.h): Setting FEATURE1_VALUE_DEFAULT unknown, use OFF or ON or a value 0 to 255"
#endif
#if FEATURE1_VALUE_MEMORY != OFF && FEATURE1_VALUE_MEMORY != ON
  #error "Configuration (Config.h): Setting FEATURE1_VALUE_MEMORY unknown, use OFF or ON"
#endif

#if FEATURE2_PURPOSE != OFF && (FEATURE2_PURPOSE < AUX_FEATURE_PURPOSE_FIRST || FEATURE2_PURPOSE > AUX_FEATURE_PURPOSE_LAST)
  #error "Configuration (Config.h): Setting FEATURE2_PURPOSE unknown, use OFF or AUXILIARY FEATURE purpose (from Constants.h)"
#endif
#if FEATURE2_VALUE_DEFAULT != OFF && FEATURE2_VALUE_DEFAULT != ON && (FEATURE2_VALUE_DEFAULT < 0 || FEATURE2_VALUE_DEFAULT > 255)
  #error "Configuration (Config.h): Setting FEATURE2_VALUE_DEFAULT unknown, use OFF or ON or a value 0 to 255"
#endif
#if FEATURE2_VALUE_MEMORY != OFF && FEATURE2_VALUE_MEMORY != ON
  #error "Configuration (Config.h): Setting FEATURE2_VALUE_MEMORY unknown, use OFF or ON"
#endif

#if FEATURE3_PURPOSE != OFF && (FEATURE3_PURPOSE < AUX_FEATURE_PURPOSE_FIRST || FEATURE3_PURPOSE > AUX_FEATURE_PURPOSE_LAST)
  #error "Configuration (Config.h): Setting FEATURE3_PURPOSE unknown, use OFF or AUXILIARY FEATURE purpose (from Constants.h)"
#endif
#if FEATURE3_VALUE_DEFAULT != OFF && FEATURE3_VALUE_DEFAULT != ON && (FEATURE3_VALUE_DEFAULT < 0 || FEATURE3_VALUE_DEFAULT > 255)
  #error "Configuration (Config.h): Setting FEATURE3_VALUE_DEFAULT unknown, use OFF or ON or a value 0 to 255"
#endif
#if FEATURE3_VALUE_MEMORY != OFF && FEATURE3_VALUE_MEMORY != ON
  #error "Configuration (Config.h): Setting FEATURE3_VALUE_MEMORY unknown, use OFF or ON"
#endif

#if FEATURE4_PURPOSE != OFF && (FEATURE4_PURPOSE < AUX_FEATURE_PURPOSE_FIRST || FEATURE4_PURPOSE > AUX_FEATURE_PURPOSE_LAST)
  #error "Configuration (Config.h): Setting FEATURE4_PURPOSE unknown, use OFF or AUXILIARY FEATURE purpose (from Constants.h)"
#endif
#if FEATURE4_VALUE_DEFAULT != OFF && FEATURE4_VALUE_DEFAULT != ON && (FEATURE4_VALUE_DEFAULT < 0 || FEATURE4_VALUE_DEFAULT > 255)
  #error "Configuration (Config.h): Setting FEATURE4_VALUE_DEFAULT unknown, use OFF or ON or a value 0 to 255"
#endif
#if FEATURE4_VALUE_MEMORY != OFF && FEATURE4_VALUE_MEMORY != ON
  #error "Configuration (Config.h): Setting FEATURE4_VALUE_MEMORY unknown, use OFF or ON"
#endif

#if FEATURE5_PURPOSE != OFF && (FEATURE5_PURPOSE < AUX_FEATURE_PURPOSE_FIRST || FEATURE5_PURPOSE > AUX_FEATURE_PURPOSE_LAST)
  #error "Configuration (Config.h): Setting FEATURE5_PURPOSE unknown, use OFF or AUXILIARY FEATURE purpose (from Constants.h)"
#endif
#if FEATURE5_VALUE_DEFAULT != OFF && FEATURE5_VALUE_DEFAULT != ON && (FEATURE5_VALUE_DEFAULT < 0 || FEATURE5_VALUE_DEFAULT > 255)
  #error "Configuration (Config.h): Setting FEATURE5_VALUE_DEFAULT unknown, use OFF or ON or a value 0 to 255"
#endif
#if FEATURE5_VALUE_MEMORY != OFF && FEATURE5_VALUE_MEMORY != ON
  #error "Configuration (Config.h): Setting FEATURE5_VALUE_MEMORY unknown, use OFF or ON"
#endif

#if FEATURE6_PURPOSE != OFF && (FEATURE6_PURPOSE < AUX_FEATURE_PURPOSE_FIRST || FEATURE6_PURPOSE > AUX_FEATURE_PURPOSE_LAST)
  #error "Configuration (Config.h): Setting FEATURE6_PURPOSE unknown, use OFF or AUXILIARY FEATURE purpose (from Constants.h)"
#endif
#if FEATURE6_VALUE_DEFAULT != OFF && FEATURE6_VALUE_DEFAULT != ON && (FEATURE6_VALUE_DEFAULT < 0 || FEATURE6_VALUE_DEFAULT > 255)
  #error "Configuration (Config.h): Setting FEATURE6_VALUE_DEFAULT unknown, use OFF or ON or a value 0 to 255"
#endif
#if FEATURE6_VALUE_MEMORY != OFF && FEATURE6_VALUE_MEMORY != ON
  #error "Configuration (Config.h): Setting FEATURE6_VALUE_MEMORY unknown, use OFF or ON"
#endif

#if FEATURE7_PURPOSE != OFF && (FEATURE7_PURPOSE < AUX_FEATURE_PURPOSE_FIRST || FEATURE7_PURPOSE > AUX_FEATURE_PURPOSE_LAST)
  #error "Configuration (Config.h): Setting FEATURE7_PURPOSE unknown, use OFF or AUXILIARY FEATURE purpose (from Constants.h)"
#endif
#if FEATURE7_VALUE_DEFAULT != OFF && FEATURE7_VALUE_DEFAULT != ON && (FEATURE7_VALUE_DEFAULT < 0 || FEATURE7_VALUE_DEFAULT > 255)
  #error "Configuration (Config.h): Setting FEATURE7_VALUE_DEFAULT unknown, use OFF or ON or a value 0 to 255"
#endif
#if FEATURE7_VALUE_MEMORY != OFF && FEATURE7_VALUE_MEMORY != ON
  #error "Configuration (Config.h): Setting FEATURE7_VALUE_MEMORY unknown, use OFF or ON"
#endif

#if FEATURE8_PURPOSE != OFF && (FEATURE8_PURPOSE < AUX_FEATURE_PURPOSE_FIRST || FEATURE8_PURPOSE > AUX_FEATURE_PURPOSE_LAST)
  #error "Configuration (Config.h): Setting FEATURE8_PURPOSE unknown, use OFF or AUXILIARY FEATURE purpose (from Constants.h)"
#endif
#if FEATURE8_VALUE_DEFAULT != OFF && FEATURE8_VALUE_DEFAULT != ON && (FEATURE8_VALUE_DEFAULT < 0 || FEATURE8_VALUE_DEFAULT > 255)
  #error "Configuration (Config.h): Setting FEATURE8_VALUE_DEFAULT unknown, use OFF or ON or a value 0 to 255"
#endif
#if FEATURE8_VALUE_MEMORY != OFF && FEATURE8_VALUE_MEMORY != ON
  #error "Configuration (Config.h): Setting FEATURE8_VALUE_MEMORY unknown, use OFF or ON"
#endif

#if FEATURE1_TEMP != OFF && \
    (FEATURE1_TEMP & DS_MASK) != DS18B20 && \
    (FEATURE1_TEMP & DS_MASK) != DS18S20 && \
    (FEATURE1_TEMP < TEMPERATURE_FIRST || FEATURE1_TEMP > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FEATURE1_TEMP unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif
#if FEATURE2_TEMP != OFF && \
    (FEATURE2_TEMP & DS_MASK) != DS18B20 && \
    (FEATURE2_TEMP & DS_MASK) != DS18S20 && \
    (FEATURE2_TEMP < TEMPERATURE_FIRST || FEATURE2_TEMP > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FEATURE2_TEMP unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif
#if FEATURE3_TEMP != OFF && \
    (FEATURE3_TEMP & DS_MASK) != DS18B20 && \
    (FEATURE3_TEMP & DS_MASK) != DS18S20 && \
    (FEATURE3_TEMP < TEMPERATURE_FIRST || FEATURE3_TEMP > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FEATURE3_TEMP unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif
#if FEATURE4_TEMP != OFF && \
    (FEATURE4_TEMP & DS_MASK) != DS18B20 && \
    (FEATURE4_TEMP & DS_MASK) != DS18S20 && \
    (FEATURE4_TEMP < TEMPERATURE_FIRST || FEATURE4_TEMP > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FEATURE4_TEMP unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif
#if FEATURE5_TEMP != OFF && \
    (FEATURE5_TEMP & DS_MASK) != DS18B20 && \
    (FEATURE5_TEMP & DS_MASK) != DS18S20 && \
    (FEATURE5_TEMP < TEMPERATURE_FIRST || FEATURE5_TEMP > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FEATURE5_TEMP unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif
#if FEATURE6_TEMP != OFF && \
    (FEATURE6_TEMP & DS_MASK) != DS18B20 && \
    (FEATURE6_TEMP & DS_MASK) != DS18S20 && \
    (FEATURE6_TEMP < TEMPERATURE_FIRST || FEATURE6_TEMP > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FEATURE6_TEMP unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif
#if FEATURE7_TEMP != OFF && \
    (FEATURE7_TEMP & DS_MASK) != DS18B20 && \
    (FEATURE7_TEMP & DS_MASK) != DS18S20 && \
    (FEATURE7_TEMP < TEMPERATURE_FIRST || FEATURE7_TEMP > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FEATURE7_TEMP unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif
#if FEATURE8_TEMP != OFF && \
    (FEATURE8_TEMP & DS_MASK) != DS18B20 && \
    (FEATURE8_TEMP & DS_MASK) != DS18S20 && \
    (FEATURE8_TEMP < TEMPERATURE_FIRST || FEATURE8_TEMP > TEMPERATURE_LAST)
  #error "Configuration (Config.h): Setting FEATURE8_TEMP unknown, use OFF or TEMPERATURE device (from Constants.h)"
#endif

#if FEATURE1_PIN != OFF && FEATURE1_PIN < 0
  #error "Configuration (Config.h): Setting FEATURE1_PIN unknown, use OFF or a valid microcontroller pin designation."
#endif
#if FEATURE2_PIN != OFF && FEATURE2_PIN < 0
  #error "Configuration (Config.h): Setting FEATURE2_PIN unknown, use OFF or a valid microcontroller pin designation."
#endif
#if FEATURE3_PIN != OFF && FEATURE3_PIN < 0
  #error "Configuration (Config.h): Setting FEATURE3_PIN unknown, use OFF or a valid microcontroller pin designation."
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

#if FEATURE1_VALUE_DEFAULT != OFF && FEATURE1_VALUE_DEFAULT != ON && (FEATURE1_VALUE_DEFAULT < 0 || FEATURE1_VALUE_DEFAULT > 255)
  #error "Configuration (Config.h): Setting FEATURE1_VALUE_DEFAULT unknown, use OFF, ON or a value from 0 to 255 (for ANALOG_OUT.)"
#endif
#if FEATURE2_VALUE_DEFAULT != OFF && FEATURE2_VALUE_DEFAULT != ON && (FEATURE2_VALUE_DEFAULT < 0 || FEATURE2_VALUE_DEFAULT > 255)
  #error "Configuration (Config.h): Setting FEATURE2_VALUE_DEFAULT unknown, use OFF, ON or a value from 0 to 255 (for ANALOG_OUT.)"
#endif
#if FEATURE3_VALUE_DEFAULT != OFF && FEATURE3_VALUE_DEFAULT != ON && (FEATURE3_VALUE_DEFAULT < 0 || FEATURE3_VALUE_DEFAULT > 255)
  #error "Configuration (Config.h): Setting FEATURE3_VALUE_DEFAULT unknown, use OFF, ON or a value from 0 to 255 (for ANALOG_OUT.)"
#endif
#if FEATURE4_VALUE_DEFAULT != OFF && FEATURE4_VALUE_DEFAULT != ON && (FEATURE4_VALUE_DEFAULT < 0 || FEATURE4_VALUE_DEFAULT > 255)
  #error "Configuration (Config.h): Setting FEATURE4_VALUE_DEFAULT unknown, use OFF, ON or a value from 0 to 255 (for ANALOG_OUT.)"
#endif
#if FEATURE5_VALUE_DEFAULT != OFF && FEATURE5_VALUE_DEFAULT != ON && (FEATURE5_VALUE_DEFAULT < 0 || FEATURE5_VALUE_DEFAULT > 255)
  #error "Configuration (Config.h): Setting FEATURE5_VALUE_DEFAULT unknown, use OFF, ON or a value from 0 to 255 (for ANALOG_OUT.)"
#endif
#if FEATURE6_VALUE_DEFAULT != OFF && FEATURE6_VALUE_DEFAULT != ON && (FEATURE6_VALUE_DEFAULT < 0 || FEATURE6_VALUE_DEFAULT > 255)
  #error "Configuration (Config.h): Setting FEATURE6_VALUE_DEFAULT unknown, use OFF, ON or a value from 0 to 255 (for ANALOG_OUT.)"
#endif
#if FEATURE7_VALUE_DEFAULT != OFF && FEATURE7_VALUE_DEFAULT != ON && (FEATURE7_VALUE_DEFAULT < 0 || FEATURE7_VALUE_DEFAULT > 255)
  #error "Configuration (Config.h): Setting FEATURE7_VALUE_DEFAULT unknown, use OFF, ON or a value from 0 to 255 (for ANALOG_OUT.)"
#endif
#if FEATURE8_VALUE_DEFAULT != OFF && FEATURE8_VALUE_DEFAULT != ON && (FEATURE8_VALUE_DEFAULT < 0 || FEATURE8_VALUE_DEFAULT > 255)
  #error "Configuration (Config.h): Setting FEATURE8_VALUE_DEFAULT unknown, use OFF, ON or a value from 0 to 255 (for ANALOG_OUT.)"
#endif

#if FEATURE1_ON_STATE != HIGH && FEATURE1_ON_STATE != LOW
  #error "Configuration (Config.h): Setting FEATURE1_ON_STATE unknown, use HIGH or LOW."
#endif
#if FEATURE2_ON_STATE != HIGH && FEATURE2_ON_STATE != LOW
  #error "Configuration (Config.h): Setting FEATURE2_ON_STATE unknown, use HIGH or LOW."
#endif
#if FEATURE3_ON_STATE != HIGH && FEATURE3_ON_STATE != LOW
  #error "Configuration (Config.h): Setting FEATURE3_ON_STATE unknown, use HIGH or LOW."
#endif
#if FEATURE4_ON_STATE != HIGH && FEATURE4_ON_STATE != LOW
  #error "Configuration (Config.h): Setting FEATURE4_ON_STATE unknown, use HIGH or LOW."
#endif
#if FEATURE5_ON_STATE != HIGH && FEATURE5_ON_STATE != LOW
  #error "Configuration (Config.h): Setting FEATURE5_ON_STATE unknown, use HIGH or LOW."
#endif
#if FEATURE6_ON_STATE != HIGH && FEATURE6_ON_STATE != LOW
  #error "Configuration (Config.h): Setting FEATURE6_ON_STATE unknown, use HIGH or LOW."
#endif
#if FEATURE7_ON_STATE != HIGH && FEATURE7_ON_STATE != LOW
  #error "Configuration (Config.h): Setting FEATURE7_ON_STATE unknown, use HIGH or LOW."
#endif
#if FEATURE8_ON_STATE != HIGH && FEATURE8_ON_STATE != LOW
  #error "Configuration (Config.h): Setting FEATURE8_ON_STATE unknown, use HIGH or LOW."
#endif
