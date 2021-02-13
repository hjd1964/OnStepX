//--------------------------------------------------------------------------------------------------
// telescope control
#pragma once
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../commands/ProcessCmds.h"
#include "../StepDrivers/StepDrivers.h"
#include "Axis.h"

typedef struct Limits {
  double horizon;
  double overhead;
  double pastMeridianE;
  double pastMeridianW;
} Limits;

enum MeridianFlip     {MF_NEVER, MF_ALWAYS};
enum RateCompensation {RC_NONE, RC_REFR_RA, RC_REFR_BOTH, RC_FULL_RA, RC_FULL_BOTH};
enum TrackingState    {TS_NONE, TS_SIDEREAL};
enum GotoState        {GS_NONE, GS_GOTO, GS_GOTO_SYNC, GS_GOTO_HOME, GS_GOTO_PARK, GS_GOTO_ABORT};
enum GuideState       {GU_NONE, GU_GUIDE, GU_GUIDE_STOP};
enum ParkState        {PS_NONE, PS_UNPARKED, PS_PARKING, PS_PARKED, PS_PARK_FAILED};

class Mount {
  public:
    void init(int8_t mountType);

    // handle telescope commands
    bool command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError);

  private:
    // check mount status ahead of sync or goto
    CommandError validateGoto();
    CommandError validateGotoCoords(Coordinate coords);

    // sync. to equatorial coordinates
    CommandError syncEqu(Coordinate target);

    // update where we are pointing *now*
    void updatePosition();

    // this updates differiental tracking rates for refraction, pointing model, and alt/azm tracking
    void updateTrackingRates();

    // clear any general errors as appropriate for a reset
    void resetGeneralErrors();

    
    Coordinate current, target;

    uint8_t mountType          = 0;
    bool    tracking           = false;
    bool    atHome             = true;
    bool    safetyLimitsOn     = false;
    bool    syncToEncodersOnly = false;

    Limits limits = { degToRad(-10), degToRad(85), degToRad(15), degToRad(15) };

    #if TRACK_REFRACTION_RATE_DEFAULT == ON
      RateCompensation rateCompensation = RC_REFR_RA;
    #else
      RateCompensation rateCompensation = RC_NONE;
    #endif
    bool rateCompensationDualAxis = false;

    MeridianFlip meridianFlip = MF_NEVER;

    double trackingRate         = 1.0;
    double trackingRateAxis1    = 0.0;
    double trackingRateAxis2    = 0.0;
    double guideRateAxis1       = 1.0;
    double guideRateAxis2       = 1.0;
    double deltaRateAxis1       = 1.0;
    double deltaRateAxis2       = 1.0;
    TrackingState trackingState = TS_NONE;

    GotoState gotoState         = GS_NONE;
    GotoState gotoStateAbort    = GS_NONE;
    GotoState gotoStateLast     = GS_NONE;

    GuideState guideState       = GU_NONE;

    ParkState parkState         = PS_UNPARKED;

};

#endif
