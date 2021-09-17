//--------------------------------------------------------------------------------------------------
// observatory time
#pragma once

#include "../../../Common.h"

#ifdef MOUNT_PRESENT

#include <Arduino.h>
#include "../../../Constants.h"
#include "../../../lib/convert/Convert.h"
#include "../../../commands/ProcessCmds.h"
#include "../../../lib/calendars/Calendars.h"

#if TIME_LOCATION_SOURCE == DS3231
  #include "../../../lib/tls/Tls_DS3231.h"
#endif
#if TIME_LOCATION_SOURCE == DS3234
  #include "../../../lib/tls/Tls_DS3234.h"
#endif
#if TIME_LOCATION_SOURCE == TEENSY
  #include "../../../lib/tls/Tls_Teensy.h"
#endif
#if TIME_LOCATION_SOURCE == GPS
  #include "../../../lib/tls/Tls_GPS.h"
#endif

extern volatile unsigned long fracLAST;

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
#define LocationSize 40
typedef struct Location {
  double latitude;
  double longitude;
  float  elevation;
  float  timezone;
  char   name[16];
} Location;
#pragma pack()

class Site {
  public:
    void init();
    
    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // update/apply the site latitude and longitude, necessary for LAST calculations etc.
    void updateLocation();

    // sets the Julian Date/time (UT1,) and updates sidereal time
    void setDateTime(JulianDate julianDate);

    // gets the time in sidereal hours
    double getSiderealTime();

    // sets the sidereal time from the UT date/time
    void setSiderealTime(JulianDate julianDate);

    // checks if the date and time were set
    bool isDateTimeReady();

    // gets sidereal period, in sub-micro counts per second
    unsigned long getSiderealPeriod();

    // sets sidereal period, in sub-micro counts per second
    void setSiderealPeriod(unsigned long period);

    // gets sidereal ratio
    // slower rates are < 1.0, faster rates are > 1.0
    inline float getSiderealRatio() { return (float)SIDEREAL_PERIOD/siderealPeriod; }

    // callback to tick the fracsec sidereal frac
    void tick();

    Location location;
    LocationExtras locationEx;

  private:
    // gets the time in hours that have passed in this Julian Day
    double getTime();

    // sets the time in sidereal hours
    void setLAST(JulianDate julianDate, double time);

    // convert julian date/time to local apparent sidereal time
    double julianDateToLAST(JulianDate julianDate);

    // convert julian date/time to greenwich apparent sidereal time
    double julianDateToGAST(JulianDate julianDate);

    // reads the julian date information from NV
    void readJD(bool validKey);

    // reads the location information from NV
    // locationNumber can be 0..3
    void readLocation(uint8_t locationNumber, bool validKey);

    // sets the site altitude in meters
    bool setElevation(float e);

    // adjust time (hours) into the 0 to 24 range
    double backInHours(double time);

    // adjust time (hours) into the -12 to 12 range
    double backInHourAngle(double time);

    // convert string in format MM/DD/YY to julian date
    GregorianDate strToDate(char *ymd);

    // the current UT1 date and time
    JulianDate ut1;
    double fracHOUR = 0;
    unsigned long fracSTART = 0;

    bool dateIsReady = false;
    bool timeIsReady = false;

    // sidereal period in sub-microsecond counts
    unsigned long siderealPeriod = 0;

    // handle to sidereal timer LAST task
    uint8_t handle = 0;
    // site number 0..3
    uint8_t number = 0;
};

extern Site site;

#endif