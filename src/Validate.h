// -------------------------------------------------------------------------------------------------
// Validates current configuration

// Teensy4.0 and 4.1 must run in SQW mode
#if (defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41)) && STEP_WAVE_FORM != SQUARE
  #error "Configuration (Config.h): STEP_WAVE_FORM SQUARE is required for the Teensy4.0 and 4.1"
#endif

// correct tangent arm use?
#if AXIS2_TANGENT_ARM != OFF && MOUNT_TYPE == ALTAZM
  #error "Configuration (Config.h): AXIS2_TANGENT_ARM is not supported for default MOUNT_TYPE ALTAZM"
#endif
