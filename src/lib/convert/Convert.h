//--------------------------------------------------------------------------------------------------
// convert to/from C strings
#pragma once

#include <Arduino.h>
#include "..\calendars\Calendars.h"

enum PrecisionMode {PM_LOWEST, PM_LOW, PM_HIGH, PM_HIGHEST};

// sprintf like function for float type, limited to one parameter
extern void sprintF(char *result, const char *source, float f);

class Convert {
  public:
    // convert string in format MM/DD/YY to julian date
    GregorianDate strToDate(char *ymd);

    // convert timezone string  sHH:MM to double
    // (also handles)           sHH
    bool tzToDouble(double *value, char *hm);

    // convert string in format as follows to double:
    // HH:MM           PM_LOWEST
    // HH:MM.M         PM_LOW
    // HH:MM:SS        PM_HIGH (DEFAULT) 
    // HH:MM:SS.SSSS   PM_HIGHEST
    bool hmsToDouble(double *value, char *hms, PrecisionMode p);
    // automatically detects PrecisionMode (as above)
    bool hmsToDouble(double *value, char *hms);

    // convert string in format as follows to double:
    // sDD:MM          PM_LOW
    // DDD:MM          PM_LOW
    // sDD*MM          PM_LOW
    // DDD*MM          PM_LOW
    // sDD:MM:SS       PM_HIGH
    // DDD:MM:SS       PM_HIGH
    // sDD:MM:SS.SSS   PM_HIGHEST
    bool dmsToDouble(double *value, char *dms, bool signPresent, PrecisionMode p);
    // automatically detects PrecisionMode (as above)
    bool dmsToDouble(double *value, char *dms, bool signPresent);
    
    // convert double to string in format as follows:
    // HH:MM           PM_LOWEST
    // HH:MM.M         PM_LOW
    // HH:MM:SS        PM_HIGH, 
    // HH:MM:SS.SSSS   PM_HIGHEST
    void doubleToHms(char *reply, double value, bool signPresent, PrecisionMode p);
    
    // convert double to string in format as follows:
    // sDD:MM          PM_LOW
    // DDD:MM          PM_LOW
    // sDD*MM          PM_LOW
    // DDD*MM          PM_LOW
    // DDD:MM:SS       PM_HIGH
    // sDD:MM:SS.SSS   PM_HIGHEST
    void doubleToDms(char *reply, double value, bool fullRange, bool signPresent, PrecisionMode p);

    // string to int with error checking
    bool atoi2(char *a, int16_t *i, bool sign = true);
    // string to byte with error checking
    bool atoi2(char *a, uint8_t *u, bool sign = true);
    // string to float with error checking
    bool atof2(char *a, double *d, bool sign = true);

    PrecisionMode precision = PM_HIGH;

  private:

};

extern Convert convert;