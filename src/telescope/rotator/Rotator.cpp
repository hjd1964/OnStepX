//--------------------------------------------------------------------------------------------------
// OnStepX focuser control

#include "Rotator.h"

#ifdef ROTATOR_PRESENT

#include "../../tasks/OnTask.h"
extern Tasks tasks;

#include "../axis/Axis.h"
#include "../Telescope.h"
#include "../mount/Mount.h"
#include "../mount/site/Site.h"

void rotWrapper() { rotator.monitor(); }

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
  axis3.init(false);
  axis3.resetPositionSteps(0);
  axis3.setBacklashSteps(backlash);
  axis3.setFrequencyMax(AXIS3_SLEW_RATE_DESIRED);
  axis3.setFrequencyMin(0.01F);
  axis3.setFrequencySlew(AXIS3_SLEW_RATE_DESIRED);
  axis3.setSlewAccelerationTime(AXIS3_ACCELERATION_TIME);
  axis3.setSlewAccelerationTimeAbort(AXIS3_RAPID_STOP_TIME);
  if (AXIS3_POWER_DOWN == ON) axis3.setPowerDownTime(DEFAULT_POWER_DOWN_TIME);

  // start monitor task
  VF("MSG: Rotator, start derotation task (rate 1s priority 6)... ");
  if (tasks.add(1000, 0, true, 6, rotWrapper, "RotMon")) { VL("success"); } else { VL("FAILED!"); }

  unpark();
}

// get backlash in steps
int Rotator::getBacklash() {
  return backlash;
}

// set backlash in steps
CommandError Rotator::setBacklash(int value) {
  if (value < 0 || value > 10000) return CE_PARAM_RANGE;
  if (parkState >= PS_PARKED) return CE_PARKED;

  backlash = value;
  writeSettings();
  axis3.setBacklashSteps(backlash);

  return CE_NONE;
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

// start slew in the specified direction
CommandError Rotator::slew(Direction dir) {
  if (parkState >= PS_PARKED) return CE_PARKED;

  axis3.setFrequencyBase(0.0F);

  return axis3.autoSlew(dir, slewRate);
}

// move rotator to a specific location
CommandError Rotator::gotoTarget(float target) {
  if (parkState >= PS_PARKED) return CE_PARKED;

  VF("MSG: Rotator, goto target coordinate set ("); V(target*axis3.getStepsPerMeasure()); VL("°)");
  VF("MSG: Rotator, starting goto at slew rate ("); V(AXIS3_SLEW_RATE_DESIRED); VL("°/s)");

  axis3.setFrequencyBase(0.0F);
  axis3.setTargetCoordinate(target);

  return axis3.autoSlewRateByDistance(AXIS3_SLEW_RATE_DESIRED*AXIS3_ACCELERATION_TIME, AXIS3_SLEW_RATE_DESIRED);
}

// parks rotator at current position
CommandError Rotator::park() {
  if (parkState == PS_PARKED)      return CE_NONE;
  if (parkState == PS_PARKING)     return CE_PARK_FAILED;
  if (parkState == PS_UNPARKING)   return CE_PARK_FAILED;
  if (parkState == PS_PARK_FAILED) return CE_PARK_FAILED;

  derotatorEnabled = false;

  VLF("MSG: Rotator, parking");
  axis3.setBacklash(0.0F);
  position = axis3.getInstrumentCoordinate();
  axis3.setTargetCoordinatePark(position);

  CommandError e = axis3.autoSlewRateByDistance(AXIS3_SLEW_RATE_DESIRED*AXIS3_ACCELERATION_TIME, AXIS3_SLEW_RATE_DESIRED);

  if (e == CE_NONE) {
    parkState = PS_PARKING;
    writeSettings();
  }

  return e;
}

// unparks rotator
CommandError Rotator::unpark() {
  if (parkState == PS_PARKING)     return CE_PARK_FAILED;
  if (parkState == PS_UNPARKING)   return CE_PARK_FAILED;
  if (parkState == PS_PARK_FAILED) return CE_PARK_FAILED;
  if (ROTATOR_WRITE_DELAY == 0) {
    if (parkState != PS_PARKED)    return CE_NOT_PARKED;
  }

  // simple unpark if any stepper driver indexer would be ok here
  if (axis3.settings.subdivisions == 1) {
    axis3.setInstrumentCoordinate(position);
    parkState = PS_UNPARKED;
    writeSettings();
    return CE_NONE;
  }

  axis3.setBacklash(0.0F);
  axis3.setInstrumentCoordinatePark(position);
  V("MSG: Rotator, unpark motor position "); VL(axis3.getMotorPositionSteps());

  axis3.enable(true);
  axis3.setBacklash(backlash);
  axis3.setTargetCoordinate(position);

  CommandError e = axis3.autoSlewRateByDistance(AXIS3_SLEW_RATE_DESIRED*AXIS3_ACCELERATION_TIME, AXIS3_SLEW_RATE_DESIRED);

  if (e == CE_NONE) {
    parkState = PS_UNPARKING;
    writeSettings();
  }

  return e;
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
  nv.write(NV_ROTATOR_SETTINGS_BASE + 2, (float)position);
}

// poll rotator to handle parking and derotation
void Rotator::monitor() {
  secs++;

  if (axis3.isSlewing() || position == axis3.getInstrumentCoordinate()) writeTime = secs + ROTATOR_WRITE_DELAY;

  if (!axis3.isSlewing()) {

    if (parkState == PS_PARKING) {
      if (axis3.atTarget()) {
        axis3.enable(false);
        parkState = PS_PARKED;
        writeSettings();
        #if DEBUG == VERBOSE
          long offset = axis3.getInstrumentCoordinateSteps() - axis3.getMotorPositionSteps();
          VF("MSG: Rotator, park motor target   "); VL(axis3.getTargetCoordinateSteps() - offset);
          VF("MSG: Rotator, park motor position "); VL(axis3.getMotorPositionSteps());
        #endif
      }
    } else

    if (parkState == PS_UNPARKING) {
      if (axis3.atTarget()) {
        parkState = PS_UNPARKED;
        writeSettings();
      }
    } else

    if (parkState == PS_UNPARKED) {
      #ifdef MOUNT_PRESENT
        if (derotatorEnabled && transform.mountType == ALTAZM) {
          float pr = 0.0F;
          if (!axis3.getSynchronized()) axis3.setSynchronized(true);
          Coordinate current = mount.getPosition();
          pr = parallacticRate(&current);
          if (derotatorReverse) pr = -pr;
          axis3.setSynchronized(true);
          axis3.setFrequencyBase(pr);
        }
      #endif

      // delayed write of focuser position
      if (ROTATOR_WRITE_DELAY != 0) {
        if (secs > writeTime) {
          position = axis3.getInstrumentCoordinate();
          writeSettings();
          VF("MSG: Rotator, writing position ("); V(position); VL("°) to NV"); 
        }
      }
    }
  }
}

Rotator rotator;

#endif
