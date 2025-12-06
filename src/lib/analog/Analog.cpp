// Analog.cpp
#include "Analog.h"
#include <math.h>

#include "../../Common.h"

// Track actual global resolution when platform only supports global reconfig.
// Keeps behavior honest (no "virtual range trick").
static uint16_t gPwmBits = (uint16_t)HAL_BITS_FROM_RANGE(ANALOG_WRITE_RANGE);
static uint16_t gAdcBits = (uint16_t)HAL_BITS_FROM_RANGE(ANALOG_READ_RANGE);

static inline bool toCorePin(int16_t pin, uint8_t &out) {
  if (pin == OFF) return false;
  if (pin < 0) return false;          // reject any other negative
  if (pin > 0xFF) return false;       // prevents wrap on platforms with >255 pins
  out = (uint8_t)pin;
  return true;
}

static inline uint32_t clampU32(uint32_t v, uint32_t lo, uint32_t hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

static inline float clamp01(float x) {
  if (x < 0.0f) return 0.0f;
  if (x > 1.0f) return 1.0f;
  return x;
}

static inline uint32_t rangeFromBits(uint16_t bits) {
  if (bits == 0) return 0;
  if (bits >= 32) return 0xFFFFFFFFu;
  return (1u << bits) - 1u;
}

int AnalogClass::pwmIndex(int16_t pin) const {
  for (int i = 0; i < MAX_TRACKED; i++) if (pwm_[i].pin == pin) return i;
  for (int i = 0; i < MAX_TRACKED; i++) if (pwm_[i].pin == OFF) return i;
  return 0;
}

int AnalogClass::adcIndex(int16_t pin) const {
  for (int i = 0; i < MAX_TRACKED; i++) if (adc_[i].pin == pin) return i;
  for (int i = 0; i < MAX_TRACKED; i++) if (adc_[i].pin == OFF) return i;
  return 0;
}

// ---------------- PWM ----------------

bool AnalogClass::pwmInit(int16_t pin, const AnalogPwmConfig& cfg) {
  uint8_t p;
  if (!toCorePin(pin, p)) return false;

  const int idx = pwmIndex(pin);

  const uint32_t defRange = (uint32_t)ANALOG_WRITE_RANGE;

  uint16_t reqBits  = cfg.bits;
  uint32_t reqRange = cfg.range;

  // Derive bits from range if only range provided (range must be 2^n-1)
  if (reqBits == 0 && reqRange != 0) {
    if (reqRange == 0xFFFFFFFFu) return false; // avoid wrap
    uint32_t r = reqRange + 1;
    if ((r & (r - 1)) != 0) return false;      // must be 2^n
    uint16_t b = 0;
    while (r >>= 1) b++;
    reqBits = b;
  }

  // Derive range from bits or default
  if (reqBits == 0 && reqRange == 0) {
    reqRange = defRange;
  } else if (reqRange == 0 && reqBits != 0) {
    reqRange = rangeFromBits(reqBits);
  }

  // Bounds
  if (reqBits) {
    if (reqBits < HAL_PWM_BITS_MIN || reqBits > HAL_PWM_BITS_MAX) return false;
  }
  if (cfg.hz) {
    if (cfg.hz < HAL_PWM_HZ_MIN || cfg.hz > HAL_PWM_HZ_MAX) return false;
  }

  // --- APPLY (best-effort, but no silent global stomping unless allowed) ---

  // Frequency
  if (cfg.hz) {
    if (HAL_HAS_PER_PIN_PWM_FREQUENCY) {
      #ifdef analogWriteFrequency
        analogWriteFrequency(p, cfg.hz);
      #else
        return false;
      #endif
    } else if (HAL_HAS_GLOBAL_PWM_FREQUENCY) {
      if (!HAL_ALLOW_GLOBAL_PWM_RECONFIG) return false;
      #ifdef analogWriteFrequency
        analogWriteFrequency(p, cfg.hz);
      #else
        return false;
      #endif
    } else {
      return false;
    }
  }

  // Resolution
  if (reqBits) {
    if (HAL_HAS_PER_PIN_PWM_RESOLUTION) {
      #ifdef analogWriteResolution
        analogWriteResolution(p, reqBits);
      #else
        return false;
      #endif
    } else if (HAL_HAS_GLOBAL_PWM_RESOLUTION) {
      if (!HAL_ALLOW_GLOBAL_PWM_RECONFIG) return false;
      #ifdef analogWriteResolution
        analogWriteResolution(reqBits);
        gPwmBits = reqBits; // reflect actual global state
      #else
        return false;
      #endif
    } else {
      return false;
    }
  }

  // --- COMMIT ONLY ON SUCCESS ---
  pwm_[idx].pin   = pin;
  pwm_[idx].bits  = reqBits;
  pwm_[idx].hz    = cfg.hz;
  pwm_[idx].range = reqRange ? reqRange : defRange;

  return true;
}

uint32_t AnalogClass::pwmRange(int16_t pin) const {
  const uint32_t fallback = (uint32_t)ANALOG_WRITE_RANGE;

  // If only global PWM resolution exists, report the true global range
  if (HAL_HAS_GLOBAL_PWM_RESOLUTION && !HAL_HAS_PER_PIN_PWM_RESOLUTION) {
    (void)pin;
    return rangeFromBits(gPwmBits);
  }

  for (int i = 0; i < MAX_TRACKED; i++) {
    if (pwm_[i].pin == pin) return pwm_[i].range ? pwm_[i].range : fallback;
  }
  return fallback;
}

void AnalogClass::write(int16_t pin, uint32_t v) {
  uint8_t p;
  if (!toCorePin(pin, p)) return;

  const uint32_t r = pwmRange(pin);
  v = clampU32(v, 0, r);
  analogWrite(p, (int)v);
}

void AnalogClass::write(int16_t pin, float x01) {
  uint8_t p;
  if (!toCorePin(pin, p)) return;

  x01 = clamp01(x01);
  const uint32_t r = pwmRange(pin);
  const uint32_t v = (uint32_t)lroundf(x01 * (float)r);
  analogWrite(p, (int)v);
}

void AnalogClass::writeQ16(int16_t pin, uint16_t q16) {
  uint8_t p;
  if (!toCorePin(pin, p)) return;

  const uint32_t r = pwmRange(pin);
  const uint32_t v = (uint32_t)((((uint64_t)q16) * (uint64_t)r + 32767ULL) / 65535ULL);
  analogWrite(p, (int)v);
}

// ---------------- ADC ----------------

bool AnalogClass::adcInit(int16_t pin, const AnalogAdcConfig& cfg) {
  uint8_t p;
  if (!toCorePin(pin, p)) return false;

  const int idx = adcIndex(pin);
  const uint32_t defRange = (uint32_t)ANALOG_READ_RANGE;

  uint16_t reqBits  = cfg.bits;
  uint32_t reqRange = cfg.range;

  // Derive bits from range if needed (range must be 2^n-1)
  if (reqBits == 0 && reqRange != 0) {
    if (!HAL_IS_POW2_MINUS1(reqRange)) return false;
    reqBits = (uint16_t)HAL_BITS_FROM_RANGE(reqRange);
  }

  // Derive range from bits or default
  if (reqBits == 0 && reqRange == 0) {
    reqRange = defRange;
  } else if (reqRange == 0 && reqBits != 0) {
    reqRange = rangeFromBits(reqBits);
  }

  // Bounds
  if (reqBits) {
    if (reqBits < HAL_ADC_BITS_MIN || reqBits > HAL_ADC_BITS_MAX) return false;
  }

  // Apply resolution only if allowed
  if (reqBits) {
    if (HAL_HAS_PER_PIN_ADC_RESOLUTION) {
      #ifdef analogReadResolution
        analogReadResolution(p, reqBits);
      #else
        return false;
      #endif
    } else if (HAL_HAS_GLOBAL_ADC_RESOLUTION) {
      if (!HAL_ALLOW_GLOBAL_ADC_RECONFIG) return false;
      #ifdef analogReadResolution
        analogReadResolution(reqBits);
        gAdcBits = reqBits; // reflect actual global state
      #else
        return false;
      #endif
    } else {
      return false;
    }
  }

  // commit
  adc_[idx].pin        = pin;
  adc_[idx].bits       = reqBits;
  adc_[idx].range      = reqRange ? reqRange : defRange;
  adc_[idx].oversample = cfg.oversample ? cfg.oversample : 1;
  return true;
}

uint32_t AnalogClass::adcRange(int16_t pin) const {
  const uint32_t fallback = (uint32_t)ANALOG_READ_RANGE;

  // If only global ADC resolution exists, report the true global range
  if (HAL_HAS_GLOBAL_ADC_RESOLUTION && !HAL_HAS_PER_PIN_ADC_RESOLUTION) {
    (void)pin;
    return rangeFromBits(gAdcBits);
  }

  for (int i = 0; i < MAX_TRACKED; i++) {
    if (adc_[i].pin == pin) return adc_[i].range ? adc_[i].range : fallback;
  }
  return fallback;
}

uint32_t AnalogClass::read(int16_t pin) {
  uint8_t p;
  if (!toCorePin(pin, p)) return 0;

  uint8_t os = 1;
  for (int i = 0; i < MAX_TRACKED; i++) {
    if (adc_[i].pin == pin) { os = adc_[i].oversample; break; }
  }

  if (os <= 1) return (uint32_t)analogRead(p);

  uint32_t acc = 0;
  for (uint8_t i = 0; i < os; i++) acc += (uint32_t)analogRead(p);
  return (acc + (os / 2)) / os;
}

float AnalogClass::readf(int16_t pin) {
  uint8_t p;
  if (!toCorePin(pin, p)) return 0.0f;
  (void)p;

  const uint32_t r = adcRange(pin);
  const uint32_t v = read(pin);
  if (r == 0) return 0.0f;
  return clamp01((float)v / (float)r);
}

float AnalogClass::readV(int16_t pin) {
  uint8_t p;
  if (!toCorePin(pin, p)) return 0.0f;

#if defined(ESP32)
  return (float)analogReadMilliVolts(p) * 0.001F;
#else
  const int raw = analogRead(p);
  return (float)raw * (HAL_VCC / (float)ANALOG_READ_RANGE);
#endif
}

uint16_t AnalogClass::readQ16(int16_t pin) {
  uint8_t p;
  if (!toCorePin(pin, p)) return 0;
  (void)p;

  const uint32_t r = adcRange(pin);
  const uint32_t v = read(pin);
  if (r == 0) return 0;

  const uint32_t q = (uint32_t)((((uint64_t)v) * 65535ULL + (r / 2)) / (uint64_t)r);
  return (uint16_t)clampU32(q, 0, 65535U);
}

AnalogClass analog;
