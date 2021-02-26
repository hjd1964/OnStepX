//--------------------------------------------------------------------------------------------------
// telescope mount, time and location
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;

#include "Site.h"

volatile unsigned long centisecondLAST;
unsigned long periodSubMicros;
void clockTickWrapper() { centisecondLAST++; }

void Site::init() {
  // get location
  VLF("MSG: Site::init, get Latitude/Longitude");
  location.latitude.value  = degToRad(40.0);
  location.longitude       = degToRad(75.2);
  update();

  // get date/time
  VLF("MSG: Site::init, get Date/Time");
  GregorianDate date;
  date.year = 2021; date.month  = 2; date.day = 7;
  date.hour = 12;   date.minute = 0; date.second = 0; date.centisecond = 0;
  date.timezone = 5;

  ut1 = gregorianToJulianDay(date);
  ut1.hour = (date.hour + (date.minute + (date.second + date.centisecond/100.0)/60.0)/60.0) + date.timezone;
  setTime(ut1);

  // period ms (0=idle), duration ms (0=forever), repeat, priority (highest 0..7 lowest), task_handle
  handle = tasks.add(0, 0, true, 0, clockTickWrapper, "ClkTick");
  if (!tasks.requestHardwareTimer(handle, 3, 1)) VLF("MSG: Site::init, Warning didn't get h/w timer for Clock (using s/w timer)");

  // period = nv.readLong(EE_siderealPeriod);
  setPeriodSubMicros(SIDEREAL_PERIOD);
}

void Site::update() {
  location.latitude.cosine = cos(location.latitude.value);
  location.latitude.sine   = sin(location.latitude.value);
  location.latitude.absval = fabs(location.latitude.value);
  if (location.latitude.value >= 0.0) location.latitude.sign = 1.0; else location.latitude.sign = -1.0;

  // same date and time, just calculates the sidereal time again
  ut1.hour = getTime();
  setSiderealTime(ut1, julianDateToLAST(ut1));
}

void Site::setPeriodSubMicros(unsigned long period) {
  tasks.setPeriodSubMicros(handle, lround(period/100.0));
  this->period = period;
  periodSubMicros = period;
  // nv.writeLong(EE_siderealPeriod, period);
}

double Site::getTime() {
  unsigned long cs;
  noInterrupts();
  cs = centisecondLAST;
  interrupts();
  return centisecondHOUR + csToHours((cs - centisecondSTART)/SIDEREAL_RATIO);
}

void Site::setTime(JulianDate julianDate) {
  setSiderealTime(julianDate, julianDateToLAST(julianDate));
}

double Site::getSiderealTime() {
  long cs;
  noInterrupts();
  cs = centisecondLAST;
  interrupts();
  return backInHours(csToHours(cs));
}

void Site::setSiderealTime(JulianDate julianDate, double time) {
//  DL("ST3"); delay(100);
  long cs = lround(hoursToCs(time));
  centisecondHOUR = julianDate.hour;
  centisecondSTART = cs;
  noInterrupts();
  centisecondLAST = cs;
  interrupts();
}

double Site::backInHours(double time) {
  while (time >= 24.0) time -= 24.0;
  while (time < 0.0)   time += 24.0;
  return time;
}

double Site::backInHourAngle(double time) {
  while (time >= 12.0) time -= 24.0;
  while (time < -12.0) time += 24.0;
  return time;
}

double Site::julianDateToLAST(JulianDate julianDate) {
 // DL("ST 1"); delay(100);
  double gast = julianDateToGAST(julianDate);
//  DL("ST 2"); delay(100);
  return backInHours(gast - radToHrs(location.longitude));
}

double Site::julianDateToGAST(JulianDate julianDate) {
  GregorianDate date;

  date = julianDayToGregorian(julianDate);
  date.hour = 0; date.minute = 0; date.second = 0; date.centisecond = 0;
  JulianDate julianDay0 = gregorianToJulianDay(date);
  double D= (julianDate.day - 2451545.0) + julianDate.hour/24.0;
  double D0=(julianDay0.day - 2451545.0);
  double H = julianDate.hour;
  double T = D/36525.0;
  double gmst = 6.697374558 + 0.06570982441908*D0;
  gmst = gmst + SIDEREAL_RATIO*H + 0.000026*T*T;

  // equation of the equinoxes
  double O = 125.04  - 0.052954 *D;
  double L = 280.47  + 0.98565  *D;
  double E = 23.4393 - 0.0000004*D;
  double W = -0.000319*sin(degToRad(O)) - 0.000024*sin(degToRad(2*L));
  double eqeq = W*cos(degToRad(E));
  double gast = gmst + eqeq;

  return backInHours(gast);
}

JulianDate Site::gregorianToJulianDay(GregorianDate date) {
  JulianDate julianDay;
  
  int y = date.year;
  int m = date.month;
  if (m == 1 || m == 2) { y--; m += 12; }
  double B = 2.0 - floor(y/100.0) + floor(y/400.0);
  julianDay.day = B + floor(365.25*y) + floor(30.6001*(m + 1.0)) + date.day + 1720994.5;
  julianDay.hour = 0;
  julianDay.timezone = date.timezone;

  return julianDay;
}

GregorianDate Site::julianDayToGregorian(JulianDate julianDate) {
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
  date.timezone = julianDate.timezone;

  return date;
}
