// -----------------------------------------------------------------------------------
// Constants

#pragma once
// Configuration options -----------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------------------
// Pinmaps -------------------------------------------------------------------------------------------------------------------------

#define PINMAP_FIRST                 1
#define Classic                      1 // Original pin-map
#define ClassicShield                2 // Original pin-map, for Steve's shield where ST4 port is on the Mega2560's "alternate pins"
#define ClassicInstein               3 // Original pin-map w/Instein ST4 port... this isn't tested, I have no idea if it's safe to use!
#define InsteinESP1                  4 // ESP32 Instein PINMAP (latest variant September/2020)

// RAMPS - 3D printer shields/boards that work with OnStep, all are 5-axis designs
#define Ramps14                     10 // Ramps v1.4 shield for Mega2560
#define Ramps15                     10 // Ramps v1.5
#define MksGenL                     11 // Like Ramps above but better, Mega2560 built-in, crystal oscillator, 24V support
#define MksGenL1                    11
#define MksGenL2                    12 // Adds SPI bus to all stepper drivers (TMC only)
#define MksGenL21                   13 // As above except Focuser2 CS is on pin12

// FYSETC S6 - 3D printer board that works with OnStep, a 6-axis design with 5-axes supported
#define FYSETC_S6                   15 // FYSETC S6 Version 1.2
#define FYSETC_S6_1                 15 // FYSETC S6 Version 1.2
#define FYSETC_S6_2                 16 // FYSETC S6 Version 2.0

// Mini - Small 2-axis design suitable for embedded or mounting behind a panel can even be built with connectors up
#define MiniPCB                     20 // All for Teensy3.2
#define MiniPCB13                   21 // MiniPCB v1.3 adds better support for ESP-01 flashing and optional I2C

// Mini2 - Small 2-axis design for aluminum mini-case
#define MiniPCB2                    30 // Teensy3.2

// Max - Larger 4-axis design suitable for embedded or mounting behind a panel
#define MaxPCB                      40 // First custom "full feature" board. Teensy3.5/Teensy3.6

// Max2 & Max3 - Larger 4-axis design for aluminum case
#define MaxPCB2                     41 // Improved "full feature" board, Teensy3.5/Teensy3.6
#define MaxPCB3                     42 // Adds SPI bus to all stepper drivers (TMC only,) flashes WeMos D1 Mini through OnStep
#define MaxSTM3                     43 // Update to the MaxPCB3 using an Blackpill F411CE instead of the Teensy3.5/3.6
#define MaxSTM3I                    44 // As above but using an onboard STM32F411CE with M24C64 EEPROM as default

#define MaxESP2                     50 // Similar to MaxPCB2 except 3 axes and uses cheaper/more available ESP32 MCU
#define MaxESP3                     51 // Adds 4th axis and option to flash the WeMos D1 Mini WiFi through OnStep

// Arduino CNC Sheild on WeMos D2 R32 (ESP32)
#define CNC3                        52 // Similar features to MaxESP3

// Khalid and Dave's PCB for STM32 Blue pill (STM32F303CC)
#define STM32Blue                   60

#define PINMAP_LAST                 60
// ---------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------

// Misc. ---------------------------------------------------------------------------------------------------------------------------
#define OFF                         -1
#define ON                          -2
#define ON_BOTH                     -3
#define ON_PULLUP                   -4
#define ON_PULLDOWN                 -5
#define CONSOLE                     -6
#define PROFILER                    -7
#define SHARED                      -14
#define INVALID                     -32767

// mount types                     
#define MOUNT_TYPE_FIRST            1
#define GEM                         1 // German Equatorial Mount, meridian flips enabled
#define FORK                        2 // Fork Mount, meridian flips disabled
#define ALTAZM                      3 // Altitude Azimuth Mounts, Dobsonians, etc.
#define MOUNT_TYPE_LAST             3

// misc. math
#define RAD                         57.29577951308232
#define RAD_HOUR_RATIO              3.819718634205488
#define SIDEREAL_RATIO              1.002737909350795

// for handling degenerate spherical coordinates near the poles
#define SmallestRad                 0.000005

// conversion math
#define degToRad(x)                 (x/RAD)
#define radToDeg(x)                 (x*RAD)
#define hrsToRad(x)                 (x/RAD_HOUR_RATIO)
#define radToHrs(x)                 (x*RAD_HOUR_RATIO)
#define csToRad(x)                  (x/1375098.708313976)
#define radToCs(x)                  (x*1375098.708313976)
#define arcsecToRad(x)              ((x/3600.0)/RAD)
#define radToArcsec(x)              ((x*RAD)*3600.0)

