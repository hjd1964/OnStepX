// general purpose encoder class

#include "EncoderBase.h"

#if AXIS1_ENCODER != OFF || AXIS2_ENCODER != OFF || AXIS3_ENCODER != OFF || \
    AXIS4_ENCODER != OFF || AXIS5_ENCODER != OFF || AXIS6_ENCODER != OFF || \
    AXIS7_ENCODER != OFF || AXIS8_ENCODER != OFF || AXIS9_ENCODER != OFF

#include "Arduino.h"
#include "../tasks/OnTask.h"

Encoder *encoder[9];

void encoderCallback1() { encoder[0]->poll(); }
void encoderCallback2() { encoder[1]->poll(); }
void encoderCallback3() { encoder[2]->poll(); }
void encoderCallback4() { encoder[3]->poll(); }
void encoderCallback5() { encoder[4]->poll(); }
void encoderCallback6() { encoder[5]->poll(); }
void encoderCallback7() { encoder[6]->poll(); }
void encoderCallback8() { encoder[7]->poll(); }
void encoderCallback9() { encoder[8]->poll(); }

// get device ready for use
bool Encoder::init() {
  if (ready) return true;
  if (axis < 1 || axis > 9) return false;

  encoder[axis - 1] = this;

  uint8_t taskHandle = 0;
  VF("MSG: Encoder"); V(axis); VF(", start status monitor task... ");

  switch (axis) {
    case 1:
      taskHandle = tasks.add(1000, 0, true, 6, encoderCallback1, "encSta1");
    break;
    case 2:
      taskHandle = tasks.add(1000, 0, true, 6, encoderCallback2, "encSta2");
    break;
    case 3:
      taskHandle = tasks.add(1000, 0, true, 6, encoderCallback3, "encSta3");
    break;
    case 4:
      taskHandle = tasks.add(1000, 0, true, 6, encoderCallback4, "encSta4");
    break;
    case 5:
      taskHandle = tasks.add(1000, 0, true, 6, encoderCallback5, "encSta5");
    break;
    case 6:
      taskHandle = tasks.add(1000, 0, true, 6, encoderCallback6, "encSta6");
    break;
    case 7:
      taskHandle = tasks.add(1000, 0, true, 6, encoderCallback7, "encSta7");
    break;
    case 8:
      taskHandle = tasks.add(1000, 0, true, 6, encoderCallback8, "encSta8");
    break;
    case 9:
      taskHandle = tasks.add(1000, 0, true, 6, encoderCallback9, "encSta9");
    break;
  }

  if (taskHandle) { VL("success"); } else { VLF("FAILED!"); return false; }

  #if ENCODER_FILTER > 0
    ticksToWait = halNsToTicks(ENCODER_FILTER);
    if (ticksToWait > 0x7FFFFFFFUL) ticksToWait = 0x7FFFFFFFUL;
  #endif

  #if ENCODER_VELOCITY == ON
    velTicksPerSec = HAL_TICKS_PER_SECOND();

    // Cache stop timeout (us) converted to ticks (avoids 64-bit math in readVelocityCps())
    uint64_t stopTicks64 = (uint64_t)ENCODER_VEL_STOP_US * (uint64_t)velTicksPerSec;
    stopTicks64 = (stopTicks64 + 500000ULL) / 1000000ULL;     // rounded divide by 1e6
    if (stopTicks64 > 0x7FFFFFFFULL) stopTicks64 = 0x7FFFFFFFULL;
    velStopTicks = (uint32_t)stopTicks64;

    velLastEdgeTicks   = 0;
    velLastPeriodTicks = 0;
    velLastDir         = 0;

    velWinLastCount = (int32_t)count;
    velWinLastUs    = 0;
    velBlendCps     = NAN;
    velHasEstimate = false;
  #endif

  return true;
}

