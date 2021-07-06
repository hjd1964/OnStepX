// -----------------------------------------------------------------------------------
// Time/Location source DS3234 RTC support
// uses the default SPI port

#include "../../Common.h"

#if TIME_LOCATION_SOURCE == DS3234

#include <SPI.h>
#include <RtcDS3234.h> // https://github.com/Makuna/Rtc/archive/master.zip
RtcDS3234<SPIClass> _Rtc(SPI, DS3234_CS_PIN);

#include "Tls_DS3234.h"

bool TimeLocationSource::init() {
  SPI.begin();
  _Rtc.Begin();
  if (!_Rtc.GetIsRunning()) _Rtc.SetIsRunning(true);

  // see if the RTC is present
  if (_Rtc.GetIsRunning()) {
    // frequency 0 (1Hz) on the SQW pin
    _Rtc.SetSquareWavePin(DS3234SquareWavePin_ModeClock);
    _Rtc.SetSquareWavePinClockFrequency(DS3234SquareWaveClock_1Hz);
    active = true;
  } else DLF("WRN, tls.init(): DS3234 GetIsRunning() false");
  #ifdef SSPI_SHARED
    SPI.end();
  #endif
  return active;
}

void TimeLocationSource::set(JulianDate ut1) {
  if (!active) return;

  GregorianDate greg = calendars.julianDayToGregorian(ut1);

  double f1 = fabs(ut1.hour) + TLS_CLOCK_SKEW;
  int h = floor(f1);
  double m = (f1 - h)*60.0;
  double s = (m - floor(m))*60.0;

  #ifdef SSPI_SHARED
    SPI.begin();
  #endif
  RtcDateTime updateTime = RtcDateTime(greg.year, greg.month, greg.day, h, floor(m), floor(s));
  _Rtc.SetDateTime(updateTime);
  #ifdef SSPI_SHARED
    SPI.end();
  #endif
}

void TimeLocationSource::get(JulianDate &ut1) {
  if (!active) return;

  #ifdef SSPI_SHARED
    SPI.begin();
  #endif
  RtcDateTime now = _Rtc.GetDateTime();
  if (now.Year() >= 2018 && now.Year() <= 3000 && now.Month() >= 1 && now.Month() <= 12 && now.Day() >= 1 && now.Day() <= 31 &&
      now.Hour() <= 23 && now.Minute() <= 59 && now.Second() <= 59) {
    GregorianDate greg; greg.year = now.Year(); greg.month = now.Month(); greg.day = now.Day();
    ut1 = calendars.gregorianToJulianDay(greg);
    ut1.hour = now.Hour() + now.Minute()/60.0 + now.Second()/3600.0;
  }
  #ifdef SSPI_SHARED
    SPI.end();
  #endif
}

TimeLocationSource tls;

#endif