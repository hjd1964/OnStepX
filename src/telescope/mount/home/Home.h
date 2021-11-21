//--------------------------------------------------------------------------------------------------
// telescope mount home control
#pragma once

#include "../../../Common.h"

#ifdef MOUNT_PRESENT

#include "../../../libApp/commands/ProcessCmds.h"
#include "../coordinates/Transform.h"

class Home {
  public:
    // init the home position (according to settings and mount type)
    void init();

    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // move mount to the home position
    CommandError request();

    // reset mount, moves to the home position first if home switches are present
    CommandError requestWithReset();

    // reset mount at home
    CommandError reset(bool fullReset = true);

    Coordinate position;

    bool isRequestWithReset = false;

  private:

};

extern Home home;

#endif
