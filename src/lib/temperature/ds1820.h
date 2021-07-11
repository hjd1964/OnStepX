// -----------------------------------------------------------------------------------
// Dallas/Maxim 1-Wire DS1820 device support
#pragma once

#include "../../common.h"

#if FEATURE1_PURPOSE != OFF || FEATURE2_PURPOSE != OFF || FEATURE3_PURPOSE != OFF || FEATURE4_PURPOSE != OFF || FEATURE5_PURPOSE != OFF || FEATURE6_PURPOSE != OFF || FEATURE7_PURPOSE != OFF || FEATURE8_PURPOSE != OFF
  #ifndef FEATURES_PRESENT
    #define FEATURES_PRESENT
  #endif
#endif

#ifdef FEATURES_PRESENT
  #if (FEATURE1_TEMP & DS_MASK) == DS1820 || (FEATURE2_TEMP & DS_MASK) == DS1820 || (FEATURE3_TEMP & DS_MASK) == DS1820 || (FEATURE4_TEMP & DS_MASK) == DS1820 || (FEATURE5_TEMP & DS_MASK) == DS1820 || (FEATURE6_TEMP & DS_MASK) == DS1820 || (FEATURE7_TEMP & DS_MASK) == DS1820 || (FEATURE8_TEMP & DS_MASK) == DS1820
    #define DS1820_DEVICES_PRESENT
  #endif

  #if (FEATURE1_TEMP & DS_MASK) == DS18S20 || (FEATURE2_TEMP & DS_MASK) == DS18S20 || (FEATURE3_TEMP & DS_MASK) == DS18S20 || (FEATURE4_TEMP & DS_MASK) == DS18S20 || (FEATURE5_TEMP & DS_MASK) == DS18S20 || (FEATURE6_TEMP & DS_MASK) == DS18S20 || (FEATURE7_TEMP & DS_MASK) == DS18S20 || (FEATURE8_TEMP & DS_MASK) == DS18S20
    #define DS1820_DEVICES_PRESENT
  #endif
#endif

#if !defined(DS1820_DEVICES_PRESENT) && (FOCUSER_TEMPERATURE & DS_MASK) == DS1820
  #define DS1820_DEVICES_PRESENT
#endif

#if !defined(DS1820_DEVICES_PRESENT) && (FOCUSER_TEMPERATURE & DS_MASK) == DS18S20
  #define DS1820_DEVICES_PRESENT
#endif

#ifdef DS1820_DEVICES_PRESENT

class Ds1820 {
  public:
    // scan for DS18B20 devices on the 1-wire bus and prepare for operation
    bool init();

    // read DS18B20 devices, designed for a 0.1s polling interval
    void poll();

    // nine DS1820 1-wire temperature sensors are supported, this gets the averaged
    // temperature in deg. C otherwise it falls back to the weather sensor temperature
    // index 0 is the focuser temperature, 1 through 8 are auxiliary features #1, #2, etc.
    // returns NAN if no temperature source is available or if a communications failure
    // results in no valid readings for > 30 seconds
    float getChannel(int index);
   
  private:
    // checks for polling status code from DS1820 library
    bool polling(float f);

    // checks for validated status code from DS1820 library
    float validated(float f);

    bool found = false;
    uint8_t deviceCount = 0;
    uint8_t address[9][8];
    uint64_t device[9] = { (uint64_t)FOCUSER_TEMPERATURE, (uint64_t)FEATURE1_TEMP, (uint64_t)FEATURE2_TEMP, (uint64_t)FEATURE3_TEMP, (uint64_t)FEATURE4_TEMP, (uint64_t)FEATURE5_TEMP, (uint64_t)FEATURE6_TEMP, (uint64_t)FEATURE7_TEMP, (uint64_t)FEATURE8_TEMP };

    float averageTemperature[9] = { NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN };
    unsigned long goodUntil[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
};

extern Ds1820 temperature;

#endif
