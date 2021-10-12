// -----------------------------------------------------------------------------------
// Time/Location source GPS support
// uses the specified serial port

#include "GPS.h"

#if defined(TIME_LOCATION_SOURCE) && TIME_LOCATION_SOURCE == GPS

#ifdef TLS_TIMELIB
  #include <TimeLib.h> // https://github.com/PaulStoffregen/Time/archive/master.zip
#endif

#ifndef SERIAL_GPS
  #error "SERIAL_GPS must be set to the serial port object if TIME_LOCATION_SOURCE GPS is used"
#endif

#ifndef SERIAL_GPS_BAUD
  #error "SERIAL_GPS_BAUD must be set to the baud rate if TIME_LOCATION_SOURCE GPS is used"
#endif

#if SERIAL_GPS == SoftSerial || SERIAL_GPS == HardSerial
  #ifndef SERIAL_GPS_RX
    #error "SERIAL_GPS_RX must be set to the serial port RX pin if SoftSerial or HardSerial is used"
  #endif
  #ifndef SERIAL_GPS_TX
    #error "SERIAL_GPS_TX must be set to the serial port TX pin if SoftSerial or HardSerial is used"
  #endif
#endif

#include "PPS.h"
#include "../../tasks/OnTask.h"

#include <TinyGPS++.h> // http://arduiniana.org/libraries/tinygpsplus/
TinyGPSPlus gps;

// provide for using software serial
#if SERIAL_GPS == SoftSerial
  #include <SoftwareSerial.h>
  #undef SERIAL_GPS
  SoftwareSerial SWSerialGPS(SERIAL_GPS_RX, SERIAL_GPS_TX);
  #define SERIAL_GPS SWSerialGPS
  #define SERIAL_GPS_RXTX_SET
#endif

// provide for using hardware serial
#if SERIAL_GPS == HardSerial
  #include <HardwareSerial.h>
  #undef SERIAL_GPS
  HardwareSerial HWSerialGPS(SERIAL_GPS_RX, SERIAL_GPS_TX);
  #define SERIAL_GPS HWSerialGPS
  #define SERIAL_GPS_RXTX_SET
#endif

void gpsPoll() {
  #if TIME_LOCATION_PPS_SENSE != OFF
    if (pps.synced) {
  #endif

  if (!tls.isReady()) tls.poll();

  #if TIME_LOCATION_PPS_SENSE != OFF
    }
  #endif
}

// initialize
bool TimeLocationSource::init() {
  #if defined(SERIAL_GPS_RX) && defined(SERIAL_GPS_TX) && !defined(SERIAL_GPS_RXTX_SET)
    SERIAL_GPS.begin(SERIAL_GPS_BAUD, SERIAL_8N1, SERIAL_GPS_RX, SERIAL_GPS_TX);
  #else
    SERIAL_GPS.begin(SERIAL_GPS_BAUD);
  #endif

  VF("MSG: TLS, start GPS monitor task (rate 10ms priority 7)... ");
  if (tasks.add(1, 0, true, 7, gpsPoll, "gpsPoll")) {
    VLF("success");
    active = true;
  } else {
    VLF("FAILED!");
    active = false;
  }

  // flag that start time is unknown
  startTime = 0;

  ready = false;

  return active;
}

void TimeLocationSource::set(JulianDate ut1) {
  ut1 = ut1;
}

void TimeLocationSource::get(JulianDate &ut1) {
  if (!ready) return;
  if (!timeIsValid()) return;

  GregorianDate greg; greg.year = gps.date.year(); greg.month = gps.date.month(); greg.day = gps.date.day();
  ut1 = calendars.gregorianToJulianDay(greg);
  // DUT1 = UT1 − UTC
  // UT1 = DUT1 + UTC
  ut1.hour = gps.time.hour() + gps.time.minute()/60.0 + (gps.time.second() + DUT1)/3600.0;

  // adjust date/time for DUT1 as needed
  if (ut1.hour >= 24.0L) { ut1.hour -= 24.0L; ut1 += 1.0L; } else
  if (ut1.hour < 0.0L) { ut1.hour += 24.0L; ut1 -= 1.0L; }
}

void TimeLocationSource::getSite(double &latitude, double &longitude, float &elevation) {
  if (!ready) return;
  if (!siteIsValid()) return;

  latitude = gps.location.lat();
  longitude = -gps.location.lng();
  elevation = gps.altitude.meters();
}

void TimeLocationSource::poll() {
  while (SERIAL_GPS.available() > 0) {
    gps.encode(SERIAL_GPS.read());
  }

  if (gps.location.isValid() && siteIsValid()) {
    if (gps.date.isValid() && gps.time.isValid() && timeIsValid()) {
      if (waitIsValid()) {
        VLF("MSG: TLS, GPS date/time/location is ready");

        VLF("MSG: TLS, closing GPS serial port");
        SERIAL_GPS.end();

        VLF("MSG: TLS, stopping GPS monitor task");
        tasks.setDurationComplete(tasks.getHandleByName("gpsPoll"));

        #ifdef TLS_TIMELIB
          setTime(gps.time.hour(), gps.time.minute(), gps.time.second(), gps.date.day(), gps.date.month(), gps.date.year());
        #endif

        ready = true;
      }
    }
  }
}

// starts keeping track of the wait once (PPS is synced, if applicable) and GPS has a lock 
bool TimeLocationSource::waitIsValid() {
  if (startTime == 0) startTime = millis();
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
