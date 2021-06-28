// -----------------------------------------------------------------------------------
// Time/Location source DS3234 RTC support
// uses the default SPI port
#pragma once

#include "../../Common.h"

#if TIME_LOCATION_SOURCE == DS3234

#include "../calendars/Calendars.h"

#define TLS_CLOCK_SKEW 0.000139 // +5 seconds ahead when setting time

class TimeLocationSource {
  public:
    // initialize (also enables the RTC PPS if available)
    bool init();

    // set the RTC's time
    void set(JulianDate ut1);
    
    // get the RTC's time
    void get(JulianDate &ut1);

    // get the location (does nothing)
    void getSite(double &latitude, double &longitude);

    bool active = false;
};

extern TimeLocationSource tls;

#endif