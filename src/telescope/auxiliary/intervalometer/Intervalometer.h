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

class Intervalometer {
  public:
    void init(int index);

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
    uint8_t timeToByte(float t);
    float byteToTime(uint8_t b);

    enum Pressed { P_STANDBY, P_EXP_START, P_EXP_START_PHASE2, P_EXP_DONE, P_EXP_DONE_PHASE2, P_WAIT };

    Pressed pressed = P_STANDBY;
    bool enabled = false;

    float expTime = 0.0F;
    float expDelay = 0.0F;
    int expCount = 0;
    int thisCount = 0;

    unsigned long expDone = 0;
    unsigned long waitDone = 0;

    int index = 0;
};
#endif
