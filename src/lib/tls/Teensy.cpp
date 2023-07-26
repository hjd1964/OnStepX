// -----------------------------------------------------------------------------------
// Time/Location source TEENSY 3.2 RTC support

#include "Teensy.h"

#if defined(TIME_LOCATION_SOURCE) && TIME_LOCATION_SOURCE == TEENSY

#include <TimeLib.h> // https://github.com/PaulStoffregen/Time/archive/master.zip

bool TimeLocationSource::init() {
  ready = true;
  return ready;
}

void TimeLocationSource::set(JulianDate ut1) {
  GregorianDate greg = calendars.julianDayToGregorian(ut1);
  greg.year -= 2000;
  if (greg.year >= 100) greg.year -= 100;

  double f1 = fabs(ut1.hour);
  int h = floor(f1);
  double m = (f1 - h)*60.0;
  double s = (m - floor(m))*60.0;

  set(h, floor(m), floor(s), greg.day, greg.month, greg.year);
}

void TimeLocationSource::set(int year, int month, int day, int hour, int minute, int second) {
  setTime(hour, minute, second, day, month, year);
  unsigned long TeensyTime = now();              // get time in epoch

  // due to broken Teensy4.x RTC libraries, no point calling this as all it does is corrupt the possibly good time
  #if !defined(ARDUINO_TEENSY40) && !defined(ARDUINO_TEENSY41)
    Teensy3Clock.set(TeensyTime);                // set Teensy time
  #endif
}

void TimeLocationSource::get(JulianDate &ut1) {
  unsigned long TeensyTime = Teensy3Clock.get(); // get time from Teensy RTC

  // due to broken Teensy4.x RTC libraries, apply clock skew on read (where we have control) instead of write as typically done
  // this allows "correcting" local time (as set during Sketch upload) into UTC
  setTime(TeensyTime - TLS_CLOCK_SKEW*3600);     // set system time

  if (year() >= 0 && year() <= 3000 && month() >= 1 && month() <= 12 && day() >= 1 && day() <= 31 &&
      hour() <= 23 && minute() <= 59 && second() <= 59) {
    GregorianDate greg; greg.year = year(); greg.month = month(); greg.day = day();
    ut1 = calendars.gregorianToJulianDay(greg);
    ut1.hour = hour() + minute()/60.0 + second()/3600.0;
  }
}

TimeLocationSource tls;

#endif