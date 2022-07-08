//--------------------------------------------------------------------------------------------------
// Mount status LED and buzzer
#pragma once

#include "../../../Common.h"

#ifdef MOUNT_PRESENT

#include "../../../libApp/commands/ProcessCmds.h"
#include "../../../lib/sound/Sound.h"

#define SF_STOPPED 0
#define SF_SLEWING 1

class Status {
  public:
    // get mount status ready
    void init();

    // late init once tracking is enabled
    void ready();

    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // mount status LED flash rate (in ms)
    void flashRate(int period);

    void general();

    Sound sound;

  private:
    uint8_t statusTaskHandle = 0;
};

extern Status mountStatus;

#endif
