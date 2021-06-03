// -----------------------------------------------------------------------------------
// telescope mount control, parking

#include "../Common.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../telescope/Telescope.h"
#include "Mount.h"

CommandError Mount::parkSet() {
  if (park.state == PS_PARK_FAILED)     return CE_PARK_FAILED;
  if (park.state == PS_PARKED)          return CE_PARKED;
  if (gotoState != GS_NONE)             return CE_SLEW_IN_MOTION;
  if (guideState != GU_NONE)            return CE_SLEW_IN_MOTION;
  if (axis1.fault() || axis2.fault())   return CE_SLEW_ERR_HARDWARE_FAULT;

  VLF("MSG: Setting park position");

  TrackingState priorTrackingState = trackingState;
  trackingState = TS_NONE;
  updateTrackingRates();

  // get our current position in equatorial coordinates
  updatePosition(CR_MOUNT_EQU);

  // save as the park position
  park.position.h = current.h;
  park.position.d = current.d;
  park.position.pierSide = current.pierSide;
  park.saved = true;
  nv.updateBytes(NV_MOUNT_PARK_BASE, &park, ParkSize);

  #if ALIGN_MAX_NUM_STARS > 1  
    transform.align.modelWrite();
  #endif

  trackingState = priorTrackingState;
  updateTrackingRates();
  
  VLF("MSG: Setting park done");
  return CE_NONE;
}

// moves the telescope to the park position
CommandError Mount::parkGoto() {
  if (!park.saved)                      return CE_NO_PARK_POSITION_SET;
  if (park.state == PS_PARKED)          return CE_PARKED;
  if (!axis1.isEnabled())               return CE_SLEW_ERR_IN_STANDBY;
  if (gotoState != GS_NONE)             return CE_SLEW_IN_MOTION;
  if (guideState != GU_NONE)            return CE_SLEW_IN_MOTION;
  if (axis1.fault() || axis2.fault())   return CE_SLEW_ERR_HARDWARE_FAULT;

  CommandError e = validateGoto();
  if (e != CE_NONE)                     return e;
  
  // stop tracking
  TrackingState priorTrackingState = trackingState;
  trackingState = TS_NONE;
  updateTrackingRates();

  #if AXIS1_PEC == ON
    // turn off PEC while we park
    pecDisable();
    pec.state = PEC_NONE;
    park.wormSensePositionSteps = wormSenseSteps;
  #endif

  // record our park status
  ParkState priorParkState = park.state; 
  
  // update state to parking
  park.state = PS_PARKING;
  nv.updateBytes(NV_MOUNT_PARK_BASE, &park, ParkSize);

  // get the park coordinate ready
  Coordinate parkTarget;
  parkTarget.h = park.position.h;
  parkTarget.d = park.position.d;
  parkTarget.pierSide = park.position.pierSide;

  axis1.setBacklash(0);
  axis2.setBacklash(0);

  // goto the park (mount) target coordinate
  e = gotoEqu(&parkTarget, PSS_SAME_ONLY, false);
  if (e != CE_NONE) {
    trackingState = priorTrackingState;
    updateTrackingRates();

    park.state = priorParkState;
    nv.updateBytes(NV_MOUNT_PARK_BASE, &park, ParkSize);

    VLF("ERR, parkGoto(): Failed to start goto");
    return e;
  } else { VLF("MSG: Parking started"); }
  return CE_NONE;
}

void Mount::parkFinish() {
  if (park.state != PS_PARK_FAILED) {
    park.state = PS_PARKED;
    nv.updateBytes(NV_MOUNT_PARK_BASE, &park, ParkSize);

    #if ALIGN_MAX_NUM_STARS > 1  
      transform.align.modelWrite();
    #endif

    VLF("MSG: Parking done");
  } else { DLF("ERR, parkFinish(): Parking failed"); }

  axis1.enable(false);
  axis2.enable(false);
}

