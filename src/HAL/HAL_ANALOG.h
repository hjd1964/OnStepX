
// Analog HAL capabilities (defaults) ----------------------------------------------------------------
// Defaults: no per-pin control, no global reconfig (safe)
#pragma once

#ifndef HAL_HAS_PER_PIN_PWM_RESOLUTION
  #define HAL_HAS_PER_PIN_PWM_RESOLUTION 0
#endif
#ifndef HAL_HAS_PER_PIN_PWM_FREQUENCY
  #define HAL_HAS_PER_PIN_PWM_FREQUENCY 0
#endif
#ifndef HAL_HAS_GLOBAL_PWM_RESOLUTION
  #define HAL_HAS_GLOBAL_PWM_RESOLUTION 0
#endif
#ifndef HAL_HAS_GLOBAL_PWM_FREQUENCY
  #define HAL_HAS_GLOBAL_PWM_FREQUENCY 0
#endif

#ifndef HAL_HAS_PER_PIN_ADC_RESOLUTION
  #define HAL_HAS_PER_PIN_ADC_RESOLUTION 0
#endif
#ifndef HAL_HAS_GLOBAL_ADC_RESOLUTION
  #define HAL_HAS_GLOBAL_ADC_RESOLUTION 0
#endif

#ifndef HAL_ALLOW_GLOBAL_PWM_RECONFIG
  #define HAL_ALLOW_GLOBAL_PWM_RECONFIG 0
#endif
#ifndef HAL_ALLOW_GLOBAL_ADC_RECONFIG
  #define HAL_ALLOW_GLOBAL_ADC_RECONFIG 0
#endif

// reasonable bounds (overridden per platform below)
#ifndef HAL_PWM_BITS_MIN
  #define HAL_PWM_BITS_MIN 1
#endif
#ifndef HAL_PWM_BITS_MAX
  #define HAL_PWM_BITS_MAX 16
#endif
#ifndef HAL_ADC_BITS_MIN
  #define HAL_ADC_BITS_MIN 1
#endif
#ifndef HAL_ADC_BITS_MAX
  #define HAL_ADC_BITS_MAX 12
#endif
#ifndef HAL_PWM_HZ_MIN
  #define HAL_PWM_HZ_MIN 1U
#endif
#ifndef HAL_PWM_HZ_MAX
  #define HAL_PWM_HZ_MAX 40000U
#endif

// some validation
#if HAL_PWM_BITS_MIN < 1 || HAL_PWM_BITS_MIN > 32
  #error "Error: HAL_PWM_BITS_MIN outside of 1 to 32 range."
#endif
#if HAL_PWM_BITS_MAX < 1 || HAL_PWM_BITS_MAX > 32
  #error "Error: HAL_PWM_BITS_MAX outside of 1 to 32 range."
#endif
#if HAL_ADC_BITS_MIN < 1 || HAL_ADC_BITS_MIN > 32
  #error "Error: HAL_ADC_BITS_MIN outside of 1 to 32 range."
#endif
#if HAL_ADC_BITS_MAX < 1 || HAL_ADC_BITS_MAX > 32
  #error "Error: HAL_ADC_BITS_MAX outside of 1 to 32 range."
#endif

// global default ranges
#ifndef ANALOG_READ_RANGE
  #define ANALOG_READ_RANGE 1023U  // 2^n - 1
#endif
#ifndef ANALOG_WRITE_RANGE
  #define ANALOG_WRITE_RANGE 255U  // 2^n - 1
#endif
