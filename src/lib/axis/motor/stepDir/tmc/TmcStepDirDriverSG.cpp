// -----------------------------------------------------------------------------------
// axis step/dir motor driver + StallGuard helper
//

#include <math.h>
#include <stdio.h>
#include "TmcStepDirDriverSG.h"
#include "../../../../nv/Nv.h"

#if defined(STEP_DIR_MOTOR_PRESENT)

// Dual-rate adaptation:
// - SG Train ON  => fast EMA (explicit training)
#ifndef BASE_TC_FAST
  #define BASE_TC_FAST 128.0f
#endif
#ifndef DEV_TC_FAST
  #define DEV_TC_FAST  256.0f
#endif
// - SG Train OFF => slow EMA drift tracking
#ifndef BASE_TC_SLOW
  #define BASE_TC_SLOW 4096.0f
#endif
#ifndef DEV_TC_SLOW
  #define DEV_TC_SLOW  8192.0f
#endif

// Margin for model:
#define MARGIN_GENERAL 80 // general was 40
#define MARGIN_NOISE   4  // noise relative was 3

TmcStepDirDriverSG::TmcStepDirDriverSG(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings,
                                       int16_t currentHold, int16_t currentRun, int16_t currentSlewing, int8_t intpol)
                                       :TmcStepDirDriver(axisNumber, Pins, Settings, currentHold, currentRun, currentSlewing, intpol) {

  sgEnabled.valueDefault      = (float)SG_ENABLED;
  sgTrain.valueDefault        = (float)SG_TRAIN;

  // Floor disabled by default (0%)
  sgFloorPct.valueDefault     = (float)SG_FLOOR;
  sgRampAllowPct.valueDefault = (float)SG_RAMP_ALLOW;

  sgArmFps.valueDefault       = (float)SG_ARM_FPS;
  sgArmFaultMs.valueDefault   = (float)SG_FAULT_MS;
  sgSensitivity.valueDefault  = (float)SG_SENS;
}

bool TmcStepDirDriverSG::init() {
  if (!TmcStepDirDriver::init()) return false;

  #ifdef DRIVER_TMC_STEPPER_STALLGUARD

    #ifdef DRIVER_TMC_STEPPER_STALLGUARD_BASIC
      sgArmFps.value       = (float)SG_ARM_FPS;
      sgArmFaultMs.value   = (float)SG_FAULT_MS;
      sgSensitivity.value  = (float)SG_SENS;
    #endif

    sgLastFps = 0.0f;
    sgRampF   = 0.0f;
    sgTrainPrev = sgTrain.value;
    sgModelSavePending = false;
    sgModelSaveIntervalMs = (uint32_t)SG_MODEL_SAVE_MS;
    if (nv().device().endurance() == NvDevice::Endurance::Mid) sgModelSaveIntervalMs = (uint32_t)SG_MODEL_SAVE_MS_MID; else
    if (nv().device().endurance() == NvDevice::Endurance::High) sgModelSaveIntervalMs = (uint32_t)SG_MODEL_SAVE_MS_HIGH;
    sgModelLastSaveMs = millis();

    // Default model in RAM first
    sgModelInit(sgModel);

    // create the nvKey for this axis
    char keyStr[24];
    sprintf(keyStr, "STALLGUARD_AXIS%u", (unsigned)axisNumber);
    nvKey = nv().kv().computeKey(keyStr);

    if (sgEnabled.value == ON) {
      // Read existing model (if present)
      KvPartition::Status result = nv().kv().get(nvKey, sgModel);
      if (result == KvPartition::Status::Ok) {
        VF("MSG:"); V(axisPrefix); VLF("StallGuard model NV file read success");
      } else if (result == KvPartition::Status::NotFound) {
        VF("MSG:"); V(axisPrefix); VLF("StallGuard model NV file not found (using defaults)");
      } else {
        DF("WRN:"); D(axisPrefix); DLF("StallGuard model NV file read failed");
      }

      sgModelDebugDumpNv();

      // If loaded model is stale/incompatible, reset to defaults and store back
      if (!sgModelValid(sgModel)) {
        DF("WRN:"); D(axisPrefix); DLF("StallGuard model invalid/discarded");
        sgModelInit(sgModel);
        sgRuntimeFromModel();
        if (nv().kv().put(nvKey, sgModel) != KvPartition::Status::Ok) { DF("WRN:"); D(axisPrefix); DLF("StallGuard save failed"); }
      } else {
        sgRuntimeFromModel();
      }

    } else {
      // Delete existing model (if present)
      KvPartition::Status result = nv().kv().del(nvKey);
      if (result == KvPartition::Status::Ok) {
        VF("MSG:"); V(axisPrefix); VLF("StallGuard model NV file found and deleted");
      } else if (result != KvPartition::Status::NotFound) {
        DF("WRN:"); D(axisPrefix); DLF("StallGuard model NV file delete error");
      }
    }

    // bin cache invalid until first use
    sgBinValid  = false;
    sgBinArmFps = 0.0f;
    sgBinHi     = 0.0f;
    for (int i = 0; i < (int)(SG_BINS - 1); i++) sgBinEdge[i] = 0.0f;
    for (int i = 0; i < (int)SG_BINS; i++) sgBinAnchor[i] = 0.0f;
  #endif

  return true;
}

