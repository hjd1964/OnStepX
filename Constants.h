// -----------------------------------------------------------------------------------
// Constants

// misc.                     
#define OFF                            -1
#define ON                             -2
#define ON_BOTH                        -3
#define ON_PULLUP                      -4
#define ON_PULLDOWN                    -5
#define INVALID                    -32767

// mount types                     
#define MOUNT_TYPE_FIRST                1
#define GEM                             1 // German Equatorial Mount, meridian flips enabled
#define FORK                            2 // Fork Mount, meridian flips disabled
#define ALTAZM                          3 // Altitude Azimuth Mounts, Dobsonians, etc.
#define MOUNT_TYPE_LAST                 3

// misc. math
#define RAD             57.29577951308232
#define RAD_HOUR_RATIO  3.819718634205488
#define SIDEREAL_RATIO  1.002737909350795
#define SmallestRad              0.000005 // for handling degenerate spherical coordinates near the poles

// conversion math
#define degToRad(x)               (x/RAD)
#define radToDeg(x)               (x*RAD)
#define hrsToRad(x)    (x/RAD_HOUR_RATIO)
#define radToHrs(x)    (x*RAD_HOUR_RATIO)
#define csToRad(x)  (x/1375098.708313976)
#define radToCs(x)  (x*1375098.708313976)
#define arcsecToRad(x)   ((x/3600.0)/RAD)
#define radToArcsec(x)   ((x*RAD)*3600.0)

// helper macros, etc.
#define pinModeEx(pin,mode)         { if (pin != OFF) { pinMode(pin,mode); } }
#define pinModeInit(pin,mode,state) { pinModeEx(pin,mode); digitalWrite(pin,state); }
#define digitalWriteEx(pin,value)   { if (pin != OFF) digitalWrite(pin,value); }
int     digitalReadEx(int pin)      { if (pin != OFF) return digitalRead(pin); else return 0; }
#ifndef delaySpi
  #define delaySpi() delayMicroseconds(1)
#endif
#ifdef HAL_HAS_DIGITAL_FAST
  #define digitalWriteF(pin,value)    { digitalWriteFast(pin,value); }
#else
  #define digitalWriteF(pin,value)    { digitalWrite(pin,value); }
#endif

// Status --------------------------------------------------------------------------------------------------------------------------
// Note: the following error codes are obsolete ERR_SYNC, ERR_PARK
enum GeneralErrors {
  ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT_MIN, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM,
  ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC, ERR_PARK, ERR_GOTO_SYNC, ERR_UNSPECIFIED,
  ERR_ALT_MAX, ERR_WEATHER_INIT, ERR_SITE_INIT, ERR_NV_INIT};

#if DEBUG != OFF
  // command errors
  #define L_CE_NONE "No Errors"
  #define L_CE_0 "reply 0"
  #define L_CE_CMD_UNKNOWN "command unknown"
  #define L_CE_REPLY_UNKNOWN "invalid reply"
  #define L_CE_PARAM_RANGE "parameter out of range"
  #define L_CE_PARAM_FORM "bad parameter format"
  #define L_CE_ALIGN_FAIL "align failed"
  #define L_CE_ALIGN_NOT_ACTIVE "align not active"
  #define L_CE_NOT_PARKED_OR_AT_HOME "not parked or at home"
  #define L_CE_PARKED "already parked"
  #define L_CE_PARK_FAILED "park failed"
  #define L_CE_NOT_PARKED "not parked"
  #define L_CE_NO_PARK_POSITION_SET "no park position set"
  #define L_CE_GOTO_FAIL "goto failed"
  #define L_CE_LIBRARY_FULL "library full"
  #define L_CE_GOTO_ERR_BELOW_HORIZON "goto below horizon"
  #define L_CE_GOTO_ERR_ABOVE_OVERHEAD "goto above overhead"
  #define L_CE_SLEW_ERR_IN_STANDBY "slew in standby"
  #define L_CE_SLEW_ERR_IN_PARK "slew in park"
  #define L_CE_GOTO_ERR_GOTO "already in goto"
  #define L_CE_SLEW_ERR_OUTSIDE_LIMITS "outside limits"
  #define L_CE_SLEW_ERR_HARDWARE_FAULT "hardware fault"
  #define L_CE_MOUNT_IN_MOTION "mount in motion"
  #define L_CE_GOTO_ERR_UNSPECIFIED "other"
  #define L_CE_UNK "unknown"
  
  char commandErrorStr[30][25] = {
    L_CE_NONE, L_CE_0, L_CE_CMD_UNKNOWN, L_CE_REPLY_UNKNOWN, L_CE_PARAM_RANGE,
    L_CE_PARAM_FORM, L_CE_ALIGN_FAIL, L_CE_ALIGN_NOT_ACTIVE, L_CE_NOT_PARKED_OR_AT_HOME,
    L_CE_PARKED, L_CE_PARK_FAILED, L_CE_NOT_PARKED, L_CE_NO_PARK_POSITION_SET, L_CE_GOTO_FAIL,
    L_CE_LIBRARY_FULL, L_CE_GOTO_ERR_BELOW_HORIZON, L_CE_GOTO_ERR_ABOVE_OVERHEAD,
    L_CE_SLEW_ERR_IN_STANDBY, L_CE_SLEW_ERR_IN_PARK, L_CE_GOTO_ERR_GOTO, L_CE_SLEW_ERR_OUTSIDE_LIMITS,
    L_CE_SLEW_ERR_HARDWARE_FAULT, L_CE_MOUNT_IN_MOTION, L_CE_GOTO_ERR_UNSPECIFIED, L_CE_UNK};
#else
  char commandErrorStr[0][0];
#endif
