// -----------------------------------------------------------------------------------
// Dallas/Maxim 1-Wire DS1820 device support
#pragma once

#include "../../common.h"

#if FEATURE1_PURPOSE != OFF || FEATURE2_PURPOSE != OFF || FEATURE3_PURPOSE != OFF || FEATURE4_PURPOSE != OFF || FEATURE5_PURPOSE != OFF || FEATURE6_PURPOSE != OFF || FEATURE7_PURPOSE != OFF || FEATURE8_PURPOSE != OFF
  #define FEATURES_PRESENT
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
    // scan for DS18B20 and DS2413 devices on the 1-wire bus and prepare for operation
    bool init();

    // read DS18B20 devices, designed for a 0.1s polling interval
    void poll();

    // nine DS1820 1-wire temperature sensors are supported, this gets
    // the temperature in deg. C otherwise it falls back to the weather sensor temperature
    // index 0 is the focuser temperature, 1 is auxiliary feature #1, etc.
    float getChannel(int index);
   
  private:
    uint8_t ds1820_device_count = 0;
    uint8_t ds1820_index = 0;
    uint8_t ds1820_address[9][8];
    uint64_t ds1820_device[9] = { (uint64_t)FOCUSER_TEMPERATURE, (uint64_t)FEATURE1_TEMP, (uint64_t)FEATURE2_TEMP, (uint64_t)FEATURE3_TEMP, (uint64_t)FEATURE4_TEMP, (uint64_t)FEATURE5_TEMP, (uint64_t)FEATURE6_TEMP, (uint64_t)FEATURE7_TEMP, (uint64_t)FEATURE8_TEMP };

    bool polling(float f);
    float validated(float f);

    bool  ds1820_found = false;
    float temperature = 10.0;
    float averageTemperature = 10.0;
    float featureTemperature[9] = { NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN };
    float averageFeatureTemperature[9] = { NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN };
};

extern Ds1820 temperature;

#endif
