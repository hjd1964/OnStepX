//--------------------------------------------------------------------------------------------------
// telescope mount control
#pragma once
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../debug/Debug.h"
#include "../commands/ProcessCmds.h"
#include "../StepDrivers/StepDrivers.h"
#include "Axis.h"

typedef struct Limits {
  double minAltitude;
  double maxAltitude;
  double pastMeridianE;
  double pastMeridianW;
  bool   autoMeridianFlip;
} Limits;

enum MeridianFlip     {MF_NEVER, MF_ALWAYS};
enum RateCompensation {RC_NONE, RC_REFR_RA, RC_REFR_BOTH, RC_FULL_RA, RC_FULL_BOTH};
enum TrackingState    {TS_NONE, TS_SIDEREAL};
enum GotoState        {GS_NONE, GS_GOTO, GS_GOTO_SYNC, GS_GOTO_ABORT};
enum GotoStage        {GG_NONE, GG_START, GG_WAYPOINT, GG_DESTINATION};
enum GotoType         {GT_NONE, GT_HOME, GT_PARK};
enum GuideState       {GU_NONE, GU_GUIDE, GU_PULSE_GUIDE};
enum GuideRate        {GR_QUARTER, GR_HALF, GR_1X, GR_2X, GR_4X, GR_8X, GR_20X, GR_48X, GR_HALF_MAX, GR_MAX, GR_CUSTOM};
enum ParkState        {PS_NONE, PS_UNPARKED, PS_PARKING, PS_PARKED, PS_PARK_FAILED};
enum PecState         {PEC_NONE, PEC_READY_PLAY, PEC_PLAY, PEC_READY_RECORD, PEC_RECORD};

class Mount {
  public:
    void init(int8_t mountType);

    // handle mount commands
    bool command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError);

    // the goto monitor
    void monitor();

  private:
    // check mount status ahead of sync or goto
    CommandError validateGoto();
    CommandError validateGotoCoords(Coordinate coords);

    CommandError setMountTarget(Coordinate *coords, bool pierSideChangeCheck);

    // sync. to equatorial coordinates
    CommandError syncEqu(Coordinate *coords);

    // goto equatorial coordinates
    CommandError gotoEqu(Coordinate *coords);

    void setWaypoint();

    void pierSide();

    // change tracking state
    void setTrackingState(TrackingState state);

    // update where we are pointing *now*
    void updatePosition();

    // this updates differiental tracking rates for refraction, pointing model, and alt/azm tracking
    void updateTrackingRates();

    // estimate average microseconds per step lower limit
    double usPerStepLowerLimit();

    // clear any general errors as appropriate for a reset
    void resetGeneralErrors();

    double     timerRateRatio      = 1.0;

    uint8_t    mountType           = 0;
    bool       soundEnabled        = false;
    bool       syncToEncodersOnly  = false;

    // tracking
    Coordinate current;
    bool       tracking            = false;
    double     radsPerCentisecond  = 0.0;
    double     trackingRate        = 1.0;
    double     trackingRateAxis1   = 0.0;
    double     trackingRateAxis2   = 0.0;
    double     deltaRateAxis1      = 0.0;
    double     deltaRateAxis2      = 0.0;
    double     gotoRateAxis1       = 0.0;
    double     gotoRateAxis2       = 0.0;
    double     stepsPerSecondAxis1 = AXIS1_STEPS_PER_DEGREE/240.0;
    TrackingState trackingState    = TS_NONE;
    #if TRACK_REFRACTION_RATE_DEFAULT == ON
      RateCompensation rateCompensation = RC_REFR_RA;
    #else
      RateCompensation rateCompensation = RC_NONE;
    #endif

    // goto
    Coordinate gotoTarget;
    Coordinate start, destination, target;
    GotoState  gotoState           = GS_NONE;
    GotoStage  gotoStage           = GG_START;
    GotoState  gotoStateAbort      = GS_NONE;
    GotoState  gotoStateLast       = GS_NONE;
    MeridianFlip meridianFlip      = MF_ALWAYS;
    int8_t     preferredPierSide   = PIER_SIDE_PREFERRED_DEFAULT;
    bool       autoMeridianFlip    = false;
    uint8_t    monitorTaskHandle   = 0;
    double     gotoTargetAxis1     = 0.0;
    double     gotoTargetAxis2     = 0.0;
    double     usPerStepCurrent    = 64.0;
    double     usPerStepDefault    = 64.0;
    double     usPerStepBase       = 128.0;

    // limits
    Limits limits = { degToRad(-10), degToRad(85), degToRad(15), degToRad(15), false };
    bool       safetyLimitsOn      = false;

    // homing
    Coordinate home;
    bool       atHome              = true;
    bool       waitingHome         = false;
    bool       waitingHomeContinue = false;
    bool       pauseHome           = false;
  
    // guiding
    double     guideRateAxis1      = 0.0;
    double     guideRateAxis2      = 0.0;
    GuideState guideState          = GU_NONE;
    GuideRate  guideRate           = GR_20X;
    GuideRate  pulseGuideRate      = GR_1X;

    // pec
    PecState   pecState            = PEC_NONE;
    bool       pecRecorded         = false;

    // park
    ParkState  parkState           = PS_UNPARKED;

};

#endif
