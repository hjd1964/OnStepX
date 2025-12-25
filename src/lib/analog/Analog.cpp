// Analog.cpp
#include <math.h>

#include "Analog.h"

// this was only for pin 38 of a Teensy4.1, now any pin
#if defined(ARDUINO_TEENSY41) && defined(PWM_CUSTOM_PIN) && \
    defined(PWM_CUSTOM_ANALOG_WRITE_FREQUENCY) && defined(PWM_CUSTOM_ANALOG_WRITE_RANGE)
  #define HAS_CUSTOM_PIN
#endif

#ifdef HAS_CUSTOM_PIN
  static IntervalTimer itimer4;
  static volatile uint16_t _pwm_custom_period = 0;
  static volatile uint8_t _pwm_custom_toggle = 0;
  static float _base_freq_divider = PWM_CUSTOM_ANALOG_WRITE_FREQUENCY/(1.0F/(PWM_CUSTOM_ANALOG_WRITE_RANGE/1000000.0F));

  void PWM_CUSTOM_HWTIMER() {
    if (_pwm_custom_period == 0 || _pwm_custom_period == PWM_CUSTOM_ANALOG_WRITE_RANGE) {
      itimer4.update(PWM_CUSTOM_ANALOG_WRITE_RANGE/_base_freq_divider);
      digitalWriteF(PWM_CUSTOM_PIN, _pwm_custom_period == PWM_CUSTOM_ANALOG_WRITE_RANGE);
      _pwm_custom_toggle = 0;
      return;
    } else {
      if (!_pwm_custom_toggle) {
        itimer4.update((PWM_CUSTOM_ANALOG_WRITE_RANGE - _pwm_custom_period)/_base_freq_divider);
        digitalWriteF(PWM_CUSTOM_PIN, LOW);
      } else {
        itimer4.update(_pwm_custom_period/_base_freq_divider);
        digitalWriteF(PWM_CUSTOM_PIN, HIGH);
      }
    }
    _pwm_custom_toggle = !_pwm_custom_toggle;
  }

  #define analogWriteCustomPin(x) do { \
    uint32_t _v = (uint32_t)(x); \
    if (_v > (uint32_t)PWM_CUSTOM_ANALOG_WRITE_RANGE) _v = (uint32_t)PWM_CUSTOM_ANALOG_WRITE_RANGE; \
    _pwm_custom_period = (uint16_t)_v; \
  } while (0)
#endif

// track actual global resolution when platform only supports global reconfig.
// keeps behavior honest (no "virtual range trick").
static uint16_t gPwmBits = (uint16_t)BITS_FROM_RANGE(ANALOG_WRITE_RANGE);
static uint16_t gAdcBits = (uint16_t)BITS_FROM_RANGE(ANALOG_READ_RANGE);
static volatile bool ready = false;

static inline bool toCorePin(int16_t pin, uint8_t &out) {
  if (!ready) return false;           // not initialized
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
  if (isnan(x) || isinf(x)) return 0.0F;
  if (x < 0.0F) return 0.0F;
  if (x > 1.0F) return 1.0F;
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
  return -1;
}

int AnalogClass::adcIndex(int16_t pin) const {
  for (int i = 0; i < MAX_TRACKED; i++) if (adc_[i].pin == pin) return i;
  for (int i = 0; i < MAX_TRACKED; i++) if (adc_[i].pin == OFF) return i;
  return -1;
}

// prepare global adc read resolution and global pwm write resolution/frequency
void AnalogClass::begin() {
  if (ready) return;

  VLF("MSG: Analog, begin");

  // ADC global default
  (void)gAdcBits;
  #if HAL_HAS_GLOBAL_ADC_RESOLUTION
    const uint16_t adcBits = (uint16_t)BITS_FROM_RANGE(ANALOG_READ_RANGE);
    analogReadResolution(adcBits);
    gAdcBits = adcBits;
  #endif

  // PWM global default (ONLY if your platform truly uses a global resolution)
  (void)gPwmBits;
  #if HAL_HAS_GLOBAL_PWM_RESOLUTION && !HAL_HAS_PER_PIN_PWM_RESOLUTION
    const uint16_t pwmBits = (uint16_t)BITS_FROM_RANGE(ANALOG_WRITE_RANGE);
    analogWriteResolution(pwmBits);
    gPwmBits = pwmBits;
    VF("MSG: Analog, pwmInit global PWM resolution changed to bits="); VL(pwmBits);
  #endif

  // PWM global freq (again: only if global is real on that platform)
  #if defined(ANALOG_WRITE_FREQUENCY) && HAL_HAS_GLOBAL_PWM_FREQUENCY
    analogWriteFrequency(ANALOG_WRITE_FREQUENCY);
    VF("MSG: Analog, pwmInit global PWM freq changed to hz="); VL(ANALOG_WRITE_FREQUENCY);
  #endif

  ready = true;
}

