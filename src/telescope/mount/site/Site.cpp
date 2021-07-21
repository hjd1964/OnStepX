//--------------------------------------------------------------------------------------------------
// telescope mount, time and location

#include "Site.h"

#ifdef MOUNT_PRESENT

#include "../../../tasks/OnTask.h"
extern Tasks tasks;
#include "../../Telescope.h"

// base clock period (in 1/16us units per second) for adjusting the length of a sidereal second and all timing in OnStepX
unsigned long periodSubMicros;

// centisecond sidereal clock
volatile unsigned long centisecondLAST;
IRAM_ATTR void clockTickWrapper() { centisecondLAST++; }

#if TIME_LOCATION_SOURCE == GPS
  void gpsCheck() {
    if (tls.ready) {
      VF("MSG: Tls_GPS, setting site from GPS");
      double latitude, longitude;
      float elevation;
      tls.getSite(latitude, longitude, elevation);
      site.location.latitude = degToRad(latitude);
      site.location.longitude = degToRad(longitude);
      site.location.elevation = degToRad(elevation);
      strcpy(site.location.name, "GPS");
      site.updateLocation();

      VF("MSG: Tls_GPS, setting date/time from GPS");
      JulianDate jd;
      tls.get(jd);
      site.setDateTime(jd);

      VF("MSG: Tls_GPS, stopping GPS polling task.");
      tasks.remove(tasks.getHandleByName("gpsPoll"));
      tls.ready = false;
    }
  }
#endif

void Site::init(bool validKey) {
  // get location
  VLF("MSG: Site, get Latitude/Longitude from NV");
  readLocation(number, validKey);
  updateLocation();

  // get date/time from the RTC/GPS or NV
  #if TIME_LOCATION_SOURCE != OFF
    initError.tls = !tls.init();
    if (!initError.tls) {
      #if TIME_LOCATION_SOURCE != GPS
        tls.get(ut1);
        dateIsReady = true;
        timeIsReady = true;
        VLF("MSG: Site, get Date/Time from TLS");
      #else
        VLF("MSG: Site, using Date/Time from NV");
        readJD(validKey);
        VF("MSG: Transform, start GPS check task (rate 5000ms priority 7)... ");
        if (tasks.add(5000, 0, true, 7, gpsCheck, "gpsChk")) { VL("success"); } else { VL("FAILED!"); }
      #endif
    } else {
      DLF("WRN: Site::init(); Warning TLS initialization failed");
      VLF("WRN: Site, fallback to Date/Time from NV");
      readJD(validKey);
    }
  #else
    VLF("MSG: Site, get Date/Time from NV");
    readJD(validKey);
  #endif

  setSiderealTime(ut1);

  VF("MSG: Site, start centisecond timer task (rate 10ms priority 0)... ");
  delay(1000);
  // period ms (0=idle), duration ms (0=forever), repeat, priority (highest 0..7 lowest), task_handle
  handle = tasks.add(0, 0, true, 0, clockTickWrapper, "ClkTick");
  if (handle) {
    VL("success"); 
    if (!tasks.requestHardwareTimer(handle, 3, 1)) { DLF("WRN: Site::init(); Warning didn't get h/w timer for Clock (using s/w timer)"); }
  } else { VL("FAILED!"); }

  // period = nv.readLong(EE_siderealPeriod);
  setPeriodSubMicros(SIDEREAL_PERIOD);
}

void Site::updateLocation() {
  locationEx.latitude.cosine = cos(location.latitude);
  locationEx.latitude.sine   = sin(location.latitude);
  locationEx.latitude.absval = fabs(location.latitude);
  if (location.latitude >= 0.0) locationEx.latitude.sign = 1.0; else locationEx.latitude.sign = -1.0;

  // same date and time, just calculates the sidereal time again
  ut1.hour = getTime();
  setSiderealTime(ut1);
}

