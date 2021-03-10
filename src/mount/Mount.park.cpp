// -----------------------------------------------------------------------------------
// telescope mount control, parking
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../lib/nv/NV.h"
extern NVS nv;
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../coordinates/Transform.h"
#include "../commands/ProcessCmds.h"
#include "../motion/Axis.h"
#include "Mount.h"

// sets the park postion as the current position
CommandError Mount::parkSet() {
  if (park.state == PS_PARK_FAILED)     return CE_PARK_FAILED;
  if (park.state == PS_PARKED)          return CE_PARKED;
  if (gotoState == GS_NONE)             return CE_SLEW_IN_MOTION;
  if (guideState == GU_NONE)            return CE_SLEW_IN_MOTION;
  if (axis1.fault() || axis2.fault())   return CE_SLEW_ERR_HARDWARE_FAULT;

  VLF("MSG: Setting park position");

  TrackingState priorTrackingState = trackingState;
  trackingState = TS_NONE;
  updateTrackingRates();

  // get our current position in equatorial coordinates
  updatePosition();
  if (transform.mountType == ALTAZM) transform.horToEqu(&current);

  // save as the park position
  park.position.h = current.h;
  park.position.d = current.d;
  park.position.pierSide = current.pierSide;
  park.saved = true;
  if (ParkSize < sizeof(Park)) { DL("ERR: Mount::parkSet(); ParkSize error NV subsystem writes disabled"); nv.readOnly(true); }
  nv.updateBytes(NV_PARK_BASE, &park, ParkSize);

  //saveAlignModel();

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
    // save the worm sense position
    nv.writeLong(EE_wormSensePos, wormSensePos);
  #endif

  // record our park status
  ParkState priorParkState = park.state; 
  
  // get the park info from NV
  if (ParkSize < sizeof(Park)) { DL("ERR: Mount::unPark(); ParkSize error NV subsystem writes disabled"); nv.readOnly(true); }
  nv.readBytes(NV_PARK_BASE, &park, ParkSize);

  // update state to parking
  park.state = PS_PARKING;
  nv.updateBytes(NV_PARK_BASE, &park, ParkSize);

  // get the park coordinate ready
  Coordinate parkTarget;
  parkTarget.h = park.position.h;
  parkTarget.d = park.position.d;
  parkTarget.pierSide = park.position.pierSide;

  // goto the park (mount) target coordinate
  e = gotoEqu(&parkTarget, PSS_SAME_ONLY, false);
  if (e != CE_NONE) {
    trackingState = priorTrackingState;
    updateTrackingRates();

    park.state = priorParkState;
    nv.updateBytes(NV_PARK_BASE, &park, ParkSize);

    VLF("ERR, parkGoto(): Failed to start goto");
    return e;
  } else { VLF("MSG: Parking started"); }
  return CE_NONE;
}

// records the park position, updates status, shuts down the stepper motors
void Mount::parkFinish() {
  if (park.state != PS_PARK_FAILED) {
    park.state = PS_PARKED;
    nv.updateBytes(NV_PARK_BASE, &park, ParkSize);

    //saveAlignModel();

    VLF("MSG: Parking done");
  } else { DLF("ERR, parkFinish(): Parking failed"); }

  axis1.enable(false);
  axis2.enable(false);
}

// adjusts targetAxis1/2 to the nearest park position for micro-step modes up to PARK_MAX_MICROSTEP
#define PARK_MAX_MICROSTEP 256
void Mount::parkNearest() {
  axis1.disableBacklash();
  long positionSteps = axis1.getTargetCoordinateSteps();
  positionSteps -= PARK_MAX_MICROSTEP*2L; 
  for (int l = 0; l < PARK_MAX_MICROSTEP*4; l++) { if (positionSteps % PARK_MAX_MICROSTEP*4L == 0) break; positionSteps++; }
  axis1.enableBacklash();
  axis1.setTargetCoordinateSteps(positionSteps);

  axis2.disableBacklash();
  positionSteps = axis2.getTargetCoordinateSteps();
  positionSteps -= PARK_MAX_MICROSTEP*2L; 
  for (int l = 0; l < PARK_MAX_MICROSTEP*4; l++) { if (positionSteps % PARK_MAX_MICROSTEP*4L == 0) break; positionSteps++; }
  axis2.enableBacklash();
  axis2.setTargetCoordinateSteps(positionSteps);
}

CommandError Mount::parkRestore(bool withTrackingOn) {
  if (!park.saved)                      return CE_NO_PARK_POSITION_SET;
  if (park.state != PS_PARKED) {
    #if STRICT_PARKING == ON
      VLF("MSG: Unpark ignored, not parked"); return CE_NOT_PARKED;
    #endif
    if (!atHome)                        return CE_NOT_PARKED;
  }
  if (gotoState == GS_NONE)             return CE_SLEW_IN_MOTION;
  if (guideState == GU_NONE)            return CE_SLEW_IN_MOTION;
  if (axis1.fault() || axis2.fault())   return CE_SLEW_ERR_HARDWARE_FAULT;
  if (!transform.site.dateTimeReady())  return CE_PARKED;

  VLF("MSG: Unparking");

  // make sure limits are on
  limitsEnabled = true;

  // stop tracking and disable the stepper drivers
  setTrackingState(TS_NONE);
  updateTrackingRates();
  axis1.enable(false);
  axis2.enable(false);

  // load the pointing model
  // loadAlignModel();

  // get the park info from NV
  if (ParkSize < sizeof(Park)) { DL("ERR: Mount::parkRestore(); ParkSize error NV subsystem writes disabled"); nv.readOnly(true); }
  nv.readBytes(NV_PARK_BASE, &park, ParkSize);

  axis1.setMotorCoordinateSteps(0);
  axis2.setMotorCoordinateSteps(0);

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

  // update our state and start tracking
  if (withTrackingOn) {
    park.state = PS_UNPARKED;
    nv.updateBytes(NV_PARK_BASE, &park, ParkSize);
    setTrackingState(TS_SIDEREAL);
    updateTrackingRates();
  }

  VLF("MSG: Unparking done");
  return CE_NONE;
}

#endif
