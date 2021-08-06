//--------------------------------------------------------------------------------------------------
// telescope mount control, guiding

#include "Guide.h"

#ifdef MOUNT_PRESENT

#include "../../../commands/ProcessCmds.h"
#include "../../../tasks/OnTask.h"
extern Tasks tasks;

#include "../../Telescope.h"
#include "../Mount.h"
#include "../goto/Goto.h"
#include "../home/Home.h"
#include "../park/Park.h"
#include "../home/Home.h"
#include "../limits/Limits.h"

inline void guideWrapper() { guide.poll(); }

void Guide::init() {
  // confirm the data structure size
  if (GuideSettingsSize < sizeof(GuideSettings)) { initError.nv = true; DL("ERR: Guide::init(); GuideSettingsSize error NV subsystem writes disabled"); nv.readOnly(true); }

  // write the default settings to NV
  if (!validKey) {
    VLF("MSG: Mount, guide writing default settings to NV");
    nv.writeBytes(NV_MOUNT_GUIDE_BASE, &settings, sizeof(GuideSettings));
  }

  // read the settings
  nv.readBytes(NV_MOUNT_GUIDE_BASE, &settings, sizeof(GuideSettings));

  // reset normal guide rate to 20X, value stored in NV isn't used
  settings.rateSelect = GR_20X;

  // start guide monitor task
  VF("MSG: Mount, start guide monitor task (rate 10ms priority 1)... ");
  if (tasks.add(10, 0, true, 1, guideWrapper, "MntGuid")) { VL("success"); } else { VL("FAILED!"); }
}

// start guide at a given direction and rate on Axis1
CommandError Guide::startAxis1(GuideAction guideAction, GuideRateSelect rateSelect, unsigned long guideTimeLimit) {
  if (guideAction == GA_NONE || guideActionAxis1 == guideAction) return CE_NONE;
  CommandError e = validate(1, guideAction); if (e != CE_NONE) return e;

  guideActionAxis1 = guideAction;
  float rate = rateSelectToRate(rateSelect);

  VF("MSG: guideStartAxis1(); guide ");
  if (guideAction == GA_REVERSE) { VF("reverse"); } else { VF("forward"); }
  VF(" started at "); V(rate); VL("X");

  // unlimited 0 means the maximum period, about 49 days
  if (guideTimeLimit == 0) guideTimeLimit = 0x1FFFFFFF;
  guideFinishTimeAxis1 = millis() + guideTimeLimit;

  if (rate <= 2) {
    state = GU_PULSE_GUIDE;
    mount.axis1.setPowerDownOverrideTime(30000);
    mount.axis2.setPowerDownOverrideTime(30000);
    if (guideAction == GA_REVERSE) rateAxis1 = -rate; else rateAxis1 = rate;
    mount.update();
  } else {
    state = GU_GUIDE;
    mount.axis1.setFrequencySlew(degToRadF(rate/240.0F));
    axis1AutoSlew(guideAction);
  }

  return CE_NONE;
}

// stop guide on Axis1, use GA_BREAK to stop in either direction or specifiy the direction to be stopped GA_FORWARD or GA_REVERSE
// set abort true to rapidly stop (broken limit, etc)
void Guide::stopAxis1(GuideAction stopDirection, bool abort) {
  if (guideActionAxis1 > GA_BREAK) {
    if (stopDirection != GA_BREAK && stopDirection != guideActionAxis1) return;
    if (rateAxis1 == 0.0F) {
      VF("MSG: guideStopAxis1(); requesting guide ");
      guideActionAxis1 = GA_BREAK;
      if (abort) { VL("abort"); mount.axis1.autoSlewAbort(); } else { VL("stop"); mount.axis1.autoSlewStop(); }
    } else {
      VLF("MSG: guideStopAxis1(); guide stopped");
      guideActionAxis1 = GA_NONE;
      rateAxis1 = 0.0F;
      mount.update();
    }
  }
}

