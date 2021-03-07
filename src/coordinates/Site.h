//--------------------------------------------------------------------------------------------------
// observatory time
#pragma once
#include <Arduino.h>
#include "../../Constants.h"
#include "../coordinates/Convert.h"
#include "../commands/ProcessCmds.h"

extern volatile unsigned long centisecondLAST;

typedef struct LatitudeExtras {
  double sine;
  double cosine;
  double absval;
  double sign;
} Latitude;

typedef struct LocationExtras {
  LatitudeExtras latitude;
  bool ready;
} LocationExtras;

#pragma pack(1)
#define LocationSize 36
typedef struct Location {
  double latitude;
  double longitude;
  float  timezone;
  char   name[16];
} Location;
#pragma pack()

// ambient temperature (°C), pressure (mb), humidity (RH %), and altitude (meters)
typedef struct SiteConditions {
  float temperature;
  float pressure;
  float humidity;
  float altitude;
} SiteConditions;

extern SiteConditions siteConditions;

typedef struct SiteErrors {
  bool init;
  bool TLSinit;
} SiteErrors;

class Site {
  public:
    // gets date/time/location from NV and/or the various TLS sources
    void init();
    
    // update/apply the site latitude and longitude, necessary for LAST calculations etc.
    void update();

    // adjusts the period of the centisecond sidereal clock, in sub-micro counts per second
    // adjust up/down to compensate for MCU oscillator inaccuracy
    void setPeriodSubMicros(unsigned long period);

    // handle date/time commands
    bool command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError);

    // gets the time in sidereal hours
    double getSiderealTime();

    // callback to tick the centisecond sidereal clock
    void tick();

    Location location;
    LocationExtras locationEx;
    Convert convert;

    SiteErrors error = {false, false};

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

    // reads the location information from NV
    // locationNumber can be 0..3
    void readLocation(uint8_t locationNumber);

    // reads the julian date information from NV
    void readJD();

    inline double dewPoint(SiteConditions conditions) { return conditions.temperature - ((100.0 - conditions.humidity) / 5.0); }

    JulianDate ut1;
    double centisecondHOUR = 0;
    unsigned long centisecondSTART = 0;

    bool dateIsReady = false;
    bool timeIsReady = false;

    unsigned long period = 0;
    // handle to centisecond LAST task
    uint8_t handle = 0;
    // site number 0..3
    uint8_t number = 0;
};
