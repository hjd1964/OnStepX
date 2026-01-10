//--------------------------------------------------------------------------------------------------
// remote auxiliary features control, using the CANbus interface (CAN client encode)

#include "Features.h"

#if defined(FEATURES_CAN_CLIENT_PRESENT)

#include "../FeaturesBase.h"
#include "../../../lib/convert/Convert.h"

// ---------- helpers ----------

// Dew heater: -5.0..+20.0C encoded as 0..250 in 0.1C steps (one byte).
static inline uint8_t packDew01C(float v) {
  if (v < -5.0f) v = -5.0f;
  if (v > 20.0f) v = 20.0f;
  const int enc = (int)lroundf((v + 5.0f) * 10.0f);
  if (enc < 0) return 0;
  if (enc > 250) return 250;
  return (uint8_t)enc;
}

bool Features::encodeRequest(char *command, char *parameter) {

  if (command[0] == 'G' && command[1] == 'X') {

    // :GXY0#
    if (parameter[0] == 'Y' && parameter[1] == '0' && parameter[2] == 0) {
      if (!beginNewRequest(FEAT_OP_GET_ACTIVE_Y0)) return false;
    } else

    // :GXY[n]#  where [n]=1..8
    if (parameter[0] == 'Y' && parameter[2] == 0) {
      const char c = parameter[1];
      if (c < '1' || c > '8') return false;

      const uint8_t idx = (uint8_t)(c - '1'); // 0..7
      if (!beginNewRequest(FEAT_OP_GET_INFO_Yn)) return false;
      if (!writeU8(idx)) return false;
    } else

    // :GXX[n]#  where [n]=1..8
    if (parameter[0] == 'X' && parameter[2] == 0) {
      const char c = parameter[1];
      if (c < '1' || c > '8') return false;

      const uint8_t idx = (uint8_t)(c - '1'); // 0..7
      if (!beginNewRequest(FEAT_OP_GET_VALUE_Xn)) return false;
      if (!writeU8(idx)) return false;
    } else return false;

  } else

  if (command[0] == 'S' && command[1] == 'X') {

    // Expected: "Xn,..." (per Features.command.cpp)
    // Examples: "X1,V1"   "X2,V255"  "X1,Z0.5"  "X3,E12.3"
    if (parameter[0] != 'X' || parameter[2] != ',') return false;

    const uint8_t sub = (uint8_t)parameter[3];
    int idx = parameter[1] - '0';
    if (idx < 1 || idx > 8)  return false;

    char* conv_end;
    float f = strtof(&parameter[4], &conv_end);
    if (&parameter[4] == conv_end) return false;
    long i = lroundf(f);

    if (!beginNewRequest(FEAT_OP_SET_VALUE_Xn)) return false;

    if (!writeU8(idx)) return false;
    if (!writeU8(sub)) return false;

    // Subcommand packing
    if (sub == (uint8_t)'V') {
      if (!writeU8(i)) return false;
    } else

    if (sub == (uint8_t)'Z' || sub == (uint8_t)'S') {
      // Z/S: dew heater params packed as 0.1C steps in one byte
      if (!writeU8(packDew01C(f))) return false;
    } else

    if (sub == (uint8_t)'E' || sub == (uint8_t)'D') {
      if (!writeU8(convert.packSeconds(f))) return false;
    } else

    if (sub == (uint8_t)'C') {
      // C: intervalometer count (0..255) packed u8
      if (!writeU8(i)) return false;
    } else return false;

  } else return false;

  return true;
}

#endif