// start guide at a given direction and rate on Axis2
CommandError Guide::startAxis2(GuideAction guideAction, GuideRateSelect rateSelect, unsigned long guideTimeLimit) {
  if (guideAction == GA_NONE || guideActionAxis2 == guideAction) return CE_NONE;
  CommandError e = validate(2, guideAction); if (e != CE_NONE) return e;

  guideActionAxis2 = guideAction;
  float rate = rateSelectToRate(rateSelect);

  VF("MSG: guideStartAxis2(); guide ");
  if (guideAction == GA_REVERSE) { VF("reverse"); } else { VF("forward"); }
  VF(" started at "); V(rate); VL("X");

  // unlimited 0 means the maximum period, about 49 days
  if (guideTimeLimit == 0) guideTimeLimit = 0x1FFFFFFF;
  guideFinishTimeAxis2 = millis() + guideTimeLimit;

  if (rate <= 2) {
    state = GU_PULSE_GUIDE;
    mount.axis1.setPowerDownOverrideTime(30000);
    mount.axis2.setPowerDownOverrideTime(30000);
    if (guideAction == GA_REVERSE) rateAxis2 = -rate; else rateAxis2 = rate;
    mount.update();
  } else {
    state = GU_GUIDE;
    mount.axis2.setFrequencySlew(degToRadF(rate/240.0F));
    axis2AutoSlew(guideAction);
  }

  return CE_NONE;
}

// stop guide on Axis2, use GA_BREAK to stop in either direction or specifiy the direction to be stopped GA_FORWARD or GA_REVERSE
// set abort true to rapidly stop (broken limit, etc)
void Guide::stopAxis2(GuideAction stopDirection, bool abort) {
  if (guideActionAxis2 > GA_BREAK) {
    if (stopDirection != GA_BREAK && stopDirection != guideActionAxis2) return;
    if (rateAxis2 == 0.0F) {
      VF("MSG: guideStopAxis2(); requesting guide ");
      guideActionAxis2 = GA_BREAK;
      if (abort) { VL("abort"); mount.axis2.autoSlewAbort(); } else { VL("stop"); mount.axis2.autoSlewStop(); }
    } else {
      VLF("MSG: guideStopAxis2(); guide stopped");
      guideActionAxis2 = GA_NONE;
      rateAxis2 = 0.0F;
      mount.update();
    }
  }
}

// start spiral guide at the specified rate (spiral size is porportional to rate)
CommandError Guide::startSpiral(GuideRateSelect rateSelect, unsigned long guideTimeLimit) {
  if (state == GU_SPIRAL_GUIDE) { stopSpiral(); return CE_NONE; }
  if (guideActionAxis1 != GA_NONE || guideActionAxis2 != GA_NONE) return CE_SLEW_IN_MOTION;
  CommandError e = validate(0, GA_SPIRAL); if (e != CE_NONE) return e;

  if (rateSelect < GR_2X)       rateSelect = GR_2X;
  if (rateSelect > GR_HALF_MAX) rateSelect = GR_HALF_MAX;
  this->settings.rateSelect = rateSelect;

  VF("MSG: guideSpiralStart(); using guide rates to "); V(rateSelectToRate(rateSelect)); VL("X");

  // unlimited 0 means the maximum period, about 49 days
  if (guideTimeLimit == 0) guideTimeLimit = 0x1FFFFFFF;
  spiralStartTime = millis();
  guideFinishTimeAxis1 = spiralStartTime + guideTimeLimit;
  guideFinishTimeAxis2 = guideFinishTimeAxis1;

  // setup and call the polling routine once to start the guides
  Coordinate location = mount.getMountPosition(CR_MOUNT);
  spiralScaleAxis1 = cos(location.a2);
  spiralPoll();

  state = GU_SPIRAL_GUIDE;
  return CE_NONE;
}

// stop spiral guide
void Guide::stopSpiral() {
  stopAxis1(GA_FORWARD);
  stopAxis1(GA_REVERSE);
  stopAxis2(GA_FORWARD);
  stopAxis2(GA_REVERSE);
}

// start guide home (for use with home switches)
CommandError Guide::startHome(unsigned long guideTimeLimit) {
  #if SLEW_GOTO == ON
    mount.axis1.setFrequencySlew(goTo.rate);
    mount.axis2.setFrequencySlew(goTo.rate);
  #else
    mount.axis1.setFrequencySlew(degToRadF(0.2F));
    mount.axis2.setFrequencySlew(degToRadF(0.2F));
  #endif

  // use guiding and switches to find home
  guide.state = GU_HOME_GUIDE;
  guideActionAxis1 = guideActionAxis2 = GA_HOME;
  guideFinishTimeAxis1 = guideFinishTimeAxis2 = millis() + guideTimeLimit; 
  mount.axis1.autoSlewHome();
  mount.axis2.autoSlewHome();

  return CE_NONE;
}

// keep guide rate <= half max
float Guide::limitGuideRate(float rate) {
  #if SLEW_GOTO == ON
    float rateLimit = radToDegF(goTo.rate)*120.0F;
    if (rate > rateLimit) rate = rateLimit;
  #endif
  return rate;
}

