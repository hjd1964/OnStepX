// -----------------------------------------------------------------------------------
// Dew Heater control

#include "DewHeater.h"

#ifdef FEATURES_PRESENT

#include "../../../lib/nv/Nv.h"

void DewHeater::init(uint8_t index) {
  if (index > 7) return;
  this->index = index;

  char keyStr[26];
  snprintf(keyStr, sizeof(keyStr), "FEATURE%u_DEW_SETTINGS", index);
  nvKey = nv().kv().computeKey(keyStr);

  // write the default settings to NV
 if (!nv().kv().getOrInit(nvKey, settings)) { DF("WRN: Nv, init failed for "); VL(keyStr); }
  settings.zero = constrain(settings.zero, -5, 20);
  settings.span = constrain(settings.span, settings.zero, 20);

  if (settings.zero >= settings.span) {
    if (settings.span > -5.0) settings.zero = settings.span - 0.1; else settings.span = settings.zero + 0.1;
    DLF("ERR: dewHeater::init(), NV zero >= span (corrected)");
  }
}

void DewHeater::poll(float deltaAboveDewPointC) {
  if (isnan(deltaAboveDewPointC)) { heaterOn = false; return; }
    
  if (!enabled) return;

  int switchTimeMs = 0;
  switchTimeMs = map(lroundf(deltaAboveDewPointC*10.0F), lroundf(settings.zero*10.0F), lroundf(settings.span*10.0F), DEW_HEATER_PULSE_WIDTH_MS, 0);
  switchTimeMs = constrain(switchTimeMs, 0, DEW_HEATER_PULSE_WIDTH_MS);
  #ifdef DEW_HEATER_MAX_POWER
    switchTimeMs = lroundf(switchTimeMs*(DEW_HEATER_MAX_POWER/100.0));
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
  return settings.zero;
}

void DewHeater::setZero(float t) {
  if (t >= -5.0 && t <= 20.0) {
    settings.zero = t;
    if (settings.zero >= settings.span) settings.zero = settings.span - 0.1;
    nv().kv().put(nvKey, settings);
  }
}

float DewHeater::getSpan() {
  return settings.span;
}

void DewHeater::setSpan(float t) {
  if (t >= -5.0 && t <= 20.0) {
    settings.span = t;
    if (settings.span <= settings.zero) settings.span = settings.zero + 0.1;
    nv().kv().put(nvKey, settings);
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

#endif
