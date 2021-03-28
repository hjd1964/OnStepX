//--------------------------------------------------------------------------------------------------
// OnStepX telescope control
#pragma once
#include "../commands/ProcessCmds.h"
#include "../mount/Mount.h"

typedef struct InitError {
  uint8_t nv:1;
  uint8_t site:1;
  uint8_t mount:1;
  uint8_t axis:1;
  uint8_t driver:1;
  uint8_t tls:1;
  uint8_t weather:1;
} InitError;

extern InitError initError;

// ambient temperature (°C), pressure (mb), humidity (RH %), and altitude (meters)
typedef struct Ambient {
  float temperature;
  float pressure;
  float humidity;
  float altitude;
} Ambient;

class Telescope {
  public:
    // setup the location, time keeping, and coordinate converson
    void init();

    // update the location for time keeping and coordinate conversion
    void updateSite();

    // handle observatory commands
    bool command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError);

    // equipment
    #if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF
      Mount mount;
    #endif
    #if AXIS3_DRIVER_MODEL != OFF
      Rotator rotator1;
    #endif
    #if AXIS4_DRIVER_MODEL != OFF
      Focuser focuser1;
    #endif
    #if AXIS5_DRIVER_MODEL != OFF
      Focuser focuser2;
    #endif
    #if AXIS6_DRIVER_MODEL != OFF
      Focuser focuser3;
    #endif

    Ambient ambient = { 10.0, 1010.0, 50, 100 };

  private:
    inline float dewPoint(Ambient conditions) { return conditions.temperature - ((100.0F - conditions.humidity) / 5.0F); }
};

extern Telescope telescope;
