//--------------------------------------------------------------------------------------------------
// observatory site and time
#pragma once
#include "../commands/ProcessCmds.h"
#include "Mount.h"

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

  private:

};
