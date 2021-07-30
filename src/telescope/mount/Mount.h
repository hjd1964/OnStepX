//--------------------------------------------------------------------------------------------------
// telescope mount control
#pragma once

#include "../../Common.h"

#ifdef MOUNT_PRESENT

#if PEC_STEPS_PER_WORM_ROTATION == 0
  #define AXIS1_PEC OFF
#else
  #define AXIS1_PEC ON
#endif

#include "../../commands/ProcessCmds.h"
#include "../../lib/sound/Sound.h"
#include "../axis/Axis.h"
#include "coordinates/Transform.h"
#include "library/Library.h"

enum MeridianFlip: uint8_t     {MF_NEVER, MF_ALWAYS};
enum RateCompensation: uint8_t {RC_NONE, RC_REFR_RA, RC_REFR_BOTH, RC_FULL_RA, RC_FULL_BOTH};
enum TrackingState: uint8_t    {TS_NONE, TS_SIDEREAL};
enum GotoState: uint8_t        {GS_NONE, GS_GOTO};
enum GotoStage: uint8_t        {GG_NONE, GG_WAYPOINT, GG_DESTINATION, GG_READY_ABORT, GG_ABORT};
enum GotoType: uint8_t         {GT_NONE, GT_HOME, GT_PARK};
enum PierSideSelect: uint8_t   {PSS_NONE, PSS_EAST, PSS_WEST, PSS_BEST, PSS_EAST_ONLY, PSS_WEST_ONLY, PSS_SAME_ONLY};
enum GuideState: uint8_t       {GU_NONE, GU_PULSE_GUIDE, GU_GUIDE, GU_SPIRAL_GUIDE, GU_HOME_GUIDE};
enum GuideRateSelect: uint8_t  {GR_QUARTER, GR_HALF, GR_1X, GR_2X, GR_4X, GR_8X, GR_20X, GR_48X, GR_HALF_MAX, GR_MAX, GR_CUSTOM};
enum GuideAction: uint8_t      {GA_NONE, GA_BREAK, GA_FORWARD, GA_REVERSE, GA_SPIRAL, GA_HOME };
enum ParkState: uint8_t        {PS_NONE, PS_UNPARKED, PS_PARKING, PS_PARKED, PS_PARK_FAILED};
enum PecState: uint8_t         {PEC_NONE, PEC_READY_PLAY, PEC_PLAY, PEC_READY_RECORD, PEC_RECORD};
enum CoordReturn: uint8_t      {CR_MOUNT, CR_MOUNT_EQU, CR_MOUNT_ALT, CR_MOUNT_HOR, CR_MOUNT_ALL};

typedef struct MeridianFlipHome {
  bool paused;
  bool resume;
} MeridianFlipHome;

#pragma pack(1)
typedef struct AltitudeLimits {
  float min;
  float max;
} AltitudeLimits;

#define LimitsSize 16
typedef struct Limits {
  AltitudeLimits altitude;
  float pastMeridianE;
  float pastMeridianW;
} Limits;

typedef struct Backlash {
  float axis1;
  float axis2;
} Backlash;

#define MiscSize 14
typedef struct Misc {
  bool syncToEncodersOnly:1;
  bool meridianFlipAuto  :1;
  bool meridianFlipPause :1;
  bool buzzer            :1;
  float usPerStepCurrent;
  GuideRateSelect pulseGuideRateSelect;
  Backlash backlash;
} Misc;

#define PecSize 6
typedef struct Pec {
  bool recorded:1;
  PecState state;
  long wormRotationSteps;
} Pec;

typedef struct ParkPosition {
  float h;
  float d;
  PierSide pierSide;
} ParkPosition;

#define ParkSize 15
typedef struct Park {
  ParkPosition position;
  bool         saved:1;
  ParkState    state;
  long         wormSensePositionSteps;
} Park;
#pragma pack()

typedef struct AlignState {
  uint8_t currentStar;
  uint8_t lastStar;
} AlignState;

typedef struct MinMaxError {
  uint8_t min:1;
  uint8_t max:1;
} MinMaxError;

typedef struct MerdianError {
  uint8_t east:1;
  uint8_t west:1;
} MerdianError;

typedef struct LimitError {
  MinMaxError axis1;
  MinMaxError axis2;
} LimitError;

typedef struct MountError {
  MinMaxError   altitude;
  LimitError    limit;
  LimitError    limitSense;
  MerdianError  meridian;
  uint8_t       parkFailed: 1;
  uint8_t       motorFault: 1;
  uint8_t       initWeather: 1;
} MountError;

class Mount {
  public:
    void init(bool validKey);