// ---------------- PWM ----------------

bool AnalogClass::pwmInit(int16_t pin, const AnalogPwmConfig& cfg) {
  uint8_t p;
  if (!toCorePin(pin, p)) {
    DF("ERR: Analog, pwmInit invalid pin="); DL(pin);
    return false;
  }

  const int idx = pwmIndex(pin);
  if (idx < 0) { DF("ERR: Analog, pwmInit table full pin="); DL(pin); return false; }
  const uint32_t defRange = (uint32_t)ANALOG_WRITE_RANGE;
  uint16_t reqBits  = cfg.bits;
  uint32_t reqRange = cfg.range;
  uint32_t reqHz = cfg.hz;

  // if this is a T4.1 allow one extra custom PWM pin
  #ifdef HAS_CUSTOM_PIN
    if (pin == PWM_CUSTOM_PIN) {
      pinMode(PWM_CUSTOM_PIN, OUTPUT);
      pwm_[idx].custom = true;
      itimer4.priority(0);
      itimer4.begin(PWM_CUSTOM_HWTIMER, 100);
      VF("MSG: Analog, pwmInit emulating PWM using Teensy4.1 iTimer4 on pin"); VL(pin);
      reqBits = BITS_FROM_RANGE(PWM_CUSTOM_ANALOG_WRITE_RANGE);
      reqRange = PWM_CUSTOM_ANALOG_WRITE_RANGE;
      reqHz = PWM_CUSTOM_ANALOG_WRITE_FREQUENCY;
    } else pwm_[idx].custom = false;
  #endif

  if (!pwm_[idx].custom) {

    // derive bits from range if only range provided (range must be 2^n-1)
    if (reqBits == 0 && reqRange != 0) {
      if (reqRange == 0xFFFFFFFFu) {
        DF("ERR: Analog, pwmInit range wrap pin="); D(pin); DF(" range="); DL(reqRange);
        return false;
      }
      uint32_t r = reqRange + 1;
      if ((r & (r - 1)) != 0) {
        DF("ERR: Analog, pwmInit range not 2^n-1 pin="); D(pin); DF(" range="); DL(reqRange);
        return false;
      }
      uint16_t b = 0;
      while (r >>= 1) b++;
      reqBits = b;
    }

    // derive range from bits or default
    if (reqBits == 0 && reqRange == 0) {
      reqRange = defRange;
    } else if (reqRange == 0 && reqBits != 0) {
      reqRange = rangeFromBits(reqBits);
    }

    // bounds
    if (reqBits) {
      if (reqBits < HAL_PWM_BITS_MIN || reqBits > HAL_PWM_BITS_MAX) {
        DF("ERR: Analog, pwmInit bits out of bounds pin="); D(pin);
        DF(" bits="); D(reqBits); DF(" ("); D(HAL_PWM_BITS_MIN); DF(".."); D(HAL_PWM_BITS_MAX); DLF(")");
        return false;
      }
    }
    if (reqHz) {
      if (reqHz < HAL_PWM_HZ_MIN || reqHz > HAL_PWM_HZ_MAX) {
        DF("ERR: Analog, pwmInit hz out of bounds pin="); D(pin);
        DF(" hz="); D(reqHz); DF(" ("); D(HAL_PWM_HZ_MIN); DF(".."); D(HAL_PWM_HZ_MAX); DLF(")");
        return false;
      }
    }

    // --- APPLY (best-effort) ---

    // warn generally the "per-pin resolutions/frequencies" can still affect other pins if the core
    // maps channels to shared timers. This may interfere with tone() or other PWM users.
    static bool warned = false;
    (void)warned;

    // frequency
    if (reqHz) {
      #if HAL_HAS_PER_PIN_PWM_FREQUENCY
        analogWriteFrequency(p, reqHz);
        if (!warned) {
          warned = true;
          VLF("WRN:Analog, adding new resolutions/frequencies may affect other PWM pins (tone, servo, due to shared timers.)");
        }
      #elif HAL_HAS_GLOBAL_PWM_FREQUENCY
        if (!HAL_ALLOW_GLOBAL_PWM_RECONFIG) {
          DF("ERR: Analog, pwmInit global PWM freq reconfig disallowed pin="); DL(pin);
          return false;
        }
        analogWriteFrequency(reqHz);
        VF("WRN: Analog, pwmInit global PWM freq changed hz="); VL(reqHz);
      #else
        DF("ERR: Analog, pwmInit no PWM frequency control pin="); DL(pin);
        return false;
      #endif
    }

    // resolution
    if (reqBits) {
      #if HAL_HAS_PER_PIN_PWM_RESOLUTION
        analogWriteResolution(p, reqBits);
        if (!warned) {
          warned = true;
          VLF("WRN: Analog, adding new resolutions/frequencies may affect other PWM pins (tone, servo, due to shared timers.)");
        }
      #elif HAL_HAS_GLOBAL_PWM_RESOLUTION
        if (!HAL_ALLOW_GLOBAL_PWM_RECONFIG) {
          DF("ERR: Analog, pwmInit global PWM resolution reconfig disallowed pin="); DL(pin);
          return false;
        }
        analogWriteResolution(reqBits);
        gPwmBits = reqBits; // reflect actual global state
        VF("WRN: Analog, pwmInit global PWM bits set bits="); VL(reqBits);
      #else
        DF("ERR: Analog, pwmInit no PWM resolution control pin="); DL(pin);
        return false;
      #endif
    } else reqBits = gPwmBits;
  }

  pwm_[idx].pin   = pin;
  pwm_[idx].bits  = reqBits;
  pwm_[idx].hz    = reqHz;
  pwm_[idx].range = reqRange ? reqRange : defRange;

  VF("MSG: Analog, pwmInit ok pin="); V(pin);
  VF(" bits="); V(reqBits);
  VF(" hz="); V(reqHz);
  VF(" range="); VL(pwm_[idx].range);

  return true;
}

