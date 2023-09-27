// -----------------------------------------------------------------------------------
// Time/Location source SD3031 RTC support
// uses the default I2C port in most cases; though HAL_Wire can redirect to another port (as is done for the Teensy3.5/3.6)
#pragma once

#include "../../Common.h"

#if defined(TIME_LOCATION_SOURCE) && TIME_LOCATION_SOURCE == SD3031

#include "../calendars/Calendars.h"

#ifndef TLS_CLOCK_SKEW
#define TLS_CLOCK_SKEW 0.0 // in hours
#endif

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
