//--------------------------------------------------------------------------------------------------
// OnStepX focuser control
#pragma once

#include "../../Common.h"

#if AXIS3_DRIVER_MODEL != OFF

#include "../../commands/ProcessCmds.h"
#include "../motion/StepDrivers.h"
#include "../motion/Axis.h"
#include "../mount/coordinates/Transform.h"

#define ROTATOR_PRESENT

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

    // poll to set derotator rate
    void derotatePoll();

    Axis axis;

  private:
    // enable or disable the derotator
    void setDerotatorEnabled(bool value);
    // returns parallactic angle in degrees
    double parallacticAngle(Coordinate *coord);
    // returns parallactic rate in degrees per second
    double parallacticRate(Coordinate *coord);

    void readSettings();
    void writeSettings();

    float moveRate = 3.0F;

    int16_t backlash = 0;

    bool derotatorEnabled = false;
    bool derotatorReverse = false;
};

#endif
