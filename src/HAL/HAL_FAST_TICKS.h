// -------------------------------------------------------------------------------------------------
// HAL_FAST_TICKS.h
//
// Fast ticks: 32-bit wrapping timestamp intended for ISRs / high-rate timing.
//
// Interface:
//   HAL_FAST_TICKS_INIT()      call once at startup (safe no-op where not needed)
//   HAL_FAST_TICKS()           returns uint32_t ticks (wrapping OK)
//   HAL_TICKS_PER_SECOND()     returns uint32_t tick frequency in Hz (cached)
//   delayNanoseconds(ns)       short busy-wait delay (NOT intended for ISR use)
//   nanoseconds32()            convenience conversion (NOT intended for ISR use)
//
// Policy (as requested):
// - Teensy 3.x/4.x: use DWT->CYCCNT (enable once).
// - ESP32 Xtensa (ESP32/S2/S3): use CCOUNT.
// - ESP8266: use ESP.getCycleCount().
// - STM32: most use micros(), STM32H723/H743/H750 use DWT->CYCCNT.
// - ESP32 RISC-V (C3/C6/H2): use micros().
// - Everything else: use micros().
//
// Notes:
// - Tick frequency is cached in _halTicksPerSecond/_halTicksPerMicro (defined in HAL.cpp).
// -------------------------------------------------------------------------------------------------
#pragma once

#include <stdint.h>
#include <Arduino.h>

// Make header self-contained (HAL.h may not have been included yet)
#ifndef IRAM_ATTR
  #define IRAM_ATTR
#endif

// Must be defined once in HAL.cpp:
//   uint32_t _halTicksPerSecond = 1000000UL;
//   uint32_t _halTicksPerMicro  = 1;
extern uint32_t _halTicksPerSecond;
extern uint32_t _halTicksPerMicro;

// ----------------------------------------
// Platform identification helpers
// ----------------------------------------

// Teensy (3.x/4.x) detection
#if defined(TEENSYDUINO) || defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41) || \
    defined(ARDUINO_TEENSY_MICROMOD) || \
    defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) || \
    defined(__IMXRT1062__)
  #define HAL_IS_TEENSY 1
#else
  #define HAL_IS_TEENSY 0
#endif

// STM32 H7 detection (STM32duino)
#if defined(ARDUINO_ARCH_STM32) && (defined(STM32H723xx) || defined(STM32H743xx) || defined(STM32H750xx))
  #define HAL_IS_STM32_H7 1
#else
  #define HAL_IS_STM32_H7 0
#endif

// ESP32 RISC-V detection (C3/C6/H2)
#if (defined(ARDUINO_ARCH_ESP32) || defined(ESP_PLATFORM)) && (defined(__riscv) || defined(__riscv__))
  #define HAL_IS_ESP32_RISCV 1
#else
  #define HAL_IS_ESP32_RISCV 0
#endif

// ESP32 Xtensa detection (ESP32/S2/S3)
#if (defined(ARDUINO_ARCH_ESP32) || defined(ESP_PLATFORM)) && (defined(__XTENSA__) || defined(__xtensa__))
  #define HAL_IS_ESP32_XTENSA 1
#else
  #define HAL_IS_ESP32_XTENSA 0
#endif

// ----------------------------------------
// Platform-specific fast tick source
// ----------------------------------------

// ---------- ESP32 Xtensa: CCOUNT ----------
#if HAL_IS_ESP32_XTENSA

static inline uint32_t IRAM_ATTR HAL_FAST_TICKS() {
  uint32_t ccount;
  __asm__ __volatile__("rsr %0, ccount" : "=a"(ccount));
  return ccount;
}

static inline uint32_t HAL_TICKS_PER_SECOND() { return _halTicksPerSecond; }

static inline void HAL_FAST_TICKS_INIT() {
  #if defined(F_CPU)
    _halTicksPerSecond = (uint32_t)F_CPU;
  #else
    _halTicksPerSecond = (uint32_t)ESP.getCpuFreqMHz() * 1000000UL;
  #endif
  _halTicksPerMicro = _halTicksPerSecond / 1000000UL;
  if (_halTicksPerMicro == 0) _halTicksPerMicro = 1;
}

// ---------- ESP8266: cycle count ----------
#elif defined(ESP8266)

static inline uint32_t IRAM_ATTR HAL_FAST_TICKS() {
  return (uint32_t)ESP.getCycleCount();
}

static inline uint32_t HAL_TICKS_PER_SECOND() { return _halTicksPerSecond; }

static inline void HAL_FAST_TICKS_INIT() {
  #if defined(F_CPU)
    _halTicksPerSecond = (uint32_t)F_CPU; // typically 80 or 160 MHz
  #else
    _halTicksPerSecond = 80000000UL;
  #endif
  _halTicksPerMicro = _halTicksPerSecond / 1000000UL;
  if (_halTicksPerMicro == 0) _halTicksPerMicro = 1;
}

