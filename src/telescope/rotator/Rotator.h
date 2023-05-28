//--------------------------------------------------------------------------------------------------
// OnStepX focuser control
#pragma once

#include "../../Common.h"

#ifdef ROTATOR_PRESENT

#include "../../lib/axis/Axis.h"
#include "../../libApp/commands/ProcessCmds.h"
#include "../Telescope.h"
#include "../mount/coordinates/Transform.h"

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
#define RotatorSettingsSize 11
typedef struct RotatorSettings {
  ParkState parkState;
  int16_t backlash;  // in steps
  float position;    // in degrees
  float gotoRate;    // in degrees/s
} RotatorSettings;
#pragma pack()

class Rotator {
  public:
    // initialize rotator
    void init();
    void begin();

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

    // set move rate
    void setMoveRate(int value);

    // start slew in the specified direction
    CommandError move(Direction dir);

    // get goto rate, 1 for 0.5x base, 2 for 0.66x base, 3 for base, 4 for 1.5x base, 5 for 2x base
    int getGotoRate();

    // set goto rate, 1 for 0.5x base, 2 for 0.66x base, 3 for base, 4 for 1.5x base, 5 for 2x base
    void setGotoRate(int value);

    // move rotator to a specific location
    CommandError gotoTarget(float target);

    // parks rotator at current position
    CommandError park();

    // unparks rotator
    CommandError unpark();

    void readSettings();
    void writeSettings();

    float moveRate = 0.1;  // in degs/sec

    RotatorSettings settings = {PS_UNPARKED, 0, 0.0F, AXIS3_SLEW_RATE_BASE_DESIRED};

    bool derotatorEnabled = false;
    bool derotatorReverse = false;
    bool homing = false;

    unsigned long writeTime = 0;
    unsigned long secs = 0;

    uint8_t parkHandle = 0;
};

#ifdef AXIS3_STEP_DIR_PRESENT
  extern StepDirMotor motor3;
#elif defined(AXIS3_SERVO_PRESENT)
  extern ServoMotor motor3;
#endif
extern Axis axis3;

extern Rotator rotator;

#endif
