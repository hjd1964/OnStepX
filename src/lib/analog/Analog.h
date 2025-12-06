// Analog.h
#pragma once
#include <Arduino.h>
#include <stdint.h>
#include "../../Common.h"

// --- Pin sentinel ---
#ifndef OFF
  #define OFF (-1)
#endif

// --- Helpers (compile-time) ---
#ifndef HAL_PWM_RANGE_FROM_BITS
  #define HAL_PWM_RANGE_FROM_BITS(bits) ((1 << (bits)) - 1)
#endif

#ifndef HAL_IS_POW2_MINUS1
  #define HAL_IS_POW2_MINUS1(R) ((R) != 0 && ((((R) + 1) & (R)) == 0))
#endif

#ifndef HAL_BITS_FROM_RANGE
  #define HAL_BITS_FROM_RANGE(R) ( \
    (R)<=1?1 : (R)<=3?2 : (R)<=7?3 : (R)<=15?4 : (R)<=31?5 : (R)<=63?6 : \
    (R)<=127?7 : (R)<=255?8 : (R)<=511?9 : (R)<=1023?10 : (R)<=2047?11 : \
    (R)<=4095?12 : (R)<=8191?13 : (R)<=16383?14 : (R)<=32767?15 : 16 )
#endif

#if !HAL_IS_POW2_MINUS1(ANALOG_WRITE_RANGE)
  #error "ANALOG_WRITE_RANGE must be (2^n - 1)."
#endif
#if HAL_BITS_FROM_RANGE(ANALOG_WRITE_RANGE) < HAL_PWM_BITS_MIN || HAL_BITS_FROM_RANGE(ANALOG_WRITE_RANGE) > HAL_PWM_BITS_MAX
  #error "ANALOG_WRITE_RANGE exceeds platform PWM resolution limits."
#endif
#if HAL_BITS_FROM_RANGE(ANALOG_READ_RANGE) < HAL_ADC_BITS_MIN || HAL_BITS_FROM_RANGE(ANALOG_READ_RANGE) > HAL_ADC_BITS_MAX
  #error "ANALOG_READ_RANGE exceeds platform ADC resolution limits."
#endif

// Optional: Servo PWM range only allowed to differ when per-pin PWM resolution exists
#ifdef SERVO_ANALOG_WRITE_RANGE
  #if !HAL_IS_POW2_MINUS1(SERVO_ANALOG_WRITE_RANGE)
    #error "SERVO_ANALOG_WRITE_RANGE must be (2^n - 1)."
  #endif
  #if !HAL_HAS_PER_PIN_PWM_RESOLUTION && SERVO_ANALOG_WRITE_RANGE != ANALOG_WRITE_RANGE
    #error "Global-only PWM: SERVO_ANALOG_WRITE_RANGE must equal ANALOG_WRITE_RANGE."
  #endif
#endif

// Optional small PODs for config (extendable without breaking API)
struct AnalogPwmConfig {
  uint32_t hz    = 0;   // 0 = leave default
  uint16_t bits  = 0;   // 0 = leave default
  uint32_t range = 0;   // 0 = auto (2^bits-1) or leave
};

struct AnalogAdcConfig {
  uint16_t bits       = 0;   // 0 = leave default
  uint32_t range      = 0;   // 0 = auto (2^bits-1) or leave
  uint8_t  oversample = 1;
};

class AnalogClass {
public:
  // --- PWM (write) ---
  bool pwmInit(int16_t pin, const AnalogPwmConfig& cfg = {});
  void write(int16_t pin, uint32_t value);   // native 0..range
  void write(int16_t pin, float x01);        // normalized 0..1
  void writeQ16(int16_t pin, uint16_t q16);  // normalized 0..65535
  uint32_t pwmRange(int16_t pin) const;

  // --- ADC (read) ---
  bool adcInit(int16_t pin, const AnalogAdcConfig& cfg = {});
  uint32_t read(int16_t pin);                // native 0..range
  float    readf(int16_t pin);               // normalized 0..1
  float    readV(int16_t pin);               // volts (best-effort)
  uint16_t readQ16(int16_t pin);             // normalized 0..65535
  uint32_t adcRange(int16_t pin) const;

private:
  struct PwmState { int16_t pin = OFF; uint16_t bits = 0; uint32_t hz = 0; uint32_t range = 0; };
  struct AdcState { int16_t pin = OFF; uint16_t bits = 0; uint8_t  oversample = 1; uint32_t range = 0; };

  static constexpr uint8_t MAX_TRACKED = 32;
  mutable PwmState pwm_[MAX_TRACKED]{};
  mutable AdcState adc_[MAX_TRACKED]{};

  int pwmIndex(int16_t pin) const;
  int adcIndex(int16_t pin) const;
};

extern AnalogClass analog;
