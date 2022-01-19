//--------------------------------------------------------------------------------------------------
// telescope mount home control
#pragma once

#include "../../../Common.h"

#ifdef MOUNT_PRESENT

#include "../../../libApp/commands/ProcessCmds.h"
#include "../coordinates/Transform.h"

enum HomeState: uint8_t {HS_NONE, HS_HOMING};

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

    // once homed mark as done
    void requestDone();

    // reset mount at home
    CommandError reset(bool fullReset = true);

    Coordinate position;

    bool isRequestWithReset = false;

    HomeState state;

  private:
    bool wasTracking = false;

};

extern Home home;

#endif
