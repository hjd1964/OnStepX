//--------------------------------------------------------------------------------------------------
// convert to/from C strings
#pragma once

#include <Arduino.h>

enum PrecisionMode {PM_LOWEST, PM_LOW, PM_HIGH, PM_HIGHEST, PM_UNKNOWN};

// extended strncpy() that forces a maximum length
void strncpyex(char *result, const char *source, size_t length);

// sprintf like function for float type, limited to one parameter
extern void sprintF(char *result, const char *source, double f);

class Convert {
  public:
    // convert timezone string  sHH:MM to double (in hours):
    // (also handles)           sHH
    bool tzToDouble(double *value, char *hm);

    // convert string in format as follows to double (in hours):
    // HH:MM           PM_LOWEST
    // HH:MM.M         PM_LOW
    // HH:MM:SS        PM_HIGH (DEFAULT) 
    // HH:MM:SS.SSSS   PM_HIGHEST
    bool hmsToDouble(double *value, char *hms, PrecisionMode p);

    // convert string in format as follows to double (in hours):
    // HH:MM           PM_LOWEST
    // HH:MM.M         PM_LOW
    // HH:MM:SS        PM_HIGH (DEFAULT) 
    // HH:MM:SS.SSSS   PM_HIGHEST
    // automatically detects PrecisionMode
    bool hmsToDouble(double *value, char *hms);

    // convert string in format as follows to double (in degrees):
    // sDD:MM          PM_LOW
    // DDD:MM          PM_LOW
    // sDD*MM          PM_LOW
    // DDD*MM          PM_LOW
    // sDD:MM:SS       PM_HIGH
    // DDD:MM:SS       PM_HIGH
    // sDD:MM:SS.SSS   PM_HIGHEST
    bool dmsToDouble(double *value, char *dms, bool signPresent, PrecisionMode p);

    // convert string in format as follows to double (in degrees):
    // sDD:MM          PM_LOW
    // DDD:MM          PM_LOW
    // sDD*MM          PM_LOW
    // DDD*MM          PM_LOW
    // sDD:MM:SS       PM_HIGH
    // DDD:MM:SS       PM_HIGH
    // sDD:MM:SS.SSS   PM_HIGHEST
    // automatically detects PrecisionMode
    bool dmsToDouble(double *value, char *dms, bool signPresent);
    
    // convert double (in hours) to string in format as follows:
    // HH:MM           PM_LOWEST
    // HH:MM.M         PM_LOW
    // HH:MM:SS        PM_HIGH, 
    // HH:MM:SS.SSSS   PM_HIGHEST
    void doubleToHms(char *reply, double value, bool signPresent, PrecisionMode p);
    
    // convert double (in degrees) to string in format as follows:
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

    // string to double with error checking
    bool atof2(char *a, double *d, bool sign = true);

    PrecisionMode precision = PM_HIGH;

  private:

};

extern Convert convert;