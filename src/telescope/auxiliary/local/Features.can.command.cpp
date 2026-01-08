//--------------------------------------------------------------------------------------------------
// local auxiliary features control, using the CANbus interface

#include "Features.h"

#ifdef FEATURES_CAN_SERVER_PRESENT

#include "../../../lib/convert/Convert.h"
#include "../../../libApp/weather/Weather.h"
#include "../../../telescope/Telescope.h"

// ---------- helpers ----------

static inline uint8_t tidopPlus1(uint8_t tidop) {
  const uint8_t op  = (uint8_t)(tidop & 0x1F);
  const uint8_t tid = (uint8_t)((tidop >> 5) & 0x07);
  const uint8_t tid1 = (uint8_t)((tid + 1) & 0x07);
  return (uint8_t)((tid1 << 5) | op);
}

static inline uint16_t readU16LE(const uint8_t *p) {
  return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

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

static inline float unpackDew01C(uint8_t enc) {
  return ((float)enc / 10.0f) - 5.0f;
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

static inline float unpack01s(uint16_t enc) {
  return (float)enc / 10.0f;
}

// ---------- command processor ----------
//
// Request frames: [ tidop, ...args ]
//
// Args (after tidop):
//   FEAT_OP_GET_ACTIVE_Y0: (none)
//   FEAT_OP_GET_INFO_Yn  : [ idx ]      idx 0..7
//   FEAT_OP_GET_VALUE_Xn : [ idx ]      idx 0..7
//   FEAT_OP_SET_VALUE_Xn : [ idx, sub, data... ]
//     sub: 'V','Z','S','E','D','C' (aligned to Features.command.cpp)
//
// Responses:
//   Single: [ tidop, status, payload... ]   (payload 0..6 bytes)
//   Two-frame (GET_INFO only):
//     frame0 tidop:   payload = [purpose, name_part0...]  (purpose + up to 5 chars)
//     frame1 tidop+1: payload = [name_part1...]           (up to 6 chars)
//   No terminators transmitted; DLC is authoritative.

// default command semantics (mirrors the common command contract):
// handled=true, numericReply=true, suppressFrame=false, commandError=CE_NONE
// numericReply=true means boolean/numeric-style responses (e.g., CE_1/CE_0/errors) rather than a payload
void Features::processCommand(const uint8_t data[8], uint8_t len) {
  if (len < 1) return;

  const uint8_t tidop = data[0];
  const uint8_t op    = (uint8_t)(tidop & 0x1F);

  const uint8_t *args = (len > 1) ? &data[1] : nullptr;
  const uint8_t  argLen = (len > 1) ? (uint8_t)(len - 1) : 0;

  uint8_t payload[6] = {0};
  uint8_t payloadLen = 0;

  // Standard defaults
  bool handled        = true;
  bool numericReply   = true;
  bool suppressFrame  = false;
  CommandError commandError = CE_NONE;

  auto send1 = [&](uint8_t rspTidOp, const uint8_t *pl, uint8_t plLen) {
    const uint8_t status = packStatus(handled, numericReply, suppressFrame, commandError);
    sendResponse(rspTidOp, status, (plLen ? pl : nullptr), plLen);
  };

  auto sendErr = [&](CommandError ce) {
    commandError = ce;
    send1(tidop, nullptr, 0);
  };

  auto parseIdx = [&](int &idx) -> bool {
    if (argLen < 1) return false;
    idx = (int)args[0];
    return (idx >= 0 && idx <= 7);
  };

  switch (op) {

    // :GXY0# -> bitmap of active features (bit i => device[i].purpose != OFF)
    case FEAT_OP_GET_ACTIVE_Y0: {
      numericReply = false;

      uint8_t bitmap = 0;
      for (uint8_t i = 0; i < 8; i++) {
        if (device[i].purpose != OFF) bitmap |= (uint8_t)(1U << i);
      }
      payload[payloadLen++] = bitmap;

      send1(tidop, payload, payloadLen);

      return;
    }

    // :GXYn# -> purpose + short name (two-frame)
    case FEAT_OP_GET_INFO_Yn: {
      numericReply = false;

      int idx = 0;
      if (!parseIdx(idx)) { sendErr(CE_PARAM_RANGE); return; }
      if (device[idx].purpose == OFF) { sendErr(CE_0); return; }

      // Normalize purpose for MOMENTARY/COVER as SWITCH (matches LX200 handler)
      int purpose = device[idx].purpose;
      if (purpose == MOMENTARY_SWITCH || purpose == COVER_SWITCH) purpose = SWITCH;

      // Keep name short. Local LX200 truncates to 10; we follow that.
      const char *nm = device[idx].name ? device[idx].name : "";
      char name10[10];
      uint8_t nlen = 0;
      while (nlen < 10 && nm[nlen] != 0) { name10[nlen] = nm[nlen]; nlen++; }

      // Frame 0 MUST be a full 6-byte payload so the client transact2() will
      // reliably append frame1 data when present.
      uint8_t p0[6] = {0};
      p0[0] = (uint8_t)purpose;

      const uint8_t n0 = (nlen > 5) ? 5 : nlen;
      for (uint8_t i = 0; i < n0; i++) p0[1 + i] = (uint8_t)name10[i];
      const uint8_t p0Len = 6;

      // Frame 1: remainder up to 6 bytes
      uint8_t p1[6] = {0};
      uint8_t p1Len = 0;
      const uint8_t rem = (nlen > n0) ? (uint8_t)(nlen - n0) : 0;
      const uint8_t n1  = (rem > 6) ? 6 : rem;
      for (uint8_t i = 0; i < n1; i++) p1[p1Len++] = (uint8_t)name10[n0 + i];

      // Always send both frames (client transact2 waits for both)
      send1(tidop, p0, p0Len);
      send1(tidopPlus1(tidop), p1, p1Len);

      return;
    }

    // :GXXn# -> get value (packed by purpose)
    case FEAT_OP_GET_VALUE_Xn: {
      numericReply = false;

      int idx = 0;
      if (!parseIdx(idx)) { sendErr(CE_PARAM_RANGE); return; }
      if (device[idx].purpose == OFF) { sendErr(CE_CMD_UNKNOWN); return; }

      const int purpose = device[idx].purpose;

      if (purpose == SWITCH || purpose == MOMENTARY_SWITCH || purpose == COVER_SWITCH) {
        payload[payloadLen++] = (uint8_t)((device[idx].value != 0) ? 1 : 0);

      } else

      if (purpose == ANALOG_OUTPUT) {
        numericReply = false;

        payload[payloadLen++] = (uint8_t)device[idx].value; // 0..255

      } else

      if (purpose == DEW_HEATER) {
        numericReply = false;

        if (!device[idx].dewHeater) { sendErr(CE_REPLY_UNKNOWN); return; }

        payload[payloadLen++] = (uint8_t)(device[idx].dewHeater->isEnabled() ? 1 : 0);
        payload[payloadLen++] = packDew01C(device[idx].dewHeater->getZero());
        payload[payloadLen++] = packDew01C(device[idx].dewHeater->getSpan());

        // delta = temp(channel) - dewpoint packed as int8 in 0.5C steps
        const float delta = temperature.getChannel(idx + 1) - weather.getDewPoint();
        int d = (int)lroundf(delta * 2.0f);
        if (d < -128) d = -128;
        if (d >  127) d =  127;
        payload[payloadLen++] = (uint8_t)(int8_t)d;

      } else

      if (purpose == INTERVALOMETER) {
        numericReply = false;

        if (!device[idx].intervalometer) { sendErr(CE_REPLY_UNKNOWN); return; }

        // 6-byte fixed payload:
        // [currentCount][exposure u16 0.1s][delay u16 0.1s][count]
        payload[payloadLen++] = (uint8_t)device[idx].intervalometer->getCurrentCount();

        const uint16_t e = pack01s(device[idx].intervalometer->getExposure());
        const uint16_t d = pack01s(device[idx].intervalometer->getDelay());

        writeU16LE(&payload[payloadLen], e); payloadLen += 2;
        writeU16LE(&payload[payloadLen], d); payloadLen += 2;

        payload[payloadLen++] = (uint8_t)device[idx].intervalometer->getCount();

      } else {
        sendErr(CE_CMD_UNKNOWN);
        return;
      }

      if (payloadLen > 6) payloadLen = 6;

      send1(tidop, payload, payloadLen);
      return;
    }

    // :SXXn,* -> set value/params (status-only ack)
    case FEAT_OP_SET_VALUE_Xn: {
      if (argLen < 2) { sendErr(CE_PARAM_FORM); return; }

      const int idx = (int)args[0];
      if (idx < 0 || idx > 7) { sendErr(CE_PARAM_RANGE); return; }
      if (device[idx].purpose == OFF) { sendErr(CE_CMD_UNKNOWN); return; }

      const uint8_t sub = args[1];
      const int purpose = device[idx].purpose;

      auto need = [&](uint8_t n) -> bool { return argLen >= (uint8_t)(2 + n); };

      // 'V' shared across multiple purposes
      if (sub == (uint8_t)'V') {
        if (!need(1)) { sendErr(CE_PARAM_FORM); return; }
        const uint8_t v = args[2];

        if (purpose == SWITCH || purpose == MOMENTARY_SWITCH || purpose == COVER_SWITCH) {
          if (v > 1) { sendErr(CE_PARAM_RANGE); return; }
          device[idx].value = v;

          #ifdef COVER_SWITCH_SERVO_PRESENT
          if (purpose == COVER_SWITCH) {
            // match LX200 handler: 0=open, 1=closed
            if (v == 0) cover[idx].target = COVER_SWITCH_SERVO_OPEN_DEG;
            else        cover[idx].target = COVER_SWITCH_SERVO_CLOSED_DEG;
          } else
          #endif
          {
            digitalWriteEx(device[idx].pin, v == device[idx].active);
            if (purpose == MOMENTARY_SWITCH && v) momentarySwitchTime[idx] = 50;
          }

          commandError = CE_1; // success ack
          send1(tidop, nullptr, 0);
          return;
        }

        if (purpose == ANALOG_OUTPUT) {
          device[idx].value = v;
          analogWriteEx(device[idx].pin, analog8BitToAnalogRange(v));

          commandError = CE_1;
          send1(tidop, nullptr, 0);
          return;
        }

        if (purpose == DEW_HEATER) {
          if (!device[idx].dewHeater) { sendErr(CE_REPLY_UNKNOWN); return; }
          if (v > 1) { sendErr(CE_PARAM_RANGE); return; }
          device[idx].value = v;
          device[idx].dewHeater->enable(v ? true : false);

          commandError = CE_1;
          send1(tidop, nullptr, 0);
          return;
        }

        if (purpose == INTERVALOMETER) {
          if (!device[idx].intervalometer) { sendErr(CE_REPLY_UNKNOWN); return; }
          if (v > 1) { sendErr(CE_PARAM_RANGE); return; }
          device[idx].value = v;
          device[idx].intervalometer->enable(v ? true : false);

          commandError = CE_1;
          send1(tidop, nullptr, 0);
          return;
        }

        sendErr(CE_CMD_UNKNOWN);
        return;
      }

      // Dew heater parameters: 'Z' and 'S' are 1 byte each (0.1C steps)
      if (purpose == DEW_HEATER) {
        if (!device[idx].dewHeater) { sendErr(CE_REPLY_UNKNOWN); return; }

        if (sub == (uint8_t)'Z') {
          if (!need(1)) { sendErr(CE_PARAM_FORM); return; }
          device[idx].dewHeater->setZero(unpackDew01C(args[2]));

          commandError = CE_1;
          send1(tidop, nullptr, 0);
          return;
        }

        if (sub == (uint8_t)'S') {
          if (!need(1)) { sendErr(CE_PARAM_FORM); return; }
          device[idx].dewHeater->setSpan(unpackDew01C(args[2]));

          commandError = CE_1;
          send1(tidop, nullptr, 0);
          return;
        }
      }

      // Intervalometer parameters:
      // 'E' exposure u16 0.1s, 'D' delay u16 0.1s, 'C' count u8
      if (purpose == INTERVALOMETER) {
        if (!device[idx].intervalometer) { sendErr(CE_REPLY_UNKNOWN); return; }

        if (sub == (uint8_t)'E') {
          if (!need(2)) { sendErr(CE_PARAM_FORM); return; }
          device[idx].intervalometer->setExposure(unpack01s(readU16LE(&args[2])));

          commandError = CE_1;
          send1(tidop, nullptr, 0);
          return;
        }

        if (sub == (uint8_t)'D') {
          if (!need(2)) { sendErr(CE_PARAM_FORM); return; }
          device[idx].intervalometer->setDelay(unpack01s(readU16LE(&args[2])));

          commandError = CE_1;
          send1(tidop, nullptr, 0);
          return;
        }

        if (sub == (uint8_t)'C') {
          if (!need(1)) { sendErr(CE_PARAM_FORM); return; }
          device[idx].intervalometer->setCount((float)args[2]);

          commandError = CE_1;
          send1(tidop, nullptr, 0);
          return;
        }
      }

      sendErr(CE_PARAM_FORM);
      return;
    }

    default:
      handled = false;
      commandError = CE_CMD_UNKNOWN;
      send1(tidop, nullptr, 0);
      return;
  }
}

#endif
