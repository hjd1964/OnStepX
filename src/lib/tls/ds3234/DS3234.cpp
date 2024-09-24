// -----------------------------------------------------------------------------------
// Time/Location source DS3234 RTC support
// uses the default SPI port

#include "DS3234.h"

#if defined(TIME_LOCATION_SOURCE) && TIME_LOCATION_SOURCE == DS3234 || \
    (defined(TIME_LOCATION_SOURCE_FALLBACK) && TIME_LOCATION_SOURCE_FALLBACK == DS3234)


#ifdef TLS_TIMELIB
  #include <TimeLib.h> // https://github.com/PaulStoffregen/Time/archive/master.zip
#endif

#include <SPI.h>
#include <RtcDS3234.h> // https://github.com/Makuna/Rtc/archive/master.zip
RtcDS3234<SPIClass> rtcDS3234(SPI, DS3234_CS_PIN);

bool TlsDs3234::init() {
  SPI.begin();
  rtcDS3234.Begin();
  if (!rtcDS3234.GetIsRunning()) rtcDS3234.SetIsRunning(true);

  // see if the RTC is present
  if (rtcDS3234.GetIsRunning()) {
    // frequency 0 (1Hz) on the SQW pin
    rtcDS3234.SetSquareWavePin(DS3234SquareWavePin_ModeClock);
    rtcDS3234.SetSquareWavePinClockFrequency(DS3234SquareWaveClock_1Hz);

    #ifdef TLS_TIMELIB
      RtcDateTime now = rtcDS3234.GetDateTime();
      setTime(now.Hour(), now.Minute(), now.Second(), now.Day(), now.Month(), now.Year());
    #endif

    ready = true;
  } else DLF("WRN: tls.init(), DS3234 GetIsRunning() false");
  #ifdef SSPI_SHARED
    SPI.end();
  #endif
  return ready;
}

void TlsDs3234::set(JulianDate ut1) {
  if (!ready) return;

  GregorianDate greg = calendars.julianDayToGregorian(ut1);

  double f1 = fabs(ut1.hour) + TLS_CLOCK_SKEW;
  int h = floor(f1);
  double m = (f1 - h)*60.0;
  double s = (m - floor(m))*60.0;

  set(greg.year, greg.month, greg.day, h, floor(m), floor(s));
}

void TlsDs3234::set(int year, int month, int day, int hour, int minute, int second) {
  #ifdef TLS_TIMELIB
    setTime(hour, minute, second, day, month, year);
  #endif
  #ifdef SSPI_SHARED
    SPI.begin();
  #endif
  RtcDateTime updateTime = RtcDateTime(year, month, day, hour, minute, second);
  rtcDS3234.SetDateTime(updateTime);
  #ifdef SSPI_SHARED
    SPI.end();
  #endif
}

bool TlsDs3234::get(JulianDate &ut1) {
  if (!ready) return false;

  #ifdef SSPI_SHARED
    SPI.begin();
  #endif
  GregorianDate greg;
  RtcDateTime now = rtcDS3234.GetDateTime();
  if (now.Year() >= 2018 && now.Year() <= 3000 && now.Month() >= 1 && now.Month() <= 12 && now.Day() >= 1 && now.Day() <= 31 &&
      now.Hour() <= 23 && now.Minute() <= 59 && now.Second() <= 59) {
    greg.year = now.Year();
    greg.month = now.Month();
    greg.day = now.Day();
    ut1 = calendars.gregorianToJulianDay(greg);
    ut1.hour = now.Hour() + now.Minute()/60.0 + now.Second()/3600.0;
  } else {
    greg.year = 2000;
    greg.month = 1;
    greg.day = 1;
    ut1 = calendars.gregorianToJulianDay(greg);
    ut1.hour = 0.0;
  }
  #ifdef SSPI_SHARED
    SPI.end();
  #endif

  return true;
}

#endif