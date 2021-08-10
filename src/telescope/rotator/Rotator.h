//--------------------------------------------------------------------------------------------------
// OnStepX focuser control
#pragma once

#include "../../Common.h"

#ifdef ROTATOR_PRESENT

#include "../../commands/ProcessCmds.h"
#include "../axis/Axis.h"
#include "../mount/coordinates/Transform.h"

class Rotator {
  public:
    // initialize rotator
    void init();

    // process rotator commands
    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // get backlash in steps
    int  getBacklash();

    // set backlash in steps
    bool setBacklash(int value);

    // poll to set derotator rate
    void poll();

    Axis axis;

  private:
    #ifdef MOUNT_PRESENT
      // returns parallactic angle in degrees
      double parallacticAngle(Coordinate *coord);

      // returns parallactic rate in degrees per second
      double parallacticRate(Coordinate *coord);
    #endif

    // move rotator to a specific location
    CommandError gotoTarget(float target);

    // parks rotator at current position
    void park();

    // unparks rotator
    void unpark();

    void readSettings();
    void writeSettings();

    float slewRate = AXIS3_SLEW_RATE_DESIRED;  // in degs/sec

    int16_t backlash = 0;   // in steps
    float position = 0.0F;  // in degrees

    bool derotatorEnabled = false;
    bool derotatorReverse = false;
};

extern Rotator rotator;

#endif
