// -----------------------------------------------------------------------------------
// Time/Location source GPS support
// uses the specified serial port
#pragma once

#include "../../../Common.h"

#if defined(TIME_LOCATION_SOURCE) && TIME_LOCATION_SOURCE == GPS

#include "../TLS.h"

#ifndef GPS_TIMEOUT_MINUTES
  #define GPS_TIMEOUT_MINUTES 10 // wait up to 10 minutes to get lock, use 0 to disable timeout
#endif
#ifndef GPS_MIN_WAIT_MINUTES
  #define GPS_MIN_WAIT_MINUTES 2 // minimum wait for stabilization in minutes, use 0 to disable
#endif

class TlsGPS : public TimeLocationSource {
  public:
    // initialize (also enables the RTC PPS if available)
    // pass a hook to the tls you created so it can be called
    bool init();

    // set the RTC's time
    void set(JulianDate ut1);
    void set(int year, int month, int day, int hour, int minute, int second);

    // get the RTC's time
    bool get(JulianDate &ut1);

    // get the location
    void getSite(double &latitude, double &longitude, float &elevation);

    // update from GPS
    void poll();

    // for conversion from UTC to UT1
    double DUT1 = 0.0L;

  private:
    // validate wait time
    bool waitIsValid();

    // validate time
    bool timeIsValid();

    // validate site
    bool siteIsValid();

    unsigned long startTime = 0;

    bool active = false;
    bool activeLink = false;
};

#endif
