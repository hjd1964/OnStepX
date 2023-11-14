// -------------------------------------------------------------------------------------------------
// Validates pinmap model for current configuration
#pragma once

// --------------------------------------------------------------------------------------

#if !defined(SERIAL_A) && SERIAL_A_BAUD_DEFAULT != OFF
  #error "Configuration (Config.h): SERIAL_A isn't defined, SERIAL_A_BAUD_DEFAULT should be OFF."
#endif

#if !defined(SERIAL_B) && SERIAL_B_BAUD_DEFAULT != OFF
  #error "Configuration (Config.h): SERIAL_B isn't defined, SERIAL_B_BAUD_DEFAULT should be OFF."
#endif

#if !defined(SERIAL_C) && SERIAL_C_BAUD_DEFAULT != OFF
  #error "Configuration (Config.h): SERIAL_C isn't defined, SERIAL_C_BAUD_DEFAULT should be OFF."
#endif

#if !defined(SERIAL_D) && SERIAL_D_BAUD_DEFAULT != OFF
  #error "Configuration (Config.h): SERIAL_D isn't defined, SERIAL_D_BAUD_DEFAULT should be OFF."
#endif

#if !defined(SERIAL_E) && SERIAL_E_BAUD_DEFAULT != OFF
  #error "Configuration (Config.h): SERIAL_E isn't defined, SERIAL_E_BAUD_DEFAULT should be OFF."
#endif

// can we flash an ESP8266?
#if SERIAL_B_ESP_FLASHING == ON  && (!defined(ADDON_GPIO0_PIN) || !defined(ADDON_RESET_PIN))
  #error "Configuration (Config.h): SERIAL_B_ESP_FLASHING not supported for this PINMAP"
#endif

// make sure a serial interface is available for any TMC UART driver
#if defined(STEP_DIR_TMC_UART_PRESENT) && !defined(SERIAL_TMC)
  #error "Configuration (Config.h): SERIAL_TMC is required for TMC UART drivers"
#endif

// powering down Dec supported only if EN is available and not shared with Axis1
#if AXIS2_POWER_DOWN == ON && (AXIS2_EN_PIN == OFF || AXIS2_EN_PIN == SHARED)
  #error "Configuration (Config.h): AXIS2_POWER_DOWN not supported on this PINMAP"
#endif

// TIME_LOCATION_SOURCE DS3234 SPI allowed?
#if TIME_LOCATION_SOURCE == DS3234 && !defined(DS3234_CS_PIN)
  #if PINMAP == Classic
      #error "Configuration (Config.h): DS3234 TIME_LOCATION_SOURCE using SPI is not supported, use PINMAP ClassicShield or add '#define ST4_ALTERNATE_PINS_ON' to move the ST4 port pins"
    #else
      #error "Configuration (Config.h): DS3234 TIME_LOCATION_SOURCE using SPI is not supported for this PINMAP"
  #endif
#endif

// focuser/rotators invalid combination?
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

// --------------------------------------------------------------------------------------
// check AUX0 pin assignments
#if AUX0_PIN != OFF
  #if AUX0_PIN == MOUNT_LED_PIN && STATUS_MOUNT_LED != OFF
    #ifdef AUX0_PIN_IN_USE
      #error "Configuration (Config.h): STATUS_MOUNT_LED enabled but AUX0_PIN is already in use, choose one feature on AUX0_PIN"
    #endif
    #define AUX0_PIN_IN_USE
  #endif
#endif

