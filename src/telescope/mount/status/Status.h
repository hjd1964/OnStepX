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

    // mount status wake on demand
    void wake();

    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // mount status LED flash rate (in ms)
    void flashRate(int period);

    // mount misc. general status indicators
    void general();

    inline void soundAlert() { if (sound.enabled) wake(); sound.alert(); }
    inline void soundBeep() { if (sound.enabled) wake(); sound.beep(); }
    inline void soundClick() { if (sound.enabled) wake(); sound.click(); }
    inline void soundToggleEnable() { sound.enabled = !sound.enabled; }

  private:
    uint8_t statusTaskHandle = 0;
    Sound sound;
};

extern Status mountStatus;

#endif
