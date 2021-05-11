//--------------------------------------------------------------------------------------------------
// telescope mount, time and location
#include "../OnStepX.h"
#include "../lib/nv/NV.h"
extern NVS nv;
#include "../tasks/OnTask.h"
extern Tasks tasks;

#include "../telescope/Telescope.h"
#include "Site.h"

// base clock period (in 1/16us units per second) for adjusting the length of a sidereal second and all timing in OnStepX
unsigned long periodSubMicros;

// centisecond sidereal clock
volatile unsigned long centisecondLAST;
IRAM_ATTR void clockTickWrapper() { centisecondLAST++; }

void Site::init(bool validKey) {
  // get location
  VLF("MSG: Site, get Latitude/Longitude");
  readLocation(number, validKey);
  update();

  // get date/time
  VLF("MSG: Site, get Date/Time");
  readJD(validKey);
  setTime(ut1);

  VF("MSG: Site, start centisecond timer task (rate 10ms priority 0)... ");
  delay(1000);
  // period ms (0=idle), duration ms (0=forever), repeat, priority (highest 0..7 lowest), task_handle
  handle = tasks.add(0, 0, true, 0, clockTickWrapper, "ClkTick");
  if (handle) {
    VL("success"); 
    if (!tasks.requestHardwareTimer(handle, 3, 1)) DLF("WRN: Site::init(); Warning didn't get h/w timer for Clock (using s/w timer)");
  } else VL("FAILED!");

  // period = nv.readLong(EE_siderealPeriod);
  setPeriodSubMicros(SIDEREAL_PERIOD);
}

void Site::update() {
  locationEx.latitude.cosine = cos(location.latitude);
  locationEx.latitude.sine   = sin(location.latitude);
  locationEx.latitude.absval = fabs(location.latitude);
  if (location.latitude >= 0.0) locationEx.latitude.sign = 1.0; else locationEx.latitude.sign = -1.0;

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
  long cs = lround(hoursToCs(time));
  centisecondHOUR = julianDate.hour;
  centisecondSTART = cs;
  noInterrupts();
  centisecondLAST = cs;
  interrupts();
}

bool Site::dateTimeReady() {
  return dateIsReady && timeIsReady;
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

  return date;
}

void Site::readLocation(uint8_t locationNumber, bool validKey) {
  if (LocationSize < sizeof(Location)) { initError.site = true; DL("ERR: Site::readLocation(); LocationSize error NV subsystem writes disabled"); nv.readOnly(true); }
  if (!validKey) {
    VLF("MSG: Site, writing default sites 0-3 to NV");
    location.latitude = 0.0;
    location.longitude = 0.0;
    location.timezone = 0.0;
    strcpy(location.name, "");
    for (uint8_t l = 0; l < 4; l++) nv.updateBytes(NV_LOCATION_BASE + l*LocationSize, &location, LocationSize);
  }
  number = locationNumber;
  nv.readBytes(NV_LOCATION_BASE + number*LocationSize, &location, LocationSize);
  if (location.latitude < -Deg90 || location.latitude > Deg90) { location.latitude = 0.0; initError.site = true; DLF("ERR: Site::readSite, bad NV latitude"); }
  if (location.longitude < -Deg360 || location.longitude > Deg360) { location.longitude = 0.0; initError.site = true; DLF("ERR: Site::readSite, bad NV longitude"); }
  if (location.timezone < -14 || location.timezone > 12) { location.timezone = 0.0; initError.site = true; DLF("ERR: Site::readSite,  bad NV timeZone"); }
}

void Site::readJD(bool validKey) {
  if (JulianDateSize < sizeof(ut1)) { initError.site = true; DL("ERR: Site::readJD(); JulianDateSize error NV subsystem writes disabled"); nv.readOnly(true); }
  if (!validKey) {
    VLF("MSG: Site, writing default date/time to NV");
    ut1.day = 2451544.5;
    ut1.hour = 0.0;
    nv.updateBytes(NV_JD_BASE, &ut1, JulianDateSize);
  }
  nv.readBytes(NV_JD_BASE, &ut1, JulianDateSize);
  if (ut1.day < 2451544.5 || ut1.day > 2816787.5) { ut1.day = 2451544.5; initError.site = true; DLF("ERR: Site::readJD(); bad NV julian date (day)"); }
  if (ut1.hour < 0 || ut1.hour > 24.0)  { ut1.hour = 0.0; initError.site = true; DLF("ERR: Site::readJD(); bad NV julian date (hour)"); }
}
