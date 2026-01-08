// -----------------------------------------------------------------------------
// CAN Auxiliary Features switchboard
// -----------------------------------------------------------------------------
#pragma once

#include "../../Common.h"

// -----------------------------------------------------------------------------
// Where the base CAN ID's start for this CANbus transport
// -----------------------------------------------------------------------------

#ifndef CAN_FEATURES_REQ_ID
  #define CAN_FEATURES_REQ_ID (0x00 + 0x0F)
#endif

#ifndef CAN_FEATURES_RSP_ID
  #define CAN_FEATURES_RSP_ID (CAN_FEATURES_REQ_ID + 0x10)
#endif

#ifndef CAN_FEATURES_HB_ID_BASE
  #define CAN_FEATURES_HB_ID_BASE (CAN_FEATURES_REQ_ID + 0x20)
#endif

// -----------------------------------------------------------------------------
// CAN_FEATURES role selection
//   OFF    : no CAN auxiliary feature support
//   ON     : telescope forwards auxiliary feature commands over CAN (client/proxy)
//   REMOTE : remote node answers auxiliary feature commands over CAN (server/local FEATURES)
// -----------------------------------------------------------------------------

#ifndef CAN_FEATURES
  #define CAN_FEATURES OFF
#endif

// -----------------------------------------------------------------------------
// Validate CAN requirements
// -----------------------------------------------------------------------------
#if (CAN_FEATURES != OFF)
  #if !defined(CAN_PLUS) || (CAN_PLUS == OFF)
    #error "Features: CAN_FEATURES requires CAN_PLUS to be enabled."
  #endif
#endif

// -----------------------------------------------------------------------------
// Derive presence macros used by Telescope.cpp command router
// -----------------------------------------------------------------------------
#if (CAN_FEATURES == ON)
  // Local FEATURES_PRESENT must NOT exist in this build; commands are proxied.
  #define FEATURES_CLIENT_PRESENT           // remote auxiliary features may be present
  #define FEATURES_CAN_CLIENT_PRESENT       // client implementation using CAN
#elif (CAN_FEATURES == REMOTE)
  // Must have a local Features implementation to serve anything.
  #define FEATURES_CAN_SERVER_PRESENT       // server implementation (compiled into local Features)
#endif

// -----------------------------------------------------------------------------
// Enforce “only one auxiliary-features provider” contract
// -----------------------------------------------------------------------------

// CLIENT build must NOT also have local FEATURES_PRESENT.
#if defined(FEATURES_PRESENT) && defined(FEATURES_CLIENT_PRESENT)
  #error "Config: both local FEATURES_PRESENT and CAN_FEATURES ON are enabled. Choose one."
#endif

// REMOTE build MUST have local FEATURES_PRESENT (otherwise it can't serve anything).
#if !defined(FEATURES_PRESENT) && defined(FEATURES_CAN_SERVER_PRESENT)
  #error "Config: CAN_FEATURES REMOTE requires local FEATURES_PRESENT."
#endif

// -----------------------------------------------------------------------------
// CANbus command opcode set (5-bit). Per-ID namespace, so Features "owns" these.
// These ops map to the LX200-ish AF commands shown in Features.command.cpp.
// -----------------------------------------------------------------------------

// Suggested v1 set (keep lean; expand as needed):
#define FEAT_OP_GET_ACTIVE_Y0    0x01  // :GXY0#  -> RX: bitmap of active features (8 bits)
#define FEAT_OP_GET_INFO_Yn      0x02  // :GXYn#  -> RX: purpose + short name (may be 2-frame)
#define FEAT_OP_GET_VALUE_Xn     0x03  // :GXXn#  -> RX: per-purpose packed values (single frame)
#define FEAT_OP_SET_VALUE_Xn     0x04  // :SXXn,* -> RX: status only (single frame)

// (Reserve additional opcodes here for future AF expansions.)