float Encoder::readVelocityCps() {
  #if ENCODER_VELOCITY != ON
    return NAN;
  #else

  // Snapshot state (safe if velLast* can be updated from ISR).
  uint32_t lastEdgeTicks, lastPeriodTicks;
  int8_t   dirSnap;
  int32_t  countSnap;

  noInterrupts();
  lastEdgeTicks   = velLastEdgeTicks;
  lastPeriodTicks = velLastPeriodTicks;
  dirSnap         = velLastDir;
  countSnap       = count;
  interrupts();

  // Cached stop timeout (ticks) computed once in init()
  const uint32_t nowTicks  = HAL_FAST_TICKS();
  const uint32_t stopTicks = velStopTicks;

  const uint32_t ageTicks = nowTicks - lastEdgeTicks;
  const bool stale = (lastEdgeTicks == 0) || (ageTicks > stopTicks);

  // (A) period/reciprocal estimate (best at low speed)
  float v_per = NAN;
  if (!stale && lastPeriodTicks != 0 && dirSnap != 0) {
    v_per = (dirSnap > 0 ? 1.0F : -1.0F) * ((float)velTicksPerSec / (float)lastPeriodTicks);
  }

  // (B) windowed count estimate (best at higher speed)
  const uint32_t nowUs = micros();
  if (velWinLastUs == 0) {
    velWinLastUs    = nowUs;
    velWinLastCount = countSnap;

    // If we already have a period estimate, expose it immediately (low-speed friendly).
    if (!isnan(v_per)) {
      velBlendCps    = v_per;
      velHasEstimate = true;
      return velBlendCps;
    }

    // Otherwise: no usable estimate yet.
    return NAN;
  }

  const uint32_t dtUs = nowUs - velWinLastUs;
  const int32_t  dc   = countSnap - velWinLastCount;

  if (dtUs >= (uint32_t)ENCODER_VEL_WINDOW_US) {
    const float v_cnt = (dtUs != 0) ? (dc*(1000000.0F/(float)dtUs)) : 0.0F;

    // Fix #1: if period estimate is unavailable, do NOT bias toward 0; use v_cnt directly.
    if (isnan(v_per)) {
      velBlendCps = v_cnt;
    } else {
      float a = fabsf((float)dc) / (float)ENCODER_VEL_BLEND_COUNTS;
      if (a < 0.0F) a = 0.0F;
      if (a > 1.0F) a = 1.0F;
      velBlendCps = a * v_cnt + (1.0F - a) * v_per;
    }

    velHasEstimate = true;

    // Reset window
    velWinLastUs    = nowUs;
    velWinLastCount = countSnap;
  } else {
    // Before we've ever produced an estimate, allow v_per to seed velBlendCps.
    if (!velHasEstimate && !isnan(v_per)) {
      velBlendCps    = v_per;
      velHasEstimate = true;
    }

    // Fix #2: stop timeout means stop timeout (once we have any estimate).
    if (velHasEstimate && stale) {
      velBlendCps = 0.0F;
    }
  }

  return velBlendCps;

#endif
}

// update encoder status
void Encoder::poll() {
  #if ENCODER_FILTER > 0
    noInterrupts();
    msNow += 1000;
    interrupts();
  #endif

  // run once every 5 seconds
  if (++tick % 5 == 0) {

    noInterrupts();
    uint32_t w = warn;
    warn = 0;
    uint32_t e = error;
    error = 0;
    interrupts();

    totalWarningCount += w;
    totalErrorCount += e;

    uint32_t errors = e;
    #ifdef ENCODER_WARN_AS_ERROR
      errors += w;
    #endif

    // look at how often errors and warnings occur on a given axis
    // VF("MSG: Encoder"); V(axis); VF(" status, errors="); V(totalErrorCount); VF(" warnings="); VL(totalWarningCount);

    if (errors > UINT16_MAX) errors = UINT16_MAX;
    errorCount[errorCountIndex++ % 12] = errors;

    // sum the errors and set the error flag as required
    errors = 0;
    for (int i = 0; i < 12; i++) { errors += errorCount[i]; }

    errorState = errors > ENCODER_ERROR_COUNT_THRESHOLD;

    if (errorState != lastErrorState) {
      if (errorState) {
        DF("WRN: Encoder"); D(axis); DF(" errors, exceeded threshold/minute at "); DL(errors);
      } else {
        DF("MSG: Encoder"); D(axis); DF(" errors, within threshold/minute at "); DL(errors);
      }
      lastErrorState = errorState;
    }
  }
}

#endif
