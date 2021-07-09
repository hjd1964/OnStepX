// -----------------------------------------------------------------------------------
// Dew Heater control

#include "DewHeater.h"

void DewHeater::init(int index, bool validKey) {
  this->index = index;

  // write the default settings to NV
  if (!validKey) {
    VF("MSG: DewHeater/Feature"); V(index + 1); VLF(", writing default settings to NV");
    nv.write(NV_FEATURE_SETTINGS_BASE + index*3, (uint8_t)round((zero + 5.0)*10.0));
    nv.write(NV_FEATURE_SETTINGS_BASE + index*3 + 1, (uint8_t)round((span + 5.0)*10.0));
  }

  zero = nv.readUC(NV_FEATURE_SETTINGS_BASE + index*3)/10.0 - 5.0;
  if (zero < -5.0) { zero = -5.0; DLF("ERR: DewHeater::init(), NV zero too low (set to -5.0)"); }
  if (zero > 20) { zero = 20.0; DLF("ERR: DewHeater::init(), NV zero too high (set to 20.0)"); }

  span = nv.readUC(NV_FEATURE_SETTINGS_BASE + index*3 + 1)/10.0 - 5.0;
  if (zero == -5.0 && span == -5.0) span = -4.9; // init. state is ok, no error or warning
  if (span < -5.0) { span = -5.0; DLF("ERR: DewHeater::init(), NV span too low (set to -5.0)"); }
  if (span > 20) { span = 20.0; DLF("ERR: DewHeater::init(), NV span too high (set to 20.0)"); }

  if (zero >= span) {
    if (span > -5.0) zero = span - 0.1; else span = zero + 0.1;
    DLF("ERR: dewHeater::init(), NV zero >= span (corrected)");
  }
}

void DewHeater::poll(float deltaAboveDewPointC) {
  if (isnan(deltaAboveDewPointC)) { heaterOn = false; return; }
    
  if (!enabled) return;

  int switchTimeMs = 0;
  switchTimeMs = map(lround(deltaAboveDewPointC*10.0), lround(zero*10.0), lround(span*10.0), DEW_HEATER_PULSE_WIDTH_MS, 0);
  switchTimeMs = constrain(switchTimeMs, 0, DEW_HEATER_PULSE_WIDTH_MS);
  #ifdef DEW_HEATER_MAX_POWER
    switchTimeMs = lround(switchTimeMs*(DEW_HEATER_MAX_POWER/100.0))
  #endif

  currentTime = millis();
  if ((long)(currentTime - (lastHeaterCycle + DEW_HEATER_PULSE_WIDTH_MS)) > 0) {
    lastHeaterCycle = currentTime;
  } else

  if (!heaterOn && (long)(currentTime - (lastHeaterCycle + switchTimeMs)) <= 0) {
    heaterOn = true;
  } else

  if (heaterOn && (long)(currentTime - (lastHeaterCycle + switchTimeMs)) > 0) {
    heaterOn = false;
  }
}

float DewHeater::getZero() {
  return zero;
}

void DewHeater::setZero(float t) {
  if (t >= -5.0 && t <= 20.0) {
    zero = t;
    if (zero >= span) zero = span - 0.1;
    nv.write(NV_FEATURE_SETTINGS_BASE + index*3, (uint8_t)round((zero + 5.0)*10.0));
  }
}

float DewHeater::getSpan() {
  return span;
}

void DewHeater::setSpan(float t) {
  if (t >= -5.0 && t <= 20.0) {
    span = t;
    if (span <= zero) span = zero + 0.1;
    nv.write(NV_FEATURE_SETTINGS_BASE + index*3 + 1, (uint8_t)round((span + 5.0)*10.0));
  }
}

bool DewHeater::isEnabled() {
  return enabled;
}

void DewHeater::enable(bool state) {
  heaterOn = false;
  enabled = state;
}

bool DewHeater::isOn() {
  return heaterOn;
}
