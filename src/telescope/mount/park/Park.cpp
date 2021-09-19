// -----------------------------------------------------------------------------------
// telescope mount control, parking

#include "Park.h"

#if defined(MOUNT_PRESENT) && SLEW_GOTO == ON

#include "../../Telescope.h"
#include "../Mount.h"
#include "../goto/Goto.h"
#include "../guide/Guide.h"
#include "../home/Home.h"
#include "../limits/Limits.h"

void Park::init() {
  // confirm the data structure size
  if (ParkSettingsSize < sizeof(ParkSettings)) { initError.nv = true; DL("ERR: Park::Init(); ParkSettingsSize error NV subsystem writes disabled"); nv.readOnly(true); }

  // write the default settings to NV
  if (!validKey) {
    VLF("MSG: Mount, park writing default settings to NV");
    nv.writeBytes(NV_MOUNT_PARK_BASE, &settings, sizeof(ParkSettings));
  }

  // read the settings
  nv.readBytes(NV_MOUNT_PARK_BASE, &settings, sizeof(ParkSettings));
  state = settings.state;
}

// sets the park position
CommandError Park::set() {
  if (state == PS_PARK_FAILED) return CE_PARK_FAILED;
  if (state == PS_PARKED)      return CE_PARKED;
  if (goTo.state != GS_NONE)   return CE_SLEW_IN_MOTION;
  if (guide.state != GU_NONE)  return CE_SLEW_IN_MOTION;
  if (mount.isFault())         return CE_SLEW_ERR_HARDWARE_FAULT;

  VLF("MSG: Mount, setting park position");

  bool wasTracking = mount.isTracking();
  mount.tracking(false);

  // get our current position in equatorial coordinates
  Coordinate current = mount.getPosition(CR_MOUNT_EQU);

  // save as the park position
  settings.state = state;
  settings.position.h = current.h;
  settings.position.d = current.d;
  settings.position.pierSide = current.pierSide;
  if (settings.position.pierSide == PIER_SIDE_NONE) {
    if (transform.meridianFlips) {
      if (current.h < 0) settings.position.pierSide = PIER_SIDE_WEST; else settings.position.pierSide = PIER_SIDE_EAST;
    } else settings.position.pierSide = PIER_SIDE_EAST;
  }
  settings.saved = true;
  nv.updateBytes(NV_MOUNT_PARK_BASE, &settings, sizeof(ParkSettings));

  #if ALIGN_MAX_NUM_STARS > 1
    transform.align.modelWrite();
  #endif

  mount.tracking(wasTracking);
  
  VLF("MSG: Mount, setting park done");
  return CE_NONE;
}

// move the mount to the park position
CommandError Park::request() {
  #if SLEW_GOTO == ON
    if (!settings.saved)         return CE_NO_PARK_POSITION_SET;
    if (state == PS_PARKED)      return CE_NONE;
    if (state == PS_PARKING)     return CE_PARK_FAILED;
    if (state == PS_PARK_FAILED) return CE_PARK_FAILED;
    if (!mount.isEnabled())      return CE_SLEW_ERR_IN_STANDBY;
    if (mount.isFault())         return CE_SLEW_ERR_HARDWARE_FAULT;
    if (goTo.state != GS_NONE)   return CE_SLEW_IN_MOTION;
    if (guide.state != GU_NONE)  return CE_SLEW_IN_MOTION;

    CommandError e = goTo.validate();
    if (e != CE_NONE) return e;
    
    // stop tracking
    bool wasTracking = mount.isTracking();
    mount.tracking(false);

    #if AXIS1_PEC == ON
      // turn off PEC while we park
      pecDisable();
      pec.state = PEC_NONE;
      settings.wormSensePositionSteps = wormSenseSteps;
    #endif

    // record our park status
    ParkState priorParkState = state;
    
    // update state to parking
    state = PS_PARKING;
    settings.state = state;
    nv.updateBytes(NV_MOUNT_PARK_BASE, &settings, sizeof(ParkSettings));

    // get the park coordinate ready
    axis1.setBacklash(0.0L);
    axis2.setBacklash(0.0L);
    Coordinate parkTarget;
    parkTarget.h = settings.position.h;
    parkTarget.d = settings.position.d;
    parkTarget.pierSide = settings.position.pierSide;

    // goto the park (mount) target coordinate
    VLF("MSG: Mount, parking");
    if (parkTarget.pierSide == PIER_SIDE_EAST) e = goTo.request(&parkTarget, PSS_EAST_ONLY, false); else
    if (parkTarget.pierSide == PIER_SIDE_WEST) e = goTo.request(&parkTarget, PSS_WEST_ONLY, false);

    if (e != CE_NONE) {
      mount.tracking(wasTracking);

      state = priorParkState;
      settings.state = state;
      nv.updateBytes(NV_MOUNT_PARK_BASE, &settings, sizeof(ParkSettings));

      VF("ERR, Mount::parkGoto(); Failed to start goto (CE "); V(e); V(")");
      return e;
    }
  #endif
  return CE_NONE;
}

