// -----------------------------------------------------------------------------------
// Gregorian and Julian calendars
#pragma once

#include <Arduino.h>

#define CALENDARS_PRESENT

#ifdef CALENDARS_PRESENT

typedef struct GregorianDate {
  int16_t  year;
  uint8_t  month;
  uint8_t  day;
  uint8_t  hour;
  uint8_t  minute;
  uint8_t  second;
  long     fracsec;
  bool     valid;
} GregorianDate;

#define JulianDateSize 16
typedef struct JulianDate {
  double   day;
  double   hour;
} JulianDate;

class Calendars {
  public:
    // convert from Gregorian date to Julian Day (does not handle hours)
    JulianDate gregorianToJulianDay(GregorianDate date);

    // convert from Julian Day to Gregorian date (does not handle hours)
    GregorianDate julianDayToGregorian(JulianDate julianDate);
  
  private:
};

extern Calendars calendars;

#endif