AnalogPwmHandle AnalogClass::pwmInitHandle(int16_t pin, const AnalogPwmConfig& cfg) {
  AnalogPwmHandle h;
  h.pin = 0;
  h.idx = ANALOG_INVALID_IDX;

  uint8_t p;
  if (!toCorePin(pin, p)) {
    DF("ERR: Analog pwmInitHandle invalid pin="); DL(pin);
    return h;
  }
  if (!pwmInit(pin, cfg)) {
    // pwmInit already emits the detailed error path
    return h;
  }

  // find the committed slot for this pin (no allocation now, should hit first loop)
  int idx = -1;
  for (int i = 0; i < MAX_TRACKED; i++) if (pwm_[i].pin == pin) { idx = i; break; }
  if (idx < 0 || idx > 0xFE) {
    DF("ERR: Analog pwmInitHandle slot not found pin="); DL(pin);
    return h;
  }

  h.pin = p;
  h.idx = (uint8_t)idx;
  return h;
}

uint32_t AnalogClass::pwmRange(int16_t pin) const {
  const uint32_t fallback = (uint32_t)ANALOG_WRITE_RANGE;

  #ifdef HAS_CUSTOM_PIN
    if (pin == (int16_t)PWM_CUSTOM_PIN) {
      for (int i = 0; i < MAX_TRACKED; i++)
        if (pwm_[i].pin == pin) return pwm_[i].range ? pwm_[i].range : fallback;
      return (uint32_t)PWM_CUSTOM_ANALOG_WRITE_RANGE;
    }
  #endif

  // if only global PWM resolution exists, report the true global range
  #if HAL_HAS_GLOBAL_PWM_RESOLUTION && !HAL_HAS_PER_PIN_PWM_RESOLUTION
    return rangeFromBits(gPwmBits);
  #endif

  for (int i = 0; i < MAX_TRACKED; i++) {
    if (pwm_[i].pin == pin) return pwm_[i].range ? pwm_[i].range : fallback;
  }
  return fallback;
}

void AnalogClass::write(int16_t pin, uint32_t v) {
  uint8_t p;
  if (!toCorePin(pin, p)) {
    VF("WRN: Analog write(u32) invalid pin="); VL(pin);
    return;
  }

  const uint32_t r = pwmRange(pin);
  v = clampU32(v, 0, r);

  #ifdef HAS_CUSTOM_PIN
    if (p == (uint8_t)PWM_CUSTOM_PIN) { analogWriteCustomPin(v); } else
  #endif
  analogWrite(p, (int)v);
}

