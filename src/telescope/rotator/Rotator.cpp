//--------------------------------------------------------------------------------------------------
// OnStepX focuser control

#include "Rotator.h"

#if AXIS3_DRIVER_MODEL != OFF

#include "../Telescope.h"
#include "../mount/site/Site.h"

// initialize rotator
void Rotator::init(bool validKey) {
  VL("MSG: Rotator, init. (Axis3)");
  axis3.init(3, false, validKey);
  axis3.setFrequencyMax(AXIS3_SLEW_RATE_DESIRED);
  axis3.setFrequencySlew(AXIS3_SLEW_RATE_DESIRED);
  axis3.setSlewAccelerationRate(AXIS3_ACCELERATION_RATE);
  axis3.setSlewAccelerationRateAbort(AXIS3_RAPID_STOP_RATE);

  // init. focuser settings
  if (!validKey) {
    VLF("MSG: Rotator, writing default settings to NV");
    writeSettings();
  }

  // read focuser settings
  readSettings();
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

// derotator enable or disable
void Rotator::derotatorEnable(bool value) {
  derotatorEnabled = value;
}

// derotator get reverse state
bool Rotator::getDerotatorReverse() {
  return derotatorReverse;
}

// derotator set reverse state
void Rotator::setDerotatorReverse(bool value) {
  derotatorReverse = value;
}

// derotator move to PA of equatorial coordinate
void Rotator::setDerotatorPA(Coordinate *coord) {
}

void Rotator::readSettings() {
  backlash = nv.readUI(NV_ROTATOR_SETTINGS_BASE);
}

void Rotator::writeSettings() {
  nv.write(NV_ROTATOR_SETTINGS_BASE, backlash);
}

// returns parallactic angle in degrees
double Rotator::ParallacticAngle(Coordinate *coord) {
  return radToDeg(atan2(sin(coord->h), cos(coord->d)*tan(site.location.latitude) - sin(coord->d)*cos(coord->h)));
}

// returns parallactic rate in degrees per second
double Rotator::ParallacticRate(Coordinate *coord) {
  // one minute of hour angle in degrees = 15/60 = 0.25
  Coordinate ahead = *coord;
  ahead.h += degToRad(0.125);
  Coordinate behind = *coord;
  behind.h -= degToRad(0.125);
  double b = ParallacticAngle(&behind);
  double a = ParallacticAngle(&ahead);
  if (b >  90.0 && a < -90.0) a += 360.0;
  if (b < -90.0 && a >  90.0) b += 360.0;
  return (a - b)/60.0;
}

#endif