// check if sg axis parameter is valid
bool TmcStepDirDriverSG::parameterIsValid(AxisParameter* parameter, bool next) {
  if (!TmcStepDirDriver::parameterIsValid(parameter, next)) return false;

  if (parameter == &sgEnabled || parameter == &decaySlewing) {
    int sgEnabledValue, decaySlewingValue;
    if (next) {
      sgEnabledValue = lround(sgEnabled.valueNv);
      decaySlewingValue = lround(decaySlewing.valueNv);
    } else {
      sgEnabledValue = lround(sgEnabled.value);
      decaySlewingValue = lround(decaySlewing.value);
    }

    if (driverModel == TMC2209) {
      if (sgEnabledValue == ON && (decaySlewingValue != OFF && decaySlewingValue != STEALTHCHOP)) {
        DF("WRN:"); D(axisPrefix);
        DLF("stallGuard SG-Enable ON requires Decay-mode-Goto StealthChop");
        return false;
      }
    } else {
      if (sgEnabledValue == ON && (decaySlewingValue != OFF && decaySlewingValue != SPREADCYCLE)) {
        DF("WRN:"); D(axisPrefix);
        DLF("stallGuard SG-Enable ON requires Decay-mode-Goto SpreadCycle");
        return false;
      }
    }
  }

  return true;
}