// Macros --------------------------------------------------------------------------------------------------------------------------
#define pinModeEx(pin,mode)           { if (pin != OFF && pin != SHARED) { pinMode(pin,mode); } }
#define pinModeInitEx(pin,mode,state) { pinModeEx(pin,mode); digitalWrite(pin,state); }
#define digitalWriteEx(pin,value)     { if (pin != OFF && pin != SHARED) digitalWrite(pin,value); }
int     digitalReadEx(int pin)        { if (pin != OFF && pin != SHARED) return digitalRead(pin); else return 0; }
#ifndef delaySpi
  #define delaySpi() delayMicroseconds(1)
#endif
#ifdef HAL_HAS_DIGITAL_FAST
  #define digitalWriteF(pin,value)    { digitalWriteFast(pin,value); }
#else
  #define digitalWriteF(pin,value)    { digitalWrite(pin,value); }
#endif
#define DEBUG_CHECK_CONSTANT(P) { Serial.begin(115200); Serial.println("Constant? "); Serial.println(__builtin_constant_p(P)); }

// Status --------------------------------------------------------------------------------------------------------------------------
// Note: the following error codes are obsolete ERR_SYNC, ERR_PARK
enum GeneralErrors {
  ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT_MIN, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM,
  ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC, ERR_PARK, ERR_GOTO_SYNC, ERR_UNSPECIFIED,
  ERR_ALT_MAX, ERR_WEATHER_INIT, ERR_SITE_INIT, ERR_NV_INIT};
const GeneralErrors generalError = ERR_NONE;

enum CommandErrors {
  CE_NONE, CE_0, CE_CMD_UNKNOWN, CE_REPLY_UNKNOWN, CE_PARAM_RANGE, CE_PARAM_FORM,
  CE_ALIGN_FAIL, CE_ALIGN_NOT_ACTIVE, CE_NOT_PARKED_OR_AT_HOME, CE_PARKED,
  CE_PARK_FAILED, CE_NOT_PARKED, CE_NO_PARK_POSITION_SET, CE_GOTO_FAIL, CE_LIBRARY_FULL,
  CE_GOTO_ERR_BELOW_HORIZON, CE_GOTO_ERR_ABOVE_OVERHEAD, CE_SLEW_ERR_IN_STANDBY, 
  CE_SLEW_ERR_IN_PARK, CE_GOTO_ERR_GOTO, CE_SLEW_ERR_OUTSIDE_LIMITS, CE_SLEW_ERR_HARDWARE_FAULT,
  CE_MOUNT_IN_MOTION, CE_GOTO_ERR_UNSPECIFIED, CE_NULL};
  
#if DEBUG != OFF
  // command errors
  #define L_CE_NONE                    "No Errors"
  #define L_CE_0                       "reply 0"
  #define L_CE_CMD_UNKNOWN             "command unknown"
  #define L_CE_REPLY_UNKNOWN           "invalid reply"
  #define L_CE_PARAM_RANGE             "parameter out of range"
  #define L_CE_PARAM_FORM              "bad parameter format"
  #define L_CE_ALIGN_FAIL              "align failed"
  #define L_CE_ALIGN_NOT_ACTIVE        "align not active"
  #define L_CE_NOT_PARKED_OR_AT_HOME   "not parked or at home"
  #define L_CE_PARKED                  "already parked"
  #define L_CE_PARK_FAILED             "park failed"
  #define L_CE_NOT_PARKED              "not parked"
  #define L_CE_NO_PARK_POSITION_SET    "no park position set"
  #define L_CE_GOTO_FAIL               "goto failed"
  #define L_CE_LIBRARY_FULL            "library full"
  #define L_CE_GOTO_ERR_BELOW_HORIZON  "goto below horizon"
  #define L_CE_GOTO_ERR_ABOVE_OVERHEAD "goto above overhead"
  #define L_CE_SLEW_ERR_IN_STANDBY     "slew in standby"
  #define L_CE_SLEW_ERR_IN_PARK        "slew in park"
  #define L_CE_GOTO_ERR_GOTO           "already in goto"
  #define L_CE_SLEW_ERR_OUTSIDE_LIMITS "outside limits"
  #define L_CE_SLEW_ERR_HARDWARE_FAULT "hardware fault"
  #define L_CE_MOUNT_IN_MOTION         "mount in motion"
  #define L_CE_GOTO_ERR_UNSPECIFIED    "other"
  #define L_CE_UNK                     "unknown"
  
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