void AnalogClass::write(int16_t pin, float x01) {
  uint8_t p;
  if (!toCorePin(pin, p)) {
    VF("WRN: Analog write(f) invalid pin="); VL(pin);
    return;
  }

  x01 = clamp01(x01);
  const uint32_t r = pwmRange(pin);
  const uint32_t v = (uint32_t)lroundf(x01 * (float)r);

  #ifdef HAS_CUSTOM_PIN
    if (p == (uint8_t)PWM_CUSTOM_PIN) { analogWriteCustomPin(v); } else
  #endif
  analogWrite(p, (int)v);
}

void AnalogClass::writeQ16(int16_t pin, uint16_t q16) {
  uint8_t p;
  if (!toCorePin(pin, p)) {
    VF("WRN: Analog writeQ16 invalid pin="); VL(pin);
    return;
  }

  const uint32_t r = pwmRange(pin);
  const uint32_t v = (uint32_t)((((uint64_t)q16) * (uint64_t)r + 32767ULL) / 65535ULL);

  #ifdef HAS_CUSTOM_PIN
    if (p == (uint8_t)PWM_CUSTOM_PIN) { analogWriteCustomPin(v); } else
  #endif
  analogWrite(p, (int)v);
}

void AnalogClass::write(const AnalogPwmHandle& h, uint32_t v) {
  if (h.idx == ANALOG_INVALID_IDX) return;

  uint32_t r;
  #ifdef HAS_CUSTOM_PIN
    if (h.pin == (uint8_t)PWM_CUSTOM_PIN) {
      r = pwm_[h.idx].range ? pwm_[h.idx].range : (uint32_t)PWM_CUSTOM_ANALOG_WRITE_RANGE;
    } else
  #endif
  #if HAL_HAS_GLOBAL_PWM_RESOLUTION && !HAL_HAS_PER_PIN_PWM_RESOLUTION
    r = rangeFromBits(gPwmBits);
  #else
    r = pwm_[h.idx].range ? pwm_[h.idx].range : (uint32_t)ANALOG_WRITE_RANGE;
  #endif

  v = clampU32(v, 0, r);

  #ifdef HAS_CUSTOM_PIN
    if (h.pin == (uint8_t)PWM_CUSTOM_PIN) { analogWriteCustomPin(v); } else
  #endif
  analogWrite(h.pin, (int)v);
}

void AnalogClass::write(const AnalogPwmHandle& h, float x01) {
  if (h.idx == ANALOG_INVALID_IDX) return;

  x01 = clamp01(x01);

  uint32_t r;
  #ifdef HAS_CUSTOM_PIN
    if (h.pin == (uint8_t)PWM_CUSTOM_PIN) {
      r = pwm_[h.idx].range ? pwm_[h.idx].range : (uint32_t)PWM_CUSTOM_ANALOG_WRITE_RANGE;
    } else
  #endif
  #if HAL_HAS_GLOBAL_PWM_RESOLUTION && !HAL_HAS_PER_PIN_PWM_RESOLUTION
    r = rangeFromBits(gPwmBits);
  #else
    r = pwm_[h.idx].range ? pwm_[h.idx].range : (uint32_t)ANALOG_WRITE_RANGE;
  #endif

  const uint32_t v = (uint32_t)lroundf(x01 * (float)r);

  #ifdef HAS_CUSTOM_PIN
    if (h.pin == (uint8_t)PWM_CUSTOM_PIN) { analogWriteCustomPin(v); } else
  #endif
  analogWrite(h.pin, (int)v);
}

void AnalogClass::writeQ16(const AnalogPwmHandle& h, uint16_t q16) {
  if (h.idx == ANALOG_INVALID_IDX) return;

  uint32_t r;
  #ifdef HAS_CUSTOM_PIN
    if (h.pin == (uint8_t)PWM_CUSTOM_PIN) {
      r = pwm_[h.idx].range ? pwm_[h.idx].range : (uint32_t)PWM_CUSTOM_ANALOG_WRITE_RANGE;
    } else
  #endif
  #if HAL_HAS_GLOBAL_PWM_RESOLUTION && !HAL_HAS_PER_PIN_PWM_RESOLUTION
    r = rangeFromBits(gPwmBits);
  #else
    r = pwm_[h.idx].range ? pwm_[h.idx].range : (uint32_t)ANALOG_WRITE_RANGE;
  #endif

  const uint32_t v = (uint32_t)((((uint64_t)q16) * (uint64_t)r + 32767ULL) / 65535ULL);

  #ifdef HAS_CUSTOM_PIN
    if (h.pin == (uint8_t)PWM_CUSTOM_PIN) { analogWriteCustomPin(v); } else
  #endif
  analogWrite(h.pin, (int)v);
}