bool TmcStepDirDriverSG::isStalled(float stepsPerSec) {

  #ifdef DRIVER_TMC_STEPPER_STALLGUARD

    if (sgEnabled.value == OFF) return false;
    if (!stallGuardModeActive()) return false;

    sgTrainPrev = sgTrain.value;

    const uint32_t nowMs = millis();
    if (sgLastMs != 0 && (uint32_t)(nowMs - sgLastMs) < (uint32_t)SG_EVAL_MIN_MS) return sgLatch;

    // Convert signed microsteps/s -> signed fullsteps/s
    const float fpsSigned = stepsPerSec / normalizedMicrosteps;
    const float spd = fabsf(fpsSigned);

    // Defer model writes while slewing; flush once speed drops below SG arm threshold.
    if (sgModelSavePending && spd < sgArmFps.value) {
      sgModelFromRuntime();

      if (nv().kv().put(nvKey, sgModel) == KvPartition::Status::Ok) {
        DF("MSG:"); D(axisPrefix); DLF("StallGuard model NV file saved");
      } else {
        DF("WRN:"); D(axisPrefix); DLF("StallGuard model NV file save failed");
      }

      sgModelSavePending = false;
      sgModelLastSaveMs = nowMs;
    }

    if (!(spd > 0.0f)) {
      stallDetectSoftReset();
      sgLastFps = fpsSigned;
      sgLastMs = nowMs;
      return false;
    }

    // Live tuning: if sensitivity changed, apply and reset model
    const uint8_t pctNow = (uint8_t)sgSensitivity.value;
    if (pctNow != sgAppliedPct) {
      // First application after boot uses sentinel 255 and should not wipe
      // the persisted learned model. Real runtime/user changes still hard-reset.
      const bool firstApply = (sgAppliedPct == 255);
      applySgSensitivityPercent(pctNow);
      sgAppliedPct = pctNow;
      if (firstApply) {
        stallDetectSoftReset();
      } else {
        stallDetectHardReset();
      }
      return false;
    }

    // Time delta (clamped) for integrators. millis() has 1ms resolution, so fast loops can see dtMs==0
    uint16_t dtMs = (sgLastMs == 0) ? 0 : (uint16_t)(nowMs - sgLastMs);
    sgLastMs = nowMs;
    if (dtMs > 50) dtMs = 50;

    // Signed accel estimate (fullsteps/sec^2)
    // IMPORTANT: if dtMs==0, do not advance sgLastFps, otherwise dv gets "eaten" on fast MCUs
    float a = 0.0f;       // signed accel
    float absA = 0.0f;    // magnitude

    if (dtMs > 0) {
      const float dv = fpsSigned - sgLastFps;
      sgLastFps = fpsSigned;
      a = dv * 1000.0f / (float)dtMs;
      absA = fabsf(a);
    }

    // Arm only above minimum speed (fullsteps/sec)
    if (spd < sgArmFps.value) {
      stallDetectSoftReset();
      sgLastFps = fpsSigned;
      return false;
    }

    const uint16_t armWarmMs = (uint16_t)SG_ARM_WARM_MS;

    const uint16_t sg = readSg10();
    // Treat sg==0 as "not usable for training" until warmed; 0xFFFF is invalid read.
    if ((sg == 0 && sgWarmMs < armWarmMs) || sg == 0xFFFF) {
      if (dtMs) { if (sgBadMs >= dtMs) sgBadMs -= dtMs; else sgBadMs = 0; }
      return sgLatch;
    }

    // Warm-up accumulator while armed and samples are valid.
    if (dtMs && sgWarmMs < armWarmMs) {
      const uint32_t w = (uint32_t)sgWarmMs + (uint32_t)dtMs;
      sgWarmMs = (w > armWarmMs) ? armWarmMs : (uint16_t)w;
    }

    // dir index: 0 = negative, 1 = positive.
    const uint8_t dir = (fpsSigned >= 0.0f) ? 1 : 0;

    const float armFps = (float)sgArmFps.value;

    // ---------------- ramp allowance / motion-state estimate ----------------
    // Keep sgRampF as a "motion state" estimate even if rampAllowPct is 0,
    // so steady-state gating works.
    if (dtMs > 0) {
      const float vref = (spd > armFps) ? spd : armFps; // floor avoids blow-up near zero
      constexpr float Tramp = 0.8f; // seconds; detector shaping constant
      float rampFactorInstant = (vref > 0.0f) ? (absA / vref) * Tramp : 0.0f;
      if (rampFactorInstant < 0.0f) rampFactorInstant = 0.0f;
      if (rampFactorInstant > 1.0f) rampFactorInstant = 1.0f;

      // Smooth ramp factor so sustained ramps are visible.
      constexpr float TAU_MS = 80.0f;
      const float alpha = (float)dtMs / (TAU_MS + (float)dtMs);
      sgRampF += alpha * (rampFactorInstant - sgRampF);
    }

    // Define "steady" from sgRampF (NOT from rampExtra).
    // Tune range ~0.08..0.20. Lower => stricter steady requirement.
    constexpr float SG_STEADY_RAMPF = 0.12f;
    const bool steadyNow = (sgRampF < SG_STEADY_RAMPF);

    uint16_t rampExtra = 0;
    const int rampPct = (int)sgRampAllowPct.value;

    // Optional ramp allowance (still available). This remains symmetric (absA-based).
    // -------------------------------------------------------------------------
    // Cross-fade bins for trip computation (avoid discontinuities at bin edges).
    uint8_t b0, b1;
    float bt;
    sgSpeedBlend(spd, armFps, b0, b1, bt);
    const uint8_t bl = (bt < 0.5f) ? b0 : b1; // nearest bin for learning

    // Seed baseline/deviation for the nearest learning bin.
    if (sgTrain.value == ON) {
      if (!sgModel.init[dir][bl]) {
        sgModel.base[dir][bl] = sgToFp(sg);
        sgModel.dev[dir][bl] = sgToFp(10);
        sgBaseF[dir][bl] = (float)sg;
        sgDevF[dir][bl] = 10.0f;
        sgModel.init[dir][bl] = true;
      }
    }

    // Blend baseline/deviation across adjacent speed bins for smoother thresholds.
    const float baseF = (1.0f - bt) * sgBaseF[dir][b0] + bt * sgBaseF[dir][b1];
    const float devF  = (1.0f - bt) * sgDevF [dir][b0] + bt * sgDevF [dir][b1];
    const uint16_t base = (uint16_t)lroundf(baseF);
    const uint16_t devE = (uint16_t)lroundf(devF);

    uint16_t margin = (uint16_t)(MARGIN_GENERAL + (uint16_t)(MARGIN_NOISE * devE));

    // Cap margin to 50% of baseline.
    uint16_t cap = (uint16_t)(base / 2);
    if (cap < MARGIN_GENERAL) cap = MARGIN_GENERAL;
    if (margin > cap) margin = cap;
    if (base > 0 && margin >= base) margin = base - 1;

    // ---------------- ramp allowance (optional) ----------------
    if (rampPct > 0 && base > 0) {
      const uint16_t rampAllowCounts = (uint16_t)((uint32_t)base * (uint32_t)rampPct / 20U);
      rampExtra = (uint16_t)lroundf((float)rampAllowCounts * sgRampF);
      if (rampExtra > cap) rampExtra = cap;
    }

    sgLastRampExtra = rampExtra;
    // ----------------------------------------------------------

    // "hard bad": sg==0 after warm-up.
    const bool hardBad = (sg == 0) && (sgWarmMs >= armWarmMs);

    // Base trip threshold.
    int trip = (int)base - (int)(margin + rampExtra);
    if (trip < 0) trip = 0;

    // Rate-of-change sensitivity boost ("floor"):
    {
      int floorPct = (int)lroundf(sgFloorPct.value);
      if (floorPct < 0) floorPct = 0;
      if (floorPct > 100) floorPct = 100;

      if (floorPct > 0 && base > 0 && sgWarmMs >= SG_ZERO_GRACE_MS) {
        const float vref = (spd > armFps) ? spd : armFps;
        const float r = (vref > 0.0f) ? (absA / vref) : 1.0f;

        #ifndef SG_FLOOR_R0
          #define SG_FLOOR_R0  0.08f
        #endif
        #ifndef SG_FLOOR_R1
          #define SG_FLOOR_R1  0.25f
        #endif

        float k;
        if (r <= SG_FLOOR_R0) k = 1.0f;
        else if (r >= SG_FLOOR_R1) k = 0.0f;
        else k = (SG_FLOOR_R1 - r) / (SG_FLOOR_R1 - SG_FLOOR_R0);

        #ifndef SG_FLOOR_COUNTS_PER_PCT
          #define SG_FLOOR_COUNTS_PER_PCT  2
        #endif
        int floorAdd = floorPct * SG_FLOOR_COUNTS_PER_PCT;
        floorAdd = (int)lroundf(k * (float)floorAdd);

        const int floorCap = (int)base/2;
        if (floorAdd > floorCap) floorAdd = floorCap;

        trip += floorAdd;
        if (trip > (int)base) trip = (int)base;
      }
    }

    const bool bad = hardBad || ((int)sg < trip);

    // debug snapshot
    sgLast       = sg;
    sgLastMargin = margin;
    sgBadLast    = bad;

    const bool steadyHealthy = steadyNow && !sgLatch && !bad;
    const bool adaptFast = (sgTrain.value == ON) && steadyHealthy;
    const bool adaptSlow = (sgTrain.value == OFF) && steadyHealthy;

    if (adaptFast || adaptSlow) {
      const float baseTc = adaptFast ? BASE_TC_FAST : BASE_TC_SLOW;
      const float devTc  = adaptFast ? DEV_TC_FAST  : DEV_TC_SLOW;

      const float sgF = (float)sg;

      // Update only the nearest bin to avoid cross-speed drift between adjacent bins.
      sgBaseF[dir][bl] += (sgF - sgBaseF[dir][bl]) / baseTc;

      // Downward-only deviation EMA update for the nearest learning bin.
      const float d = sgBaseF[dir][bl] - sgF;
      const float devDown = (d > 0.0f) ? d : 0.0f;

      sgDevF[dir][bl] += (devDown - sgDevF[dir][bl]) / devTc;

      // Queue periodic save; actual write is deferred until non-slew speeds.
      if ((nowMs - sgModelLastSaveMs) >= sgModelSaveIntervalMs) {
        sgModelSavePending = true;
      }

      // decay fault accumulator toward 0 when healthy
      if (dtMs) {
        const uint16_t d = dtMs;
        if (sgBadMs >= d) sgBadMs -= d; else sgBadMs = 0;
      }
    } else if (!bad) {
      // Healthy sample; decay fault accumulator.
      if (dtMs) {
        const uint16_t d = dtMs;
        if (sgBadMs >= d) sgBadMs -= d; else sgBadMs = 0;
      }
    } else {
      // Accumulate faults once warmed.
      if (sgWarmMs >= armWarmMs) {
        if (dtMs) {
          const uint32_t x = (uint32_t)sgBadMs + (uint32_t)dtMs;
          sgBadMs = (x > 65535U) ? 65535U : (uint16_t)x;
        }
      }
    }

    const int fpsInt = (int)lroundf(spd);
    UNUSED(fpsInt);

    const uint16_t armFaultMs = (uint16_t)sgArmFaultMs.value;
    if (sgTrain.value == OFF && sgBadMs >= armFaultMs && !sgLatch) {
      sgLatch = true;
      D("WRN:"); D(axisPrefix);
      D("SG stall detected fps="); D(fpsInt);
      D(", sg="); D((int)sg);
      D(", trip=");  DL((int)trip);
    }

    #if DEBUG != OFF && defined(DEBUG_AXIS) && DEBUG_AXIS != OFF
      if (axisNumber == DEBUG_AXIS) {
        if (spd >= sgArmFps.value) {
//          const int rampX  = (int)-rampExtra;
          const int fpsOut = (int)lroundf(spd);

          unsigned long tms = millis();
          static unsigned long next = 0;
          if ((long)(tms - next) > 0) {
            next = tms + 250; // 250ms log rate
            D("sg:"); D(sg);
            D(" sg-trip:"); D(trip);
//            D(" sg-rampExtra:"); D(rampX);
            D(" sg-binFps:"); D((int)lroundf(sgBinAnchor[bl]));
//            D(" sg-bl*10:"); D((int)bl * 10);
//            D(" sg-bt%:"); D((int)lroundf(bt * 100.0f));
            D(" sg-fps:"); D(fpsOut);
//            D(" sg-base-trip:"); D(base);
//            D(" sg-base-learn:"); D((int)lroundf(sgBaseF[dir][bl]));
            D(" sg-dev:"); D(devE);
            DL("");
          }
        }
      }
    #endif

  #else
    UNUSED(stepsPerSec);
  #endif

  return sgLatch;
}

