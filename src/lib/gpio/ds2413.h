// -----------------------------------------------------------------------------------
// Dallas/Maxim 1-Wire ds2413 device support
#pragma once

#include "../../common.h"

#if FEATURE1_PURPOSE != OFF || FEATURE2_PURPOSE != OFF || FEATURE3_PURPOSE != OFF || FEATURE4_PURPOSE != OFF || FEATURE5_PURPOSE != OFF || FEATURE6_PURPOSE != OFF || FEATURE7_PURPOSE != OFF || FEATURE8_PURPOSE != OFF
  #ifndef FEATURES_PRESENT
    #define FEATURES_PRESENT
  #endif
#endif

#ifdef FEATURES_PRESENT
  #if (FEATURE1_PIN & DS_MASK) == DS2413 || (FEATURE2_PIN & DS_MASK) == DS2413 || (FEATURE3_PIN & DS_MASK) == DS2413 || (FEATURE4_PIN & DS_MASK) == DS2413 || (FEATURE5_PIN & DS_MASK) == DS2413 || (FEATURE6_PIN & DS_MASK) == DS2413 || (FEATURE7_PIN & DS_MASK) == DS2413 || (FEATURE8_PIN & DS_MASK) == DS2413
    #define DS2413_DEVICES_PRESENT
  #endif
#endif

#ifdef DS2413_DEVICES_PRESENT

class Ds2413 {
  public:
    // scan for DS18B20 and DS2413 devices on the 1-wire bus and prepare for operation
    bool init();

    // read DS2413 devices, designed for a 0.02s polling interval
    void poll();

    // four DS2413 1-wire GPIO's are supported, this gets the last set value
    // index 0 is auxiliary feature #1, etc. each DS2413 has two GPIO's for 8 total
    int getChannel(int index);

    // four DS2413 1-wire GPIO's are supported, this sets each output on or off
    // index 0 is auxiliary feature #1, etc. each DS2413 has two GPIO's for 8 total
    void setChannel(int index, bool state);

    // four DS2413 1-wire GPIO's are supported, this gets the status of each
    // index 0 is auxiliary feature #1, etc. each DS2413 has two GPIO's for 8 total
    bool failure(int index);
  
  private:
    uint8_t ds2413_device_count = 0;
    uint8_t ds2413_index = 0;
    uint8_t ds2413_address[4][8];
    uint64_t ds2413_device[4] = { (uint64_t)FEATURE1_PIN, (uint64_t)FEATURE3_PIN, (uint64_t)FEATURE5_PIN, (uint64_t)FEATURE7_PIN };
    bool ds2413_state[8] = { false, false, false, false, false, false, false, false };
    int16_t ds2413_failures[4] = { 0, 0, 0, 0 };

    bool ds2413_found = false;
    uint64_t ds2413_features[8] = { (uint64_t)FEATURE1_PIN, (uint64_t)FEATURE2_PIN, (uint64_t)FEATURE3_PIN, (uint64_t)FEATURE4_PIN, (uint64_t)FEATURE5_PIN, (uint64_t)FEATURE6_PIN, (uint64_t)FEATURE7_PIN, (uint64_t)FEATURE8_PIN };
    int16_t ds2413_this_state[8] = { INVALID, INVALID, INVALID, INVALID, INVALID, INVALID, INVALID, INVALID };
    int16_t ds2413_last_state[8] = { INVALID, INVALID, INVALID, INVALID, INVALID, INVALID, INVALID, INVALID };
};

extern Ds2413 gpio;

#endif
