//--------------------------------------------------------------------------------------------------
// telescope mount home control
#pragma once

#include "../../../Common.h"

#ifdef MOUNT_PRESENT

#include "../../../libApp/commands/ProcessCmds.h"
#include "../coordinates/Transform.h"

enum HomeState: uint8_t {HS_NONE, HS_HOMING};

#pragma pack(1)
#define SettingsSize 9
typedef struct SenseOffset {
  long axis1;
  long axis2;
} SenseOffset;
typedef struct Settings {
  boolean automaticAtBoot;
  SenseOffset senseOffset;
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

    // get the home position
    Coordinate getPosition(CoordReturn coordReturn = CR_MOUNT_EQU);

    // home sensing
    bool useOffset();
    bool hasSense = (AXIS1_SENSE_HOME) != OFF && (AXIS2_SENSE_HOME) != OFF;
    Settings settings = {MOUNT_AUTO_HOME_DEFAULT == ON, {AXIS1_SENSE_HOME_OFFSET, AXIS2_SENSE_HOME_OFFSET}};

    bool isRequestWithReset = false;

    HomeState state;

  private:
    bool wasTracking = false;
    Coordinate position;

};

extern Home home;

#endif