void TmcStepDirDriverSG::stallDetectReset() {
  #ifdef DRIVER_TMC_STEPPER_STALLGUARD
    stallDetectSoftReset();
  #else
    sgLatch = false;
  #endif
}

#ifdef DRIVER_TMC_STEPPER_STALLGUARD

uint8_t TmcStepDirDriverSG::sgSpeedBin(float spd, float armFps) const {
  if (armFps < 1.0f) armFps = 1.0f;
  if (spd < armFps) return 0;

  float hi = (float)velocityMax / normalizedMicrosteps;
  if (hi < 1.0f) hi = 1.0f;

  if (spd > hi) spd = hi;

  constexpr float EpsThreshold = 0.01f;
  if (!sgBinValid || fabsf(sgBinHi - hi) > EpsThreshold) {
    sgBinValid  = true;
    sgBinArmFps = armFps;
    sgBinHi     = hi;

    // Fixed anchor points by max slew speed:
    // b0=low anchor, b1=0.25*Vmax, b2=0.375*Vmax, b3=0.5*Vmax, b4=0.75*Vmax, b5=Vmax
    sgBinAnchor[0] = (float)SG_BIN0_FPS;
    if (sgBinAnchor[0] < 0.0f) sgBinAnchor[0] = 0.0f;
    sgBinAnchor[1] = hi * 0.25f;
    sgBinAnchor[2] = hi * 0.33333f;
    sgBinAnchor[3] = hi * 0.5f;
    sgBinAnchor[4] = hi * 0.75f;
    sgBinAnchor[5] = hi;

    // Keep anchors strictly increasing to avoid divide-by-zero / ordering issues.
    constexpr float MinStep = 0.001f;
    for (uint8_t i = 1; i < SG_BINS; i++) {
      if (sgBinAnchor[i] <= sgBinAnchor[i - 1]) sgBinAnchor[i] = sgBinAnchor[i - 1] + MinStep;
    }
  }

  uint8_t best = 0;
  float bestErr = fabsf(spd - sgBinAnchor[0]);
  for (uint8_t b = 1; b < SG_BINS; b++) {
    const float e = fabsf(spd - sgBinAnchor[b]);
    if (e < bestErr) {
      bestErr = e;
      best = b;
    }
  }
  return best;
}