// check AUX1 pin assignments
#if AUX1_PIN != OFF
  #if AUX1_PIN == ADDON_GPIO0_PIN && SERIAL_B_ESP_FLASHING != OFF
    #ifdef AUX1_PIN_IN_USE
      #error "Configuration (Config.h): SERIAL_B_ESP_FLASHING enabled but AUX1_PIN is already in use, choose one feature on AUX1_PIN"
    #endif
    #define AUX1_PIN_IN_USE
  #endif
  #if (AUX1_PIN == AXIS1_M3_PIN && AXIS1_DRIVER_STATUS != OFF) || \
      (AUX1_PIN == AXIS2_M3_PIN && AXIS2_DRIVER_STATUS != OFF) || \
      (AUX1_PIN == AXIS3_M3_PIN && AXIS3_DRIVER_STATUS != OFF) || \
      (AUX1_PIN == AXIS4_M3_PIN && AXIS4_DRIVER_STATUS != OFF) || \
      (AUX1_PIN == AXIS5_M3_PIN && AXIS5_DRIVER_STATUS != OFF) || \
      (AUX1_PIN == AXIS6_M3_PIN && AXIS6_DRIVER_STATUS != OFF) || \
      (AUX1_PIN == AXIS7_M3_PIN && AXIS7_DRIVER_STATUS != OFF) || \
      (AUX1_PIN == AXIS8_M3_PIN && AXIS8_DRIVER_STATUS != OFF) || \
      (AUX1_PIN == AXIS9_M3_PIN && AXIS9_DRIVER_STATUS != OFF)
    #ifdef AUX1_PIN_IN_USE
      #error "Configuration (Config.h): AXISn_DRIVER_STATUS enabled but AUX1_PIN is already in use, choose one feature on AUX1_PIN"
    #endif
    #define AUX1_PIN_IN_USE
  #endif
#endif

// check AUX2 pin assignments
#if AUX2_PIN != OFF
  #if AUX2_PIN == ADDON_RESET_PIN && SERIAL_B_ESP_FLASHING != OFF
    #ifdef AUX2_PIN_IN_USE
      #error "Configuration (Config.h): SERIAL_B_ESP_FLASHING enabled but AUX2_PIN is already in use, choose one feature on AUX2_PIN"
    #endif
    #define AUX1_PIN_IN_USE
  #endif
  #if (AUX2_PIN == AXIS1_M3_PIN && AXIS1_DRIVER_STATUS != OFF) || \
      (AUX2_PIN == AXIS2_M3_PIN && AXIS2_DRIVER_STATUS != OFF) || \
      (AUX2_PIN == AXIS3_M3_PIN && AXIS3_DRIVER_STATUS != OFF) || \
      (AUX2_PIN == AXIS4_M3_PIN && AXIS4_DRIVER_STATUS != OFF) || \
      (AUX2_PIN == AXIS5_M3_PIN && AXIS5_DRIVER_STATUS != OFF) || \
      (AUX2_PIN == AXIS6_M3_PIN && AXIS6_DRIVER_STATUS != OFF) || \
      (AUX2_PIN == AXIS7_M3_PIN && AXIS7_DRIVER_STATUS != OFF) || \
      (AUX2_PIN == AXIS8_M3_PIN && AXIS8_DRIVER_STATUS != OFF) || \
      (AUX2_PIN == AXIS9_M3_PIN && AXIS9_DRIVER_STATUS != OFF)
    #ifdef AUX2_PIN_IN_USE
      #error "Configuration (Config.h): AXISn_DRIVER_STATUS enabled but AUX2_PIN is already in use, choose one feature on AUX2_PIN"
    #endif
    #define AUX2_PIN_IN_USE
  #endif
#endif

// check AUX3 pin assignments
#if AUX3_PIN != OFF
  #if (AUX3_PIN) == (AXIS1_SENSE_HOME_PIN) && (AXIS1_SENSE_HOME) != OFF
    #ifdef AUX3_PIN_IN_USE
      #error "Configuration (Config.h): AXIS1_SENSE_HOME enabled but AUX3_PIN is already in use, choose one feature on AUX3_PIN"
    #endif
    #define AUX3_PIN_IN_USE
  #endif
  #if (AUX3_PIN) == (LIMIT_SENSE_PIN) && (LIMIT_SENSE) != OFF
    #ifdef AUX3_PIN_IN_USE
      #error "Configuration (Config.h): LIMIT_SENSE enabled but AUX3_PIN is already in use, choose one feature on AUX3_PIN"
    #endif
    #define AUX3_PIN_IN_USE
  #endif
  #if AUX3_PIN == RETICLE_LED_PIN && RETICLE_LED_DEFAULT != OFF
    #if AUX3_PIN == STATUS_LED_PIN && STATUS_LED != OFF
      #error "Configuration (Config.h): RETICLE_LED_DEFAULT and STATUS_LED can't both be enabled on the same AUX3_PIN"
    #endif
    #ifdef AUX3_PIN_IN_USE
      #error "Configuration (Config.h): RETICLE_LED_DEFAULT enabled but AUX3_PIN is already in use, choose one feature on AUX3_PIN"
    #endif
    #define AUX3_PIN_IN_USE
  #endif
