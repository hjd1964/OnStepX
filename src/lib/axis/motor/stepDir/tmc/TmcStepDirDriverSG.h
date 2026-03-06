// -----------------------------------------------------------------------------------
// axis step/dir motor driver + StallGuard helper
//
// The “Arduino IDE charts” in your debug output are these runtime signals:
//
//   sg            = raw StallGuard result (0..1023) from the driver status.
//                   Lower sg => higher load / closer to stall. 0 is often “not usable”
//                   at low speed or before SG has warmed up.
//
//   trip          = computed threshold. If sg < trip => “bad sample” (potential stall).
//                   trip starts near the learned baseline (base) and is adjusted by
//                   noise margin, ramp allowance, and optional steady-state “floor”.
//
//   rampExtra     = extra margin during ramps (accel/decel) so normal
//                   transient load spikes don’t false-trip.
//                   Logged/charted as a negative number (rampX = -rampExtra)
//                   to visualize it as “headroom reduction”.
//
//   sg-bin*10     = which speed bin you’re in (lower bin index * 10 for plotting).
//                   Bins exist so base/dev can differ by speed (SG is speed dependent).
//
//   fpsOut/10     = speed in fullsteps/sec divided by 10 (cheap scaling for plotting).
//
//   base          = learned SG baseline for this direction + speed bin (counts).
//                   This is the “healthy” SG you expect at this speed when not ramping.
//
//   devE          = learned downward deviation (noise / dips) for this dir+bin (counts).
//                   Used to inflate margin: margin = M0 + M1*devE.
//
// The tunable parameters exposed in the UI (AxisParameter) are:
//
//   sgEnabled (OFF/ON)
//     - Master enable for StallGuard-based stall detection on this axis.
//     - Hard constraint:
//         * StallGuard2 devices (tmc2130/2160/5160/5161) require “decay slewing SpreadCycle”
//         * StallGuard4 devices (tmc2209) require “decay slewing StealthChop”
//
//   sgTrain (OFF/ON)
//     - Enables learning of baseline (sgB) and deviation (sgD) while moving.
//     - Training only happens when:
//         * not latched,
//         * sample is healthy (not “bad”),
//         * and motion is “steady” (low ramp factor).
//
//   sgSensitivity (0..100 percent)
//     - User-facing knob mapped by applySgSensitivityPercent() into driver settings
//       (typically SGTHRS / coolStep related thresholding depending on chip).
//     - Changing it forces a hard reset of learned model so new sensitivity starts clean.
//
//   sgArmFps (fullsteps/sec)
//     - Minimum speed where SG is considered valid and the detector “arms”.
//     - Below this: detector disarms, clears warm/fault accumulators, returns “no stall”.
//     - This avoids the common low-speed regime where StallGuard is noisy/useless.
//
//   sgArmFaultMs (milliseconds)
//     - Time-integrator for “bad samples” before declaring a stall.
//     - The detector accumulates sgBadMs when sg is below trip (or sg==0 after warmup),
//       and decays sgBadMs back toward 0 on healthy samples.
//     - When sgBadMs >= sgArmFaultMs => sgLatch=true (stall latched).
//
//   sgRampAllowPct (percent-ish)
//     - Optional ramp allowance. Adds extra margin during ramps proportional to:
//         base * (rampAllowPct/20) * sgRampF
//     - sgRampF is a normalized ramp factor derived from |accel|/speed, smoothed.
//     - Purpose: reduce false stalls during accel/decel and during direction changes.
//
//   sgFloorPct (0..100 percent)
//     - Optional steady-state sensitivity boost.
//     - When enabled, it ADDS counts to trip (making it easier to trip) *only* when
//       motion is steady, and fades out as ramping increases.
//     - Interpreted as fixed counts:
//         floorAdd = sgFloorPct * SG_FLOOR_COUNTS_PER_PCT
//       then blended by “k” based on r = |dv/dt| / vref using thresholds:
//         SG_FLOOR_R0 (fully steady) .. SG_FLOOR_R1 (fully ramping)
//
// Build-time knobs that shape the plotted behavior (these affect how the above looks):
//
//   SG_BINS
//     - Number of speed bins used for learning base/dev.
//     - More bins = more adaptation by speed, but slower learning per bin.
//
//   SG_ARM_WARM_MS
//     - Warm-up time after arming before sg==0 is treated as a “hard bad”.
//     - During warm-up, sg==0 is treated as “not usable for training” rather than a fault.
//
//   SG_ZERO_GRACE_MS
//     - Additional grace period before floor logic is allowed to act (prevents early
//       over-sensitivity while SG is still stabilizing).
//
//   SG_FLOOR_R0 / SG_FLOOR_R1
//     - Define what “steady” vs “ramping” means for the floor fade-out.
//
//   SG_FLOOR_COUNTS_PER_PCT
//     - Converts floorPct into raw SG counts (default 2 counts per %).
//
// Notes on the internal state variables you see affecting plots:
//
//   sgRampF
//     - A smoothed [0..1] ramp factor. 0 ~= steady, 1 ~= strong ramping.
//     - Used for “steadyNow” gating (training only when steady) and for rampExtra.
//
//   sgModel.base[dir][bin], sgModel.dev[dir][bin]
//     - Learned baseline and learned deviation per direction and speed bin.
//     - Stored as U10.6 fixed-point SG counts.
//     - Updated with EMA time constants BASE_TC and DEV_TC when training.
//
//   sgBadMs
//     - Fault integrator (ms). Accumulates “bad” time after warm-up,
//       decays toward 0 on healthy samples.
//     - This makes stall detection robust against single-sample noise.
//
#pragma once

