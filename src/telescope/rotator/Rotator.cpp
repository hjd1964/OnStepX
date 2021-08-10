//--------------------------------------------------------------------------------------------------
// OnStepX focuser control

#include "Rotator.h"

#ifdef ROTATOR_PRESENT

#include "../../tasks/OnTask.h"
extern Tasks tasks;

#include "../Telescope.h"
#include "../mount/Mount.h"
#include "../mount/site/Site.h"

#ifdef MOUNT_PRESENT
  void derotateWrapper() { rotator.poll(); }
#endif

// initialize rotator
void Rotator::init() {

  // wait a moment for any background processing that may be needed
  delay(1000);

  // get settings stored in NV ready
  if (!validKey) {
    VLF("MSG: Rotator, writing default settings to NV");
    writeSettings();
  }
  readSettings();

  VL("MSG: Rotator, init (Axis3)");
  axis.init(3, false);
  axis.resetPositionSteps(0);
  axis.setBacklashSteps(backlash);
  axis.setFrequencyMax(AXIS3_SLEW_RATE_DESIRED);
  axis.setFrequencyMin(0.01F);
  axis.setFrequencySlew(AXIS3_SLEW_RATE_DESIRED);
  axis.setSlewAccelerationTime(AXIS3_ACCELERATION_TIME);
  axis.setSlewAccelerationTimeAbort(AXIS3_RAPID_STOP_TIME);
  if (AXIS3_POWER_DOWN == ON) axis.setPowerDownTime(DEFAULT_POWER_DOWN_TIME);

  axis.enable(true);

  #ifdef MOUNT_PRESENT
    if (transform.mountType == ALTAZM) {
      // start task for derotation
      VF("MSG: Rotator, start derotation task (rate 1s priority 7)... ");
      if (tasks.add(1000, 0, true, 7, derotateWrapper, "RotPoll")) { VL("success"); } else { VL("FAILED!"); }
    }
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

#ifdef MOUNT_PRESENT
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

// move rotator to a specific location
CommandError Rotator::gotoTarget(float target) {
  VF("MSG: Rotator, goto target coordinate set ("); V(target*axis.getStepsPerMeasure()); VL("°)");
  VF("MSG: Rotator, starting goto at slew rate ("); V(AXIS3_SLEW_RATE_DESIRED); VL("°/s)");
  axis.setTargetCoordinate(target);
  return axis.autoSlewRateByDistance(AXIS3_SLEW_RATE_DESIRED*AXIS3_ACCELERATION_TIME, AXIS3_SLEW_RATE_DESIRED);
}

// parks rotator at current position
void Rotator::park() {
  derotatorEnabled = false;
  axis.setFrequencyBase(0.0F);
  axis.setBacklash(0.0F);
  float position = axis.getInstrumentCoordinate();
  axis.setTargetCoordinatePark(position);
  axis.autoSlewRateByDistance(AXIS3_SLEW_RATE_DESIRED*AXIS3_ACCELERATION_TIME, AXIS3_SLEW_RATE_DESIRED);
  axis.enable(false);
  #if DEBUG == VERBOSE
    tasks.yield(500);
    long offset = axis.getInstrumentCoordinateSteps() - axis.getMotorPositionSteps();
    V("MSG: Rotator, park motor target   "); VL(axis.getTargetCoordinateSteps() - offset);
    V("MSG: Rotator, park motor position "); VL(axis.getMotorPositionSteps());
  #endif
  writeSettings();
}

// unparks rotator
void Rotator::unpark() {
  axis.setBacklash(0.0F);
  position = axis.getInstrumentCoordinate();
  axis.setInstrumentCoordinatePark(position);
  V("MSG: Rotator, unpark motor position "); VL(axis.getMotorPositionSteps());
  axis.setBacklash(backlash);
  axis.setTargetCoordinate(position);
  axis.enable(true);
  axis.autoSlewRateByDistance(AXIS3_SLEW_RATE_DESIRED);
}

void Rotator::readSettings() {
  backlash = nv.readUI(NV_ROTATOR_SETTINGS_BASE);
  position = nv.readF(NV_ROTATOR_SETTINGS_BASE + 2);

  if (backlash < 0)     { backlash = 0; initError.value = true; DLF("ERR, Rotator.init(): bad NV backlash < 0 steps (set to 0)"); }
  if (backlash > 10000) { backlash = 0; initError.value = true; DLF("ERR, Rotator.init(): bad NV backlash > 10000 steps (set to 0)"); }

  if (position < AXIS3_LIMIT_MIN) { position = 0.0F; initError.value = true; DLF("ERR, Rotator.init(): bad NV park pos < AXIS3_LIMIT_MIN (set to 0.0)"); }
  if (position > AXIS3_LIMIT_MAX) { position = 0.0F; initError.value = true; DLF("ERR, Rotator.init(): bad NV park pos > AXIS3_LIMIT_MAX (set to 0.0)"); }
}

void Rotator::writeSettings() {
  nv.write(NV_ROTATOR_SETTINGS_BASE, backlash);
}

// poll to set derotator rate
void Rotator::poll() {
  if (derotatorEnabled) {
    float pr = 0.0F;
    #ifdef MOUNT_PRESENT
      if (!axis.isSlewing()) {
        Coordinate current = mount.getPosition();
        pr = parallacticRate(&current);
        if (derotatorReverse) pr = -pr;
      }
    #endif
    axis.setFrequencyBase(pr);
  }
}

Rotator rotator;

#endif