#define PARK_MAX_MICROSTEP 256
void Mount::parkNearest() {
  long positionSteps = axis1.getTargetCoordinateSteps();
  positionSteps -= PARK_MAX_MICROSTEP*2L; 
  for (int l = 0; l < PARK_MAX_MICROSTEP*4; l++) { if (positionSteps % PARK_MAX_MICROSTEP*4L == 0) break; positionSteps++; }
  axis1.setTargetCoordinateSteps(positionSteps);
  VLF("MSG: Mount::parkNearest(); target coordinate axis1 (steps) adjusted to nearest %1024 location");

  positionSteps = axis2.getTargetCoordinateSteps();
  positionSteps -= PARK_MAX_MICROSTEP*2L; 
  for (int l = 0; l < PARK_MAX_MICROSTEP*4; l++) { if (positionSteps % PARK_MAX_MICROSTEP*4L == 0) break; positionSteps++; }
  axis2.setTargetCoordinateSteps(positionSteps);
  VLF("MSG: Mount::parkNearest(); target coordinate axis2 (steps) adjusted to nearest %1024 location");
}

CommandError Mount::parkRestore(bool withTrackingOn) {
  if (!park.saved)                      return CE_NO_PARK_POSITION_SET;
  if (park.state != PS_PARKED) {
    #if STRICT_PARKING == ON
      VLF("MSG: Unpark ignored, not parked"); return CE_NOT_PARKED;
    #endif
    if (!atHome)                        return CE_NOT_PARKED;
  }
  if (gotoState != GS_NONE)             return CE_SLEW_IN_MOTION;
  if (guideState != GU_NONE)            return CE_SLEW_IN_MOTION;
  if (axis1.fault() || axis2.fault())   return CE_SLEW_ERR_HARDWARE_FAULT;
//if (!transform.site.dateTimeReady())  return CE_PARKED;

  VLF("MSG: Unparking");

  // read the park settings
  if (ParkSize < sizeof(Park)) {
    initError.nv = true;
    DL("ERR: Mount::initPark(); ParkSize error NV subsystem writes disabled");
    nv.readOnly(true);
    return CE_NO_PARK_POSITION_SET;
  }
  nv.readBytes(NV_MOUNT_PARK_BASE, &park, ParkSize);

  #if AXIS1_PEC == ON
    wormSenseSteps = park.wormSensePositionSteps;
  #endif

  // reset mount
  resetHome();

  // make sure limits are on
  limitsEnabled = true;

  // load the pointing model
  #if ALIGN_MAX_NUM_STARS > 1  
    transform.align.modelRead();
  #endif

  // get the park coordinate ready
  Coordinate parkTarget;
  parkTarget.h = park.position.h;
  parkTarget.d = park.position.d;
  parkTarget.pierSide = park.position.pierSide;

  // set the mount target
  double a1, a2;
  if (transform.mountType == ALTAZM) transform.equToHor(&parkTarget);
  transform.mountToInstrument(&parkTarget, &a1, &a2);
  axis1.setTargetCoordinate(a1);
  axis2.setTargetCoordinate(a2);

  // adjust target to the actual park position (just like we did when we parked)
  parkNearest();

  // and set the motor coordinates to agree with the target
  axis1.setMotorCoordinateSteps(axis1.getTargetCoordinateSteps());
  axis2.setMotorCoordinateSteps(axis2.getTargetCoordinateSteps());
  
  // set Meridian Flip behaviour to match mount type
  if (transform.mountType == GEM) meridianFlip = MF_ALWAYS; else meridianFlip = MF_NEVER;
  atHome = false;

  // restore backlash settings, in-case we are unparking without a power cycle
  axis1.setBacklash(misc.backlash.axis1);
  axis2.setBacklash(misc.backlash.axis2);

  // update our state and start tracking
  if (withTrackingOn) {
    park.state = PS_UNPARKED;
    nv.updateBytes(NV_MOUNT_PARK_BASE, &park, ParkSize);
    setTrackingState(TS_SIDEREAL);
    updateTrackingRates();
  }

  VLF("MSG: Unparking done");
  return CE_NONE;
}

#endif
