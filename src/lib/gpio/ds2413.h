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
    // scan for DS2413 devices on the 1-wire bus
    bool init();

    // read DS2413 devices, designed for a 20ms polling interval
    void poll();

    // four DS2413 1-wire GPIO's are supported, this gets the last set value
    // index 0 is auxiliary feature #1, etc. each DS2413 has two GPIO's for 8 total
    int getChannel(int index);

    // four DS2413 1-wire GPIO's are supported, this sets each output on or off
    // index 0 is auxiliary feature #1, etc. each DS2413 has two GPIO's for 8 total
    void setChannel(int index, bool value);

    // four DS2413 1-wire GPIO's are supported, this gets the status of each
    // index 0 is auxiliary feature #1, etc. each DS2413 has two GPIO's for 8 total
    // returns true if there was a communications failure detected in the last 5 seconds
    bool failure(int index);
  
  private:
    uint8_t deviceCount = 0;
    uint8_t address[4][8];
    bool state[8] = { false, false, false, false, false, false, false, false };

    bool found = false;
    uint64_t device[8] = { (uint64_t)FEATURE1_PIN, (uint64_t)FEATURE2_PIN, (uint64_t)FEATURE3_PIN, (uint64_t)FEATURE4_PIN, (uint64_t)FEATURE5_PIN, (uint64_t)FEATURE6_PIN, (uint64_t)FEATURE7_PIN, (uint64_t)FEATURE8_PIN };
    int16_t lastState[8] = { INVALID, INVALID, INVALID, INVALID, INVALID, INVALID, INVALID, INVALID };
    unsigned long goodUntil[4] = { 0, 0, 0, 0 };
};

extern Ds2413 gpio;

#endif