// return guide rate (sidereal x) for guide rate selection
float Guide::rateSelectToRate(GuideRateSelect rateSelect, uint8_t axis) {
  switch (rateSelect) {
    case GR_QUARTER: return 0.25F;
    case GR_HALF: return 0.5F;
    case GR_1X: return 1.0F;
    case GR_2X: return 2.0F;
    case GR_4X: return limitGuideRate(4.0F);
    case GR_8X: return limitGuideRate(8.0F);
    case GR_20X: return limitGuideRate(20.0F);
    case GR_48X: return limitGuideRate(48.0F);
    #if SLEW_GOTO == ON
      case GR_HALF_MAX: return radToDegF(goTo.rate)*120.0F;
      case GR_MAX: return radToDegF(goTo.rate)*240.0F;
    #endif
    case GR_CUSTOM: if (axis == 1) return customRateAxis1; else if (axis == 2) return customRateAxis2; else return 0.0F;
    default: return 0.0F;
  }
}

// valid guide for Axis1
bool Guide::validAxis1(GuideAction guideAction) {
  Coordinate location = mount.getMountPosition(CR_MOUNT_ALT);

  if (!limits.isEnabled()) return true;

  double a1;
  if (transform.mountType == ALTAZM) a1 = location.z; else a1 = location.h;

  if (guideAction == GA_REVERSE || guideAction == GA_SPIRAL) {
    if (transform.meridianFlips && location.pierSide == PIER_SIDE_EAST) { if (location.h < -limits.settings.pastMeridianE) return false; }
    if (a1 < mount.axis1.settings.limits.min) return false;
  }
  if (guideAction == GA_FORWARD || guideAction == GA_SPIRAL) {
    if (transform.meridianFlips && location.pierSide == PIER_SIDE_WEST) { if (location.h > limits.settings.pastMeridianW) return false; }
    if (a1 > mount.axis1.settings.limits.max) return false;
  }
  return true;
}

// valid guide for Axis2
bool Guide::validAxis2(GuideAction guideAction) {
  Coordinate location = mount.getMountPosition(CR_MOUNT_ALT);

  if (!limits.isEnabled()) return true;

  if (guideAction == GA_REVERSE || guideAction == GA_SPIRAL) {
    if (location.pierSide == PIER_SIDE_WEST) {
      if (location.a2 > mount.axis2.settings.limits.max) return false;
    } else {
      if (location.a2 < mount.axis2.settings.limits.min) return false;
    }
  }
  if (guideAction == GA_FORWARD || guideAction == GA_SPIRAL) {
    if (location.pierSide == PIER_SIDE_WEST) {
      if (location.a2 < mount.axis2.settings.limits.min) return false;
    } else {
      if (location.a2 > mount.axis2.settings.limits.max) return false;
    }
  }
  if (guideAction == GA_SPIRAL) {
    if (abs(location.a2) > degToRad(75.0)) return false;
  }
  return true;
}

// general validation of guide request
CommandError Guide::validate(int axis, GuideAction guideAction) {
  if (!mount.isEnabled()) return CE_SLEW_ERR_IN_STANDBY;
  if (mount.isFault()) return CE_SLEW_ERR_HARDWARE_FAULT;
  #if SLEW_GOTO == ON
    if (park.state == PS_PARKED) return CE_SLEW_ERR_IN_PARK;
  #endif
  if (guideAction == GA_SPIRAL && mount.isSlewing()) return CE_SLEW_IN_MOTION;

  #if SLEW_GOTO == ON
    if (goTo.state != GS_NONE) { goTo.stop(); return CE_SLEW_IN_MOTION; }
  #endif

  if (axis == 1 || guideAction == GA_SPIRAL) {
    if (!validAxis1(guideAction)) return CE_SLEW_ERR_OUTSIDE_LIMITS;
    if (settings.rateSelect < 3) {
      if (limits.isError() || mount.axis1.motionError(DIR_NONE)) return CE_SLEW_ERR_OUTSIDE_LIMITS;
    }
  }
  if (axis == 2 || guideAction == GA_SPIRAL) {
    if (!validAxis2(guideAction)) return CE_SLEW_ERR_OUTSIDE_LIMITS;
    if (settings.rateSelect < 3) {
      if (limits.isError() || mount.axis2.motionError(DIR_NONE)) return CE_SLEW_ERR_OUTSIDE_LIMITS;
    }
  }
  return CE_NONE;
}