#include "TmcStepDirDriver.h"

#if defined(STEP_DIR_MOTOR_PRESENT)

// Warmup time (ms) after arming before SG==0 is treated as "hard bad"
// and before faults are accumulated.
#ifndef SG_ARM_WARM_MS
  #define SG_ARM_WARM_MS 200
#endif

// Minimum period (ms) between StallGuard evaluations in isStalled().
// This normalizes model learning and fault integration against loop-rate variance.
#ifndef SG_EVAL_MIN_MS
  #define SG_EVAL_MIN_MS 20
#endif

#ifndef SG_MODEL_SAVE_MS
  #define SG_MODEL_SAVE_MS 300000
#endif
#ifndef SG_MODEL_SAVE_MS_MID
  #define SG_MODEL_SAVE_MS_MID 150000
#endif
#ifndef SG_MODEL_SAVE_MS_HIGH
  #define SG_MODEL_SAVE_MS_HIGH 60000
#endif

// Default init values

// Generic model parameters
#ifndef SG_ENABLED
  #define SG_ENABLED OFF
#endif
#ifndef SG_TRAIN
  #define SG_TRAIN ON
#endif
#ifndef SG_SENS
  #define SG_SENS 50
#endif
#ifndef SG_FLOOR
  #define SG_FLOOR 0
#endif
#ifndef SG_RAMP_ALLOW
  #define SG_RAMP_ALLOW 0
#endif
#ifndef SG_ARM_FPS
  #define SG_ARM_FPS 150
#endif
#ifndef SG_FAULT_MS
  #define SG_FAULT_MS 200
#endif

// Optional low-speed anchor for SG bin0 in fullsteps/sec.
// Keep at 0 for a true low anchor, or raise to manually smooth low-speed behavior.
#ifndef SG_BIN0_FPS
  #define SG_BIN0_FPS 0
#endif

// Generic fallback StallGuard seed model
#ifndef SG_MODEL_INIT
  #define SG_MODEL_INIT { { false, false, false, false, false, false }, { false, false, false, false, false, false } }
#endif
#ifndef SG_MODEL_BASE
  #define SG_MODEL_BASE { { 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0 } }
#endif
#ifndef SG_MODEL_DEV
  #define SG_MODEL_DEV  { { 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0 } }
#endif

// Axis 1 override defaults to generic model if not supplied
#ifndef AXIS1_SG_MODEL_INIT
  #define AXIS1_SG_MODEL_INIT SG_MODEL_INIT