void TmcStepDirDriverSG::sgSpeedBlend(float spd, float armFps, uint8_t &b0, uint8_t &b1, float &t) const {

  if (armFps < 1.0f) armFps = 1.0f;

  if (spd <= armFps) {
    b0 = 0; b1 = 0; t = 0.0f;
    return;
  }

  (void)sgSpeedBin(spd, armFps);

  float sp = spd;
  if (sp > sgBinHi) sp = sgBinHi;

  if (sp <= sgBinAnchor[0]) {
    b0 = 0; b1 = 0; t = 0.0f;
    return;
  }

  for (uint8_t k = 0; k < (SG_BINS - 2); k++) {
    const float lo = sgBinAnchor[k];
    const float hi2 = sgBinAnchor[k + 1];

    if (sp < hi2) {
      b0 = k;
      b1 = (uint8_t)(k + 1);
      t = (hi2 > lo) ? (sp - lo) / (hi2 - lo) : 0.0f;
      if (t < 0.0f) t = 0.0f;
      if (t > 1.0f) t = 1.0f;
      return;
    }
  }

  b0 = (uint8_t)(SG_BINS - 2);
  b1 = (uint8_t)(SG_BINS - 1);
  const float lo = sgBinAnchor[SG_BINS - 2];
  const float hi2 = sgBinAnchor[SG_BINS - 1];
  t = (hi2 > lo) ? (sp - lo) / (hi2 - lo) : 1.0f;
  if (t < 0.0f) t = 0.0f;
  if (t > 1.0f) t = 1.0f;
}

