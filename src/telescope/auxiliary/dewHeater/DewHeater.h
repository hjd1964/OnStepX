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

class DewHeater {
  public:
    void init(int index);

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

    float zero = -5.0F;
    float span = 15.0F;

    int index = 0;
};

#endif