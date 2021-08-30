//--------------------------------------------------------------------------------------------------
// telescope mount control
#pragma once

#include "../../../Common.h"

#if defined(MOUNT_PRESENT) && SLEW_GOTO == ON

#include "../../Telescope.h"
#include "../coordinates/Transform.h"

#pragma pack(1)
typedef struct ParkPosition {
  float h;
  float d;
  PierSide pierSide;
} ParkPosition;

#define ParkSettingsSize 15
typedef struct ParkSettings {
  ParkPosition position;
  bool         saved;
  ParkState    state;
  long         wormSensePositionSteps;
} ParkSettings;
#pragma pack()

class Park {
  public:
    void init();

    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // sets a park position
    CommandError set();

    // move the mount to the park position
    CommandError request();

    // once parked save the park state
    void requestDone();

    // returns a parked telescope to operation
    CommandError restore(bool withTrackingOn);

    // resets park state, clears any errors but does not erase the park position
    inline void reset() { state = PS_NONE; nv.updateBytes(NV_MOUNT_PARK_BASE, &settings, sizeof(ParkSettings)); }

    ParkState state;

    ParkSettings settings = {{0, 0, PIER_SIDE_NONE}, false, PS_NONE, 0};

  private:

};

extern Park park;

#endif
