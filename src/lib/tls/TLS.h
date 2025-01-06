// -----------------------------------------------------------------------------------
// Time/Location source
#pragma once

#include "../../Common.h"

#ifdef TIME_LOCATION_SOURCE

#include "../calendars/Calendars.h"

class TimeLocationSource {
  public:
    // initialize (also enables the RTC PPS if available)
    virtual bool init();

    // restart the device
    virtual void restart() {}

     // true if date/time ready
    inline bool isReady() { return ready; }

    // set the RTC's time
    virtual void set(JulianDate ut1);
    virtual void set(int year, int month, int day, int hour, int minute, int second);
    
    // get the RTC's time, returns true if current
    virtual bool get(JulianDate &ut1);

    // get the location
    virtual void getSite(double &latitude, double &longitude, float &elevation) { UNUSED(latitude); UNUSED(longitude); UNUSED(elevation); }

    // secondary way to enable PPS
    virtual void ppsEnable() {}

    // update from GPS or NTP
    virtual void poll() {}

    // not used, date/time is stored as UT1
    double DUT1 = 0.0L;

  protected:
    bool ready = false;
};

#endif