#endif
#ifndef AXIS1_SG_MODEL_BASE
  #define AXIS1_SG_MODEL_BASE SG_MODEL_BASE
#endif
#ifndef AXIS1_SG_MODEL_DEV
  #define AXIS1_SG_MODEL_DEV SG_MODEL_DEV
#endif

// Axis 2 override defaults to generic model if not supplied
#ifndef AXIS2_SG_MODEL_INIT
  #define AXIS2_SG_MODEL_INIT SG_MODEL_INIT
#endif
#ifndef AXIS2_SG_MODEL_BASE
  #define AXIS2_SG_MODEL_BASE SG_MODEL_BASE
#endif
#ifndef AXIS2_SG_MODEL_DEV
  #define AXIS2_SG_MODEL_DEV SG_MODEL_DEV
#endif

// Number of speed bins used for learning base/dev. (fixed at 6)
#define SG_BINS 6

typedef struct SgModel {
  uint8_t  version;
  int16_t  normalizedMicrostepsSlewing;
  float    mAGoto;
  float    sgSens;

  bool     init[2][SG_BINS];
  uint16_t base[2][SG_BINS]; // U10.6
  uint16_t dev[2][SG_BINS];  // U10.6
} SgModel;

class TmcStepDirDriverSG : public TmcStepDirDriver {
  public:
    TmcStepDirDriverSG(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings,
                       int16_t currentHold, int16_t currentRun, int16_t currentSlewing, int8_t intpol);

    // set up driver
    bool init();

    uint8_t getParameterCount() { return numParameters; }

    AxisParameter* getParameter(uint8_t number) {
      if (number == 0 || number > numParameters) return &invalid;
      return parameter[number];
    }

    // check if axis parameter is valid
    bool parameterIsValid(AxisParameter* parameter, bool next = false);

    // indicate that this driver can detect stalls
    bool hasStallDetect() const { return true; }

    // Returns true if the motor is overloaded/stalled
    // NOTE: stepsPerSec is signed microsteps/sec; converted internally to signed fullsteps/sec
    bool isStalled(float stepsPerSec);

    // reset any internal stall-detect state (baseline, latch, etc.)
    void stallDetectReset() override;

  protected:

    // read the 32bit status register
    virtual uint32_t readSgRegister() { return 0xFFFFFFFFUL; }

    // should return true when in a mode where SG is meaningful
    virtual bool stallGuardModeActive() const { return true; }

    // apply normalized sensitivity percent (0..100) to the concrete chip register
    virtual void applySgSensitivityPercent(uint8_t pct) = 0;

    #ifdef DRIVER_TMC_STEPPER_STALLGUARD
      // read SG result (0..1023), or 0xFFFF on invalid read
      uint16_t readSg10();

      // speed-bin selection using [armFps .. velocityMax] if available
      uint8_t sgSpeedBin(float spd, float armFps) const;

      // cross-fade between adjacent bins to avoid abrupt trip jumps at bin edges.
      void sgSpeedBlend(float spd, float armFps, uint8_t &b0, uint8_t &b1, float &t) const;

      // reset
      void stallDetectSoftReset();
      void stallDetectHardReset();

      // U10.6 fixed-point helpers for learned model storage
      static constexpr uint8_t  SG_FP_SHIFT = 6;
      static constexpr uint16_t SG_FP_ONE   = (uint16_t)(1u << SG_FP_SHIFT);

      static inline uint16_t sgToFp(uint16_t x) {
        return (uint16_t)(x << SG_FP_SHIFT);
      }

      static inline uint16_t sgFpToInt(uint16_t xfp) {
        return (uint16_t)((xfp + (SG_FP_ONE >> 1)) >> SG_FP_SHIFT);
      }

      void sgModelInit(SgModel& model) const;
      bool sgModelValid(const SgModel& model) const;
      void sgModelDebugDumpNv() const;
      void sgRuntimeFromModel();
      void sgModelFromRuntime();

      SgModel sgModel = {};
      float   sgBaseF[2][SG_BINS] = {{0}};
      float   sgDevF[2][SG_BINS]  = {{0}};

