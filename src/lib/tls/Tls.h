// -----------------------------------------------------------------------------------
// Time/Location source support

#include "../../Common.h"

// To enable TimeLib support (for non-astronomical time applications) use the following:
//#define TLS_TIMELIB

#ifndef TIME_LOCATION_SOURCE
  #define TIME_LOCATION_SOURCE OFF
#endif

#if TIME_LOCATION_SOURCE != OFF
  #if TIME_LOCATION_SOURCE == SD3031
    #include "SD3031.h"
  #elif TIME_LOCATION_SOURCE == DS3231
    #include "DS3231.h"
  #elif TIME_LOCATION_SOURCE == DS3234
    #include "DS3234.h"
  #elif TIME_LOCATION_SOURCE == GPS
    #include "GPS.h"
  #elif TIME_LOCATION_SOURCE == NTP
    #include "NTP.h"
  #elif TIME_LOCATION_SOURCE == TEENSY
    #include "Teensy.h" 
  #else
    #error "Configuration (Config.h): TIME_LOCATION_SOURCE unknown"
  #endif
#endif
