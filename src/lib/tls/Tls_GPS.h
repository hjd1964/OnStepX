// -----------------------------------------------------------------------------------
// Time/Location source GPS support
// uses the specified serial port
#pragma once

#include "../../Common.h"

#if TIME_LOCATION_SOURCE == GPS

#include "../calendars/Calendars.h"

#ifndef GPS_TIMEOUT_MINUTES
  #define GPS_TIMEOUT_MINUTES 10 // wait up to 10 minutes to get lock, use 0 to disable timeout
#endif
#ifndef GPS_MIN_WAIT_MINUTES
  #define GPS_MIN_WAIT_MINUTES 2 // minimum wait for stabilization in minutes, use 0 to disable
#endif

class TimeLocationSource {
  public:
    // initialize (also enables the RTC PPS if available)
    bool init();

    // set the RTC's time
    void set(JulianDate ut1);
    
    // get the RTC's time
    void get(JulianDate &ut1);

    // get the location
    void getSite(double &latitude, double &longitude, float &elevation);

    // update from GPS
    bool poll();

    bool active = false;
    bool ready = false;

  private:
    // validate wait time
    bool waitIsValid();

    // validate time
    bool timeIsValid();

    // validate site
    bool siteIsValid();

    unsigned long startTime = 0;
};

extern TimeLocationSource tls;

#endif
