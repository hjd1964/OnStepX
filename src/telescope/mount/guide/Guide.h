//--------------------------------------------------------------------------------------------------
// telescope mount control
#pragma once

#include "../../../Common.h"

#ifdef MOUNT_PRESENT

#include "../../../libApp/commands/ProcessCmds.h"
#include "../../../lib/sound/Sound.h"
#include "../../../lib/axis/Axis.h"
#include "../coordinates/Transform.h"

// default time limit for guiding home is 10 minutes
#define GUIDE_HOME_TIME_LIMIT 600.0

// default time for spiral guides is 103.4 seconds
#define GUIDE_SPIRAL_TIME_LIMIT 103.4
enum GuideState: uint8_t       {GU_NONE, GU_PULSE_GUIDE, GU_GUIDE, GU_SPIRAL_GUIDE, GU_HOME_GUIDE, GU_HOME_GUIDE_ABORT};
enum GuideRateSelect: uint8_t  {GR_QUARTER, GR_HALF, GR_1X, GR_2X, GR_4X, GR_8X, GR_20X, GR_48X, GR_HALF_MAX, GR_MAX, GR_CUSTOM};
enum GuideAction: uint8_t      {GA_NONE, GA_BREAK, GA_FORWARD, GA_REVERSE, GA_SPIRAL, GA_HOME };

#pragma pack(1)
#define GuideSettingsSize 3
typedef struct GuideSettings {
  GuideRateSelect pulseRateSelect;
  GuideRateSelect axis1RateSelect;
  GuideRateSelect axis2RateSelect;
} GuideSettings;
#pragma pack()

class Guide {
  public:
    void init();

    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // start guide at a given direction and rate on Axis1
    CommandError startAxis1(GuideAction guideAction, GuideRateSelect rateSelect, unsigned long guideTimeLimit);

    // stop guide on Axis1, use GA_BREAK to stop in either direction or specifiy the direction to be stopped GA_FORWARD or GA_REVERSE
    // set abort true to rapidly stop (broken limit, etc)
    void stopAxis1(GuideAction stopDirection = GA_BREAK, bool abort = false);

    // start guide at a given direction and rate on Axis2
    CommandError startAxis2(GuideAction guideAction, GuideRateSelect rateSelect, unsigned long guideTimeLimit);

    // stop guide on Axis2, use GA_BREAK to stop in either direction or specifiy the direction to be stopped GA_FORWARD or GA_REVERSE
    // set abort true to rapidly stop (broken limit, etc)
    void stopAxis2(GuideAction stopDirection = GA_BREAK, bool abort = false);

    // start spiral guide at the specified rate (spiral size is porportional to rate)
    CommandError startSpiral(GuideRateSelect rateSelect, unsigned long guideTimeLimit);

    // start guide home (for use with home switches)
    CommandError startHome();

    // stop both axes of guide
    void stop();

    // abort both axes of guide
    void abort();

    // returns true if a pulse guide is happening
    inline bool activePulseGuide() { return state == GU_PULSE_GUIDE; }

    // returns true if a guide is happening
    inline bool active() {
      return (guideActionAxis1 != GA_NONE || guideActionAxis2 != GA_NONE) && !activePulseGuide(); 
    }

    // returns true if any guide is happening on Axis1
    inline bool activeAxis1() { return guideActionAxis1 != GA_NONE; }

    // returns true if any guide is happening on Axis2
    inline bool activeAxis2() { return guideActionAxis2 != GA_NONE; }

    void poll();

    void spiralPoll();

    // enables or disables backlash for the GUIDE_DISABLE_BACKLASH option
    void backlashEnableControl(bool enable);

    GuideState state = GU_NONE;

    float rateAxis1 = 0.0F;
    float rateAxis2 = 0.0F;

    GuideSettings settings = { GR_HALF, GR_20X, GR_20X };

  private:
    // keep guide rate <= half max
    float limitGuideRate(float rate);

    // return guide rate (sidereal x) for guide rate selection
    float rateSelectToRate(GuideRateSelect rateSelect, uint8_t axis = 1);

    // valid guide for Axis1
    bool validAxis1(GuideAction guideAction);

    // valid guide for Axis2
    bool validAxis2(GuideAction guideAction);

    // general validation of guide request
    CommandError validate(int axis, GuideAction guideAction);

    // start axis1 movement
    void axis1AutoSlew(GuideAction guideAction);

    // start axis2 movement
    void axis2AutoSlew(GuideAction guideAction);

    GuideRateSelect spiralGuideRateSelect = GR_20X;
    
    GuideAction guideActionAxis1 = GA_NONE;
    GuideAction guideActionAxis2 = GA_NONE;

    PierSide pierSide = PIER_SIDE_NONE;

    float customRateAxis1 = 0.0F;
    float customRateAxis2 = 0.0F;

    float spiralScaleAxis1 = 0.0F;
    unsigned long spiralStartTime = 0;

    unsigned long guideFinishTimeAxis1 = 0;
    unsigned long guideFinishTimeAxis2 = 0;

};

extern Guide guide;

#endif
