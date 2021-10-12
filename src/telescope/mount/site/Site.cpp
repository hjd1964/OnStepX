//--------------------------------------------------------------------------------------------------
// telescope mount, time and location

#include "Site.h"

#ifdef MOUNT_PRESENT

#include "../../../lib/tasks/OnTask.h"

#include "../../Telescope.h"
#include "../park/Park.h"

// fractional second sidereal clock (fracsec or millisecond)
volatile unsigned long fracLAST;
IRAM_ATTR void clockTickWrapper() { fracLAST++; }

#define fsToHours(x) ((x)/(3600.0*FRACTIONAL_SEC))
#define hoursToFs(x) ((x)*(3600.0*FRACTIONAL_SEC))
#define fsToDays(x)  ((x)/(86400.0*FRACTIONAL_SEC))
#define daysToFs(x)  ((x)*(86400.0*FRACTIONAL_SEC))

#if TIME_LOCATION_SOURCE == GPS
  void gpsCheck() {
    if (tls.isReady()) {
      VLF("MSG: Mount, setting site from GPS");
      double latitude, longitude;
      float elevation;
      tls.getSite(latitude, longitude, elevation);
      site.location.latitude = degToRad(latitude);
      site.location.longitude = degToRad(longitude);
      site.location.elevation = degToRad(elevation);
      strcpy(site.location.name, "GPS");
      site.updateLocation();

      VLF("MSG: Mount, setting date/time from GPS");
      JulianDate jd;
      tls.get(jd);
      site.setDateTime(jd);
      #if SLEW_GOTO == ON
        if (park.state == PS_PARKED) park.restore(false);
      #endif

      VLF("MSG: Mount, stopping GPS monitor task");
      tasks.setDurationComplete(tasks.getHandleByName("gpsChk"));
    } else

    if ((long)(millis() - site.updateTimeoutTime) > 0) {
      VLF("MSG: Mount, GPS timed out stopping monitor task");
      tasks.setDurationComplete(tasks.getHandleByName("gpsChk"));
      VLF("WRN: TLS, GPS timed out stopping monitor task");
      tasks.setDurationComplete(tasks.getHandleByName("gpsPoll"));
      initError.tls = true; 
    }
  }
#endif

void Site::init() {
  // get location
  VLF("MSG: Mount, site get Latitude/Longitude from NV");
  readLocation(number, validKey);
  updateLocation();

  // get date/time from the RTC/GPS or NV
  #if TIME_LOCATION_SOURCE != OFF
    initError.tls = !tls.init();
    if (!initError.tls) {
      if (tls.isReady()) {
        tls.get(ut1);
        dateIsReady = true;
        timeIsReady = true;
        VLF("MSG: Mount, site get Date/Time from TLS");
      } else {
        VLF("MSG: Site, falling back to Date/Time from NV");
        readJD(validKey);
        #if TIME_LOCATION_SOURCE == GPS
          updateTimeoutTime = millis() + GPS_TIMEOUT_MINUTES*60000UL;
          VF("MSG: Transform, start GPS check task (rate 5000ms priority 7)... ");
          if (tasks.add(5000, 0, true, 7, gpsCheck, "gpsChk")) { VLF("success"); } else { VLF("FAILED!"); }
        #endif
      }
    } else {
      DLF("WRN: Site::init(); Warning TLS initialization failed");
      VLF("WRN: Site::init(); fallback to last Date/Time from NV");
      readJD(validKey);
    }
  #else
    VLF("MSG: Site, get Date/Time from NV");
    readJD(validKey);
  #endif

  setSiderealTime(ut1);

  VF("MSG: Mount, site start sidereal timer task (rate 10ms priority 0)... ");
  delay(1000);
  // period ms (0=idle), duration ms (0=forever), repeat, priority (highest 0..7 lowest), task_handle
  handle = tasks.add(0, 0, true, 0, clockTickWrapper, "ClkTick");
  if (handle) {
    VLF("success"); 
    if (!tasks.requestHardwareTimer(handle, 3, 1)) { DLF("WRN: Site::init(); didn't get h/w timer for Clock (using s/w timer)"); }
  } else { VLF("FAILED!"); }

  setSiderealPeriod(SIDEREAL_PERIOD);
}

// update/apply the site latitude and longitude, necessary for LAST calculations etc.
void Site::updateLocation() {
  locationEx.latitude.cosine = cos(location.latitude);
  locationEx.latitude.sine   = sin(location.latitude);
  locationEx.latitude.absval = fabs(location.latitude);
  if (location.latitude >= 0.0) locationEx.latitude.sign = 1.0; else locationEx.latitude.sign = -1.0;

  // same date and time, just calculates the sidereal time again
  ut1.hour = getTime();
  setSiderealTime(ut1);
}

