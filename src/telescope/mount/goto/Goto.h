//--------------------------------------------------------------------------------------------------
// telescope mount control
#pragma once

#include "../../../Common.h"

#if defined(MOUNT_PRESENT)

#include "../../../libApp/commands/ProcessCmds.h"
#include "../coordinates/Transform.h"

#if GOTO_AVOID_OVERHEAD == ON
  #include "GotoPlanner.h"
#endif

enum MeridianFlip: uint8_t         {MF_NEVER, MF_ALWAYS};
enum MeridianFlipHomeMode: uint8_t {MFHM_OFF, MFHM_VISIT, MFHM_PAUSE};
enum GotoState: uint8_t            {GS_NONE, GS_GOTO};
enum GotoStage: uint8_t            {GG_NONE, GG_ABORT, GG_READY_ABORT,
                                    #if GOTO_AVOID_OVERHEAD == ON
                                      GG_PLAN, GG_WAYPOINT_PLANNED,
                                    #endif
                                    GG_WAYPOINT_HOME, GG_WAYPOINT_AVOID, GG_NEAR_DESTINATION_START, GG_NEAR_DESTINATION_WAIT, GG_NEAR_DESTINATION, GG_DESTINATION};
enum GotoType: uint8_t             {GT_NONE, GT_HOME, GT_PARK};
enum PierSideSelect: uint8_t       {PSS_NONE, PSS_EAST, PSS_WEST, PSS_BEST, PSS_AUTO, PSS_EAST_ONLY, PSS_WEST_ONLY, PSS_SAME_ONLY};
#if GOTO_AVOID_OVERHEAD == ON
enum AutoFlipState: uint8_t        {AFS_NONE, AFS_ACTIVE, AFS_FAILED};
#endif

typedef struct MeridianFlipHome {
  bool paused;
  bool resume;
} MeridianFlipHome;

#pragma pack(1)
typedef struct GotoSettings {
  bool meridianFlipAuto;
  MeridianFlipHomeMode meridianFlipHomeMode;
  PierSideSelect preferredPierSide;
  float usPerStepCurrent;
} GotoSettings;
#pragma pack()

constexpr MeridianFlipHomeMode meridianFlipHomeModeDefault =
  MFLIP_HOME_DEFAULT == OFF ? MFHM_OFF : MFLIP_HOME_DEFAULT == VISIT ? MFHM_VISIT : MFHM_PAUSE;

typedef struct AlignState {
  uint8_t currentStar;
  uint8_t lastStar;
} AlignState;

class Goto {
  public:
    void init();

    bool command(char *reply, char *command, char *parameter, bool *suppressFrame, bool *numericReply, CommandError *commandError);

    // goto to equatorial target position (Native coordinate system) using the defaut preferredPierSide
    CommandError request();

    // goto equatorial position (Native or Mount coordinate system)
    CommandError request(Coordinate coords, PierSideSelect pierSideSelect, bool native = true);

    #if GOTO_AVOID_OVERHEAD == ON
    // goto equatorial position (Mount coordinate system) for an automatic meridian flip
    CommandError requestAutoFlip(Coordinate coords, PierSideSelect pierSideSelect);

    // state of the automatic meridian flip from this pier side, a failed flip is reported once
    AutoFlipState autoFlipState(PierSide pierSide);
    #endif

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

    // abort any presently active goto
    void abort();

    // general status checks ahead of sync or goto
    CommandError validate();

    // add an align star (at the current position relative to target)
    CommandError alignAddStar(bool sync = false);

    // reset the alignment model
    void alignReset();

    // check if an align is in progress
    inline bool alignActive() { return alignState.lastStar > 0 && alignState.currentStar <= alignState.lastStar; }

    // check if an align is done
    inline bool alignDone() { return alignState.lastStar > 0 && alignState.currentStar > alignState.lastStar; }

    // returns true if the pause at home mode is selected
    inline bool isHomePauseEnabled() { return settings.meridianFlipHomeMode == MFHM_PAUSE; }

    // returns the meridian flip home behavior
    inline MeridianFlipHomeMode getHomeMode() { return settings.meridianFlipHomeMode; }

    // returns true if paused at the home position, waiting to continue, during a goto
    inline bool isHomePaused() { return meridianFlipHome.paused; }

