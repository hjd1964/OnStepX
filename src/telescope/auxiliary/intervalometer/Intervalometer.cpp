// -----------------------------------------------------------------------------------
// Intervalometer control

#include "Intervalometer.h"

#ifdef FEATURES_PRESENT

#include "../../../lib/nv/Nv.h"
#include "../../../lib/convert/Convert.h"

void Intervalometer::init(int index) {
  this->index = index;

  // write the default settings to NV
  if (!nv.hasValidKey()) {
    VF("MSG: Intervalometer/Feature"); V(index + 1); VLF(", writing defaults to NV");
    nv.write(NV_FEATURE_SETTINGS_BASE + index*5, convert.packSeconds(expTime));
    nv.write(NV_FEATURE_SETTINGS_BASE + index*5 + 1, convert.packSeconds(expDelay));
    nv.write(NV_FEATURE_SETTINGS_BASE + index*5 + 2, (uint8_t)expCount);
  }

  expTime = convert.unpackSeconds(nv.readUC(NV_FEATURE_SETTINGS_BASE + index*5));
  expDelay = convert.unpackSeconds(nv.readUC(NV_FEATURE_SETTINGS_BASE + index*5 + 1));
  expCount = nv.readUC(NV_FEATURE_SETTINGS_BASE + index*5 + 2);
}

void Intervalometer::poll() {
  if (!enabled) return;

  if (pressed == P_EXP_START) {
    // count and stop when done
    if (thisCount == 0) { pressed = P_STANDBY; enabled = false; return; }
    thisCount--;

    // start a new exposure
    pressed = P_EXP_DONE;
    expDone = millis() + (unsigned long)(expTime*1000.0); // set exposure time in ms
  } else 

  // wait until exposure is done
  if (pressed == P_EXP_DONE && (long)(millis() - expDone) > 0) {
    // finish an exposure
    pressed = P_WAIT;
    waitDone = millis() + (unsigned long)(expDelay*1000.0); // set wait time in ms
  } else

  // wait until pause between exposures is done
  if (pressed == P_WAIT && (long)(millis() - waitDone) > 0) {
    // start next count
    pressed = P_EXP_START;
  }
}

float Intervalometer::getExposure() {
  return expTime;
}

void Intervalometer::setExposure(float t) {
  if (pressed == P_STANDBY && t >= 0 && t <= 3600) {
    expTime = t;
    nv.write(NV_FEATURE_SETTINGS_BASE + index*5, convert.packSeconds(expTime));
  }
}

float Intervalometer::getDelay() {
  return expDelay;
}

void Intervalometer::setDelay(float t) {
  if (pressed == P_STANDBY && t >= 1 && t <= 3600) {
    expDelay = t;
    nv.write(NV_FEATURE_SETTINGS_BASE + index*5 + 1, convert.packSeconds(expDelay));
  }
}

float Intervalometer::getCurrentCount() {
  return thisCount;
}

float Intervalometer::getCount() {
  return expCount;
}

void Intervalometer::setCount(float c) {
  if (pressed == P_STANDBY && c >= 0 && c <= 255) {
    expCount = c;
    nv.write(NV_FEATURE_SETTINGS_BASE + index*5 + 2, (uint8_t)expCount);
  }
}

bool Intervalometer::isEnabled() {
  return enabled;
}

void Intervalometer::enable(bool state) {
  enabled = state;
  if (enabled) { thisCount = expCount; pressed = P_EXP_START; } else { thisCount=0; pressed = P_STANDBY; }
}

bool Intervalometer::isOn() {
  return pressed == P_EXP_DONE;
}

#endif
