//--------------------------------------------------------------------------------------------------
// observatory time
#pragma once
#include <Arduino.h>
#include "../../Constants.h"
#include "../coordinates/Convert.h"
#include "../coordinates/Transform.h"
#include "../commands/ProcessCmds.h"

class Clock {
  public:
    // sets date/time from NV and/or the various TLS sources
    // and sets up an event to tick the centisecond sidereal clock
    void init();
    
    // adjusts the period of the centisecond sidereal clock, in counts per second
    void setPeriodSubMicros(unsigned long period);
    unsigned long getPeriodSubMicros();

    // update/apply the site longitude, necessary for LAST calculations
    void updateSite();

    // handle date/time commands
    bool command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError);

    // gets the time in sidereal hours
    double getSiderealTime();

    // callback to tick the centisecond sidereal clock
    void tick();

  private:

    // sets the time in hours that have passed in this Julian Day
    void setTime(JulianDate julianDate);

    // gets the time in hours that have passed in this Julian Day
    double getTime();

    // sets the time in sidereal hours
    void setSiderealTime(JulianDate julianDate, double time);

    // adjust time (hours) into the 0 to 24 range
    double backInHours(double time);

    // adjust time (hours) into the -12 to 12 range
    double backInHourAngle(double time);

    // convert julian date/time to local apparent sidereal time
    double julianDateToLAST(JulianDate julianDate);

    // convert julian date/time to greenwich apparent sidereal time
    double julianDateToGAST(JulianDate julianDate);

    // convert Gregorian date (year, month, day) to Julian Day
    JulianDate gregorianToJulianDay(GregorianDate date);

    // convert Julian Day to Gregorian date (year, month, day)
    GregorianDate julianDayToGregorian(JulianDate julianDate);

    JulianDate ut1;
    double centisecondHOUR = 0;
    unsigned long centisecondSTART = 0;

    bool dateIsReady = false;
    bool timeIsReady = false;

    unsigned long period;
    uint8_t handle = 0;
};
