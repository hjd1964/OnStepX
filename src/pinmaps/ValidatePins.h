// -------------------------------------------------------------------------------------------------
// Validates pinmap model for current configuration
#pragma once

// helps keep track of Aux pin assignments
#define PIN_NOT_ASSIGNED 0
#define PIN_SHARED_I2C 1
#define PIN_DEDICATED 2
#define ASSIGNED_AUX0 PIN_NOT_ASSIGNED
#define ASSIGNED_AUX1 PIN_NOT_ASSIGNED
#define ASSIGNED_AUX2 PIN_NOT_ASSIGNED
#define ASSIGNED_AUX3 PIN_NOT_ASSIGNED
#define ASSIGNED_AUX4 PIN_NOT_ASSIGNED
#define ASSIGNED_AUX5 PIN_NOT_ASSIGNED
#define ASSIGNED_AUX6 PIN_NOT_ASSIGNED
#define ASSIGNED_AUX7 PIN_NOT_ASSIGNED
#define ASSIGNED_AUX8 PIN_NOT_ASSIGNED

// GENERAL PURPOSE PINMAP VALIDATION ---------------

#if AXIS1_DRIVER_STATUS == HIGH || AXIS1_DRIVER_STATUS == LOW
  #if AXIS2_DRIVER_MODEL == TMC2130 || AXIS2_DRIVER_MODEL == TMC5160
    #error "Configuration (Config.h): AXIS1_DRIVER_STATUS LOW/HIGH doesn't make sense when using TMC_SPI stepper drivers"
  #elif !defined(AXIS1_FAULT_PIN)
    #error "Configuration (Config.h): AXIS1_DRIVER_STATUS LOW/HIGH feature isn't supported on this PINMAP"
  #elif AXIS1_DRIVER_MODEL != GENERIC && AXIS1_DRIVER_MODEL != SERVO
    #warning "Configuration (Config.h): AXIS1_DRIVER_STATUS LOW/HIGH use often requires modification of stepper drivers"
  #endif
#endif
#if AXIS2_DRIVER_STATUS == HIGH || AXIS2_DRIVER_STATUS == LOW
  #if AXIS2_DRIVER_MODEL == TMC2130 || AXIS2_DRIVER_MODEL == TMC5160
    #error "Configuration (Config.h): AXIS2_DRIVER_STATUS LOW/HIGH doesn't make sense when using TMC_SPI stepper drivers"
  #elif !defined(AXIS2_FAULT_PIN)
    #error "Configuration (Config.h): AXIS2_DRIVER_STATUS LOW/HIGH feature isn't supported on this PINMAP"
  #elif AXIS2_DRIVER_MODEL != GENERIC && AXIS2_DRIVER_MODEL != SERVO
    #warning "Configuration (Config.h): AXIS2_DRIVER_STATUS LOW/HIGH use often requires modification of stepper drivers"
  #endif
#endif

#if SERIAL_B_ESP_FLASHING == ON  && (!defined(ADDON_GPIO0_PIN) || !defined(ADDON_RESET_PIN))
  #error "Configuration (Config.h): SERIAL_B_ESP_FLASHING not supported for this PINMAP"
#endif

// powering down Dec supported only if EN is available and not shared with Axis1
#if AXIS2_POWER_DOWN == ON && (AXIS2_EN_PIN == OFF || AXIS2_EN_PIN == SHARED_PIN)
  #error "Configuration (Config.h): AXIS2_POWER_DOWN not supported on this PINMAP"
#endif

// focusers/rotator allowed?
#if ROTATOR == ON
  #if AXIS3_STEP_PIN == OFF || AXIS3_DIR_PIN == OFF
    #error "Configuration (Config.h): AXIS3 step/dir interface is not supported on this PINMAP"
  #endif
#endif
#if FOCUSER1 == ON
  #if AXIS4_STEP_PIN == OFF || AXIS4_DIR_PIN == OFF
    #error "Configuration (Config.h): AXIS4 step/dir interface is not supported on this PINMAP"
  #endif
#endif
#if FOCUSER2 == ON
  #if AXIS5_STEP_PIN == OFF || AXIS5_DIR_PIN == OFF
    #error "Configuration (Config.h): AXIS5 step/dir interface is not supported on this PINMAP"
  #endif
#endif

// leds allowed?
#if LED_STATUS != OFF && STATUS_LED_PIN == OFF
  #error "Configuration (Config.h): LED_STATUS not supported for this PINMAP, must be OFF"
#endif

#if STATUS_MOUNT_LED != OFF && STATUS_MOUNT_LED_PIN == OFF
  #error "Configuration (Config.h): STATUS_MOUNT_LED not supported for this PINMAP, must be OFF"
#endif

#if RETICLE_LED != OFF && RETICLE_LED_PIN == OFF
  #error "Configuration (Config.h): RETICLE_LED not supported for this PINMAP, must be OFF"
#endif

