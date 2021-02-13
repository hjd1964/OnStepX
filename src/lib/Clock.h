//--------------------------------------------------------------------------------------------------
// observatory site and time
#pragma once
#include <Arduino.h>
#include "../../Constants.h"

class Clock {
  public:
    // sets date/time from NV and/or the various TLS sources
    // and sets up an event to tick the centisecond sidereal clock
    void init(Site site);
    
    // adjusts the period of the centisecond sidereal clock, in counts per second
    unsigned long getPeriodSubMicros();
    void setPeriodSubMicros(unsigned long period);

    // set and apply the site longitude, necessary for LAST calculations
    void setSite(Site site);

    // handle date/time commands
    bool command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError);

    // gets local apparent sidereal time in hours
    double getLAST();

    // callback to tick the centisecond sidereal clock
    void tick();

  private:

    // sets the time in hours that have passed in this Julian Day
    void setTime(JulianDate julianDate);

    // gets the time in hours that have passed in this Julian Day
    double getTime();

    // sets the time in sidereal hours
    void setSiderealTime(JulianDate julianDate, double time);

    // gets the time in sidereal hours
    double getSiderealTime();

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

    Site site;
    JulianDate ut1;
    double centisecondHOUR = 0;
    unsigned long centisecondSTART = 0;

    bool dateIsReady = false;
    bool timeIsReady = false;

    unsigned long period = SIDEREAL_PERIOD;
    uint8_t handle = 0;
};
