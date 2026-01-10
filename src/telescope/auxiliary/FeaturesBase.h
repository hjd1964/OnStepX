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
#define FEAT_OP_SET_VALUE_Xn     0x02  // :SXXn,* -> RX: status only (single frame)
#define FEAT_OP_GET_INFO_Yn      0x1E  // :GXYn#  -> RX: purpose + short name (2-frame)
#define FEAT_OP_GET_VALUE_Xn     0x1F  // :GXXn#  -> RX: per-purpose packed values (2-frame)

// -----------------------------------------------------------------------------
// Power telemetry packing (appended to :GXXn# replies when POWER_MONITOR_PRESENT)
// -----------------------------------------------------------------------------
// Encoding matches the LX200 :GXXn# ASCII additions from Features::strCatPower():
//   ,<V>,<I>,<flags>
// where <V>/<I> are formatted as %1.1f or "NAN" and <flags> is 5 chars:
//   P (present) or !, then C/v/V/T each replaced by ! on fault.
//
// On-wire, we use signed int16 fixed-point with 0.1 units (LE), and a flags bitfield.
//
// Voltage/current encoding:
//   enc = round(value * 10)
//   enc = FEAT_POWER_NAN_I16 (0x8000) means NAN
//
// Flags bitfield (fault bits are 1 when faulted):
//   bit0: present (P)
//   bit1: over-current fault      (C -> !)
//   bit2: under-voltage fault     (v -> !)
//   bit3: over-voltage fault      (V -> !)
//   bit4: over-temperature fault  (T -> !)
#define FEAT_POWER_NAN_I16        0x8000
#define FEAT_POWER_FLAGS_PRESENT  0x01
#define FEAT_POWER_FAULT_OC       0x02
#define FEAT_POWER_FAULT_UV       0x04
#define FEAT_POWER_FAULT_OV       0x08
#define FEAT_POWER_FAULT_OT       0x10

// (Reserve additional opcodes here for future AF expansions.)
