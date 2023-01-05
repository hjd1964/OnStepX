// -----------------------------------------------------------------------------------
// Misc functions to help with commands, etc.
#pragma once

#include "../../../../Common.h"

enum DriverType {DT_NONE, DT_SERVO, DT_STEP_DIR_STANDARD, DT_STEP_DIR_TMC_SPI, DT_STEP_DIR_LEGACY};

typedef struct AxisSettings {
   double stepsPerMeasure;
   int8_t reverse;
   int16_t min;
   int16_t max;
   int16_t microsteps;
   int16_t microstepsGoto;
   int16_t currentHold;
   int16_t currentRun;
   int16_t currentGoto;
   float p, i, d, pGoto, iGoto, dGoto;
   double param1;
   double param2;
   double param3;
   double param4;
   double param5;
   double param6;
   DriverType driverType;
} AxisSettings;

// remove leading and trailing 0's
void stripNum(char* s);

// convert hex to int, returns -1 on error
int hexToInt(String s);

// convert time to compact byte representation for intervalometer
uint8_t timeToByte(float t);

// convert compact byte representation to time for intervalometer
float byteToTime(uint8_t b);

// convert axis settings string into numeric form
bool decodeAxisSettings(char* s, AxisSettings* a);

// convert axis settings string into numeric form
bool decodeAxisSettingsX(char* s, AxisSettings* a);

// validate axis settings for a given axis and mount type
bool validateAxisSettings(int axisNum, bool altAz, AxisSettings a);

// validate axis settings for a given axis and mount type
bool validateAxisSettingsX(int axisNum, AxisSettings a);

// return temperature string with proper value and units for this locale
void localeTemperature(char* temperatureStr);

// return pressure string with proper value and units for this locale
void localePressure(char* pressureStr);

// return humidity string with proper value and units for this locale
void localeHumidity(char* humidityStr);
