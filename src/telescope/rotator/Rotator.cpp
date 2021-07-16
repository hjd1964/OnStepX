//--------------------------------------------------------------------------------------------------
// OnStepX focuser control

#include "Rotator.h"

#ifdef ROTATOR_PRESENT

#include "../../tasks/OnTask.h"
extern Tasks tasks;
#include "../Telescope.h"
#include "../mount/site/Site.h"

#ifdef MOUNT_PRESENT
  void derotateWrapper() { telescope.rotator.derotatePoll(); }
#endif

// initialize rotator
void Rotator::init(bool validKey) {

  // wait a moment for any background processing that may be needed
  delay(1000);

  // get settings stored in NV ready
  if (!validKey) {
    VLF("MSG: Rotator, writing default settings to NV");
    writeSettings();
  }
  readSettings();

  VL("MSG: Rotator, init (Axis3)");
  axis.init(3, false, validKey);
  axis.setMotorCoordinateSteps(0);
  axis.setBacklashSteps(backlash);
  axis.setFrequencyMax(AXIS3_SLEW_RATE_DESIRED);
  axis.setFrequencySlew(AXIS3_SLEW_RATE_DESIRED);
  axis.setSlewAccelerationRate(AXIS3_ACCELERATION_RATE);
  axis.setSlewAccelerationRateAbort(AXIS3_RAPID_STOP_RATE);
  if (AXIS3_DRIVER_POWER_DOWN == ON) axis.setPowerDownTime(DEFAULT_POWER_DOWN_TIME);

  #ifdef MOUNT_PRESENT
    // start task for derotation
    VF("MSG: Rotator, start derotation task (rate 1s priority 7)... ");
    if (tasks.add(1000, 0, true, 7, derotateWrapper, "RotPoll")) { VL("success"); } else { VL("FAILED!"); }
  #endif
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
  axis.setBacklashSteps(backlash);
  return true;
}

// enable or disable the derotator
void Rotator::setDerotatorEnabled(bool value) {
  #ifdef MOUNT_PRESENT
    if (telescope.mount.transform.mountType != ALTAZM) return;
    derotatorEnabled = value;
    if (!derotatorEnabled) axis.setFrequencyBase(0.0F);
  #else
    value = value;
  #endif
}

#ifdef MOUNT_PRESENT
  // poll to set the derotator rate
  void Rotator::derotatePoll() {
    if (derotatorEnabled) {
      float pr = 0.0F;
      #ifdef MOUNT_PRESENT
      if (!axis.autoSlewActive()) {
        Coordinate current = telescope.mount.getPosition();
        pr = parallacticRate(&current);
        if (derotatorReverse) pr = -pr;
      }
      #endif
      axis.setFrequencyBase(pr);
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
#endif

void Rotator::readSettings() {
  backlash = nv.readUI(NV_ROTATOR_SETTINGS_BASE);

  if (backlash < 0)     { backlash = 0; initError.value = true; DLF("ERR, Rotator.init(): bad NV backlash < 0 steps (set to 0)"); }
  if (backlash > 10000) { backlash = 0; initError.value = true; DLF("ERR, Rotator.init(): bad NV backlash > 10000 steps (set to 0)"); }
}

void Rotator::writeSettings() {
  nv.write(NV_ROTATOR_SETTINGS_BASE, backlash);
}

#endif
