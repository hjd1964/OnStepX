// -----------------------------------------------------------------------------------
// Intervalometer control

#include "Intervalometer.h"

#ifdef FEATURES_PRESENT

void Intervalometer::init(int index) {
  this->index = index;

  // write the default settings to NV
  if (!nv.hasValidKey()) {
    VF("MSG: Intervalometer/Feature"); V(index + 1); VLF(", writing defaults to NV");
    nv.write(NV_FEATURE_SETTINGS_BASE + index*5, timeToByte(expTime));
    nv.write(NV_FEATURE_SETTINGS_BASE + index*5 + 1, timeToByte(expDelay));
    nv.write(NV_FEATURE_SETTINGS_BASE + index*5 + 2, (uint8_t)expCount);
  }

  expTime = byteToTime(nv.readUC(NV_FEATURE_SETTINGS_BASE + index*5));
  expDelay = byteToTime(nv.readUC(NV_FEATURE_SETTINGS_BASE + index*5 + 1));
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
    nv.write(NV_FEATURE_SETTINGS_BASE + index*5, timeToByte(expTime));
  }
}

float Intervalometer::getDelay() {
  return expDelay;
}

void Intervalometer::setDelay(float t) {
  if (pressed == P_STANDBY && t >= 1 && t <= 3600) {
    expDelay = t;
    nv.write(NV_FEATURE_SETTINGS_BASE + index*5 + 1, timeToByte(expDelay));
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

uint8_t Intervalometer::timeToByte(float t) {
  float f = 10.0F;                             // default is 1 second
  if (t <= 0.0162F) f = 0.0F; else             // 0.0156 (1/64 second)        (0)
  if (t <= 0.0313F) f = 1.0F; else             // 0.0313 (1/32 second)        (1)
  if (t <= 0.0625F) f = 2.0F; else             // 0.0625 (1/16 second)        (2)
  if (t <= 1.0F) f = 2.0F + t*8.0F; else       // 0.125 seconds to 1 seconds  (2 to 10)
  if (t <= 10.0F) f = 6.0F + t*4.0F; else      // 0.25 seconds to 10 seconds  (10 to 46)
  if (t <= 30.0F) f = 26.0F + t*2.0F; else     // 0.5 seconds to 30 seconds   (46 to 86)
  if (t <= 120.0F) f = 56.0F + t; else         // 1 second to 120 seconds     (86 to 176)
  if (t <= 600.0F) f = 168.0F + t/15.0F; else  // 15 seconds to 300 seconds   (176 to 208)
  if (t <= 3360.0F) f = 198.0F + t/60.0F; else // 1 minute to 56 minutes      (208 to 254)
  if (t <= 3600.0F) f = 255.0F;                // 1 hour                      (255)
  if (f < 0.0F) f = 0.0F;
  if (f > 255.0F) f = 255.0F;
  return lroundf(f);
}

float Intervalometer::byteToTime(uint8_t b) {
  float f = 1.0;                               // default is 1 second
  if (b == 0) f = 0.016125F; else              // 0.0156 (1/64 second)        (0)
  if (b == 1) f = 0.03125F; else               // 0.0313 (1/32 second)        (1)
  if (b == 2) f = 0.0625F; else                // 0.0625 (1/16 second)        (2)
  if (b <= 10) f = (b - 2.0F)/8.0F; else       // 0.125 seconds to 1 seconds  (2 to 10)
  if (b <= 46) f = (b - 6.0F)/4.0F; else       // 0.25 seconds to 10 seconds  (10 to 46)
  if (b <= 86) f = (b - 26.0F)/2.0F; else      // 0.5 seconds to 30 seconds   (46 to 86)
  if (b <= 176) f = (b - 56.0F); else          // 1 second to 120 seconds     (86 to 176)
  if (b <= 208) f = (b - 168.0F)*15.0F; else   // 15 seconds to 300 seconds   (176 to 208)
  if (b <= 254) f = (b - 198.0F)*60.0F; else   // 1 minute to 56 minutes      (208 to 254)
  if (b == 255) f = 3600.0F;                   // 1 hour                      (255)
  return f;
}

#endif
