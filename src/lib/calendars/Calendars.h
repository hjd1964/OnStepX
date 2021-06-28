// -----------------------------------------------------------------------------------
// Gregorian and Julian calendars
#pragma once

#include <Arduino.h>

typedef struct GregorianDate {
  int16_t  year;
  uint8_t  month;
  uint8_t  day;
  uint8_t  hour;
  uint8_t  minute;
  uint8_t  second;
  long     centisecond;
  bool     valid;
} GregorianDate;

#define JulianDateSize 16
typedef struct JulianDate {
  double   day;
  double   hour;
} JulianDate;

class Calendars {
  public:
    JulianDate gregorianToJulianDay(GregorianDate date);

    GregorianDate julianDayToGregorian(JulianDate julianDate);
  
  private:
};

extern Calendars calendars;
