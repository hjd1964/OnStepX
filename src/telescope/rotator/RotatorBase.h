
// -----------------------------------------------------------------------------
// Where the base CAN ID's start for this CANbus transport
// -----------------------------------------------------------------------------
#pragma once

#include "../../Common.h"

#ifndef CAN_ROTATOR_REQ_ID
  #define CAN_ROTATOR_REQ_ID (0x00 + 3)
#endif

#ifndef CAN_ROTATOR_RSP_ID
  #define CAN_ROTATOR_RSP_ID (CAN_ROTATOR_REQ_ID + 0x10)
#endif

// -----------------------------------------------------------------------------
// CAN_ROTATOR role selection
//   OFF    : no CAN rotator support
//   ON     : telescope/mount forwards rotator commands over CAN (client/proxy)
//   REMOTE : remote node answers rotator commands over CAN (server/local axis3)
// -----------------------------------------------------------------------------

#ifndef CAN_ROTATOR
  #define CAN_ROTATOR OFF
#endif

// -----------------------------------------------------------------------------
// Validate CAN requirements
// -----------------------------------------------------------------------------
#if (CAN_ROTATOR != OFF)
  #if !defined(CAN_PLUS) || (CAN_PLUS == OFF)
    #error "Rotator: CAN_ROTATOR requires CAN_PLUS to be enabled."
  #endif
#endif

// -----------------------------------------------------------------------------
// Derive presence macros used by Telescope.cpp command router
// -----------------------------------------------------------------------------
#if (CAN_ROTATOR == ON)
  // ROTATOR_PRESENT must NOT exist
  #define ROTATOR_CLIENT_PRESENT            // a remote rotator may be present
  #define ROTATOR_CAN_CLIENT_PRESENT        // client implementation using CAN
#elif (CAN_ROTATOR == REMOTE)
  // ROTATOR_PRESENT must exist since we need Axis3
  #define ROTATOR_CAN_SERVER_PRESENT        // server implementation (compiled into local rotator)
#endif

// -----------------------------------------------------------------------------
// Enforce “only one rotator” contract
// -----------------------------------------------------------------------------

// CLIENT build must NOT also have a local Axis3 rotator.
#if defined(ROTATOR_PRESENT) && defined(ROTATOR_CLIENT_PRESENT)
  #error "Config: both local AXIS3 rotator (ROTATOR_PRESENT) and CAN_ROTATOR ON are enabled. Choose one."
#endif

// REMOTE build MUST have local Axis3 rotator (otherwise it can't serve anything).
#if !defined(ROTATOR_PRESENT) && defined(ROTATOR_CAN_SERVER_PRESENT)
  #error "Config: CAN_ROTATOR REMOTE requires a local AXIS3 rotator (ROTATOR_PRESENT)."
#endif

// -----------------------------------------------------------------------------
// CANbus command opcode set (5-bit). Per-ID namespace, so rotator "owns" these.
// -----------------------------------------------------------------------------

#define ROT_OP_PARK_HP          0x08  // :hP#          | TX: none                         -> RX: none (numericReply=1; CE_1 on success)
#define ROT_OP_UNPARK_HR        0x09  // :hR#          | TX: none                         -> RX: none (numericReply=1; CE_1 on success)

#define ROT_OP_ACTIVE_RA        0x0A  // :rA#          | TX: none                         -> RX: none (numericReply=1; CE_1 on success)

#define ROT_OP_GET_STATUS_T     0x01  // :rT#          | TX: none                         -> RX: ASCII bytes "M"/"S"["D"]["R"] + rateDigit ('1'..'5')
#define ROT_OP_GET_MIN_I        0x0B  // :rI#          | TX: none                         -> RX: i32LE deg (rounded)
#define ROT_OP_GET_MAX_M        0x0C  // :rM#          | TX: none                         -> RX: i32LE deg (rounded)
#define ROT_OP_GET_DEG_PERSTEP_D 0x0D // :rD#          | TX: none                         -> RX: f32LE deg_per_step

#define ROT_OP_BACKLASH_b       0x0E  // :rb#          | TX: (none)                       -> RX: i16LE steps (numericReply=0)
                                      // :rb[n]#       | TX: i16LE steps                  -> RX: none (numericReply=1; CE_1 on success)

#define ROT_OP_STOP_Q           0x02  // :rQ#          | TX: none                         -> RX: none (numericReply=0)
#define ROT_OP_SET_RATE_1_9     0x0F  // :r[1..9]#     | TX: u8 rateDigit (1..9)          -> RX: none (numericReply=0)

#define ROT_OP_GET_WORKRATE_W   0x10  // :rW#          | TX: none                         -> RX: f32LE deg_per_sec (settings.gotoRate)
#define ROT_OP_CONTINUOUS_c     0x11  // :rc#          | TX: none                         -> RX: none (numericReply=0)  // ignored/no-op

#define ROT_OP_MOVE_CW_GT       0x03  // :r>#          | TX: none                         -> RX: none (numericReply=0)
#define ROT_OP_MOVE_CCW_LT      0x04  // :r<#          | TX: none                         -> RX: none (numericReply=0)

#define ROT_OP_GET_ANGLE_G      0x06  // :rG#          | TX: none                         -> RX: f32LE deg (axis3.getInstrumentCoordinate())
#define ROT_OP_GOTO_REL_rr      0x12  // :rr[deg]#     | TX: f32LE deg_relative           -> RX: none (numericReply=0)

#define ROT_OP_GOTO_S           0x05  // :rS[deg]#     | TX: f32LE deg_absolute           -> RX: none (numericReply=1; CE_1 on success else error)

#define ROT_OP_ZERO_Z           0x07  // :rZ#          | TX: none                         -> RX: none (numericReply=0)
#define ROT_OP_HALFTRAVEL_F     0x13  // :rF#          | TX: none                         -> RX: none (numericReply=0)
#define ROT_OP_HALFTARGET_C     0x14  // :rC#          | TX: none                         -> RX: none (numericReply=0)

#define ROT_OP_DEROT_EN_PLUS    0x15  // :r+#          | TX: none                         -> RX: none (numericReply=0)
#define ROT_OP_DEROT_DIS_MINUS  0x16  // :r-#          | TX: none                         -> RX: none (numericReply=0)

#define ROT_OP_GOTO_PAR_P       0x17  // :rP#          | TX: none                         -> RX: none (numericReply=0)
#define ROT_OP_DEROT_REV_R      0x18  // :rR#          | TX: none                         -> RX: none (numericReply=0)

#define ROT_OP_AVAIL_GX98       0x19  // :GX98#        | TX: none                         -> RX: u8 ASCII 'D'/'R'/'N'  (numericReply=0)