uint16_t TmcStepDirDriverSG::readSg10() {
  const uint32_t now = millis();

  if (sgLastGoodMs != 0 && (uint32_t)(now - sgLastGoodMs) >= 500) {
    sgCached = 0xFFFF;
    sgLastGoodMs = 0;
    sgRetryAtMs = now + 2;
  }

  if ((int32_t)(now - sgRetryAtMs) < 0) return sgCached;

  const uint32_t sr = readSgRegister();

  if (sr != 0 && sr != 0xFFFFFFFFUL) {
    if (sgCached == 0xFFFF) {
      sgCached = (uint16_t)(sr & 0x03FF);
    } else {
      sgCached = (uint16_t)((sgCached * 9U + (uint16_t)(sr & 0x03FF) + 5U) / 10U);
    }
    sgLastGoodMs = now;
    sgRetryAtMs  = now + 20;
  } else {
    sgRetryAtMs  = now + 2;
  }

  return sgCached;
}

void TmcStepDirDriverSG::stallDetectSoftReset() {
  sgLatch = false;
  sgBadMs  = 0;
  sgWarmMs = 0;
  sgRampF = 0.0f;

  sgCached     = 0xFFFF;
  sgLastGoodMs = 0;
  sgRetryAtMs  = 0;

  sgLast          = 0xFFFF;
  sgLastMargin    = 0;
  sgLastRampExtra = 0;
  sgBadLast       = false;
}

void TmcStepDirDriverSG::stallDetectHardReset() {
  sgModelInit(sgModel);
  sgRuntimeFromModel();

  // Persist cleared model if possible so next boot starts clean too
  sgModelFromRuntime();
  if (nv().kv().put(nvKey, sgModel) == KvPartition::Status::Ok) {
    VF("MSG:"); V(axisPrefix); VLF("StallGuard model reset and file saved");
  } else {
    DF("WRN:"); D(axisPrefix); DLF("StallGuard model reset but file save failed");
  }

  stallDetectSoftReset();
  sgModelLastSaveMs = millis();

  sgBinValid  = false;
  sgBinArmFps = 0.0f;
  sgBinHi     = 0.0f;
  for (int i = 0; i < (int)(SG_BINS - 1); i++) sgBinEdge[i] = 0.0f;
  for (int i = 0; i < (int)SG_BINS; i++) sgBinAnchor[i] = 0.0f;
}

