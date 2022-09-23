//--------------------------------------------------------------------------------------------------
// observatory time
#pragma once

#include "../../../Common.h"

#ifdef MOUNT_PRESENT

#include "../../../lib/convert/Convert.h"
#include "../../../libApp/commands/ProcessCmds.h"
#include "../../../lib/calendars/Calendars.h"
#include "../../../lib/tls/Tls.h"
#include "../../../lib/tls/PPS.h"

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

    // update the initError status and restore the park position if necessary
    void updateTLS();

    // gets the UT1 Julian date/time
    JulianDate getDateTime();

    // sets the UT1 Julian date/time and updates sidereal time
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

    // for internal use, when to timeout date/time updates
    unsigned long updateTimeoutTime = 0;

    bool dateIsReady = false;
    bool timeIsReady = false;

  private:
    // gets the time in hours that have passed since Julian Day was set (UT1)
    double getTime();

    // sets the time in sidereal hours
    void setLAST(JulianDate julianDate, double time);

    // convert julian date/time to local apparent sidereal time
    double julianDateToLAST(JulianDate julianDate);

    // convert julian date/time to greenwich apparent sidereal time
    double julianDateToGAST(JulianDate julianDate);

    // reads the julian date information from NV
    void readJD();

    // reads the location information from NV
    // locationNumber can be 0..3
    void readLocation(uint8_t number);

    // sets the site altitude in meters
    bool setElevation(float e);

    // convert UT1 to local standard date/time
    JulianDate UT1ToLocal(JulianDate ut1);

    // convert local standard to UT1 date/time
    JulianDate localToUT1(JulianDate local);

    // adjust into the 0 to 24 range
    double rangeHours(double time);

    // adjust into the 0 to 12 range
    double rangeAmPm(double time);

    // convert string in format MM/DD/YY or MM/DD/YYYY to Date (changes only date)
    bool strToDate(char *ymd, GregorianDate *date);

    // the current UT1 date and time
    JulianDate ut1;
    double fracHOUR = 0;
    unsigned long fracSTART = 0;

    bool writeDate = true;
    bool writeTime = true;

    // sidereal period in sub-microsecond counts
    unsigned long siderealPeriod = 0;

    // handle to sidereal timer LAST task
    uint8_t taskHandle = 0;

    // site number 0..3
    uint8_t locationNumber = 0;
};

extern Site site;

#endif