// ---------------- ADC ----------------

bool AnalogClass::adcInit(int16_t pin, const AnalogAdcConfig& cfg) {
  uint8_t p;
  if (!toCorePin(pin, p)) {
    DF("ERR: Analog adcInit invalid pin="); DL(pin);
    return false;
  }

  const int idx = adcIndex(pin);
  if (idx < 0) { DF("ERR: Analog adcInit table full pin="); DL(pin); return false; }
  const uint32_t defRange = (uint32_t)ANALOG_READ_RANGE;

  uint16_t reqBits  = cfg.bits;
  uint32_t reqRange = cfg.range;

  // derive bits from range if needed (range must be 2^n-1)
  if (reqBits == 0 && reqRange != 0) {
    if (!IS_POW2_MINUS1(reqRange)) {
      DF("ERR: Analog adcInit range not 2^n-1 pin="); D(pin); DF(" range="); DL(reqRange);
      return false;
    }
    reqBits = (uint16_t)BITS_FROM_RANGE(reqRange);
  }

  // derive range from bits or default
  if (reqBits == 0 && reqRange == 0) {
    reqRange = defRange;
  } else if (reqRange == 0 && reqBits != 0) {
    reqRange = rangeFromBits(reqBits);
  }

  // bounds
  if (reqBits) {
    if (reqBits < HAL_ADC_BITS_MIN || reqBits > HAL_ADC_BITS_MAX) {
      DF("ERR: Analog, adcInit bits out of bounds pin="); D(pin);
      DF(" bits="); D(reqBits); DF(" ("); D(HAL_ADC_BITS_MIN); DF(".."); D(HAL_ADC_BITS_MAX); DLF(")");
      return false;
    }
  }

  // apply resolution only if allowed
  if (reqBits) {
    #if HAL_HAS_PER_PIN_ADC_RESOLUTION
      analogReadResolution(p, reqBits);
    #elif HAL_HAS_GLOBAL_ADC_RESOLUTION
      if (!HAL_ALLOW_GLOBAL_ADC_RECONFIG) {
        DF("ERR: Analog, adcInit global ADC reconfig disallowed pin="); DL(pin);
        return false;
      }
      analogReadResolution(reqBits);
      gAdcBits = reqBits; // reflect actual global state
      VF("WRN: Analog, adcInit global ADC bits set bits="); VL(reqBits);
    #else
      DF("ERR: Analog, adcInit no ADC resolution control pin="); DL(pin);
      return false;
    #endif
  }

  adc_[idx].pin        = pin;
  adc_[idx].bits       = reqBits;
  adc_[idx].range      = reqRange ? reqRange : defRange;
  adc_[idx].oversample = cfg.oversample ? cfg.oversample : 1;

  VF("MSG: Analog, adcInit ok pin="); V(pin);
  VF(" bits="); V(reqBits);
  VF(" range="); V(adc_[idx].range);
  VF(" os="); VL(adc_[idx].oversample);

  return true;
}

AnalogAdcHandle AnalogClass::adcInitHandle(int16_t pin, const AnalogAdcConfig& cfg) {
  AnalogAdcHandle h;
  h.pin = 0;
  h.oversample = 1;
  h.idx = ANALOG_INVALID_IDX;

  uint8_t p;
  if (!toCorePin(pin, p)) {
    DF("ERR: Analog, adcInitHandle invalid pin="); DL(pin);
    return h;
  }
  if (!adcInit(pin, cfg)) {
    // adcInit already emits the detailed error path
    return h;
  }

  int idx = -1;
  for (int i = 0; i < MAX_TRACKED; i++) if (adc_[i].pin == pin) { idx = i; break; }
  if (idx < 0 || idx > 0xFE) {
    DF("ERR: Analog, adcInitHandle slot not found pin="); DL(pin);
    return h;
  }

  h.pin = p;
  h.idx = (uint8_t)idx;
  h.oversample = adc_[idx].oversample ? adc_[idx].oversample : 1;
  return h;
}

