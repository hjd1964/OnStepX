// -----------------------------------------------------------------------------------
// telescope mount control, parking

#include "Mount.h"

#ifdef MOUNT_PRESENT

#include "../Telescope.h"

void Mount::parkInit(bool validKey) {
  // confirm the data structure size
  if (ParkSize < sizeof(Park)) { initError.nv = true; DL("ERR: Mount::parkInit(); ParkSize error NV subsystem writes disabled"); nv.readOnly(true); }

  // write the default park settings
  if (!validKey) {
    VLF("MSG: Mount, writing default parking settings to NV");
    nv.writeBytes(NV_MOUNT_PARK_BASE, &park, ParkSize);
  }

  // read the park settings
  nv.readBytes(NV_MOUNT_PARK_BASE, &park, ParkSize);
}

CommandError Mount::parkSet() {
  if (park.state == PS_PARK_FAILED)     return CE_PARK_FAILED;
  if (park.state == PS_PARKED)          return CE_PARKED;
  if (gotoState != GS_NONE)             return CE_SLEW_IN_MOTION;
  if (guideState != GU_NONE)            return CE_SLEW_IN_MOTION;
  if (axis1.fault() || axis2.fault())   return CE_SLEW_ERR_HARDWARE_FAULT;

  VLF("MSG: Mount, setting park position");

  TrackingState priorTrackingState = trackingState;
  trackingState = TS_NONE;
  updateTrackingRates();

  // get our current position in equatorial coordinates
  updatePosition(CR_MOUNT_EQU);

  // save as the park position
  park.position.h = current.h;
  park.position.d = current.d;
  park.position.pierSide = current.pierSide;
  if (park.position.pierSide == PIER_SIDE_NONE) {
    if (meridianFlip == MF_ALWAYS) {
      if (current.h < 0) park.position.pierSide = PIER_SIDE_WEST; else park.position.pierSide = PIER_SIDE_EAST;
    } else park.position.pierSide = PIER_SIDE_EAST;
  }
  park.saved = true;
  nv.updateBytes(NV_MOUNT_PARK_BASE, &park, ParkSize);

  #if ALIGN_MAX_NUM_STARS > 1
    transform.align.modelWrite();
  #endif

  trackingState = priorTrackingState;
  updateTrackingRates();
  
  VLF("MSG: Mount, setting park done");
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

  CommandError e = validateGotoState();
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
  axis1.setBacklash(0.0L);
  axis2.setBacklash(0.0L);
  Coordinate parkTarget;
  parkTarget.h = park.position.h;
  parkTarget.d = park.position.d;
  parkTarget.pierSide = park.position.pierSide;

  // goto the park (mount) target coordinate
  if (parkTarget.pierSide == PIER_SIDE_EAST) e = gotoEqu(&parkTarget, PSS_EAST_ONLY, false); else
  if (parkTarget.pierSide == PIER_SIDE_WEST) e = gotoEqu(&parkTarget, PSS_WEST_ONLY, false);

  if (e != CE_NONE) {
    trackingState = priorTrackingState;
    updateTrackingRates();

    park.state = priorParkState;
    nv.updateBytes(NV_MOUNT_PARK_BASE, &park, ParkSize);

    VF("ERR, Mount::parkGoto(); Failed to start goto (CE "); V(e); V(")");
    return e;
  } else { VLF("MSG: Mount, parking started"); }
  return CE_NONE;
}

void Mount::parkFinish() {
  if (park.state != PS_PARK_FAILED) {
    #if DEBUG == VERBOSE
      long index = axis1.getInstrumentCoordinateSteps() - axis1.getMotorPositionSteps();
      V("MSG: Mount, park axis1 motor target   "); VL(axis1.getTargetCoordinateSteps() - index);
      V("MSG: Mount, park axis1 motor position "); VL(axis1.getMotorPositionSteps());
      index = axis2.getInstrumentCoordinateSteps() - axis2.getMotorPositionSteps();
      V("MSG: Mount, park axis2 motor target   "); VL(axis2.getTargetCoordinateSteps() - index);
      V("MSG: Mount, park axis2 motor position "); VL(axis2.getMotorPositionSteps());
    #endif

    // save the axis state
    park.state = PS_PARKED;
    nv.updateBytes(NV_MOUNT_PARK_BASE, &park, ParkSize);

    #if ALIGN_MAX_NUM_STARS > 1  
      transform.align.modelWrite();
    #endif

    VLF("MSG: Mount, parking done");
  } else { DLF("ERR, Mount::parkFinish(); Parking failed"); }

  axis1.enable(false);
  axis2.enable(false);
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
  if (!site.dateTimeReady())            return CE_PARKED;

  VLF("MSG: Unparking");

  #if AXIS1_PEC == ON
    wormSenseSteps = park.wormSensePositionSteps;
  #endif

  // reset the mount, zero backlash
  resetHome(false);
  axis1.setBacklashSteps(0);
  axis2.setBacklashSteps(0);

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
  axis1.setInstrumentCoordinatePark(a1);
  axis2.setInstrumentCoordinatePark(a2);

  // set Meridian Flip behaviour to match mount type
  if (transform.mountType == GEM) meridianFlip = MF_ALWAYS; else meridianFlip = MF_NEVER;

  V("MSG: Mount, unpark axis1 motor position "); VL(axis1.getMotorPositionSteps());
  V("MSG: Mount, unpark axis2 motor position "); VL(axis2.getMotorPositionSteps());

  // restore backlash settings
  axis1.setBacklash(misc.backlash.axis1);
  axis2.setBacklash(misc.backlash.axis2);
  
  // make sure limits are on
  limitsEnabled = true;

  // update our state and start tracking
  if (withTrackingOn) {
    park.state = PS_UNPARKED;
    nv.updateBytes(NV_MOUNT_PARK_BASE, &park, ParkSize);
    setTrackingState(TS_SIDEREAL);
    updateTrackingRates();
  }

  VLF("MSG: Mount, unparking done");
  return CE_NONE;
}

#endif
