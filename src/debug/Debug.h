// Helper macros for debugging, with less typing
#pragma once

#include <Arduino.h>
#include "../Constants.h"
#include "../Config.common.h"
#include "../HAL/HAL.h"

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

  static const char commandErrorStr[30][25] = {
  L_CE_NONE, L_CE_0, L_CE_CMD_UNKNOWN, L_CE_REPLY_UNKNOWN, L_CE_PARAM_RANGE,
  L_CE_PARAM_FORM, L_CE_ALIGN_FAIL, L_CE_ALIGN_NOT_ACTIVE, L_CE_NOT_PARKED_OR_AT_HOME,
  L_CE_PARKED, L_CE_PARK_FAILED, L_CE_NOT_PARKED, L_CE_NO_PARK_POSITION_SET, L_CE_GOTO_FAIL,
  L_CE_LIBRARY_FULL, L_CE_GOTO_ERR_BELOW_HORIZON, L_CE_GOTO_ERR_ABOVE_OVERHEAD,
  L_CE_SLEW_ERR_IN_STANDBY, L_CE_SLEW_ERR_IN_PARK, L_CE_GOTO_ERR_GOTO, L_CE_SLEW_ERR_OUTSIDE_LIMITS,
  L_CE_SLEW_ERR_HARDWARE_FAULT, L_CE_MOUNT_IN_MOTION, L_CE_GOTO_ERR_UNSPECIFIED, L_CE_UNK};

  #define D(x)       SERIAL_DEBUG.print(x)
  #define DF(x)      SERIAL_DEBUG.print(F(x))
  #define DL(x)      SERIAL_DEBUG.println(x)
  #define DLF(x)     SERIAL_DEBUG.println(F(x))
  #define DLCCHK(P)  { DF("Constant? "); DLF(__builtin_constant_p(P)); }
#else
  #define D(x)
  #define DF(x)
  #define DL(x)
  #define DLF(x)

  static const char commandErrorStr[0][0] = {};
#endif

#if DEBUG == VERBOSE
  #define V(x)       SERIAL_DEBUG.print(x)
  #define VF(x)      SERIAL_DEBUG.print(F(x))
  #define VL(x)      SERIAL_DEBUG.println(x)
  #define VLF(x)     SERIAL_DEBUG.println(F(x))
#else
  #define V(x)
  #define VF(x)
  #define VL(x)
  #define VLF(x)
#endif

#if DEBUG == PROFILER
  #define TASKS_PROFILER_ENABLE
#endif
