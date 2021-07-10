//--------------------------------------------------------------------------------------------------
// OnStepX telescope control
#pragma once

#include "../Common.h"
#include "../commands/ProcessCmds.h"
#include "../lib/weather/Weather.h"
#include "mount/Mount.h"
#include "focuser/Focuser.h"
#include "rotator/Rotator.h"
#include "auxiliary/Features.h"

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

    // equipment
    #ifdef MOUNT_PRESENT
      Mount mount;
    #endif
    #ifdef ROTATOR_PRESENT
      Rotator rotator;
    #endif
    #ifdef FOCUSER_PRESENT
      Focuser focuser;
    #endif
    #ifdef FEATURES_PRESENT
      Features features;
    #endif

  private:
    Firmware firmware;
};

extern Telescope telescope;
