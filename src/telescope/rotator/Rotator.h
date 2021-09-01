//--------------------------------------------------------------------------------------------------
// OnStepX focuser control
#pragma once

#include "../../Common.h"

#ifdef ROTATOR_PRESENT

#include "../../commands/ProcessCmds.h"
#include "../mount/coordinates/Transform.h"
#include "../Telescope.h"

// time to write position to nv after last movement of Rotator
#ifndef ROTATOR_WRITE_DELAY
  #if NV_ENDURANCE == NVE_VHIGH
    #define ROTATOR_WRITE_DELAY 5
  #elif NV_ENDURANCE == NVE_HIGH
    #define ROTATOR_WRITE_DELAY 60
  #else
    #define ROTATOR_WRITE_DELAY 300
  #endif
#endif

#pragma pack(1)
#define RotatorSettingsSize 7
typedef struct RotatorSettings {
  ParkState parkState;
  int16_t backlash;  // in steps
  float position;    // in degrees
} RotatorSettings;
#pragma pack()

class Rotator {
  public:
    // initialize rotator
    void init();

    // process rotator commands
    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // poll rotator to handle parking and derotation
    void monitor();

  private:
    // get backlash in steps
    int getBacklash();

    // set backlash in steps
    CommandError setBacklash(int value);

    #ifdef MOUNT_PRESENT
      // returns parallactic angle in degrees
      double parallacticAngle(Coordinate *coord);

      // returns parallactic rate in degrees per second
      double parallacticRate(Coordinate *coord);
    #endif

    // start slew in the specified direction
    CommandError slew(Direction dir);

    // move rotator to a specific location
    CommandError gotoTarget(float target);

    // parks rotator at current position
    CommandError park();

    // unparks rotator
    CommandError unpark();

    void readSettings();
    void writeSettings();

    float slewRate = AXIS3_SLEW_RATE_DESIRED;  // in degs/sec

    RotatorSettings settings = {PS_NONE, 0, 0.0F};

    bool derotatorEnabled = false;
    bool derotatorReverse = false;

    unsigned long writeTime = 0;
    unsigned long secs = 0;

    uint8_t parkHandle = 0;
};

extern Rotator rotator;

#endif
