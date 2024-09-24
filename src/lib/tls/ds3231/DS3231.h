// -----------------------------------------------------------------------------------
// Time/Location source DS3231 RTC support
// uses the default I2C port in most cases; though HAL_Wire can redirect to another port (as is done for the Teensy3.5/3.6)
#pragma once

#include "../../../Common.h"

#if defined(TIME_LOCATION_SOURCE) && TIME_LOCATION_SOURCE == DS3231 || \
    (defined(TIME_LOCATION_SOURCE_FALLBACK) && TIME_LOCATION_SOURCE_FALLBACK == DS3231)

#include "../TLS.h"

#ifndef TLS_CLOCK_SKEW
#define TLS_CLOCK_SKEW 0.000139 // +5 seconds ahead when setting time
#endif

class TlsDs3231 : public TimeLocationSource {
  public:
    // initialize (also enables the RTC PPS if available)
    bool init();

    // set the RTC's time
    void set(JulianDate ut1);
    void set(int year, int month, int day, int hour, int minute, int second);

    // get the RTC's time
    bool get(JulianDate &ut1);

    // secondary way to enable PPS
    void ppsEnable();

    // not used, date/time is stored as UT1
    double DUT1 = 0.0L;

  private:
};

#endif
