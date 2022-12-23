// -----------------------------------------------------------------------------------
// Thermistor device support
#pragma once

#include "../../Common.h"

#ifdef THERMISTOR_DEVICES_PRESENT

typedef struct ThermistorSettings {
  float tNom;
  float rNom;
  float beta;
  float rSeries;
} ThermistorSettings;

class Thermistor {
  public:
    // prepare for operation
    bool init();

    // read devices, designed for a 0.1s polling interval
    void poll();

    // nine temperature sensors are supported, this gets the averaged
    // temperature in deg. C otherwise it falls back to the weather sensor temperature
    // index 0 is the ambient temperature, 1 through 8 are point temperatures #1, #2, etc.
    // returns NAN if no temperature source is available or if a communications failure
    // results in no valid readings for > 30 seconds
    float getChannel(int index);
   
  private:
    bool found = false;
    uint8_t deviceCount = 0;
    uint64_t device[9] = { (uint64_t)FOCUSER_TEMPERATURE, (uint64_t)FEATURE1_TEMP, (uint64_t)FEATURE2_TEMP, (uint64_t)FEATURE3_TEMP, (uint64_t)FEATURE4_TEMP, (uint64_t)FEATURE5_TEMP, (uint64_t)FEATURE6_TEMP, (uint64_t)FEATURE7_TEMP, (uint64_t)FEATURE8_TEMP };
    int16_t devicePin[9] = { FOCUSER_TEMPERATURE_PIN, FEATURE1_TEMPERATURE_PIN, FEATURE2_TEMPERATURE_PIN, FEATURE3_TEMPERATURE_PIN, FEATURE4_TEMPERATURE_PIN, FEATURE5_TEMPERATURE_PIN, FEATURE6_TEMPERATURE_PIN, FEATURE7_TEMPERATURE_PIN, FEATURE8_TEMPERATURE_PIN };

    float averageTemperature[9] = { NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN };
    unsigned long goodUntil[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    ThermistorSettings settings[2] = {
      { THERMISTOR1_TNOM, THERMISTOR1_RNOM, THERMISTOR1_BETA, THERMISTOR1_RSERIES },
      { THERMISTOR2_TNOM, THERMISTOR2_RNOM, THERMISTOR2_BETA, THERMISTOR2_RSERIES }
    };
};

extern Thermistor temperature;

#endif
