// -----------------------------------------------------------------------------------
// Time/Location source DS3231 RTC support
// uses the default I2C port in most cases; though HAL_Wire can redirect to another port (as is done for the Teensy3.5/3.6)

#include "../../Common.h"

#if TIME_LOCATION_SOURCE == DS3231

#include <Wire.h>
#include <RtcDS3231.h> // https://github.com/Makuna/Rtc/archive/master.zip
RtcDS3231<TwoWire> _Rtc(HAL_Wire);

#include "Tls_DS3231.h"

bool TimeLocationSource::init() {
  HAL_Wire.begin();
  HAL_Wire.setClock(HAL_WIRE_CLOCK);
  HAL_Wire.beginTransmission(0x68);
  bool error = HAL_Wire.endTransmission() != 0;
  if (!error) {
    _Rtc.Begin();
    HAL_Wire.setClock(HAL_WIRE_CLOCK);
    if (!_Rtc.GetIsRunning()) _Rtc.SetIsRunning(true);

    // see if the RTC is present
    if (_Rtc.GetIsRunning()) {
      // frequency 0 (1Hz) on the SQW pin
      _Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeClock);
      _Rtc.SetSquareWavePinClockFrequency(DS3231SquareWaveClock_1Hz);
      active = true;
    } else { DLF("WRN, tls.init(): DS3231 GetIsRunning() false"); }
  } else { DLF("WRN, tls.init(): DS3231 not found at I2C address 0x68"); }
  #ifdef HAL_WIRE_RESET_AFTER_CONNECT
    HAL_Wire.end();
    HAL_Wire.begin();
    HAL_Wire.setClock(HAL_WIRE_CLOCK);
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

  RtcDateTime updateTime = RtcDateTime(greg.year, greg.month, greg.day, h, floor(m), floor(s));
  _Rtc.SetDateTime(updateTime);
}

void TimeLocationSource::get(JulianDate &ut1) {
  if (!active) return;

  RtcDateTime now = _Rtc.GetDateTime();
  if (now.Year() >= 2018 && now.Year() <= 3000 && now.Month() >= 1 && now.Month() <= 12 && now.Day() >= 1 && now.Day() <= 31 &&
      now.Hour() <= 23 && now.Minute() <= 59 && now.Second() <= 59) {
    GregorianDate greg; greg.year = now.Year(); greg.month = now.Month(); greg.day = now.Day();
    ut1 = calendars.gregorianToJulianDay(greg);
    ut1.hour = now.Hour() + now.Minute()/60.0 + now.Second()/3600.0;
  }
}

void TimeLocationSource::getSite(double &latitude, double &longitude) {
}

TimeLocationSource tls;

#endif
