//--------------------------------------------------------------------------------------------------
// remote auxiliary features control, using the CANbus interface (CAN client encode)

#include "Features.h"

#if defined(FEATURES_CAN_CLIENT_PRESENT)

#include "../FeaturesBase.h"

// ---------- helpers ----------

static inline void writeU16LE(uint8_t *p, uint16_t v) {
  p[0] = (uint8_t)(v & 0xFF);
  p[1] = (uint8_t)((v >> 8) & 0xFF);
}

// Dew heater: -5.0..+20.0C encoded as 0..250 in 0.1C steps (one byte).
static inline uint8_t packDew01C(float v) {
  if (v < -5.0f) v = -5.0f;
  if (v > 20.0f) v = 20.0f;
  const int enc = (int)lroundf((v + 5.0f) * 10.0f);
  if (enc < 0) return 0;
  if (enc > 250) return 250;
  return (uint8_t)enc;
}

// Intervalometer: seconds encoded as 0.1s units in u16.
static inline uint16_t pack01s(float sec) {
  if (sec < 0.0f) sec = 0.0f;
  if (sec > 6553.5f) sec = 6553.5f;
  const int v = (int)lroundf(sec * 10.0f);
  if (v < 0) return 0;
  if (v > 65535) return 65535;
  return (uint16_t)v;
}

static inline bool parseFloat(const char *s, float &out) {
  if (!s) return false;
  char *endp = nullptr;
  out = strtof(s, &endp);
  return (endp != s);
}

static inline bool parseU8FromFloatStr(const char *s, uint8_t &out) {
  float f = 0.0f;
  if (!parseFloat(s, f)) return false;
  long v = lroundf(f);
  if (v < 0) v = 0;
  if (v > 255) v = 255;
  out = (uint8_t)v;
  return true;
}

//--------------------------------------------------------------------------------------------------
// Encode LX200-ish AF commands into CAN request payloads.
//
// Inputs (from Telescope command router):
//   command   : 2-char opcode, e.g. "GX" or "SX"
//   parameter : remainder string (no '#'), e.g. "Y0", "Y1", "X2", "X1,V255"
//
// On-wire args:
//   GET_ACTIVE_Y0 : [tidop]
//   GET_INFO_Yn   : [tidop, idx]
//   GET_VALUE_Xn  : [tidop, idx]
//   SET_VALUE_Xn  : [tidop, idx, sub, data...]
//     sub: 'V','Z','S','E','D','C'
//     data sizes: V/Z/S/C -> 1 byte; E/D -> u16LE (0.1s)

bool Features::encodeRequest(uint8_t &opcode, uint8_t &tidop,
                             uint8_t requestPayload[8], uint8_t &requestLen,
                             char *command, char *parameter) {
  if (!requestPayload) return false;
  requestLen = 0;

  if (!command || !parameter) return false;

  // 3-bit TID, no pipelining
  static uint8_t tid = 0;
  tid = (uint8_t)((tid + 1) & 0x07);

  auto begin = [&](uint8_t op) -> void {
    opcode = op;
    tidop  = packTidOp(tid, opcode);
    requestPayload[0] = tidop;
    requestLen = 1;
  };

  // -------------------- GET commands (GX) --------------------

  if (command[0] == 'G' && command[1] == 'X') {

    // :GXY0#
    if (parameter[0] == 'Y' && parameter[1] == '0' && parameter[2] == 0) {
      begin(FEAT_OP_GET_ACTIVE_Y0);
      return true;
    }

    // :GXY[n]#  where [n]=1..8
    if (parameter[0] == 'Y' && parameter[2] == 0) {
      const char c = parameter[1];
      if (c < '1' || c > '8') return false;

      const uint8_t idx = (uint8_t)(c - '1'); // 0..7
      begin(FEAT_OP_GET_INFO_Yn);
      requestPayload[1] = idx;
      requestLen = 2;
      return true;
    }

    // :GXX[n]#  where [n]=1..8
    if (parameter[0] == 'X' && parameter[2] == 0) {
      const char c = parameter[1];
      if (c < '1' || c > '8') return false;

      const uint8_t idx = (uint8_t)(c - '1'); // 0..7
      begin(FEAT_OP_GET_VALUE_Xn);
      requestPayload[1] = idx;
      requestLen = 2;
      return true;
    }

    return false;
  }

  // -------------------- SET commands (SX) --------------------

  if (command[0] == 'S' && command[1] == 'X') {

    // Expected: "Xn,..." (per Features.command.cpp)
    // Examples: "X1,V1"   "X2,V255"  "X1,Z0.5"  "X3,E12.3"
    if (parameter[0] != 'X') return false;
    const char c = parameter[1];
    if (c < '1' || c > '8') return false;
    if (parameter[2] != ',') return false;

    const uint8_t idx = (uint8_t)(c - '1'); // 0..7
    const uint8_t sub = (uint8_t)parameter[3];
    const char   *valStr = &parameter[4];

    begin(FEAT_OP_SET_VALUE_Xn);
    requestPayload[1] = idx;
    requestPayload[2] = sub;
    requestLen = 3;

    // Subcommand packing
    if (sub == (uint8_t)'V') {
      // V: u8 (0/1 or 0..255 depending on purpose)
      uint8_t v = 0;
      if (!parseU8FromFloatStr(valStr, v)) return false;
      requestPayload[3] = v;
      requestLen = 4;
      return true;
    }

    if (sub == (uint8_t)'Z' || sub == (uint8_t)'S') {
      // Z/S: dew heater params packed as 0.1C steps in one byte
      float f = 0.0f;
      if (!parseFloat(valStr, f)) return false;
      requestPayload[3] = packDew01C(f);
      requestLen = 4;
      return true;
    }

    if (sub == (uint8_t)'E' || sub == (uint8_t)'D') {
      // E/D: intervalometer exposure/delay in seconds, packed u16LE (0.1s)
      float f = 0.0f;
      if (!parseFloat(valStr, f)) return false;
      const uint16_t enc = pack01s(f);
      writeU16LE(&requestPayload[3], enc);
      requestLen = 5;
      return true;
    }

    if (sub == (uint8_t)'C') {
      // C: intervalometer count (0..255) packed u8
      uint8_t v = 0;
      if (!parseU8FromFloatStr(valStr, v)) return false;
      requestPayload[3] = v;
      requestLen = 4;
      return true;
    }

    return false;
  }

  return false;
}

#endif