// ---------- Teensy 3.x/4.x: DWT->CYCCNT ----------
#elif HAL_IS_TEENSY

static inline void HAL_FAST_TICKS_INIT() {
  #if defined(DWT) && defined(CoreDebug) && defined(DWT_CTRL_CYCCNTENA_Msk)
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    if ((DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) == 0) {
      DWT->CYCCNT = 0;                    // optional but recommended
      DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
      __DSB(); __ISB();
    }
    #if defined(F_CPU)
      _halTicksPerSecond = (uint32_t)F_CPU;
    #else
      _halTicksPerSecond = 1000000UL;
    #endif
  #else
    // fallback tick source is micros()
    _halTicksPerSecond = 1000000UL;
  #endif

  _halTicksPerMicro = _halTicksPerSecond / 1000000UL;
  if (_halTicksPerMicro == 0) _halTicksPerMicro = 1;
}

static inline uint32_t HAL_TICKS_PER_SECOND() { return _halTicksPerSecond; }

static inline uint32_t IRAM_ATTR HAL_FAST_TICKS() {
  #if defined(DWT) && defined(DWT_CTRL_CYCCNTENA_Msk)
    return (uint32_t)DWT->CYCCNT;
  #else
    return (uint32_t)micros();
  #endif
}

// - STM32H7: use DWT->CYCCNT (enable once).
// - Other STM32: use micros().
#elif HAL_IS_STM32_H7

// Some STM32 cores expose these via CMSIS headers already included by Arduino.h.
// If not, you may need: #include "core_cm7.h"

static inline void HAL_FAST_TICKS_INIT() {
  // Ensure SystemCoreClock is up to date (harmless if already correct).
  #if defined(SystemCoreClockUpdate)
    SystemCoreClockUpdate();
  #endif

  // Enable DWT CYCCNT
  #if defined(DWT) && defined(CoreDebug) && defined(DWT_CTRL_CYCCNTENA_Msk) && defined(CoreDebug_DEMCR_TRCENA_Msk)
    // Enable tracing (required to access DWT on Cortex-M)
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    #if defined(DWT_LAR)
      // DWT->LAR = 0xC5ACCE55;  // unlock (if implemented)
    #endif

    // Enable the cycle counter
    if ((DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) == 0) {
      DWT->CYCCNT = 0;
      DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
      __DSB(); __ISB();
    }

    // Tick frequency is core clock
    #if defined(F_CPU)
      _halTicksPerSecond = (uint32_t)F_CPU;
    #elif defined(SystemCoreClock)
      _halTicksPerSecond = (uint32_t)SystemCoreClock;
    #else
      _halTicksPerSecond = 1000000UL; // last-resort fallback
    #endif
  #else
    // If DWT isn't available for some reason, fall back to micros().
    _halTicksPerSecond = 1000000UL;
  #endif

  _halTicksPerMicro = _halTicksPerSecond / 1000000UL;
  if (_halTicksPerMicro == 0) _halTicksPerMicro = 1;
}

static inline uint32_t HAL_TICKS_PER_SECOND() { return _halTicksPerSecond; }

static inline uint32_t IRAM_ATTR HAL_FAST_TICKS() {
  #if defined(DWT) && defined(CoreDebug) && defined(DWT_CTRL_CYCCNTENA_Msk) && defined(CoreDebug_DEMCR_TRCENA_Msk)
    return (uint32_t)DWT->CYCCNT;     // 32-bit wraps naturally
  #else
    return (uint32_t)micros();        // fallback
  #endif
}

#else

static inline void HAL_FAST_TICKS_INIT() {
  _halTicksPerSecond = 1000000UL;
  _halTicksPerMicro  = 1;
}

static inline uint32_t HAL_TICKS_PER_SECOND() { return _halTicksPerSecond; }

static inline uint32_t IRAM_ATTR HAL_FAST_TICKS() { return (uint32_t)micros(); }

#endif

// ----------------------------------------
// Helpers (non-ISR intent)
// ----------------------------------------

static inline uint32_t nanoseconds32() {
  // rolls over ~4.29s when returning uint32_t
  return (uint32_t)((uint64_t)HAL_FAST_TICKS() * 1000000000ULL / (uint64_t)HAL_TICKS_PER_SECOND());
}

static inline uint32_t halNsToTicks(uint32_t ns) {
  // ticks = ns * (ticks/us) / 1000
  return (uint32_t)(((uint64_t)ns * (uint64_t)_halTicksPerMicro + 999ULL) / 1000ULL);
}

#if !HAL_IS_TEENSY
  static inline void delayNanoseconds(uint32_t ns) {
    if (ns >= 20000UL) {
      delayMicroseconds((ns + 999UL) / 1000UL);
      return;
    }
    const uint32_t wait = halNsToTicks(ns);
    if (wait == 0) return;
    const uint32_t start = HAL_FAST_TICKS();
    while ((uint32_t)(HAL_FAST_TICKS() - start) < wait) { }
  }
#endif