#endif

// check AUX4 pin assignments
#if AUX4_PIN != OFF
  #if (AUX4_PIN) == (AXIS2_SENSE_HOME_PIN) && (AXIS2_SENSE_HOME) != OFF
    #ifdef AUX4_PIN_IN_USE
      #error "Configuration (Config.h): AXIS2_SENSE_HOME enabled but AUX4_PIN is already in use, choose one feature on AUX4_PIN"
    #endif
    #define AUX4_PIN_IN_USE
  #endif
  #if AUX4_PIN == RETICLE_LED_PIN && RETICLE_LED_DEFAULT != OFF
    #ifdef AUX4_PIN_IN_USE
      #error "Configuration (Config.h): RETICLE_LED_DEFAULT enabled but AUX4_PIN is already in use, choose one feature on AUX4_PIN"
    #endif
    #define AUX4_PIN_IN_USE
  #endif
#endif

// check AUX5 pin assignments
// not checked, for Axis3 enable or Serial

// check AUX6 pin assignments
// not checked, for Axis4 enable or Serial

// check AUX7 pin assignments
#if AUX7_PIN != OFF
  #if (AUX7_PIN) == (LIMIT_SENSE_PIN) && (LIMIT_SENSE) != OFF
    #ifdef AUX7_PIN_IN_USE
      #error "Configuration (Config.h): LIMIT_SENSE enabled but AUX7_PIN is already in use, choose one feature on AUX7_PIN"
    #endif
    #define AUX7_PIN_IN_USE
  #endif
  #if (AUX7_PIN) == (PPS_SENSE_PIN) && (TIME_LOCATION_PPS_SENSE) != OFF
    #ifdef AUX7_PIN_IN_USE
      #error "Configuration (Config.h): TIME_LOCATION_PPS_SENSE enabled but AUX7_PIN is already in use, choose one feature on AUX7_PIN"
    #endif
    #define AUX7_PIN_IN_USE
  #endif
#endif

// check AUX8 pin assignments
#if AUX8_PIN != OFF
  #if AUX8_PIN == MOUNT_LED_PIN && STATUS_MOUNT_LED != OFF
    #ifdef AUX8_PIN_IN_USE
      #error "Configuration (Config.h): STATUS_MOUNT_LED enabled but AUX8_PIN is already in use, choose one feature on AUX8_PIN"
    #endif
    #define AUX8_PIN_IN_USE
  #endif
  #if AUX8_PIN == RETICLE_LED_PIN && RETICLE_LED_DEFAULT != OFF
    #if AUX8_PIN == STATUS_LED_PIN && STATUS_LED != OFF
      #error "Configuration (Config.h): RETICLE_LED_DEFAULT and STATUS_LED can't both be enabled on the same AUX8_PIN"
    #endif
    #ifdef AUX8_PIN_IN_USE
      #error "Configuration (Config.h): RETICLE_LED_DEFAULT enabled but AUX8_PIN is already in use, choose one feature on AUX8_PIN"
    #endif
    #define AUX8_PIN_IN_USE
  #endif
  #if AUX8_PIN == STATUS_BUZZER_PIN && STATUS_BUZZER != OFF
    #ifdef AUX8_PIN_IN_USE
      #error "Configuration (Config.h): STATUS_BUZZER enabled but AUX8_PIN is already in use, choose one feature on AUX8_PIN"
    #endif
    #define AUX8_PIN_IN_USE
  #endif
