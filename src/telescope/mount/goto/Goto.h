//--------------------------------------------------------------------------------------------------
// telescope mount control
#pragma once

#include "../../../Common.h"

#if defined(MOUNT_PRESENT)

#include "../../../libApp/commands/ProcessCmds.h"
#include "../coordinates/Transform.h"

enum MeridianFlip: uint8_t     {MF_NEVER, MF_ALWAYS};
enum GotoState: uint8_t        {GS_NONE, GS_GOTO};
enum GotoStage: uint8_t        {GG_NONE, GG_ABORT, GG_READY_ABORT, GG_WAYPOINT_HOME, GG_WAYPOINT_AVOID, GG_NEAR_DESTINATION_START, GG_NEAR_DESTINATION_WAIT, GG_NEAR_DESTINATION, GG_DESTINATION};
enum GotoType: uint8_t         {GT_NONE, GT_HOME, GT_PARK};
enum PierSideSelect: uint8_t   {PSS_NONE, PSS_EAST, PSS_WEST, PSS_BEST, PSS_EAST_ONLY, PSS_WEST_ONLY, PSS_SAME_ONLY};

typedef struct MeridianFlipHome {
  bool paused;
  bool resume;
} MeridianFlipHome;

#pragma pack(1)
#define GotoSettingsSize 6
typedef struct GotoSettings {
  bool meridianFlipAuto  :1;
  bool meridianFlipPause :1;
  PierSideSelect preferredPierSide;
  float usPerStepCurrent;
} GotoSettings;
#pragma pack()

typedef struct AlignState {
  uint8_t currentStar;
  uint8_t lastStar;
} AlignState;

class Goto {
  public:
    void init();

    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // goto to equatorial target position (Native coordinate system) using the defaut preferredPierSide
    CommandError request();

    // goto equatorial position (Native or Mount coordinate system)
    CommandError request(Coordinate coords, PierSideSelect pierSideSelect, bool native = true);

    // sync to equatorial target position (Native coordinate system) using the default preferredPierSide
    CommandError requestSync();

    // sync to equatorial position (Native or Mount coordinate system)
    CommandError requestSync(Coordinate coords, PierSideSelect pierSideSelect, bool native = true);

    // get target equatorial position (Native coordinate system)
    inline Coordinate getGotoTarget() { return gotoTarget; }

    // set target equatorial position (Native coordinate system)
    inline void setGotoTarget(Coordinate *coords) { gotoTarget = *coords; }

    // checks for valid target and determines pier side (Mount coordinate system)
    CommandError setTarget(Coordinate *coords, PierSideSelect pierSideSelect, bool isGoto = true);

    // stop any presently active goto
    void stop();

    // general status checks ahead of sync or goto
    CommandError validate();

    // add an align star (at the current position relative to target)
    CommandError alignAddStar();

    // reset the alignment model
    void alignReset();

    // check if an align is in progress
    inline bool alignActive() { return alignState.lastStar > 0 && alignState.currentStar <= alignState.lastStar; }

    // check if an align is done
    inline bool alignDone() { return alignState.lastStar > 0 && alignState.currentStar > alignState.lastStar; }

    // returns true if the pause at home feature is enabled
    inline bool isHomePauseEnabled() { return settings.meridianFlipPause; }

    // returns true if paused at the home position, waiting to continue, during a goto
    inline bool isHomePaused() { return meridianFlipHome.paused; }

    // if paused at the home position, continue the goto
    inline void homeContinue() { meridianFlipHome.resume = true; }

    // returns true if the automatic meridian flip feature is enabled
    inline bool isAutoFlipEnabled() { return settings.meridianFlipAuto; }

    // monitor goto
    void poll();

    // for determining goto state
    GotoState state = GS_NONE;
    GotoStage stage = GG_NONE;

    // current goto rate in radians per second
    float rate;

    // flag to start tracking if this is the first goto
    bool firstGoto = true;

    // flag to indicate that encoders are present
    bool absoluteEncodersPresent = false;
    bool encodersPresent = false;

  private:

    #if GOTO_FEATURE == ON
    // set any additional destinations required for a goto
    void waypoint(Coordinate *current);

    // start slews with approach correction and parking/homing support
    CommandError startAutoSlew();
    #endif

    // update acceleration rates for goto and guiding
    void updateAccelerationRates();

    // estimate average microseconds per step lower limit
    float usPerStepLowerLimit();

    // requested goto/sync destination Native coordinate (eq or hor)
    Coordinate gotoTarget = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, PIER_SIDE_NONE};
    // goto starts from this Mount coordinate (eq or hor)
    Coordinate start;
    // goto next destination Mount coordinate (eq or hor)
    Coordinate destination;
    // goto final destination Mount coordinate (eq or hor)
    Coordinate target = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, PIER_SIDE_NONE};
    // last align (goto) target Mount coordinate (eq or hor)
    Coordinate lastAlignTarget = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, PIER_SIDE_NONE};
    GotoState  stateAbort           = GS_NONE;
    GotoState  stateLast            = GS_NONE;
    uint8_t    taskHandle           = 0;
    int        nearDestinationRefineStages;
    unsigned long nearTargetTimeout = 0;
    unsigned long nearTargetTimeoutAxis1 = 0;
    unsigned long nearTargetTimeoutAxis2 = 0;
    unsigned long nearDestinationTimeout = 0;

    MeridianFlipHome meridianFlipHome = {false, false};

    AlignState alignState = {0, 0};

    float      usPerStepBase        = 128.0F;
    float      radsPerSecondCurrent;

    double slewDestinationDistHA = 0.0;
    double slewDestinationDistDec = 0.0;

    GotoSettings settings = {MFLIP_AUTOMATIC_DEFAULT == ON, MFLIP_PAUSE_HOME_DEFAULT == ON, (PierSideSelect)PIER_SIDE_PREFERRED_DEFAULT, 1000001.0F};
};

extern Goto goTo;

#endif
