// general purpose encoder class
#pragma once

#include "../../Common.h"

#include <stdint.h>

// --------------------------------------------------------------------------------------------------------

#ifndef ENCODER_VELOCITY
  #define ENCODER_VELOCITY OFF
#endif

#if ENCODER_VELOCITY == ON

  // ENCODER_VEL_WINDOW_US:
  //   Window length for dc/dt estimate (microseconds). Smaller = more responsive/noisier
  //   larger = smoother/more lag. Typical: 5000–20000 us for 100–1000 Hz control loops
  #ifndef ENCODER_VEL_WINDOW_US
    #define ENCODER_VEL_WINDOW_US 10000UL  // default 10ms (10000UL)
  #endif

  // ENCODER_VEL_STOP_US:
  //   If no new edges/deltas occur for this long, velocity is forced to 0
  //   If velocity drops to 0 at very slow tracking/creep speeds, increase this
  //   Typical: 300000 us (0.3s) up to 2000000 us (2s) for very low counts/sec systems
  #ifndef ENCODER_VEL_STOP_US
    #define ENCODER_VEL_STOP_US 300000UL  // 0.3s (300000UL)
  #endif

  // ENCODER_VEL_BLEND_COUNTS:
  //   Blend point between period (t/d) and windowed (d/t) estimates
  //   Larger values favor period estimate longer (better at low counts/sec)
  //   Typical: 4–12 counts
  #ifndef ENCODER_VEL_BLEND_COUNTS
    #define ENCODER_VEL_BLEND_COUNTS 4.0F // 4 counts (4.0F)
  #endif

  // --------------------------------------------------------------------------
  // An example for very low count systems (copy/paste into your Config.h to override):
  //
  //   #define ENCODER_VEL_WINDOW_US      20000UL   // 20ms smoother
  //   #define ENCODER_VEL_STOP_US      1000000UL   // 1.0s avoid false “stops”
  //   #define ENCODER_VEL_BLEND_COUNTS     8.0F    // rely on period estimate longer
  // --------------------------------------------------------------------------

#endif

// ENCODER_ERROR_COUNT_THRESHOLD:
// allow up to 20 errors per minute before throwing a fault
#ifndef ENCODER_ERROR_COUNT_THRESHOLD
  #define ENCODER_ERROR_COUNT_THRESHOLD 20 // 20 errors (20)
#endif

// OFF is disabled, ON disregards unexpected quadrature encoder signals, or 
// a value > 0 (nanoseconds) disregards repeat signal events for that timer period
#ifndef ENCODER_FILTER
  #define ENCODER_FILTER OFF
#endif

// these should allow time for an encoder signal to stabalize
#if ENCODER_FILTER > 0
  // once a signal state changes don't allow the ISR to run again for ENCODER_FILTER nanoseconds
  #define ENCODER_FILTER_UNTIL() do {                                   \
    const uint32_t _msNow = (uint32_t)msNow;                            \
    if ((int32_t)(_msNow - ticksInvalidMillis) >= 0) { ticksNext = 0; } \
    const uint32_t _ticksNow = HAL_FAST_TICKS();                        \
    if (ticksNext && (int32_t)(_ticksNow - ticksNext) < 0) return;      \
    ticksNext = _ticksNow + ticksToWait;                                \
    ticksInvalidMillis = _msNow + 2000U;                                \
  } while (0)
#endif

// signal mode for pulse and cw/ccw encoders, default RISING or CHANGE or FALLING
// quadrature encoders ignore this and use CHANGE
#ifndef ENCODER_SIGNAL_MODE
  #define ENCODER_SIGNAL_MODE RISING
#endif

// --------------------------------------------------------------------------------------------------------

#ifndef AXIS1_ENCODER
  #define AXIS1_ENCODER OFF
#endif
#ifndef AXIS2_ENCODER
  #define AXIS2_ENCODER OFF
#endif
#ifndef AXIS3_ENCODER
  #define AXIS3_ENCODER OFF
#endif
#ifndef AXIS4_ENCODER
  #define AXIS4_ENCODER OFF
#endif
#ifndef AXIS5_ENCODER
  #define AXIS5_ENCODER OFF
#endif
#ifndef AXIS6_ENCODER
  #define AXIS6_ENCODER OFF
#endif
#ifndef AXIS7_ENCODER
  #define AXIS7_ENCODER OFF
#endif
#ifndef AXIS8_ENCODER
  #define AXIS8_ENCODER OFF
#endif
#ifndef AXIS9_ENCODER
  #define AXIS9_ENCODER OFF
#endif

#if AXIS1_ENCODER == AS37_H39B_B || AXIS2_ENCODER == AS37_H39B_B
  #define HAS_AS37_H39B_B
#endif

#if AXIS1_ENCODER == LIKA_ASC85 || AXIS2_ENCODER == LIKA_ASC85
  #define HAS_LIKA_ASC85
#endif

#if AXIS1_ENCODER == JTW_24BIT || AXIS2_ENCODER == JTW_24BIT
  #define HAS_JTW_24BIT
#endif

#if AXIS1_ENCODER == JTW_26BIT || AXIS2_ENCODER == JTW_26BIT
  #define HAS_JTW_26BIT
#endif

#if defined(HAS_AS37_H39B_B) || defined(HAS_LIKA_ASC85) || defined(HAS_JTW_24BIT) || defined(HAS_JTW_26BIT) 
  #define HAS_BISS_C
#endif