// update the initError status and restore the park position if necessary
void Site::updateTlsStatus() {
  if (initError.tls && dateIsReady && timeIsReady) {
    initError.tls = false;
    #if SLEW_GOTO == ON
      if (park.state == PS_PARKED) park.restore(false);
    #endif
  }
}

// sets the Julian Date/time (UT1,) and updates sidereal time
void Site::setDateTime(JulianDate julianDate) {
  ut1 = julianDate;
  dateIsReady = true;
  timeIsReady = true;
  setSiderealTime(julianDate);
}

// gets the time in sidereal hours
double Site::getSiderealTime() {
  long fs;
  noInterrupts();
  fs = fracLAST;
  interrupts();
  return backInHours(fsToHours(fs));
}

// sets the UT time (in hours) that have passed in this Julian Day
void Site::setSiderealTime(JulianDate julianDate) {
  setLAST(julianDate, julianDateToLAST(julianDate));
}

// checks if the date and time were set
bool Site::isDateTimeReady() {
  return dateIsReady && timeIsReady;
}

// gets sidereal period, in sub-micro counts per second
unsigned long Site::getSiderealPeriod() {
  return siderealPeriod;
}

// sets sidereal period, in sub-micro counts per second
void Site::setSiderealPeriod(unsigned long period) {
  siderealPeriod = period;
  tasks.setPeriodSubMicros(handle, lround(siderealPeriod/FRACTIONAL_SEC));
}

// gets the time in hours that have passed in this Julian Day
double Site::getTime() {
  unsigned long cs;
  noInterrupts();
  cs = fracLAST;
  interrupts();
  return fracHOUR + fsToHours((cs - fracSTART)/SIDEREAL_RATIO);
}

// sets the time in sidereal hours
void Site::setLAST(JulianDate julianDate, double time) {
  long fs = lround(hoursToFs(time));
  fracHOUR = julianDate.hour;
  fracSTART = fs;
  noInterrupts();
  fracLAST = fs;
  interrupts();
}

// convert julian date/time to local apparent sidereal time
double Site::julianDateToLAST(JulianDate julianDate) {
  // DL("ST 1"); delay(100);
  double gast = julianDateToGAST(julianDate);
  // DL("ST 2"); delay(100);
  return backInHours(gast - radToHrs(location.longitude));
}

// convert julian date/time to greenwich apparent sidereal time
double Site::julianDateToGAST(JulianDate julianDate) {
  GregorianDate date;

  date = calendars.julianDayToGregorian(julianDate);
  date.hour = 0; date.minute = 0; date.second = 0; date.fracsec = 0;
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

// reads the julian date information from NV
void Site::readJD(bool validKey) {
  if (JulianDateSize < sizeof(ut1)) { initError.nv = true; DL("ERR: Site::readJD(); JulianDateSize error NV subsystem writes disabled"); nv.readOnly(true); }
  if (!validKey) {
    VLF("MSG: Mount, site writing default date/time to NV");
    ut1.day = 2451544.5;
    ut1.hour = 0.0;
    nv.updateBytes(NV_SITE_JD_BASE, &ut1, JulianDateSize);
  }
  nv.readBytes(NV_SITE_JD_BASE, &ut1, JulianDateSize);
  if (ut1.day < 2451544.5 || ut1.day > 2816787.5) { ut1.day = 2451544.5; initError.value = true; DLF("ERR: Site::readJD(); bad NV julian date (day)"); }
  if (ut1.hour < 0 || ut1.hour > 24.0)  { ut1.hour = 0.0; initError.value = true; DLF("ERR: Site::readJD(); bad NV julian date (hour)"); }
}

// reads the location information from NV
// locationNumber can be 0..3
void Site::readLocation(uint8_t locationNumber, bool validKey) {
  if (LocationSize < sizeof(Location)) { initError.nv = true; DL("ERR: Site::readLocation(); LocationSize error NV subsystem writes disabled"); nv.readOnly(true); }
  if (!validKey) {
    VLF("MSG: Mount, site writing default sites 0-3 to NV");
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

// sets the site altitude in meters
bool Site::setElevation(float e) {
 if (e >= -100.0 && e < 20000.0) location.elevation = e; else return false;
 return true;
}

// adjust time (hours) into the 0 to 24 range
double Site::backInHours(double time) {
  while (time >= 24.0) time -= 24.0;
  while (time < 0.0)   time += 24.0;
  return time;
}

// adjust time (hours) into the -12 to 12 range
double Site::backInHourAngle(double time) {
  while (time >= 12.0) time -= 24.0;
  while (time < -12.0) time += 24.0;
  return time;
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
