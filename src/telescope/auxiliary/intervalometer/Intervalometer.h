// -----------------------------------------------------------------------------------
// Intervalometer control
#pragma once

#include "../../../Common.h"

#if FEATURE1_PURPOSE != OFF || FEATURE2_PURPOSE != OFF || FEATURE3_PURPOSE != OFF || FEATURE4_PURPOSE != OFF || FEATURE5_PURPOSE != OFF || FEATURE6_PURPOSE != OFF || FEATURE7_PURPOSE != OFF || FEATURE8_PURPOSE != OFF
  #ifndef FEATURES_PRESENT
    #define FEATURES_PRESENT
  #endif
#endif

#ifdef FEATURES_PRESENT

typedef struct FeatureIntervalometerSettings {
  float expTime;
  float expDelay;
  uint8_t expCount;
} FeatureIntervalometerSettings;

class Intervalometer {
  public:
    void init(uint8_t index);

    void poll();

    float getExposure();
    void setExposure(float t);

    float getDelay();
    void setDelay(float t);

    float getCurrentCount();
    float getCount();
    void setCount(float c);

    bool isEnabled();
    void enable(bool state);

    bool isOn();

  private:
    enum Pressed { P_STANDBY, P_EXP_START, P_EXP_START_PHASE2, P_EXP_DONE, P_EXP_DONE_PHASE2, P_WAIT };

    Pressed pressed = P_STANDBY;
    bool enabled = false;

    FeatureIntervalometerSettings settings = {0.0F, 0.0F, 0};
    uint8_t thisCount = 0;

    unsigned long expDone = 0;
    unsigned long waitDone = 0;

    uint8_t index = 0;

    int16_t nvKey;
};
#endif