// TIME_LOCATION_SOURCE DS3234 SPI allowed?
#if (TIME_LOCATION_SOURCE == DS3234S || TIME_LOCATION_SOURCE == DS3234M) && !defined(DS3234_CS_PIN)
  #if PINMAP == Classic
      #error "Configuration (Config.h): DS3234 TIME_LOCATION_SOURCE using SPI is not supported, use PINMAP ClassicShield or add '#define ST4_ALTERNATE_PINS_ON' to move the ST4 port pins"
    #else
      #error "Configuration (Config.h): DS3234 TIME_LOCATION_SOURCE using SPI is not supported for this PINMAP"
  #endif
#endif

// WEATHER BME280 SPI allowed?
#if WEATHER == BME280_SPI && !defined(BME280_CS_PIN)
  #error "Configuration (Config.h): BME280 weather using SPI is not supported for this PINMAP"
#endif

// focuser/rotators any invalid combinations?
#if ROTATOR == ON && FOCUSER1 == ON
  #if AXIS3_STEP_PIN == AXIS4_STEP_PIN
    #error "Configuration (Config.h): AXIS3 and AXIS4 step/dir interface is shared, so enabling both is not supported on this PINMAP"
  #endif
#endif

#if ROTATOR == ON && FOCUSER2 == ON
  #if AXIS3_STEP_PIN == AXIS5_STEP_PIN
    #error "Configuration (Config.h): AXIS3 and AXIS5 step/dir interface is shared, so enabling both is not supported on this PINMAP"
  #endif
#endif

#if FEATURE1_PURPOSE != OFF && FEATURE1_PIN == AUX && defined(AUX1_PIN)
  #ifdef AUX1_PIN_INUSE
    #error "Configuration (Config.h): FEATURE1_PIN AUX enabled but AUX1_PIN is already in use, choose one feature on AUX1_PIN"
  #endif
  #undef FEATURE1_PIN
  #define FEATURE1_PIN AUX1_PIN
  #define AUX1_PIN_INUSE
#endif
#if FEATURE2_PURPOSE != OFF && FEATURE2_PIN == AUX && defined(AUX2_PIN)
  #ifdef AUX2_PIN_INUSE
    #error "Configuration (Config.h): FEATURE2_PIN AUX enabled but AUX2_PIN is already in use, choose one feature on AUX2_PIN"
  #endif
  #undef FEATURE2_PIN
  #define FEATURE2_PIN AUX2_PIN
  #define AUX2_PIN_INUSE
#endif
#if FEATURE3_PURPOSE != OFF && FEATURE3_PIN == AUX && defined(AUX3_PIN)
  #ifdef AUX3_PIN_INUSE
    #error "Configuration (Config.h): FEATURE3_PIN AUX enabled but AUX3_PIN is already in use, choose one feature on AUX3_PIN"
  #endif
  #undef FEATURE3_PIN
  #define FEATURE3_PIN AUX3_PIN
  #define AUX3_PIN_INUSE
#endif
#if FEATURE4_PURPOSE != OFF && FEATURE4_PIN == AUX && defined(AUX3_PIN)
  #ifdef AUX4_PIN_INUSE
    #error "Configuration (Config.h): FEATURE4_PIN AUX enabled but AUX4_PIN is already in use, choose one feature on AUX4_PIN"
  #endif
  #undef FEATURE4_PIN
  #define FEATURE4_PIN AUX4_PIN
  #define AUX4_PIN_INUSE
#endif
#if FEATURE5_PURPOSE != OFF && FEATURE5_PIN == AUX && defined(AUX5_PIN)
  #ifdef AUX5_PIN_INUSE
    #error "Configuration (Config.h): FEATURE5_PIN AUX enabled but AUX5_PIN is already in use, choose one feature on AUX5_PIN"
  #endif
  #undef FEATURE5_PIN
  #define FEATURE5_PIN AUX5_PIN
  #define AUX5_PIN_INUSE
#endif
#if FEATURE6_PURPOSE != OFF && FEATURE6_PIN == AUX && defined(AUX6_PIN)
  #ifdef AUX6_PIN_INUSE
    #error "Configuration (Config.h): FEATURE6_PIN AUX enabled but AUX6_PIN is already in use, choose one feature on AUX6_PIN"
  #endif
  #undef FEATURE6_PIN
  #define FEATURE6_PIN AUX6_PIN
  #define AUX6_PIN_INUSE
#endif
#if FEATURE7_PURPOSE != OFF && FEATURE7_PIN == AUX && defined(AUX7_PIN)
  #ifdef AUX7_PIN_INUSE
    #error "Configuration (Config.h): FEATURE7_PIN AUX enabled but AUX7_PIN is already in use, choose one feature on AUX7_PIN"
  #endif
  #undef FEATURE7_PIN
  #define FEATURE7_PIN AUX7_PIN
  #define AUX7_PIN_INUSE
#endif
#if FEATURE8_PURPOSE != OFF && FEATURE8_PIN == AUX && defined(AUX8_PIN)
  #ifdef AUX8_PIN_INUSE
    #error "Configuration (Config.h): FEATURE8_PIN AUX enabled but AUX8_PIN is already in use, choose one feature on AUX8_PIN"
  #endif
  #undef FEATURE8_PIN
  #define FEATURE8_PIN AUX8_PIN
  #define AUX8_PIN_INUSE
#endif
