//--------------------------------------------------------------------------------------------------
// OnStepX telescope control
#pragma once

#include "../Common.h"
#include "../commands/ProcessCmds.h"

enum ParkState: uint8_t {PS_UNPARKED, PS_PARKED, PS_UNPARKING, PS_PARKING, PS_PARK_FAILED};

typedef struct InitError {
  uint8_t nv:1;       // NV data size/structure error (disables writes to NV)
  uint8_t value:1;    // invalid value error
  uint8_t driver:1;   // the stepper driver wasn't detected properly (TMC SPI drivers only)
  uint8_t weather:1;  // the weather source (BME280, etc.) was not found
  uint8_t tls:1;      // the time location source (DS3231, GPS, etc.) was not found
} InitError;

extern InitError initError;

typedef struct Version {
  uint8_t major;
  uint8_t minor;
  char patch[2];
  uint8_t config;
} Version;

typedef struct Firmware {
  char name[8];
  Version version;
  char date[20];
  char time[20];
} Firmware;

class Telescope {
  public:
    // setup the location, time keeping, and coordinate converson
    void init(const char *fwName, int fwMajor, int fwMinor, const char *fwPatch, int fwConfig);

    // update the location for time keeping and coordinate conversion
    void updateSite();

    // handle observatory commands
    bool command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError);

    void statusInit();

  private:
    Firmware firmware;
};

extern bool validKey;         // true if the NV key matches the Telescope key value
extern bool xBusy;            // true during timing sensitive operations (for disabling I2C etc.)
extern const int AnalogRange; // help for analogWrite() range conversions
#define analog8BitToAnalogRange(v) roundf((v/255.0F)*AnalogRange)

extern Telescope telescope;
