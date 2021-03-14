//--------------------------------------------------------------------------------------------------
// convert to/from C strings
#pragma once
#include <Arduino.h>

enum PrecisionMode {PM_LOWEST, PM_LOW, PM_HIGH, PM_HIGHEST};

typedef struct GregorianDate {
  int16_t  year;
  uint8_t  month;
  uint8_t  day;
  uint8_t  hour;
  uint8_t  minute;
  uint8_t  second;
  long     centisecond;
  bool     valid;
} GregorianDate;

#define JulianDateSize 16
typedef struct JulianDate {
  double   day;
  double   hour;
} JulianDate;

// sprintf like function for float type, limited to one parameter
extern void sprintF(char* result, const char* source, float f);

class Convert {
  public:
    // convert string in format MM/DD/YY to julian date
    GregorianDate strToDate(char *ymd);

    // convert timezone string  sHH:MM to double
    // (also handles)           sHH
    bool tzToDouble(double *value, char *hm);

    // convert string in format HH:MM:SS to double
    // (also handles)           HH:MM.M
    //                          HH:MM:SS
    //                          HH:MM:SS.SSSS
    bool hmsToDouble(double *value, char *hms, PrecisionMode p);
    // automatically detects PrecisionMode (as above)
    bool hmsToDouble(double *value, char *hms);

    // convert string in format sDD:MM:SS to double
    // (also handles)           sDD:MM:SS.SSS
    //                          DDD:MM:SS
    //                          sDD:MM
    //                          DDD:MM
    //                          sDD*MM
    //                          DDD*MM
    bool dmsToDouble(double *value, char *dms, bool signPresent, PrecisionMode p);
    // automatically detects PrecisionMode (as above)
    bool dmsToDouble(double *value, char *dms, bool signPresent);
    
    // convert double to string in a variety of formats (as above)
    void doubleToHms(char *reply, double value, bool signPresent, PrecisionMode p);
    // convert double to string in a variety of formats (as above) 
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