// once parked save the park state
void Park::requestDone() {
  if (state != PS_PARK_FAILED) {
    #if DEBUG == VERBOSE
      long index = axis1.getInstrumentCoordinateSteps() - axis1.getMotorPositionSteps();
      V("MSG: Mount, park axis1 motor target   "); VL(axis1.getTargetCoordinateSteps() - index);
      V("MSG: Mount, park axis1 motor position "); VL(axis1.getMotorPositionSteps());
      index = axis2.getInstrumentCoordinateSteps() - axis2.getMotorPositionSteps();
      V("MSG: Mount, park axis2 motor target   "); VL(axis2.getTargetCoordinateSteps() - index);
      V("MSG: Mount, park axis2 motor position "); VL(axis2.getMotorPositionSteps());
    #endif

    // save the axis state
    state = PS_PARKED;
    settings.state = state;
    nv.updateBytes(NV_MOUNT_PARK_BASE, &settings, sizeof(ParkSettings));

    #if ALIGN_MAX_NUM_STARS > 1  
      transform.align.modelWrite();
    #endif

    VLF("MSG: Mount, parking done");
  } else { DLF("ERR, Mount::parkFinish(); Parking failed"); }

  axis1.enable(false);
  axis2.enable(false);
}

// returns a parked telescope to operation
CommandError Park::restore(bool withTrackingOn) {
  if (!settings.saved)         return CE_NO_PARK_POSITION_SET;
  if (state != PS_PARKED) {
    #if STRICT_PARKING == ON
      VLF("MSG: Unpark ignored, not parked");
      return CE_NOT_PARKED;
    #endif
    if (!mount.isHome())       return CE_NOT_PARKED;
  }
  if (goTo.state != GS_NONE)   return CE_SLEW_IN_MOTION;
  if (guide.state != GU_NONE)  return CE_SLEW_IN_MOTION;
  if (mount.isFault())         return CE_SLEW_ERR_HARDWARE_FAULT;
  if (!site.isDateTimeReady()) return CE_PARKED;

  VLF("MSG: Unparking");

  #if AXIS1_PEC == ON
    wormSenseSteps = settings.wormSensePositionSteps;
  #endif

  // reset the mount, zero backlash
  home.reset();
  axis1.setBacklashSteps(0);
  axis2.setBacklashSteps(0);

  // load the pointing model
  #if ALIGN_MAX_NUM_STARS > 1  
    transform.align.modelRead();
  #endif

  // get the park coordinate ready
  Coordinate parkTarget;
  parkTarget.h = settings.position.h;
  parkTarget.d = settings.position.d;
  parkTarget.pierSide = settings.position.pierSide;

  // set the mount target
  double a1, a2;
  if (transform.mountType == ALTAZM) transform.equToHor(&parkTarget);
  transform.mountToInstrument(&parkTarget, &a1, &a2);
  axis1.setInstrumentCoordinatePark(a1);
  axis2.setInstrumentCoordinatePark(a2);

  V("MSG: Mount, unpark axis1 motor position "); VL(axis1.getMotorPositionSteps());
  V("MSG: Mount, unpark axis2 motor position "); VL(axis2.getMotorPositionSteps());

  // restore backlash settings
  axis1.setBacklash(mount.settings.backlash.axis1);
  axis2.setBacklash(mount.settings.backlash.axis2);
  
  state = PS_UNPARKED;
  settings.state = state;
  nv.updateBytes(NV_MOUNT_PARK_BASE, &settings, sizeof(ParkSettings));

  limits.enabled(true);
  mount.syncToEncoders(true);
  if (withTrackingOn) mount.tracking(true);

  VLF("MSG: Mount, unparking done");
  return CE_NONE;
}

Park park;

#endif
