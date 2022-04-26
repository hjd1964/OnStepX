// -----------------------------------------------------------------------------------
// Time/Location source TEENSY 3.2 RTC support
#pragma once

#include "../../Common.h"

#if defined(TIME_LOCATION_SOURCE) && TIME_LOCATION_SOURCE == TEENSY

#include "../calendars/Calendars.h"

#define TLS_CLOCK_SKEW 0.000139 // +5 seconds ahead when setting time

class TimeLocationSource {
  public:
    // initialize (also enables the RTC PPS if available)
    bool init();

     // true if date/time ready
    inline bool isReady() { return ready; }

    // set the RTC's time
    void set(JulianDate ut1);
    void set(int year, int month, int day, int hour, int minute, int second);
    
    // get the RTC's time
    void get(JulianDate &ut1);

    // not used, date/time is stored as UT1
    double DUT1 = 0.0L;

  private:
    bool ready = false;
};

extern TimeLocationSource tls;

#endif
