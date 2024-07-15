// -----------------------------------------------------------------------------------
// Time/Location source DS3234 RTC support
// uses the default SPI port
#pragma once

#include "../../../Common.h"

#if defined(TIME_LOCATION_SOURCE) && TIME_LOCATION_SOURCE == DS3234 || \
    (defined(TIME_LOCATION_SOURCE_FALLBACK) && TIME_LOCATION_SOURCE_FALLBACK == DS3234)

#include "../TLS.h"

#if !defined(DS3234_CS_PIN) || DS3234_CS_PIN == OFF
  #error "Configuration (Config.h): DS3234_CS_PIN must be defined for TIME_LOCATION_SOURCE DS3234"
#endif

#ifndef TLS_CLOCK_SKEW
#define TLS_CLOCK_SKEW 0.000139 // +5 seconds ahead when setting time
#endif

class TlsDs3234 : public TimeLocationSource {
  public:
    // initialize (also enables the RTC PPS if available)
    bool init();

    // set the RTC's time
    void set(JulianDate ut1);
    void set(int year, int month, int day, int hour, int minute, int second);

    // get the RTC's time
    bool get(JulianDate &ut1);

    // not used, date/time is stored as UT1
    double DUT1 = 0.0L;

  private:
};

#endif