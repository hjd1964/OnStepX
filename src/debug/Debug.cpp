// Helper macros for debugging, with less typing
#include "Debug.h"

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