uint32_t AnalogClass::adcRange(int16_t pin) const {
  const uint32_t fallback = (uint32_t)ANALOG_READ_RANGE;

  // If only global ADC resolution exists, report the true global range
  #if HAL_HAS_GLOBAL_ADC_RESOLUTION && !HAL_HAS_PER_PIN_ADC_RESOLUTION
    (void)pin;
    return rangeFromBits(gAdcBits);
  #endif

  for (int i = 0; i < MAX_TRACKED; i++) {
    if (adc_[i].pin == pin) return adc_[i].range ? adc_[i].range : fallback;
  }
  return fallback;
}

uint32_t AnalogClass::read(int16_t pin) {
  uint8_t p;
  if (!toCorePin(pin, p)) {
    VF("WRN: Analog, read invalid pin="); VL(pin);
    return 0;
  }

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
  if (!toCorePin(pin, p)) {
    VF("WRN: Analog, readf invalid pin="); VL(pin);
    return 0.0f;
  }
  (void)p;

  const uint32_t r = adcRange(pin);
  const uint32_t v = read(pin);
  if (r == 0) return 0.0f;
  return clamp01((float)v / (float)r);
}

float AnalogClass::readV(int16_t pin) {
  uint8_t p;
  if (!toCorePin(pin, p)) {
    VF("WRN: Analog, readV invalid pin="); VL(pin);
    return 0.0f;
  }

#if defined(ESP32)
  return (float)analogReadMilliVolts(p) * 0.001F;
#else
  const uint32_t r = adcRange(pin);
  if (r == 0) return 0.0F;
  const uint32_t raw = read(pin);
  return (float)raw * (HAL_VCC / (float)r);
#endif
}

uint16_t AnalogClass::readQ16(int16_t pin) {
  uint8_t p;
  if (!toCorePin(pin, p)) {
    VF("WRN: Analog, readQ16 invalid pin="); VL(pin);
    return 0;
  }
  (void)p;

  const uint32_t r = adcRange(pin);
  const uint32_t v = read(pin);
  if (r == 0) return 0;

  const uint32_t q = (uint32_t)((((uint64_t)v) * 65535ULL + (r / 2)) / (uint64_t)r);
  return (uint16_t)clampU32(q, 0, 65535U);
}

uint32_t AnalogClass::read(const AnalogAdcHandle& h) {
  if (h.idx == ANALOG_INVALID_IDX) return 0;

  const uint8_t os = (h.oversample <= 1) ? 1 : h.oversample;
  if (os == 1) return (uint32_t)analogRead(h.pin);

  uint32_t acc = 0;
  for (uint8_t i = 0; i < os; i++) acc += (uint32_t)analogRead(h.pin);
  return (acc + (os / 2)) / os;
}

float AnalogClass::readf(const AnalogAdcHandle& h) {
  if (h.idx == ANALOG_INVALID_IDX) return 0.0f;

  uint32_t r;
  #if HAL_HAS_GLOBAL_ADC_RESOLUTION && !HAL_HAS_PER_PIN_ADC_RESOLUTION
    r = rangeFromBits(gAdcBits);
  #else
    r = adc_[h.idx].range ? adc_[h.idx].range : (uint32_t)ANALOG_READ_RANGE;
  #endif

  if (r == 0) return 0.0f;
  const uint32_t v = read(h);
  return clamp01((float)v / (float)r);
}

uint16_t AnalogClass::readQ16(const AnalogAdcHandle& h) {
  if (h.idx == ANALOG_INVALID_IDX) return 0;

  uint32_t r;
  #if HAL_HAS_GLOBAL_ADC_RESOLUTION && !HAL_HAS_PER_PIN_ADC_RESOLUTION
    r = rangeFromBits(gAdcBits);
  #else
    r = adc_[h.idx].range ? adc_[h.idx].range : (uint32_t)ANALOG_READ_RANGE;
  #endif

  if (r == 0) return 0;
  const uint32_t v = read(h);
  const uint32_t q = (uint32_t)((((uint64_t)v) * 65535ULL + (r / 2)) / (uint64_t)r);
  return (uint16_t)clampU32(q, 0, 65535U);
}

AnalogClass analog;
