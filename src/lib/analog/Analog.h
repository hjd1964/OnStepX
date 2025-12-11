// Analog.h
#pragma once
#include <Arduino.h>
#include <stdint.h>
#include "../../Common.h"

// --- Pin sentinel ---
#ifndef OFF
  #define OFF (-1)
#endif

// --- Some error checking ---
#if !IS_POW2_MINUS1(ANALOG_WRITE_RANGE)
  #error "ANALOG_WRITE_RANGE must be (2^n - 1)."
#endif

#if !IS_POW2_MINUS1(ANALOG_WRITE_RANGE)
  #error "ANALOG_WRITE_RANGE must be (2^n - 1)."
#endif
#if ANALOG_WRITE_RANGE < RANGE_FROM_BITS(HAL_PWM_BITS_MIN)
  #error "ANALOG_WRITE_RANGE below platform PWM resolution limits."
#endif
#if ANALOG_WRITE_RANGE > RANGE_FROM_BITS(HAL_PWM_BITS_MAX)
  #error "ANALOG_WRITE_RANGE exceeds platform PWM resolution limits."
#endif

#if !IS_POW2_MINUS1(ANALOG_READ_RANGE)
  #error "ANALOG_READ_RANGE must be (2^n - 1)."
#endif
#if ANALOG_READ_RANGE < RANGE_FROM_BITS(HAL_ADC_BITS_MIN)
  #error "ANALOG_READ_RANGE below platform ADC resolution limits."
#endif
#if ANALOG_READ_RANGE > RANGE_FROM_BITS(HAL_ADC_BITS_MAX)
  #error "ANALOG_READ_RANGE exceeds platform ADC resolution limits."
#endif

// Optional: Servo PWM range only allowed to differ when per-pin PWM resolution exists
#ifdef SERVO_ANALOG_WRITE_RANGE
  #if !IS_POW2_MINUS1(SERVO_ANALOG_WRITE_RANGE)
    #error "SERVO_ANALOG_WRITE_RANGE must be (2^n - 1)."
  #endif
  #if !HAL_HAS_PER_PIN_PWM_RESOLUTION && SERVO_ANALOG_WRITE_RANGE != ANALOG_WRITE_RANGE
    #error "Global-only PWM: SERVO_ANALOG_WRITE_RANGE must equal ANALOG_WRITE_RANGE."
  #endif
#endif

static constexpr uint8_t ANALOG_INVALID_IDX = 0xFF;

struct AnalogPwmConfig {
  uint32_t hz = 0;         // 0 = leave default
  uint16_t bits = 0;       // 0 = leave default
  uint32_t range = 0;      // 0 = auto (2^bits-1) or leave
};

struct AnalogPwmHandle {
  uint8_t pin = 0;         // core pin for analogWrite()
  uint8_t idx = ANALOG_INVALID_IDX; // index into pwm_[] cache
};

struct AnalogAdcConfig {
  uint16_t bits = 0;       // 0 = leave default
  uint32_t range = 0;      // 0 = auto (2^bits-1) or leave
  uint8_t  oversample = 1;
};

struct AnalogAdcHandle {
  uint8_t pin = 0;         // core pin for analogRead()
  uint8_t oversample = 1;  // cached oversample (avoid adc_[] lookup each read)
  uint8_t idx = ANALOG_INVALID_IDX; // index into adc_[] cache
};

class AnalogClass {
public:
  // prepare global adc read resolution and global pwm write resolution/frequency
  void begin();

  // pwm write, with per pin frequency/resolution where possible
  bool pwmInit(int16_t pin, const AnalogPwmConfig& cfg = {});
  AnalogPwmHandle pwmInitHandle(int16_t pin, const AnalogPwmConfig& cfg);
  void write(int16_t pin, uint32_t value);   // native 0..range
  void write(int16_t pin, float x01);        // normalized 0..1
  void writeQ16(int16_t pin, uint16_t q16);  // normalized 0..65535
  void write(const AnalogPwmHandle& h, uint32_t v);
  void write(const AnalogPwmHandle& h, float x01);
  void writeQ16(const AnalogPwmHandle& h, uint16_t q16);
  uint32_t pwmRange(int16_t pin) const;

  // adc read, with per pin resolution where possible
  bool adcInit(int16_t pin, const AnalogAdcConfig& cfg = {});
  AnalogAdcHandle adcInitHandle(int16_t pin, const AnalogAdcConfig& cfg);
  uint32_t read(int16_t pin);                // native 0..range
  float    readf(int16_t pin);               // normalized 0..1
  float    readV(int16_t pin);               // volts (best-effort)
  uint16_t readQ16(int16_t pin);             // normalized 0..65535
  uint32_t read(const AnalogAdcHandle& h);
  float    readf(const AnalogAdcHandle& h);
  uint16_t readQ16(const AnalogAdcHandle& h);
  uint32_t adcRange(int16_t pin) const;

private:
  struct PwmState { int16_t pin = OFF; uint16_t bits = 0; uint32_t hz = 0; uint32_t range = 0; bool custom = false; };
  struct AdcState { int16_t pin = OFF; uint16_t bits = 0; uint8_t  oversample = 1; uint32_t range = 0; };

  static constexpr uint8_t MAX_TRACKED = 32; // must be <= 254
  mutable PwmState pwm_[MAX_TRACKED]{};
  mutable AdcState adc_[MAX_TRACKED]{};

  int pwmIndex(int16_t pin) const;
  int adcIndex(int16_t pin) const;
};

extern AnalogClass analog;
