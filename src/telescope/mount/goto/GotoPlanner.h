//--------------------------------------------------------------------------------------------------
// telescope mount control, goto planning engine interface
#pragma once

#include "../../../Common.h"

#if defined(MOUNT_PRESENT) && GOTO_FEATURE == ON && GOTO_PLANNER == ON

#include "Goto.h"

class GotoPlanner {
  public:
    // plan the goto segment from the current position to its destination (Mount coordinates)
    // from holds the axis frame and a pier side only, near the pole compare positions as instrument coordinates
    // not by pier side, the axis1 target of to is its instrument coordinate plus to->a1Correction
    // stage is the goto stage the destination belongs to, returns CE_NONE or the error the goto is refused with
    virtual CommandError plan(Coordinate *from, Coordinate *to, GotoStage stage) = 0;

    // get the next waypoint of the planned route (Mount coordinates), false when there are none left
    virtual bool nextWaypoint(Coordinate *waypoint) = 0;
};

extern GotoPlanner& gotoPlanner;

#endif
