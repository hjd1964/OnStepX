// -------------------------------------------------------------------------------------------------
// Validates current configuration

// correct tangent arm use?
#if AXIS2_TANGENT_ARM != OFF && MOUNT_TYPE == ALTAZM
  #error "Configuration (Config.h): AXIS2_TANGENT_ARM is not supported for default MOUNT_TYPE ALTAZM"
#endif
