//--------------------------------------------------------------------------------------------------
// OnStepX focuser control

#include "Rotator.h"

#if AXIS3_DRIVER_MODEL != OFF

#include "../../tasks/OnTask.h"
extern Tasks tasks;
#include "../Telescope.h"
#include "../mount/site/Site.h"

void derotateWrapper() { telescope.rotator.derotatePoll(); }

// initialize rotator
void Rotator::init(bool validKey) {
  VL("MSG: Rotator, init. (Axis3)");
  axis3.init(3, false, validKey);
  axis3.setFrequencyMax(AXIS3_SLEW_RATE_DESIRED);
  axis3.setFrequencySlew(AXIS3_SLEW_RATE_DESIRED);
  axis3.setSlewAccelerationRate(AXIS3_ACCELERATION_RATE);
  axis3.setSlewAccelerationRateAbort(AXIS3_RAPID_STOP_RATE);

  // get settings stored in NV ready
  if (!validKey) {
    VLF("MSG: Rotator, writing default settings to NV");
    writeSettings();
  }
  readSettings();

  // start task for derotation
  VF("MSG: Rotator, start derotation task (rate 1s priority 7)... ");
  if (tasks.add(1000, 0, true, 7, derotateWrapper, "DeRot")) { VL("success"); } else { VL("FAILED!"); }
}

// get backlash in steps
int Rotator::getBacklash() {
  return backlash;
}

// set backlash in steps
bool Rotator::setBacklash(int value) {
  if (value < 0 || value > 10000) return false;
  backlash = value;
  writeSettings();
  return true;
}

// enable or disable the derotator
void Rotator::setDerotatorEnabled(bool value) {
  if (telescope.mount.transform.mountType != ALTAZM) return;
  derotatorEnabled = value;
  if (!derotatorEnabled) axis3.setFrequencyBase(0.0F);
}

// poll to set the derotator rate
void Rotator::derotatePoll() {
  if (derotatorEnabled) {
    float pr = 0.0F;
    #ifdef MOUNT_PRESENT
    if (!axis3.autoSlewActive()) {
      Coordinate current = telescope.mount.getPosition();
      pr = parallacticRate(&current);
      if (derotatorReverse) pr = -pr;
    }
    #endif
    axis3.setFrequencyBase(pr);
  }
}

// returns parallactic angle in degrees
double Rotator::parallacticAngle(Coordinate *coord) {
  return radToDeg(atan2(sin(coord->h), cos(coord->d)*tan(site.location.latitude) - sin(coord->d)*cos(coord->h)));
}

// returns parallactic rate in degrees per second
double Rotator::parallacticRate(Coordinate *coord) {
  // one minute of hour angle in degrees = 15/60 = 0.25
  Coordinate ahead = *coord;
  ahead.h += degToRad(0.125);
  Coordinate behind = *coord;
  behind.h -= degToRad(0.125);
  double b = parallacticAngle(&behind);
  double a = parallacticAngle(&ahead);
  if (b >  90.0 && a < -90.0) a += 360.0;
  if (b < -90.0 && a >  90.0) b += 360.0;
  return (a - b)/60.0;
}

void Rotator::readSettings() {
  backlash = nv.readUI(NV_ROTATOR_SETTINGS_BASE);
}

void Rotator::writeSettings() {
  nv.write(NV_ROTATOR_SETTINGS_BASE, backlash);
}

#endif