// start axis1 movement
void Guide::axis1AutoSlew(GuideAction guideAction) {
  if (guideAction == GA_REVERSE) mount.axis1.autoSlew(DIR_REVERSE); else mount.axis1.autoSlew(DIR_FORWARD);
}

// start axis2 movement
void Guide::axis2AutoSlew(GuideAction guideAction) {
  Coordinate location = mount.getMountPosition(CR_MOUNT);
  if (location.pierSide == PIER_SIDE_WEST) {
    if (guideAction == GA_REVERSE) mount.axis2.autoSlew(DIR_FORWARD); else mount.axis2.autoSlew(DIR_REVERSE);
  } else {
    if (guideAction == GA_REVERSE) mount.axis2.autoSlew(DIR_REVERSE); else mount.axis2.autoSlew(DIR_FORWARD);
  }
}

// set guide spiral rates in RA/Azm and Dec/Alt, rate is in x-sidereal, guide elapsed time is in ms 
void Guide::spiralPoll() {
  // current elapsed time in seconds
  float T = ((long)(millis() - spiralStartTime))/1000.0;

  // actual rate we'll be using (in sidereal X)
  float rate = rateSelectToRate(settings.rateSelect);
  float maxRate = rateSelectToRate(GR_MAX);
  if (rate > maxRate/2.0) rate = maxRate/2.0;

  // apparaent FOV (in arc-seconds) = rate*15.0*2.0;
  // current radius assuming movement at 2 seconds per fov
  double radius = pow(T/6.28318, 1.0/1.74);

  // current angle in radians
  float angle = (radius - trunc(radius))*6.28318;

  // calculate the Axis rates for this moment (in sidereal X)
  customRateAxis1 = rate*cos(angle);
  customRateAxis2 = rate*sin(angle);

  // set any new directions
  guideActionAxis1 = GA_FORWARD;
  guideActionAxis2 = GA_FORWARD;
  if (customRateAxis1 < 0) { customRateAxis1 = fabs(customRateAxis1); guideActionAxis1 = GA_REVERSE; }
  if (customRateAxis2 < 0) { customRateAxis2 = fabs(customRateAxis2); guideActionAxis2 = GA_REVERSE; }
  axis1AutoSlew(guideActionAxis1);
  axis2AutoSlew(guideActionAxis2);

  // adjust Axis1 rate due to spherical coordinates and limit it to rates we can reach.  worst case the
  // shape of the spiral will degrade to an 2:1 aspect oval at half max rate (fastest allowed) and |Axis2| = 75°
  customRateAxis1 /= spiralScaleAxis1;
  if (customRateAxis1 > maxRate) customRateAxis1 = maxRate;

  // set the new guide rates
  mount.axis1.setFrequencySlew(siderealToRadF(customRateAxis1));
  mount.axis2.setFrequencySlew(siderealToRadF(customRateAxis2));
}

void Guide::poll() {
  // check fast guide completion axis1
  if (guideActionAxis1 == GA_BREAK && rateAxis1 == 0.0F && !mount.axis1.isSlewing()) {
    guideActionAxis1 = GA_NONE;
    mount.update();
  } else {
    if (guideActionAxis1 > GA_BREAK && (long)(millis() - guideFinishTimeAxis1) >= 0) stopAxis1(GA_BREAK);
  }

  // check fast guide completion axis2
  if (guideActionAxis2 == GA_BREAK && rateAxis2 == 0.0F && !mount.axis2.isSlewing()) {
    guideActionAxis2 = GA_NONE;
    mount.update();
  } else {
    if (guideActionAxis2 > GA_BREAK && (long)(millis() - guideFinishTimeAxis2) >= 0) stopAxis2(GA_BREAK);
  }

  // do spiral guiding
  if (state == GU_SPIRAL_GUIDE) {
    if (guideActionAxis1 > GA_BREAK && guideActionAxis2 > GA_BREAK) spiralPoll(); else
    if (guideActionAxis1 != GA_BREAK || guideActionAxis2 != GA_BREAK) stopSpiral();
  }

  // handle end of home guiding
  if (state == GU_HOME_GUIDE && !mount.isSlewing()) {
    VLF("MSG: guidePoll(); arrived at home");
    state = GU_NONE;
    guideActionAxis1 = GA_NONE;
    guideActionAxis2 = GA_NONE;
    home.reset();
    mount.enable(true);
  }

  // watch for guides finished
  if (guideActionAxis1 == GA_NONE && guideActionAxis2 == GA_NONE) state = GU_NONE;
}

Guide guide;

#endif
