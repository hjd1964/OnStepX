//--------------------------------------------------------------------------------------------------
// telescope mount home control
#pragma once

#include "../../../Common.h"

#ifdef MOUNT_PRESENT

#include "../../../libApp/commands/ProcessCmds.h"
#include "../coordinates/Transform.h"

enum HomeState: uint8_t {HS_NONE, HS_HOMING};

#pragma pack(1)
#define SettingsSize 11
typedef struct HomeSense {
  long senseOffset;
  bool senseReverse;
} HomeSense;
typedef struct Settings {
  bool automaticAtBoot;
  HomeSense axis1;
  HomeSense axis2;
} Settings;
#pragma pack()

class Home {
  public:
    // init the home position (according to settings and mount type)
    void init();

    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // move mount to the home position
    CommandError request();

    // reset mount, moves to the home position first if home switches are present
    CommandError requestWithReset();

    // clear home state on abort
    void requestAborted();

    // after finding home switches displace the mount axes as specified
    void guideDone(bool success);

    // once homed mark as done
    void requestDone();

    // reset mount at home
    CommandError reset(bool fullReset = true);

    // get the home position (Mount coordinate system)
    Coordinate getPosition(CoordReturn coordReturn = CR_MOUNT_EQU);

    // home sensing
    bool useOffset();

    // allow axis1 by latitude reversal and home switch reversal
    void setReversal();

    bool hasSense = (((AXIS1_SENSE_HOME) != OFF) && ((AXIS2_SENSE_HOME) != OFF)) || \
                    (((AXIS1_SECTOR_GEAR) == ON) && ((AXIS1_SENSE_HOME) != OFF)) || \
                    (((AXIS2_TANGENT_ARM) == ON) && ((AXIS2_SENSE_HOME) != OFF));

    Settings settings = {MOUNT_AUTO_HOME_DEFAULT == ON, {AXIS1_SENSE_HOME_OFFSET, false}, {AXIS2_SENSE_HOME_OFFSET, false}};

    bool isRequestWithReset = false;

    HomeState state;

  private:
    bool wasTracking = false;
    Coordinate position;

};

extern Home home;

#endif
