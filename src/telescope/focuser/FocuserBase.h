#pragma once

#include "../../Common.h"

// -----------------------------------------------------------------------------
// CAN focuser transport base definitions (shared by local + remote)
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CAN IDs
// -----------------------------------------------------------------------------
#ifndef CAN_FOCUSER_REQ_ID
  #define CAN_FOCUSER_REQ_ID (0x00 + 4)
#endif

#ifndef CAN_FOCUSER_RSP_ID
  #define CAN_FOCUSER_RSP_ID (CAN_FOCUSER_REQ_ID + 0x10)
#endif

// Heartbeats are separate IDs (one per focuser 1..6).
#ifndef CAN_FOCUSER_HB_ID_BASE
  #define CAN_FOCUSER_HB_ID_BASE (CAN_FOCUSER_REQ_ID + 0x20)
#endif

// -----------------------------------------------------------------------------
// CAN_FOCUSER role selection
//   OFF    : no CAN focuser support
//   ON     : telescope/mount forwards focuser commands over CAN (client/proxy)
//   REMOTE : remote node answers focuser commands over CAN (server/local AXIS4)
// -----------------------------------------------------------------------------
#ifndef CAN_FOCUSER
  #define CAN_FOCUSER OFF
#endif

#if (CAN_FOCUSER == REMOTE)
  #ifndef CAN_FOCUSER_NUMBER
    #define CAN_FOCUSER_NUMBER 1
  #endif
  #if (CAN_FOCUSER_NUMBER < 1) || (CAN_FOCUSER_NUMBER > 6)
    #error "Config: CAN_FOCUSER_NUMBER must be 1..6 when CAN_FOCUSER == REMOTE."
  #endif
#endif

// -----------------------------------------------------------------------------
// Validate CAN requirements
// -----------------------------------------------------------------------------
#if (CAN_FOCUSER != OFF)
  #if !defined(CAN_PLUS) || (CAN_PLUS == OFF)
    #error "Focuser: CAN_FOCUSER requires CAN_PLUS to be enabled."
  #endif
#endif

// -----------------------------------------------------------------------------
// Derive presence macros used by Telescope.cpp command router
// -----------------------------------------------------------------------------
#if (CAN_FOCUSER == ON)
  #define FOCUSER_CLIENT_PRESENT
  #define FOCUSER_CAN_CLIENT_PRESENT
#elif (CAN_FOCUSER == REMOTE)
  #define FOCUSER_CAN_SERVER_PRESENT
#endif

// CLIENT build must NOT also have local Axis4..Axis9 focusers.
#if defined(FOCUSER_PRESENT) && defined(FOCUSER_CLIENT_PRESENT)
  #error "Config: both local focusers (FOCUSER_PRESENT) and CAN_FOCUSER ON are enabled. Choose one."
#endif

// REMOTE build MUST have local Axis4 focuser.
#if (CAN_FOCUSER == REMOTE) && (AXIS4_DRIVER_MODEL == OFF)
  #error "Config: CAN_FOCUSER REMOTE requires a local AXIS4 focuser (AXIS4_DRIVER_MODEL != OFF)."
#endif

// -----------------------------------------------------------------------------
// Control byte (requestPayload[1])
// -----------------------------------------------------------------------------
#define FOC_CTRL_FOCUSER_MASK     0x07  // bits 0..2 store focuser number 1..6
#define FOC_CTRL_UNIT_MICRONS     0x08  // bit 3: 1=microns, 0=steps

// -----------------------------------------------------------------------------
// CAN opcode set (5-bit)
// -----------------------------------------------------------------------------
#define FOC_OP_GET_STATUS_T       0x00
#define FOC_OP_GET_MODE_p         0x01
#define FOC_OP_GET_MIN_I          0x02
#define FOC_OP_GET_MAX_M          0x03
#define FOC_OP_GET_TEMPDIFF_e     0x04
#define FOC_OP_GET_TEMP_t         0x05
#define FOC_OP_GET_UM_PER_STEP_u  0x06

#define FOC_OP_BACKLASH_B         0x07  // get: no args, set: i32LE
#define FOC_OP_PARK_HP            0x08
#define FOC_OP_UNPARK_HR          0x09

#define FOC_OP_TCF_COEF_C         0x0A  // get: no args, set: f32LE
#define FOC_OP_TCF_ENABLE_c       0x0B  // get: no args, set: u8 (0/1)
#define FOC_OP_TCF_DEADBAND_D     0x0C  // get: no args, set: i32LE
#define FOC_OP_DC_POWER_P         0x0D  // get: no args, set: u8 (0..100)

#define FOC_OP_STOP_Q             0x0E
#define FOC_OP_SET_RATE_1_9       0x0F  // u8 digit 1..9
#define FOC_OP_GET_WORKRATE_W     0x10  // i32LE (um/s)

#define FOC_OP_MOVE_IN_PLUS       0x11
#define FOC_OP_MOVE_OUT_MINUS     0x12

#define FOC_OP_GET_POS_G          0x13  // i32LE position (units)
#define FOC_OP_GOTO_REL_R         0x14  // i32LE delta (units)
#define FOC_OP_GOTO_ABS_S         0x15  // i32LE target (units)

#define FOC_OP_ZERO_Z             0x16
#define FOC_OP_SET_HOME_H         0x17
#define FOC_OP_GOTO_HOME_h        0x18

#define FOC_OP_DRIVER_STATUS      0x19  // :GXU[n]#
