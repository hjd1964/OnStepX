// -----------------------------------------------------------------------------------
// Time/Location source DS3231 RTC support
// uses the default I2C port in most cases; though HAL_Wire can redirect to another port (as is done for the Teensy3.5/3.6)

#include "DS3231.h"

#if (defined(TIME_LOCATION_SOURCE) && TIME_LOCATION_SOURCE == DS3231) || \
    (defined(TIME_LOCATION_SOURCE_FALLBACK) && TIME_LOCATION_SOURCE_FALLBACK == DS3231)

#ifdef TLS_TIMELIB
  #include <TimeLib.h> // https://github.com/PaulStoffregen/Time/archive/master.zip
#endif

#include <Wire.h>
#include <RtcDS3231.h> // https://github.com/Makuna/Rtc/archive/master.zip
RtcDS3231<TwoWire> rtcDS3231(HAL_Wire);

#include "../PPS.h"

bool TlsDs3231::init() {
  HAL_Wire.begin();
  #ifdef HAL_WIRE_CLOCK
    HAL_Wire.setClock(HAL_WIRE_CLOCK);
  #endif

  HAL_Wire.beginTransmission(0x68);
  bool error = HAL_Wire.endTransmission() != 0;
  if (!error) {
    rtcDS3231.Begin();
    #ifdef HAL_WIRE_CLOCK
      HAL_Wire.setClock(HAL_WIRE_CLOCK);
    #endif

    if (!rtcDS3231.GetIsRunning()) rtcDS3231.SetIsRunning(true);

    // see if the RTC is present
    if (rtcDS3231.GetIsRunning()) {
      #if TIME_LOCATION_SOURCE == DS3231
        ppsEnable();
      #endif

      #ifdef TLS_TIMELIB
        RtcDateTime now = rtcDS3231.GetDateTime();
        setTime(now.Hour(), now.Minute(), now.Second(), now.Day(), now.Month(), now.Year());
      #endif

      ready = true;
    } else { DLF("WRN: tls.init(), DS3231 GetIsRunning() false"); }
  } else { DLF("WRN: tls.init(), DS3231 (I2C 0x68) not found"); }
  #ifdef HAL_WIRE_RESET_AFTER_CONNECT
    HAL_Wire.end();
    HAL_Wire.begin();
    #ifdef HAL_WIRE_CLOCK
      HAL_Wire.setClock(HAL_WIRE_CLOCK);
    #endif
  #endif
  return ready;
}

void TlsDs3231::set(JulianDate ut1) {
  if (!ready) return;

  GregorianDate greg = calendars.julianDayToGregorian(ut1);

  double f1 = fabs(ut1.hour) + TLS_CLOCK_SKEW;
  int h = floor(f1);
  double m = (f1 - h)*60.0;
  double s = (m - floor(m))*60.0;

  set(greg.year, greg.month, greg.day, h, floor(m), floor(s));
}

void TlsDs3231::set(int year, int month, int day, int hour, int minute, int second) {
  #ifdef TLS_TIMELIB
    setTime(hour, minute, second, day, month, year);
  #endif
  RtcDateTime updateTime = RtcDateTime(year, month, day, hour, minute, second);
  rtcDS3231.SetDateTime(updateTime);
}

bool TlsDs3231::get(JulianDate &ut1) {
  if (!ready) return false;

  unsigned long t = 0.5;
  #if defined(TIME_LOCATION_PPS_SENSE) && (TIME_LOCATION_PPS_SENSE) != OFF
    // wait until we're roughly in the middle of a second
    if (pps.synced) {
      do { t = micros() - pps.lastMicros; } while (t < 400000 || t > 600000);
    }
  #endif

  GregorianDate greg;
  RtcDateTime now = rtcDS3231.GetDateTime();
  if (now.Year() >= 2018 && now.Year() <= 3000 && now.Month() >= 1 && now.Month() <= 12 && now.Day() >= 1 && now.Day() <= 31 &&
      now.Hour() <= 23 && now.Minute() <= 59 && now.Second() <= 59) {
    greg.year = now.Year();
    greg.month = now.Month();
    greg.day = now.Day();
    ut1 = calendars.gregorianToJulianDay(greg);
    ut1.hour = now.Hour() + now.Minute()/60.0 + (now.Second() + t/1000000.0)/3600.0;
  } else {
    greg.year = 2000;
    greg.month = 1;
    greg.day = 1;
    ut1 = calendars.gregorianToJulianDay(greg);
    ut1.hour = 0.0;
  }

  return true;
}

void TlsDs3231::ppsEnable() {
  // frequency 0 (1Hz) on the SQW pin
  rtcDS3231.SetSquareWavePin(DS3231SquareWavePin_ModeClock);
  rtcDS3231.SetSquareWavePinClockFrequency(DS3231SquareWaveClock_1Hz);
}

#endif
