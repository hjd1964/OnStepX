// -----------------------------------------------------------------------------------
// Gregorian and Julian calendars

#include "Calendars.h"

#ifdef CALENDARS_PRESENT

// convert from Gregorian date to Julian Day (does not handle hours)
JulianDate Calendars::gregorianToJulianDay(GregorianDate date) {
  JulianDate julianDay;
  
  int y = date.year;
  int m = date.month;
  if (m == 1 || m == 2) { y--; m += 12; }
  double B = 2.0 - floor(y/100.0) + floor(y/400.0);
  julianDay.day = B + floor(365.25*y) + floor(30.6001*(m + 1.0)) + date.day + 1720994.5;
  julianDay.hour = 0;

  return julianDay;
}

// convert from Julian Day to Gregorian date (does not handle hours)
GregorianDate Calendars::julianDayToGregorian(JulianDate julianDate) {
  double A, B, C, D, D1, E, F, G, I;
  GregorianDate date;
  
  I = floor(julianDate.day + 0.5);
 
  F = 0.0;
  if (I > 2299160.0) {
    A = int((I - 1867216.25)/36524.25);
    B = I + 1.0 + A - floor(A/4.0);
  } else B = I;

  C = B + 1524.0;
  D = floor((C - 122.1)/365.25);
  E = floor(365.25*D);
  G = floor((C - E)/30.6001);

  D1 = C - E + F - floor(30.6001*G);
  date.day = floor(D1);
  if (G < 13.5)         date.month = floor(G - 1.0);    else date.month = floor(G - 13.0);
  if (date.month > 2.5) date.year  = floor(D - 4716.0); else date.year  = floor(D - 4715.0);

  return date;
}

Calendars calendars;

#endif
