//--------------------------------------------------------------------------------------------------
// local auxiliary features control, using the CANbus interface (CAN server)

#include "Features.h"

#if defined(FEATURES_CAN_SERVER_PRESENT)

#include "../FeaturesBase.h"
#include "../../../lib/convert/Convert.h"
#include "../../../libApp/weather/Weather.h"
#include "../../../telescope/Telescope.h"

// Dew heater: -5.0..+20.0C encoded as 0..250 in 0.1C steps (one byte).
static inline uint8_t packDew01C(float v) {
  if (v < -5.0f) v = -5.0f;
  if (v > 20.0f) v = 20.0f;
  long enc = lroundf((v + 5.0f) * 10.0f);
  if (enc < 0) enc = 0;
  if (enc > 250) enc = 250;
  return (uint8_t)enc;
}

static inline float unpackDew01C(uint8_t enc) {
  return ((float)enc / 10.0f) - 5.0f;
}

void Features::processCommand() {

  bool handled        = true;
  bool numericReply   = true;
  bool suppressFrame  = false;
  CommandError commandError = CE_NONE;

  switch (opCode()) {

    // ------------------------------------------------------------
    // :GXY0#  -> bitmap of active features (8 bits)
    // Payload: [u8 bitmap]
    // ------------------------------------------------------------
    case FEAT_OP_GET_ACTIVE_Y0: {
      uint8_t bitmap = 0;
      for (uint8_t i = 0; i < 8; i++) {
        if (device[i].purpose != OFF) bitmap |= (uint8_t)(1U << i);
      }
      writeU8(bitmap);
      numericReply = false;
    } break;

    // ------------------------------------------------------------
    // :GXYn#  (wire: idx 0..7) -> purpose + short name
    // Payload: [u8 purpose][name bytes... up to 10]
    // (No terminator; client adds it.)
    // ------------------------------------------------------------
    case FEAT_OP_GET_INFO_Yn: {
      uint8_t idx = 0;
      if (!readU8(idx)) { commandError = CE_PARAM_FORM; break; }
      if (idx > 7)      { commandError = CE_PARAM_RANGE; break; }

      const int pRaw = device[idx].purpose;
      if (pRaw == OFF) { commandError = CE_0; break; }

      // Match Features.command.cpp: report MOMENTARY/COVER as SWITCH in purpose field.
      int p = pRaw;
      if (p == MOMENTARY_SWITCH || p == COVER_SWITCH) p = SWITCH;

      writeU8((uint8_t)p);

      char name10[10];
      memset(name10, 0, sizeof(name10));
      strncpy(name10, device[idx].name, 10);

      const uint8_t n = (uint8_t)strnlen(name10, 10);
      if (n > 0) writeBytes((const uint8_t*)name10, n);

      // payload <= 11 bytes -> fits dual (12 max)
      numericReply = false;
    } break;

    // ------------------------------------------------------------
    // :GXXn#  (wire: idx 0..7) -> per-purpose packed values
    // Payload always begins with [u8 purpose]
    // Then purpose-specific bytes:
    //   SWITCH/ANALOG:        [u8 value]
    //   DEW_HEATER:           [u8 en][u8 zEnc][u8 sEnc][u8 dEnc(0.5C int8 as u8)]
    //   INTERVALOMETER:       [u8 cur][u8 expB][u8 delB][u8 count]
    // Optional (when POWER_MONITOR_PRESENT): append 5 bytes:
    //   [i16 volts_q0.1][i16 amps_q0.1][u8 flags]
    // Total stays <= 12.
    // ------------------------------------------------------------
    case FEAT_OP_GET_VALUE_Xn: {
      uint8_t idx = 0;
      if (!readU8(idx)) { commandError = CE_PARAM_FORM; break; }
      if (idx > 7)      { commandError = CE_PARAM_RANGE; break; }

      const int p = device[idx].purpose;
      if (p == OFF) { commandError = CE_CMD_UNKNOWN; break; }

      writeU8((uint8_t)p);

      if (p == SWITCH || p == MOMENTARY_SWITCH || p == COVER_SWITCH) {
        writeU8((uint8_t)device[idx].value);
      } else

      if (p == ANALOG_OUTPUT) {
        writeU8((uint8_t)device[idx].value);
      } else

      if (p == DEW_HEATER) {
        const uint8_t en   = (uint8_t)(device[idx].dewHeater->isEnabled() ? 1 : 0);
        const uint8_t zEnc = packDew01C(device[idx].dewHeater->getZero());
        const uint8_t sEnc = packDew01C(device[idx].dewHeater->getSpan());
        const float delta = (float)(temperature.getChannel((int)idx + 1) - weather.getDewPoint());

        writeU8(en);
        writeU8(zEnc);
        writeU8(sEnc);
        writeFixedI16(delta, 10);
      } else

      if (p == INTERVALOMETER) {
        const uint8_t cur   = (uint8_t)device[idx].intervalometer->getCurrentCount();
        const uint8_t expB  = (uint8_t)convert.packSeconds(device[idx].intervalometer->getExposure());
        const uint8_t delB  = (uint8_t)convert.packSeconds(device[idx].intervalometer->getDelay());
        const uint8_t count = (uint8_t)device[idx].intervalometer->getCount();

        writeU8(cur);
        writeU8(expB);
        writeU8(delB);
        writeU8(count);
      } else {
        // Unknown purpose: return stored value byte
        writeU8((uint8_t)device[idx].value);
      }

      // Power telemetry: always append when compiled in so client sees remaining()==5.
      #ifdef POWER_MONITOR_PRESENT
        uint8_t flags = 0;

        const bool present = powerMonitor.hasChannel((int)idx);
        if (present) flags |= FEAT_POWER_FLAGS_PRESENT;

        if (powerMonitor.errOverCurrent((int)idx))     flags |= FEAT_POWER_FAULT_OC;
        if (powerMonitor.errUnderVoltage((int)idx))    flags |= FEAT_POWER_FAULT_UV;
        if (powerMonitor.errOverVoltage((int)idx))     flags |= FEAT_POWER_FAULT_OV;
        if (powerMonitor.errOverTemperature((int)idx)) flags |= FEAT_POWER_FAULT_OT;

        float volts = NAN;
        float amps  = NAN;
        if (powerMonitor.hasVoltage((int)idx)) volts = powerMonitor.getVoltage((int)idx);
        if (powerMonitor.hasCurrent((int)idx)) amps  = powerMonitor.getCurrent((int)idx);

        // Exactly 5 bytes appended
        writeFixedI16(volts, 10);
        writeFixedI16(amps,  10);
        writeU8(flags);
      #endif

      numericReply = false;
    } break;

    // ------------------------------------------------------------
    // :SXXn,...  (wire: [idx1 1..8][sub][u8 payload])
    // Response is numeric-only (numericReply=true). Success = CE_NONE.
    // ------------------------------------------------------------
    case FEAT_OP_SET_VALUE_Xn: {
      uint8_t idx1 = 0;
      uint8_t sub  = 0;
      if (!readU8(idx1) || !readU8(sub)) { commandError = CE_PARAM_FORM; break; }
      if (idx1 < 1 || idx1 > 8)          { commandError = CE_PARAM_RANGE; break; }

      const uint8_t idx = (uint8_t)(idx1 - 1);
      const int p = device[idx].purpose;
      if (p == OFF) { commandError = CE_CMD_UNKNOWN; break; }

      if (sub == (uint8_t)'V') {
        uint8_t v = 0;
        if (!readU8(v)) { commandError = CE_PARAM_FORM; break; }
        device[idx].value = (long)v;

        if (p == SWITCH || p == MOMENTARY_SWITCH || p == COVER_SWITCH) {
          if (v > 1) { commandError = CE_PARAM_RANGE; break; }

          #ifdef COVER_SWITCH_SERVO_PRESENT
          if (p == COVER_SWITCH) {
            if (v == 0) cover[idx].target = COVER_SWITCH_SERVO_OPEN_DEG;
            else        cover[idx].target = COVER_SWITCH_SERVO_CLOSED_DEG;
          } else
          #endif
          {
            digitalWriteEx(device[idx].pin, (v != 0) == device[idx].active);
            if (p == MOMENTARY_SWITCH && device[idx].value) momentarySwitchTime[idx] = 50;
          }
        } else

        if (p == ANALOG_OUTPUT) {
          analogWriteEx(device[idx].pin, analog8BitToAnalogRange(v));
        } else

        if (p == DEW_HEATER) {
          if (v > 1) { commandError = CE_PARAM_RANGE; break; }
          device[idx].dewHeater->enable(v != 0);
        } else

        if (p == INTERVALOMETER) {
          if (v > 1) { commandError = CE_PARAM_RANGE; break; }
          device[idx].intervalometer->enable(v != 0);
        }

      } else

      if (sub == (uint8_t)'Z' || sub == (uint8_t)'S') {
        uint8_t enc = 0;
        if (!readU8(enc)) { commandError = CE_PARAM_FORM; break; }
        if (p != DEW_HEATER) { commandError = CE_CMD_UNKNOWN; break; }

        const float f = unpackDew01C(enc);
        if (sub == (uint8_t)'Z') device[idx].dewHeater->setZero(f);
        else                     device[idx].dewHeater->setSpan(f);

      } else

      if (sub == (uint8_t)'E' || sub == (uint8_t)'D') {
        uint8_t tb = 0;
        if (!readU8(tb)) { commandError = CE_PARAM_FORM; break; }
        if (p != INTERVALOMETER) { commandError = CE_CMD_UNKNOWN; break; }

        const float sec = convert.unpackSeconds(tb);
        if (sub == (uint8_t)'E') device[idx].intervalometer->setExposure(sec);
        else                     device[idx].intervalometer->setDelay(sec);

      } else

      if (sub == (uint8_t)'C') {
        uint8_t c = 0;
        if (!readU8(c)) { commandError = CE_PARAM_FORM; break; }
        if (p != INTERVALOMETER) { commandError = CE_CMD_UNKNOWN; break; }

        device[idx].intervalometer->setCount((float)c);

      } else {
        commandError = CE_PARAM_FORM;
      }

      // numericReply stays true
    } break;

    default:
      handled = false;
      commandError = CE_CMD_UNKNOWN;
    break;
  }

  sendResponse(handled, suppressFrame, numericReply, commandError);
}

#endif
