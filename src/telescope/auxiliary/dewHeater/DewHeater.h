// -----------------------------------------------------------------------------------
// Dew Heater control
#pragma once

#include "../../../Common.h"

#if FEATURE1_PURPOSE != OFF || FEATURE2_PURPOSE != OFF || FEATURE3_PURPOSE != OFF || FEATURE4_PURPOSE != OFF || FEATURE5_PURPOSE != OFF || FEATURE6_PURPOSE != OFF || FEATURE7_PURPOSE != OFF || FEATURE8_PURPOSE != OFF
  #ifndef FEATURES_PRESENT
    #define FEATURES_PRESENT
  #endif
#endif

#ifdef FEATURES_PRESENT

#ifndef DEW_HEATER_PULSE_WIDTH_MS
  #define DEW_HEATER_PULSE_WIDTH_MS 2000
#endif

typedef struct FeatureDewSettings {
  float zero;
  float span;
} FeatureDewSettings;

class DewHeater {
  public:
    void init(uint8_t index);

    void poll(float deltaAboveDewPointC);

    float getZero();
    void setZero(float t);

    float getSpan();
    void setSpan(float t);

    bool isEnabled();
    void enable(bool state);

    bool isOn();

  private:
    unsigned long lastHeaterCycle = 0;
    unsigned long currentTime = 0;

    bool heaterOn = false;
    bool enabled = false;

    FeatureDewSettings settings = {-5.0F, 15.0F};

    uint8_t index = 0;

    uint32_t nvKey;
};

#endif