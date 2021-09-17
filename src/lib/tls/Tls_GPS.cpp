// -----------------------------------------------------------------------------------
// Time/Location source GPS support
// uses the specified serial port

#include "../../Common.h"

#if TIME_LOCATION_SOURCE == GPS

#ifndef SERIAL_GPS
  #error "SERIAL_GPS must be set to the serial port object if TIME_LOCATION_SOURCE GPS is used"
#endif

#ifndef SERIAL_GPS_BAUD
  #error "SERIAL_GPS_BAUD must be set to the baud rate if TIME_LOCATION_SOURCE GPS is used"
#endif

#include "PPS.h"
#include "../../telescope/Telescope.h"
#include "../../tasks/OnTask.h"

#include <TinyGPS++.h> // http://arduiniana.org/libraries/tinygpsplus/
TinyGPSPlus gps;

#include "Tls_GPS.h"

// provide for using software serial
#if SERIAL_GPS == SoftSerial
  #include <SoftwareSerial.h>
  #undef SoftSerial
  #define SERIAL_GPS_NO_RXTX_INIT
  SoftwareSerial SoftSerial(SERIAL_GPS_RX, SERIAL_GPS_TX);
#endif

void gpsPoll() {
  #if TIME_LOCATION_PPS_SENSE == ON
    if (pps.synced) {
  #endif

  if (!tls.active && tls.poll()) {
    SERIAL_GPS.end();
    initError.tls = false;
  }

  #if TIME_LOCATION_PPS_SENSE == ON
    }
  #endif
}

// initialize
bool TimeLocationSource::init() {
  #if defined(SERIAL_GPS_RX) && defined(SERIAL_GPS_TX) && !defined(SERIAL_GPS_NO_RXTX_INIT)
    SERIAL_GPS.begin(SERIAL_GPS_BAUD, SERIAL_8N1, SERIAL_GPS_RX, SERIAL_GPS_TX);
  #else
    SERIAL_GPS.begin(SERIAL_GPS_BAUD);
  #endif

  VF("MSG: Tls_GPS, start GPS poll task (rate 10ms priority 7)... ");
  if (tasks.add(10, GPS_TIMEOUT_MINUTES*60000UL, true, 7, gpsPoll, "gpsPoll")) { VL("success"); } else { VL("FAILED!"); }

  startTime = millis();
  active = false;
  ready = false;
  return active;
}

void TimeLocationSource::set(JulianDate ut1) {
  ut1 = ut1;
}

void TimeLocationSource::get(JulianDate &ut1) {
  if (!active) return;
  if (!timeIsValid()) return;

  GregorianDate greg; greg.year = gps.date.year(); greg.month = gps.date.month(); greg.day = gps.date.day();
  ut1 = calendars.gregorianToJulianDay(greg);
  ut1.hour = gps.time.hour() + gps.time.minute()/60.0 + gps.time.second()/3600.0;
}

void TimeLocationSource::getSite(double &latitude, double &longitude, float &elevation) {
  if (!active) return;
  if (!siteIsValid()) return;

  latitude = gps.location.lat();
  longitude = -gps.location.lng();
  elevation = gps.altitude.meters();
}

bool TimeLocationSource::poll() {
  if (gps.location.isValid() && siteIsValid() && gps.date.isValid() && gps.time.isValid() && timeIsValid() && waitIsValid()) {
    active = true;
    ready = true;
    return true;
  }
  while (SERIAL_GPS.available() > 0) gps.encode(SERIAL_GPS.read());
  return false;
}

bool TimeLocationSource::waitIsValid() {
  unsigned long t = millis() - startTime;
  return (t/1000UL)/3600UL >= GPS_MIN_WAIT_MINUTES;
}

bool TimeLocationSource::timeIsValid() {
  if (gps.date.year() <= 3000 && gps.date.month() >= 1 && gps.date.month() <= 12 && gps.date.day() >= 1 && gps.date.day() <= 31 &&
      gps.time.hour() <= 23 && gps.time.minute() <= 59 && gps.time.second() <= 59) return true; else return false;
}

bool TimeLocationSource::siteIsValid() {
  if (gps.location.lat() >= -90 && gps.location.lat() <= 90 && gps.location.lng() >= -360 && gps.location.lng() <= 360) return true; else return false;
}

TimeLocationSource tls;

#endif