#endif

// --------------------------------------------------------------------------------------
// check/assign AUX pins to auxiliary features
#if FEATURE1_PURPOSE != OFF && FEATURE1_PIN == AUX && AUX1_PIN != OFF
  #ifdef AUX1_PIN_IN_USE
    #error "Configuration (Config.h): FEATURE1_PIN AUX enabled but AUX1_PIN is already in use, choose one feature on AUX1_PIN"
  #endif
  #undef FEATURE1_PIN
  #define FEATURE1_PIN AUX1_PIN
  #define AUX1_PIN_IN_USE
#endif
#if FEATURE2_PURPOSE != OFF && FEATURE2_PIN == AUX && AUX2_PIN != OFF
  #ifdef AUX2_PIN_IN_USE
    #error "Configuration (Config.h): FEATURE2_PIN AUX enabled but AUX2_PIN is already in use, choose one feature on AUX2_PIN"
  #endif
  #undef FEATURE2_PIN
  #define FEATURE2_PIN AUX2_PIN
  #define AUX2_PIN_IN_USE
#endif
#if FEATURE3_PURPOSE != OFF && FEATURE3_PIN == AUX && AUX3_PIN != OFF
  #ifdef AUX3_PIN_IN_USE
    #error "Configuration (Config.h): FEATURE3_PIN AUX enabled but AUX3_PIN is already in use, choose one feature on AUX3_PIN"
  #endif
  #undef FEATURE3_PIN
  #define FEATURE3_PIN AUX3_PIN
  #define AUX3_PIN_IN_USE
#endif
#if FEATURE4_PURPOSE != OFF && FEATURE4_PIN == AUX && AUX4_PIN != OFF
  #ifdef AUX4_PIN_IN_USE
    #error "Configuration (Config.h): FEATURE4_PIN AUX enabled but AUX4_PIN is already in use, choose one feature on AUX4_PIN"
  #endif
  #undef FEATURE4_PIN
  #define FEATURE4_PIN AUX4_PIN
  #define AUX4_PIN_IN_USE
#endif
#if FEATURE5_PURPOSE != OFF && FEATURE5_PIN == AUX && AUX5_PIN != OFF
  #ifdef AUX5_PIN_IN_USE
    #error "Configuration (Config.h): FEATURE5_PIN AUX enabled but AUX5_PIN is already in use, choose one feature on AUX5_PIN"
  #endif
  #undef FEATURE5_PIN
  #define FEATURE5_PIN AUX5_PIN
  #define AUX5_PIN_IN_USE
#endif
#if FEATURE6_PURPOSE != OFF && FEATURE6_PIN == AUX && AUX6_PIN != OFF
  #ifdef AUX6_PIN_IN_USE
    #error "Configuration (Config.h): FEATURE6_PIN AUX enabled but AUX6_PIN is already in use, choose one feature on AUX6_PIN"
  #endif
  #undef FEATURE6_PIN
  #define FEATURE6_PIN AUX6_PIN
  #define AUX6_PIN_IN_USE
#endif
#if FEATURE7_PURPOSE != OFF && FEATURE7_PIN == AUX && AUX7_PIN != OFF
  #ifdef AUX7_PIN_IN_USE
    #error "Configuration (Config.h): FEATURE7_PIN AUX enabled but AUX7_PIN is already in use, choose one feature on AUX7_PIN"
  #endif
  #undef FEATURE7_PIN
  #define FEATURE7_PIN AUX7_PIN
  #define AUX7_PIN_IN_USE
#endif
#if FEATURE8_PURPOSE != OFF && FEATURE8_PIN == AUX && AUX8_PIN != OFF
  #ifdef AUX8_PIN_IN_USE
    #error "Configuration (Config.h): FEATURE8_PIN AUX enabled but AUX8_PIN is already in use, choose one feature on AUX8_PIN"
  #endif
  #undef FEATURE8_PIN
  #define FEATURE8_PIN AUX8_PIN
  #define AUX8_PIN_IN_USE
#endif
