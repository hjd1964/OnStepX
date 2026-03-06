// -----------------------------------------------------------------------------------
// Intervalometer control

#include "Intervalometer.h"

#ifdef FEATURES_PRESENT

#include "../../../lib/nv/Nv.h"

void Intervalometer::init(uint8_t index) {
  if (index > 7) return;
  this->index = index;

  char keyStr[26];
  snprintf(keyStr, sizeof(keyStr), "FEATURE%u_INTV_SETTINGS", index);
  nvKey = nv().kv().computeKey(keyStr);

  if (!nv().kv().getOrInit(nvKey, settings)) { DF("WRN: Nv, init failed for "); VL(keyStr); }
  settings.expCount = constrain(settings.expCount, 0, 255);
  settings.expDelay = constrain(settings.expDelay, 1, 3600);
  settings.expTime = constrain(settings.expTime, 0, 3600);
}

void Intervalometer::poll() {
  if (!enabled) return;

  if (pressed == P_EXP_START) {
    // count and stop when done
    if (thisCount == 0) { pressed = P_STANDBY; enabled = false; return; }
    thisCount--;

    // start a new exposure
    pressed = P_EXP_DONE;
    expDone = millis() + (unsigned long)(settings.expTime*1000.0); // set exposure time in ms
  } else 

  // wait until exposure is done
  if (pressed == P_EXP_DONE && (long)(millis() - expDone) > 0) {
    // finish an exposure
    pressed = P_WAIT;
    waitDone = millis() + (unsigned long)(settings.expDelay*1000.0); // set wait time in ms
  } else

  // wait until pause between exposures is done
  if (pressed == P_WAIT && (long)(millis() - waitDone) > 0) {
    // start next count
    pressed = P_EXP_START;
  }
}

float Intervalometer::getExposure() {
  return settings.expTime;
}

void Intervalometer::setExposure(float t) {
  if (pressed == P_STANDBY && t >= 0 && t <= 3600) {
    settings.expTime = t;
    nv().kv().put(nvKey, settings);
  }
}

float Intervalometer::getDelay() {
  return settings.expDelay;
}

void Intervalometer::setDelay(float t) {
  if (pressed == P_STANDBY && t >= 1 && t <= 3600) {
    settings.expDelay = t;
    nv().kv().put(nvKey, settings);
  }
}

float Intervalometer::getCurrentCount() {
  return thisCount;
}

float Intervalometer::getCount() {
  return settings.expCount;
}

void Intervalometer::setCount(float count) {
  long c = lroundf(count);
  if (pressed == P_STANDBY && c >= 0 && c <= 255) {
    settings.expCount = c;
  }
}

bool Intervalometer::isEnabled() {
  return enabled;
}

void Intervalometer::enable(bool state) {
  enabled = state;
  if (enabled) {
    thisCount = settings.expCount;
    pressed = P_EXP_START;
  } else {
    thisCount = 0;
    pressed = P_STANDBY;
  }
}

bool Intervalometer::isOn() {
  return pressed == P_EXP_DONE;
}

#endif
