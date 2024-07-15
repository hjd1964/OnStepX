// -----------------------------------------------------------------------------------
// Time/Location source SD3031 RTC support
// uses the default I2C port in most cases; though HAL_Wire can redirect to another port (as is done for the Teensy3.5/3.6)

#include "SD3031.h"

#if defined(TIME_LOCATION_SOURCE) && TIME_LOCATION_SOURCE == SD3031 || \
    (defined(TIME_LOCATION_SOURCE_FALLBACK) && TIME_LOCATION_SOURCE_FALLBACK == SD3031)


#ifdef TLS_TIMELIB
  #include <TimeLib.h> // https://github.com/PaulStoffregen/Time/archive/master.zip
#endif

#include <Wire.h>
#include <DFRobot_SD3031.h> // https://github.com/cdjq/DFRobot_SD3031
DFRobot_SD3031 rtcSD3031(&HAL_Wire);

bool TlsSd3031::init() {
  HAL_Wire.begin();
  #ifdef HAL_WIRE_CLOCK
    HAL_Wire.setClock(HAL_WIRE_CLOCK);
  #endif

  bool error = !rtcSD3031.begin();
  if (!error) {
    #ifdef HAL_WIRE_CLOCK
      HAL_Wire.setClock(HAL_WIRE_CLOCK);
    #endif

    rtcSD3031.setHourSystem(rtcSD3031.e24hours);

    #ifdef TLS_TIMELIB
      sTimeData_t dateTime = rtcSD3031.getRTCTime();
      setTime(dateTime.hour, dateTime.minute, dateTime.second, dateTime.day, dateTime.month, dateTime.year);
    #endif

    ready = true;
  } else { DLF("WRN: tls.init(), SD3031 (I2C 0x32) not found"); }
  #ifdef HAL_WIRE_RESET_AFTER_CONNECT
    HAL_Wire.end();
    HAL_Wire.begin();
    #ifdef HAL_WIRE_CLOCK
      HAL_Wire.setClock(HAL_WIRE_CLOCK);
    #endif
  #endif
  return ready;
}

void TlsSd3031::set(JulianDate ut1) {
  if (!ready) return;

  GregorianDate greg = calendars.julianDayToGregorian(ut1);

  double f1 = fabs(ut1.hour) + TLS_CLOCK_SKEW;
  int h = floor(f1);
  double m = (f1 - h)*60.0;
  double s = (m - floor(m))*60.0;

  set(greg.year, greg.month, greg.day, h, floor(m), floor(s));
}

void TlsSd3031::set(int year, int month, int day, int hour, int minute, int second) {
  #ifdef TLS_TIMELIB
    setTime(hour, minute, second, day, month, year);
  #endif
  rtcSD3031.setTime(year, month, day, hour, minute, second);
}

bool TlsSd3031::get(JulianDate &ut1) {
  if (!ready) return false;

  sTimeData_t dateTime = rtcSD3031.getRTCTime();

  if (dateTime.year >= 2018 && dateTime.year <= 3000 &&
      dateTime.month >= 1 && dateTime.month <= 12 &&
      dateTime.day >= 1 && dateTime.day <= 31 &&
      dateTime.hour <= 23 && dateTime.minute <= 59 && dateTime.second <= 59) {
    GregorianDate greg;
    greg.year = dateTime.year; greg.month = dateTime.month; greg.day = dateTime.day;
    ut1 = calendars.gregorianToJulianDay(greg);
    ut1.hour = dateTime.hour + dateTime.minute/60.0 + dateTime.second/3600.0;
  }

  return true;
}

#endif