void Site::setPeriodSubMicros(unsigned long period) {
  tasks.setPeriodSubMicros(handle, lroundf(period/100.0F));
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

// sets the Julian Date (UT1,) also updates the site and sidereal time
void Site::setDateTime(JulianDate julianDate) {
  ut1 = julianDate;
  dateIsReady = true;
  timeIsReady = true;
  setSiderealTime(julianDate);
}

// sets the UT time (in hours) that have passed in this Julian Day
void Site::setSiderealTime(JulianDate julianDate) {
  setLAST(julianDate, julianDateToLAST(julianDate));
}

double Site::getSiderealTime() {
  long cs;
  noInterrupts();
  cs = centisecondLAST;
  interrupts();
  return backInHours(csToHours(cs));
}

void Site::setLAST(JulianDate julianDate, double time) {
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
  // DL("ST 2"); delay(100);
  return backInHours(gast - radToHrs(location.longitude));
}

double Site::julianDateToGAST(JulianDate julianDate) {
  GregorianDate date;

  date = calendars.julianDayToGregorian(julianDate);
  date.hour = 0; date.minute = 0; date.second = 0; date.centisecond = 0;
  JulianDate julianDay0 = calendars.gregorianToJulianDay(date);
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

void Site::readLocation(uint8_t locationNumber, bool validKey) {
  if (LocationSize < sizeof(Location)) { initError.nv = true; DL("ERR: Site::readLocation(); LocationSize error NV subsystem writes disabled"); nv.readOnly(true); }
  if (!validKey) {
    VLF("MSG: Site, writing default sites 0-3 to NV");
    location.latitude = 0.0;
    location.longitude = 0.0;
    location.timezone = 0.0;
    strcpy(location.name, "");
    for (uint8_t l = 0; l < 4; l++) nv.updateBytes(NV_SITE_BASE + l*LocationSize, &location, LocationSize);
  }
  number = locationNumber;
  nv.readBytes(NV_SITE_BASE + number*LocationSize, &location, LocationSize);
  if (location.latitude < -Deg90 || location.latitude > Deg90) { location.latitude = 0.0; initError.value = true; DLF("ERR: Site::readSite, bad NV latitude"); }
  if (location.longitude < -Deg360 || location.longitude > Deg360) { location.longitude = 0.0; initError.value = true; DLF("ERR: Site::readSite, bad NV longitude"); }
  if (location.timezone < -14 || location.timezone > 12) { location.timezone = 0.0; initError.value = true; DLF("ERR: Site::readSite,  bad NV timeZone"); }
}

void Site::readJD(bool validKey) {
  if (JulianDateSize < sizeof(ut1)) { initError.nv = true; DL("ERR: Site::readJD(); JulianDateSize error NV subsystem writes disabled"); nv.readOnly(true); }
  if (!validKey) {
    VLF("MSG: Site, writing default date/time to NV");
    ut1.day = 2451544.5;
    ut1.hour = 0.0;
    nv.updateBytes(NV_SITE_JD_BASE, &ut1, JulianDateSize);
  }
  nv.readBytes(NV_SITE_JD_BASE, &ut1, JulianDateSize);
  if (ut1.day < 2451544.5 || ut1.day > 2816787.5) { ut1.day = 2451544.5; initError.value = true; DLF("ERR: Site::readJD(); bad NV julian date (day)"); }
  if (ut1.hour < 0 || ut1.hour > 24.0)  { ut1.hour = 0.0; initError.value = true; DLF("ERR: Site::readJD(); bad NV julian date (hour)"); }
}

bool Site::setElevation(float e) {
 if (e >= -100.0 && e < 20000.0) location.elevation = e; else return false;
 return true;
}

// convert string in format MM/DD/YY to Date
GregorianDate Site::strToDate(char *ymd) {
  GregorianDate date;
  date.valid = false;
  char m[3], d[3], y[3];

  if (strlen(ymd) !=  8) return date;
  m[0] = *ymd++; m[1] = *ymd++; m[2] = 0;
  if (!convert.atoi2(m, &date.month, false)) return date;
  if (*ymd++ != '/') return date;
  d[0] = *ymd++; d[1] = *ymd++; d[2]=0;
  if (!convert.atoi2(d, &date.day, false)) return date;
  if (*ymd++ != '/') return date;
  y[0] = *ymd++; y[1] = *ymd++; y[2]=0;
  if (!convert.atoi2(y, &date.year, false)) return date;

  if (date.month < 1 || date.month > 12 || date.day < 1 || date.day > 31 || date.year < 0 || date.year > 99) return date;
  if (date.year > 20) date.year += 2000; else date.year += 2100;

  date.valid = true;
  return date;
}

Site site;

#endif
