#pragma once

#include <Arduino.h>
#include "../../ServoDriver.h"

// Caps for max resolution bits and (optional) max range
#ifndef SERVO_MAX_RES_BITS
  #define SERVO_MAX_RES_BITS 16
#endif
#ifndef SERVO_MAX_RANGE
  #define SERVO_MAX_RANGE ((1UL << SERVO_MAX_RES_BITS) - 1UL) // 65535
#endif

// Defaults to 255 for both ranges if not provided
#ifndef SERVO_ANALOG_WRITE_RANGE
  #define SERVO_ANALOG_WRITE_RANGE 255UL
#endif
#ifndef SERVO_ANALOG_WRITE_RANGE_OVERRIDE
  #define SERVO_ANALOG_WRITE_RANGE_OVERRIDE 255UL
#endif

// Map an integer RANGE to resolution bits (capped at 16)
#define SERVO_BITS_FROM_RANGE(R) \
  ( ((R) <=      1UL) ?  1 : \
    ((R) <=      3UL) ?  2 : \
    ((R) <=      7UL) ?  3 : \
    ((R) <=     15UL) ?  4 : \
    ((R) <=     31UL) ?  5 : \
    ((R) <=     63UL) ?  6 : \
    ((R) <=    127UL) ?  7 : \
    ((R) <=    255UL) ?  8 : \
    ((R) <=    511UL) ?  9 : \
    ((R) <=   1023UL) ? 10 : \
    ((R) <=   2047UL) ? 11 : \
    ((R) <=   4095UL) ? 12 : \
    ((R) <=   8191UL) ? 13 : \
    ((R) <=  16383UL) ? 14 : \
    ((R) <=  32767UL) ? 15 : \
                         16 )

// Always derive BASE resolution from BASE range
#ifdef SERVO_ANALOG_WRITE_RESOLUTION
  #undef SERVO_ANALOG_WRITE_RESOLUTION
#endif

#define SERVO_ANALOG_WRITE_RESOLUTION (SERVO_BITS_FROM_RANGE(SERVO_ANALOG_WRITE_RANGE))

// Always derive OVERRIDE resolution from OVERRIDE range
#ifdef SERVO_ANALOG_WRITE_RESOLUTION_OVERRIDE
  #undef SERVO_ANALOG_WRITE_RESOLUTION_OVERRIDE
#endif

#define SERVO_ANALOG_WRITE_RESOLUTION_OVERRIDE (SERVO_BITS_FROM_RANGE(SERVO_ANALOG_WRITE_RANGE_OVERRIDE))

// Stringify for readable assert messages
#define _SERVO_STR(x) #x
#define SERVO_STR(x) _SERVO_STR(x)

// Guardrails check that everything is consistent. Else motors can oscillate!
static_assert(
  SERVO_ANALOG_WRITE_RESOLUTION >= 1 &&
  SERVO_ANALOG_WRITE_RESOLUTION <= SERVO_MAX_RES_BITS,
  "SERVO_ANALOG_WRITE_RESOLUTION must be 1.." SERVO_STR(SERVO_MAX_RES_BITS) " bits."
);
static_assert(
  SERVO_ANALOG_WRITE_RESOLUTION_OVERRIDE >= 1 &&
  SERVO_ANALOG_WRITE_RESOLUTION_OVERRIDE <= SERVO_MAX_RES_BITS,
  "SERVO_ANALOG_WRITE_RESOLUTION_OVERRIDE must be 1.." SERVO_STR(SERVO_MAX_RES_BITS) " bits."
);

// Assert pairs are consistent
#if defined(SERVO_ANALOG_WRITE_RANGE) && defined(SERVO_ANALOG_WRITE_RESOLUTION)
static_assert(
  SERVO_ANALOG_WRITE_RANGE == ((1UL << SERVO_ANALOG_WRITE_RESOLUTION) - 1UL),
  "SERVO_ANALOG_WRITE_RANGE must equal (2^SERVO_ANALOG_WRITE_RESOLUTION)-1 (e.g., 255 ↔ 8 bits)."
);
#endif

#if defined(SERVO_ANALOG_WRITE_RANGE_OVERRIDE) && defined(SERVO_ANALOG_WRITE_RESOLUTION_OVERRIDE)
static_assert(
  SERVO_ANALOG_WRITE_RANGE_OVERRIDE == ((1UL << SERVO_ANALOG_WRITE_RESOLUTION_OVERRIDE) - 1UL),
  "SERVO_ANALOG_WRITE_RANGE_OVERRIDE must equal (2^SERVO_ANALOG_WRITE_RESOLUTION_OVERRIDE)-1."
);
#endif

// One-call PWM resolution config (no MCU logic outside this file)
static inline void servoAnalogWriteResolution(const char* axisPrefix, const ServoPins* Pins) {
  // For ES32 set resolution for ph2 pin
  const uint8_t pwmPin = static_cast<uint8_t>(Pins->ph2); // PWM pin

  // Per-pin resolution, use SERVO_ANALOG_WRITE_RESOLUTION_OVERRIDE bits
  // ESP32
  #if defined(ESP32)
    const uint8_t bits = static_cast<uint8_t>(SERVO_ANALOG_WRITE_RESOLUTION_OVERRIDE);
    VF("MSG:"); V(axisPrefix); VF(" pin "); V((unsigned)pwmPin);
    VF(": setting pin analogWriteResolution(pin,bits) -> "); VL((unsigned)bits);
    analogWriteResolution(pwmPin, bits);

  // Global resolution, use SERVO_ANALOG_WRITE_RESOLUTION
  // Teensy / STM32
  #elif defined(__TEENSYDUINO__) || defined(__ARM_STM32__)
    const uint8_t bits = static_cast<uint8_t>(SERVO_ANALOG_WRITE_RESOLUTION);
    VF("MSG:"); V(axisPrefix); VF(" pin "); V((unsigned)pwmPin);
    VF(": setting global analogWriteResolution(bits) to "); VL((unsigned)bits);
    analogWriteResolution(bits);

  // No analogWriteResolution, rely on RANGE scaling solely
  // ESP8266
  #elif defined(ESP8266)

    VF("MSG:"); V(axisPrefix); VF(" pin "); V((unsigned)pwmPin);
    VF(": no analogWriteResolution(); using range "); VL((unsigned long)SERVO_ANALOG_WRITE_RANGE);

  #else
    // Fallback: unknown/other cores
    VF("MSG:"); V(axisPrefix); VF(" pin "); V((unsigned)pwmPin);
    VF(": unknown core; skipping analogWriteResolution(), range "); VL((unsigned long)SERVO_ANALOG_WRITE_RANGE);

#endif
}