    // handle mount commands
    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);
    bool commandGoto(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);
    bool commandGuide(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);
    bool commandLimit(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);
    bool commandPec(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // get current equatorial position (Native coordinate system)
    Coordinate getPosition();

    // get target equatorial position (Native coordinate system)
    Coordinate getTarget();

    // set target equatorial position (Native coordinate system)
    void setTarget(Coordinate *coords);

    // goto target equatorial position
    CommandError requestGoto();

    // update the home position
    void updateHomePosition();

    // reset all mount related errors
    void errorReset();

    // check for any mount related error
    bool errorAny();

    // return OnStep general error code
    uint8_t errorNumber();

    // monitor goto operation
    void gotoPoll();

    // monitor guide operation
    void guidePoll();

    // monitor pec operation
    void pecPoll();

    // monitor limits
    void limitPoll();

    // calculate tracking rates for alt/azm, refraction, and pointing model in the background
    void trackPoll();

    // monitor ST4 guide port for guiding, basic hand controller, and smart hand controller
    void st4Poll();

    Transform transform;

    Axis axis1;
    Axis axis2;

    MountError error;

  private:
    // general status checks ahead of sync or goto
    CommandError validateGotoState();
    // target coordinate checks ahead of sync or goto
    CommandError validateGotoCoords(Coordinate *coords);
    // set goto or sync target
    CommandError setGotoTarget(Coordinate *coords, PierSideSelect pierSideSelect, bool native = true);
    // sync. to equatorial position (Native coordinate system)
    CommandError syncEqu(Coordinate *coords, PierSideSelect pierSideSelect, bool native = true);
    // goto equatorial position (Native coordinate system)
    CommandError gotoEqu(Coordinate *coords, PierSideSelect pierSideSelect, bool native = true);
    // set any waypoints required for a goto
    void gotoWaypoint();
    // stop any presently active goto
    void gotoStop();

    // return mount to the home position
    CommandError returnHome();
    // reset mount at home
    CommandError resetHome(bool resetPark);

    // change tracking state
    void setTrackingState(TrackingState state);
    // Alternate tracking rate calculation method
    float ztr(float a);

    // update where we are pointing *now*
    // CR_MOUNT for Horizon or Equatorial mount coordinates, depending on mode
    // CR_MOUNT_EQU for Equatorial mount coordinates, depending on mode
    // CR_MOUNT_ALT for altitude (a) and Horizon or Equatorial mount coordinates, depending on mode
    // CR_MOUNT_HOR for Horizon mount coordinates, depending on mode
    void updatePosition(CoordReturn coordReturn);

    // update tracking rates
    void updateTrackingRates();
    // update acceleration rates for goto and guiding
    void updateAccelerationRates();
    // estimate average microseconds per step lower limit
    float usPerStepLowerLimit();

    CommandError alignAddStar();

    // keep guide rate <= half max
    float limitGuideRate(float rate);
    // return guide rate (sidereal x) for guide rate selection
    float guideRateSelectToRate(GuideRateSelect guideRateSelect, uint8_t axis = 1);
    // valid guide on Axis1
    bool guideValidAxis1(GuideAction guideAction);
    // valid guide on Axis2
    bool guideValidAxis2(GuideAction guideAction);
    // valididate guide request
    CommandError guideValidate(int axis, GuideAction guideAction);
    // start axis1 movement
    void guideAxis1AutoSlew(GuideAction guideAction);
    // start axis2 movement
    void guideAxis2AutoSlew(GuideAction guideAction);

    // start guide on Axis1
    CommandError guideStartAxis1(GuideAction guideAction, GuideRateSelect guideRateSelect, unsigned long guideTimeLimit);
    // stop guide on Axis1, use GA_BREAK to stop in either direction or specifiy the direction to be stopped GA_FORWARD or GA_REVERSE
    void guideStopAxis1(GuideAction stopDirection, bool abort = false);

    // start guide on Axis2
    CommandError guideStartAxis2(GuideAction guideAction, GuideRateSelect guideRateSelect, unsigned long guideTimeLimit);
    // stop guide on Axis2, use GA_BREAK to stop in either direction or specifiy the direction to be stopped GA_FORWARD or GA_REVERSE
    void guideStopAxis2(GuideAction stopDirection, bool abort = false);

    // start spiral guide
    CommandError guideSpiralStart(GuideRateSelect guideRateSelect, unsigned long guideTimeLimit);
    // stop spiral guide
    void guideSpiralStop();
    // manage spiral guide
    void guideSpiralPoll();

    void st4Init();

    void pecInit(bool validKey);
    #if AXIS1_PEC == ON
      // disable PEC
      void pecDisable();
      // PEC low pass and linear regression filters
      void pecCleanup();
    #endif

    // initialize park info.
    void parkInit(bool validKey);
    // sets a park position
    CommandError parkSet();
    // goto park position
    CommandError parkGoto();
    // once parked save park state
    void parkFinish();
    // returns a parked telescope to operation
    CommandError parkRestore(bool withTrackingOn);

    // read in the limit information, start limit monitor
    void limitInit(bool validKey);
    void limitBroken();
    void limitStop(GuideAction stopDirection);
    void limitStopAxis1(GuideAction stopDirection);
    void limitStopAxis2(GuideAction stopDirection);

    void statusInit();
    void statusSetPeriodMillis(int period);

    Sound sound;
    Library library;

    const double radsPerCentisecond  = (degToRad(15.0/3600.0)/100.0)*SIDEREAL_RATIO;
    
    // current position in Mount coordinates (Observed Place with no corrections except index offset)
    // coordinates are either Horizon (a, z) or Equatorial (h, d) depending on the mountType
    // also includes Mount normalized axis coordinates (a1, a2) where a2 is an instrument coordinate in tangent arm mode
    Coordinate current;
    TrackingState trackingState         = TS_NONE;
    #if TRACK_REFRACTION_RATE_DEFAULT == ON
      RateCompensation rateCompensation = RC_REFR_RA;
    #else
      RateCompensation rateCompensation = RC_NONE;
    #endif
    float trackingRate                  = 1.0F;
    float trackingRateAxis1             = 0.0F;
    float trackingRateAxis2             = 0.0F;
    float stepsPerSiderealSecondAxis1   = 0.0F;
    float stepsPerCentisecondAxis1      = 0.0F;
    float stepsPerCentisecondAxis2      = 0.0F;

    // align
    AlignState alignState = {0, 0};

    // goto
    PierSideSelect preferredPierSide    = (PierSideSelect)PIER_SIDE_PREFERRED_DEFAULT;
    Coordinate gotoTarget;
    Coordinate start, destination, target;
    GotoState  gotoState                = GS_NONE;
    GotoStage  gotoStage                = GG_NONE;
    GotoState  gotoStateAbort           = GS_NONE;
    GotoState  gotoStateLast            = GS_NONE;
    uint8_t    gotoTaskHandle           = 0;
    float      usPerStepDefault         = 64.0F;
    float      usPerStepBase            = 128.0F;
    float      radsPerSecondCurrent;

    MeridianFlip meridianFlip           = MF_ALWAYS;
    MeridianFlipHome meridianFlipHome = {false, false};

    // limits
    bool       limitsEnabled            = false;
    Limits limits = { { degToRadF(-10.0F), degToRadF(85.0F) }, degToRadF(15.0F), degToRadF(15.0F) };

    // homing
    Coordinate home;
    bool       atHome                   = true;

    // guiding
    float guideRateAxis1                = 0.0F;
    float guideRateAxis2                = 0.0F;
    float customGuideRateAxis1          = 0.0F;
    float customGuideRateAxis2          = 0.0F;
    GuideState      guideState          = GU_NONE;
    GuideRateSelect guideRateSelect     = GR_20X;  // currently selected guide rate
    GuideRateSelect guideRateSelectAxis1= GR_20X;
    GuideRateSelect guideRateSelectAxis2= GR_20X;
    GuideAction     guideActionAxis1    = GA_NONE;
    GuideAction     guideActionAxis2    = GA_NONE;
    float         spiralScaleAxis1      = 0.0F;
    unsigned long spiralStartTime       = 0;
    unsigned long guideFinishTimeAxis1  = 0;
    unsigned long guideFinishTimeAxis2  = 0;

    // pec
    long      pecBufferSize             = 0;      // pec buffer size in bytes
    float     pecRateAxis1              = 0;      // tracking rate (in x) due to PEC playing
    #if AXIS1_PEC == ON
      uint8_t  pecMonitorHandle         = 0;
      uint8_t  pecSenseHandle           = 0;

      Pec pec = {false, PEC_NONE, PEC_STEPS_PER_WORM_ROTATION};

      int      pecAnalogValue           = 0;

      bool     wormIndexState           = false;
      bool     wormIndexSenseThisSecond = false;
      long     wormRotationSteps        = 0;      // step position in worm rotation sequence
      long     wormSenseSteps           = 0;      // step position

      bool     pecFirstRecording        = false;
      uint32_t pecRecordStopTimeCs      = 0;
      uint32_t wormRotationStartTimeCs  = 0;      // start time of worm rotation sequence, in centi-seconds
      long     wormRotationSeconds      = 0;      // time for a worm rotation, in seconds

      float    pecAccGuideAxis1         = 0.0F;

      bool     pecBufferStart           = false;
      long     pecBufferIndex           = 0;      // index into the pec buffer
      int8_t*  pecBuffer;
    #endif

    // park
    Park park = {{0, 0, PIER_SIDE_NONE}, false, PS_NONE, 0};

    // status LED task
    uint8_t    statusTaskHandle         = 0;

    // misc. settings stored in NV
    Misc misc = {false, false, false, STATUS_BUZZER_DEFAULT == ON, 64.0, GR_1X, {0, 0}};

    float timerRateRatio = 1.0;
};

#endif
