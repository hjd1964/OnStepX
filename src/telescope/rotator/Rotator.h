//--------------------------------------------------------------------------------------------------
// OnStepX focuser control
#pragma once

#include "../../common.h"

#if AXIS3_DRIVER_MODEL != OFF

#include "../../commands/ProcessCmds.h"
#include "../motion/StepDrivers.h"
#include "../motion/Axis.h"
#include "../mount/coordinates/Transform.h"

class Rotator {
  public:
    // initialize rotator
    void init(bool validKey);

    // process rotator commands
    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // get backlash in steps
    int  getBacklash();
    // set backlash in steps
    bool setBacklash(int value);

    // derotator enable or disable
    void derotatorEnable(bool value);
    // derotator get reverse state
    bool getDerotatorReverse();
    // derotator set reverse state
    void setDerotatorReverse(bool value);
    // derotator move to PA of equatorial coordinate
    void setDerotatorPA(Coordinate *coord);

    Axis axis3;

  private:
    // returns parallactic angle in degrees
    double ParallacticAngle(Coordinate *coord);
    // returns parallactic rate in degrees per second
    double ParallacticRate(Coordinate *coord);

    void readSettings();
    void writeSettings();

    float moveRate = 3.0F;

    int16_t backlash = 0;

    bool derotatorEnabled = false;
    bool derotatorReverse = false;
};

#endif