    // if paused at the home position, continue the goto
    inline void homeContinue() { meridianFlipHome.resume = true; }

    // returns true if the automatic meridian flip feature is enabled
    inline bool isAutoFlipEnabled() { return settings.meridianFlipAuto && transform.isEquatorial() && transform.meridianFlips; }

    // return selected slew rate
    inline float getRadsPerSecond() { return radsPerSecondCurrent; }

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
    bool encodersPresent = false;

  private:

    #if GOTO_FEATURE == ON
    // set any additional destinations required for a goto
    void waypoint(Coordinate *current);

    // start slews with approach correction and parking/homing support
    CommandError startAutoSlew();

    #if GOTO_AVOID_OVERHEAD == ON
    // true when slews are planned to keep clear of the overhead limit
    bool planNeeded();

    // plan each slew of the goto ahead, so a goto without a clear path is refused
    CommandError planRoute();

    // start the next slew once a path clear of the overhead limit is planned
    CommandError startSlew();

    // monitor planning and start the planned slew
    void planPoll();

    // stop the goto when a slew can't be planned
    void planFailed();

    // planning must finish within this time before the first slew starts
    void planTimeLimit(unsigned long milliseconds);

    // instrument coordinates the next slew goes to
    PlanPoint slewTarget();

    // instrument coordinates of a slew destination (Mount coordinate system), optionally with the approach offset
    PlanPoint instrumentTarget(Coordinate coords, bool approach);

    // slew to a planned waypoint (instrument coordinates)
    CommandError startWaypointSlew(PlanPoint waypoint);
    #else
    inline CommandError startSlew() { return startAutoSlew(); }
    #endif
    #endif

    // update acceleration rates for goto and guiding
    void updateAccelerationRates();

    // estimate average microseconds per step lower limit
    float usPerStepLowerLimit();

    // get least distance between coordinates
    inline double dist(double a, double b) { if (a > b) return a - b; else return b - a; }

    // requested goto/sync destination Native coordinate (eq or hor)
    Coordinate gotoTarget = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, PIER_SIDE_NONE};
    // goto starts from this Mount coordinate (eq or hor)
    Coordinate start;
    // goto next destination Mount coordinate (eq or hor)
    Coordinate destination;
    // goto final destination Mount coordinate (eq or hor)
    Coordinate target = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, PIER_SIDE_NONE};
    // last align (goto) target Mount coordinate (eq or hor)
    Coordinate lastAlignTarget = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, PIER_SIDE_NONE};
    GotoState  stateAbort           = GS_NONE;
    GotoState  stateLast            = GS_NONE;
    uint8_t    taskHandle           = 0;
    int        nearDestinationRefineStages;
    unsigned long nearTargetTimeout = 0;
    unsigned long nearTargetTimeoutAxis1 = 0;
    unsigned long nearTargetTimeoutAxis2 = 0;
    unsigned long nearDestinationTimeout = 0;

    MeridianFlipHome meridianFlipHome = {false, false};

    #if GOTO_AVOID_OVERHEAD == ON
    uint8_t       planTaskHandle      = 0;
    GotoStage     planStage           = GG_NONE;
    PlanPoint     planStart           = {0.0F, 0.0F};
    uint8_t       planAttempts        = 0;
    uint8_t       planWaypoint        = 0;
    unsigned long planDeadline        = 0;
    bool          planDeadlineActive  = false;
    bool          planRecovery        = false;
    bool          slowdownReset       = false;
    bool          autoFlip            = false;
    bool          autoFlipRequest     = false;
    bool          autoFlipFailed      = false;
    PierSide      autoFlipPierSide    = PIER_SIDE_NONE;
    #endif

    AlignState alignState = {0, 0};

    float      usPerStepBase        = 128.0F;
    float      radsPerSecondCurrent;

    double slewDestinationDistHA = 0.0;
    double slewDestinationDistDec = 0.0;

    GotoSettings settings = {MFLIP_AUTOMATIC_DEFAULT == ON, meridianFlipHomeModeDefault, (PierSideSelect)PIER_SIDE_PREFERRED_DEFAULT, 1000001.0F};

    uint32_t nvKey;
};

extern Goto goTo;

#endif