#if AXIS1_ENCODER != OFF || AXIS2_ENCODER != OFF || AXIS3_ENCODER != OFF || \
    AXIS4_ENCODER != OFF || AXIS5_ENCODER != OFF || AXIS6_ENCODER != OFF || \
    AXIS7_ENCODER != OFF || AXIS8_ENCODER != OFF || AXIS9_ENCODER != OFF

class Encoder {
  public:
    // get device ready for use
    virtual bool init();

    // set encoder origin
    virtual void setOrigin(int32_t counts) { origin = counts; }

    // get current position
    virtual int32_t read() { return 0; }

    // set current position
    virtual void write(int32_t position) { UNUSED(position); }

    // set the virtual encoder velocity in counts per second
    virtual void setVelocity(float countsPerSec) { UNUSED(countsPerSec); }

    // fast, control-grade velocity
    virtual float readVelocityCps();

    // set the virtual encoder direction (-1 is reverse, 1 is forward)
    virtual void setDirection(volatile int8_t *direction) { UNUSED(direction); }

    // check error state
    virtual bool errorThresholdExceeded() { return errorState; }

    // update encoder status
    void poll();

    // total number of errors
    uint32_t totalErrorCount = 0;

    // total number of warnings
    uint32_t totalWarningCount = 0;

    // true if encoder count is ready
    bool ready = false;

    // true if this is a virtual encoder
    bool isVirtual = false;

    // raw count as last read (includes origin for absolute encoders)
    volatile int32_t count = 0;

    // raw index as last set
    volatile int32_t index = 0;

    // raw origin as last set (for absolute encoders)
    uint32_t origin = 0;

    // for encoders with slow readout, available before init()/ready()/hardware detection
    virtual bool supportsTimeAlignedMotorSteps() const { return false; }
    inline bool hasMotorStepsAtLastRead() const { return hasMotorStepsAtLastReadValue; }
    inline long motorStepsAtLastRead() const { return motorStepsAtLastReadValue; }
    inline void setMotorStepsPtr(volatile long* p) { motorStepsPtr = p; }

  protected:
    #if ENCODER_VELOCITY == ON
      inline void IRAM_ATTR velNoteEdge(int8_t dir) {
        const uint32_t now = HAL_FAST_TICKS();
        const uint32_t prev = velLastEdgeTicks;
        velLastEdgeTicks = now;
        velLastDir = dir;
        if (prev != 0) velLastPeriodTicks = now - prev;
      }

      inline void velNoteDelta(int32_t dc, uint32_t dtUs) {
        if (dc == 0 || dtUs == 0) return;

        const uint32_t nowTicks = HAL_FAST_TICKS();
        const uint32_t tps = velTicksPerSec;
        const uint32_t absdc = (dc > 0) ? (uint32_t)dc : (uint32_t)(-dc);

        uint64_t dtTicks = (uint64_t)dtUs * (uint64_t)tps;
        dtTicks = (dtTicks + 500000ULL) / 1000000ULL;

        uint32_t perTicks = (uint32_t)((dtTicks + (absdc/2)) / absdc);
        if (perTicks == 0) perTicks = 1;

        noInterrupts();
        velLastEdgeTicks   = nowTicks;
        velLastPeriodTicks = perTicks;
        velLastDir         = (dc > 0) ? 1 : -1;
        interrupts();
      }

      // Feed velocity estimator from sampled count reads (for devices without edge ISR info)
      // Note: pass raw encoder count (pre-index) to avoid velocity spikes on write()/index changes
      inline void velNoteSampledCount(int32_t rawCount) {
        const uint32_t nowUs = micros();

        if (!velSampleInit) {
          velSampleInit = true;
          velSampleLastCount = rawCount;
          velSampleLastUs  = nowUs;
          return;
        }

        const uint32_t dtUs = nowUs - velSampleLastUs;
        const int32_t  dc   = rawCount - velSampleLastCount;

        velSampleLastCount = rawCount;
        velSampleLastUs  = nowUs;

        if (dc != 0 && dtUs != 0) velNoteDelta(dc, dtUs);
      }

      bool     velHasEstimate     = false;
      bool     velSampleInit      = false;
      int32_t  velSampleLastCount = 0;
      uint32_t velSampleLastUs    = 0;
      uint32_t velStopTicks       = 0;

      volatile uint32_t velLastEdgeTicks   = 0;
      volatile uint32_t velLastPeriodTicks = 0;
      volatile int8_t   velLastDir         = 0;
      uint32_t          velTicksPerSec     = 1000000UL;

      int32_t  velWinLastCount = 0;
      uint32_t velWinLastUs    = 0;
      float    velBlendCps     = 0.0F;
    #endif

    // axis number from 1 to 9
    int16_t axis = 0;

    // accumulator for warning detection
    volatile uint32_t warn = 0;

    // accumulator for error detection
    volatile uint32_t error = 0;

    // number of errors over the last minute
    uint16_t errorCount[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // keep track of the error state
    bool errorState = false;
    bool lastErrorState = false;
    uint16_t tick = UINT16_MAX;
    uint16_t errorCountIndex = UINT16_MAX;

    #if ENCODER_FILTER > 0
      // approximate time keeping for filtering
      volatile uint32_t msNow = 0;
      volatile uint32_t ticksNext = 0;
      volatile uint32_t ticksToWait = 0;
      volatile uint32_t ticksInvalidMillis = 0;
    #endif

    // for encoders with slow readout
    volatile bool hasMotorStepsAtLastReadValue = false;
    volatile long motorStepsAtLastReadValue = 0;
    volatile long * volatile motorStepsPtr = nullptr;
};

#endif