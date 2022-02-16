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
  double   hour;
  bool     valid;
} GregorianDate;

#define JulianDateSize 16
typedef struct JulianDate {
  double   day;
  double   hour;
} JulianDate;

class Calendars {
  public:
    // convert from Gregorian to Julian date/time
    JulianDate gregorianToJulian(GregorianDate date);

    // convert from Julian to Gregorian date/time
    GregorianDate julianToGregorian(JulianDate julianDate);

    // convert from Gregorian to Julian date (does not handle hours)
    JulianDate gregorianToJulianDay(GregorianDate date);

    // convert from Julian to Gregorian date (does not handle hours)
    GregorianDate julianDayToGregorian(JulianDate julianDate);
  
  private:
};

extern Calendars calendars;

#endif