      uint16_t sgBadMs  = 0;        // persistence accumulator (milliseconds)
      uint16_t sgWarmMs = 0;        // warm-up accumulator (milliseconds)
      uint32_t sgLastMs = 0;        // last millis() sample time
      uint8_t  sgAppliedPct = 255;  // last applied sensitivity percent

      // accel estimate (signed fullsteps/sec sample)
      float    sgLastFps = 0.0f;

      // smoothed ramp factor (0..1)
      float    sgRampF = 0.0f;

      // ---- speed-bin edge cache (avoid per-call powf) ----
      mutable bool  sgBinValid = false;
      mutable float sgBinArmFps = 0.0f;
      mutable float sgBinHi     = 0.0f;
      mutable float sgBinAnchor[SG_BINS] = {0.0f};
      mutable float sgBinEdge[SG_BINS - 1] = {0.0f};
      // ----------------------------------------------------

      // stallguard sample caching
      uint32_t sgLastGoodMs = 0;
      uint32_t sgRetryAtMs  = 0;
      uint16_t sgCached     = 0xFFFF;

      // debug logging
      uint16_t sgLast = 0xFFFF;
      uint16_t sgLastMargin = 0;
      uint16_t sgLastRampExtra = 0;
      bool     sgBadLast = false;
      bool     sgModelSavePending = false;
      uint32_t sgModelSaveIntervalMs = (uint32_t)SG_MODEL_SAVE_MS;
      uint32_t sgModelLastSaveMs = 0;

      // Used to avoid treating an immediate transient SG==0 as a fault right at arming.
      // (prevents hard-bad from triggering before any SG has had a chance to settle)
      static constexpr uint16_t SG_ZERO_GRACE_MS = 50;
    #endif

    // debug logging
    int16_t sgAppliedReg = 0;

    // indicates stalled state
    bool sgLatch  = false;

    // keep track of training disable events
    float sgTrainPrev = OFF;

    // key for NV model storage  
    uint32_t nvKey = 0;

    // ---------------------- PARAMETERS ----------------------
    AxisParameter sgEnabled      = {NAN, NAN, NAN, -2, -1, AXP_BOOLEAN,           "SG Enable"};
    AxisParameter sgTrain        = {NAN, NAN, NAN, -2, -1, AXP_BOOLEAN_IMMEDIATE, "SG Train"};

    // Floor raises the trip threshold in steady-state only:
    // higher => more aggressive, trips sooner (applied only when sgRampF is low).
    AxisParameter sgFloorPct     = {NAN, NAN, NAN, 0, 100, AXP_INTEGER_IMMEDIATE, "SG Floor, %"};
    AxisParameter sgRampAllowPct = {NAN, NAN, NAN, 0, 100, AXP_INTEGER_IMMEDIATE, "SG Ramp Allow, %"};

    AxisParameter sgSensitivity  = {NAN, NAN, NAN, 0, 100, AXP_INTEGER_IMMEDIATE, "SG Sens, %"};
    AxisParameter sgArmFps       = {NAN, NAN, NAN, 50, 2000, AXP_INTEGER_IMMEDIATE, "SG Arm, FPS"};
    AxisParameter sgArmFaultMs   = {NAN, NAN, NAN, 50, 500, AXP_INTEGER_IMMEDIATE, "SG Fault, ms"};


    #ifdef DRIVER_TMC_STEPPER_STALLGUARD
      #ifdef DRIVER_TMC_STEPPER_STALLGUARD_BASIC
        const int numParameters = 11;
      #else
        const int numParameters = 14;
      #endif
    #else
      const int numParameters = 7;
    #endif

    AxisParameter* parameter[15] = {
      &invalid, &microsteps, &microstepsSlewing,
      &currentHold, &currentRun, &currentSlewing,
      &decay, &decaySlewing,
//      &intpol,
      &sgEnabled, &sgTrain,
      &sgFloorPct, &sgRampAllowPct,
      
      &sgSensitivity, &sgArmFps, &sgArmFaultMs
    };
};

#endif