void TmcStepDirDriverSG::sgModelInit(SgModel& model) const {
  switch (axisNumber) {
    case 1: {
      SgModel seed = {1, 0, 0, 0, AXIS1_SG_MODEL_INIT, AXIS1_SG_MODEL_BASE, AXIS1_SG_MODEL_DEV};
      model = seed;
      break;
    }
    case 2: {
      SgModel seed = {1, 0, 0, 0, AXIS2_SG_MODEL_INIT, AXIS2_SG_MODEL_BASE, AXIS2_SG_MODEL_DEV};
      model = seed;
      break;
    }
    default: {
      SgModel seed = {1, 0, 0, 0, SG_MODEL_INIT, SG_MODEL_BASE, SG_MODEL_DEV};
      model = seed;
      break;
    }
  }

  // overwrite live metadata after applying compile-time seed arrays
  model.version = 1;
  model.normalizedMicrostepsSlewing = normalizedMicrostepsSlewing;
  model.mAGoto = currentSlewing.value;
  model.sgSens = sgSensitivity.value;
}

bool TmcStepDirDriverSG::sgModelValid(const SgModel& model) const {
  if (model.version != 1) return false;
  if (model.normalizedMicrostepsSlewing != normalizedMicrostepsSlewing) return false;
  if (lroundf(model.mAGoto) != lroundf(currentSlewing.value)) return false;
  if (lroundf(model.sgSens) != lroundf(sgSensitivity.value)) return false;
  return true;
}

void TmcStepDirDriverSG::sgRuntimeFromModel() {
  for (uint8_t d = 0; d < 2; d++) {
    for (uint8_t b = 0; b < SG_BINS; b++) {
      sgBaseF[d][b] = (float)sgModel.base[d][b] / (float)SG_FP_ONE;
      sgDevF[d][b]  = (float)sgModel.dev [d][b] / (float)SG_FP_ONE;
    }
  }
}

void TmcStepDirDriverSG::sgModelFromRuntime() {
  for (uint8_t d = 0; d < 2; d++) {
    for (uint8_t b = 0; b < SG_BINS; b++) {
      float base = sgBaseF[d][b];
      float dev  = sgDevF[d][b];
      if (base < 0.0f) base = 0.0f;
      if (dev  < 0.0f) dev  = 0.0f;

      const float baseFp = base * (float)SG_FP_ONE;
      const float devFp  = dev  * (float)SG_FP_ONE;

      sgModel.base[d][b] = (baseFp > 65535.0f) ? 65535u : (uint16_t)lroundf(baseFp);
      sgModel.dev [d][b] = (devFp  > 65535.0f) ? 65535u : (uint16_t)lroundf(devFp);
    }
  }
}

void TmcStepDirDriverSG::sgModelDebugDumpNv() const {
  #if DEBUG != OFF
    VF("MSG:"); V(axisPrefix); VF("Stallguard model metadata v=");
    V((int)sgModel.version);
    VF(" microstepsGoto=");
    V((int)sgModel.normalizedMicrostepsSlewing);
    VF(" mAGoto=");
    V((int)lroundf(sgModel.mAGoto));
    VF(" sgSens=");
    VL((int)lroundf(sgModel.sgSens));

    #ifdef SG_DEBUG_MODEL
      VLF("------------------------------------------------------------------");
      VF("#define AXIS"); V((int)axisNumber); VF("_SG_MODEL_INIT { { ");
      for (int d = 0; d < 2; d++) {
        for (int b = 0; b < SG_BINS; b++) {
          V(sgModel.init[d][b] ? "true" : "false");
          if (b < SG_BINS - 1) { VF(", "); }
        }
        VF(" }");
        if (d == 0) { VF(", { "); }
      }
      VLF(" }");

      VF("#define AXIS"); V((int)axisNumber); VF("_SG_MODEL_BASE { { ");
      for (int d = 0; d < 2; d++) {
        for (int b = 0; b < SG_BINS; b++) {
          V((int)sgModel.base[d][b]);
          if (b < SG_BINS - 1) { VF(", "); }
        }
        VF(" }");
        if (d == 0) { V(", { "); }
      }
      VLF(" }");

      VF("#define AXIS"); V((int)axisNumber); VF("_SG_MODEL_DEV { { ");
      for (int d = 0; d < 2; d++) {
        for (int b = 0; b < SG_BINS; b++) {
          V((int)sgModel.dev[d][b]);
          if (b < SG_BINS - 1) { VF(", "); }
        }
        VF(" }");
        if (d == 0) { VF(", { "); }
      }
      VLF(" }");
      VLF("------------------------------------------------------------------");
    #endif
  #endif
}

#endif

#endif
