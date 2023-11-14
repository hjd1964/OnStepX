// -----------------------------------------------------------------------------------
// telescope mount control, parking

#include "Park.h"

#if defined(MOUNT_PRESENT)

#include "../../../lib/tasks/OnTask.h"

#include "../../Telescope.h"
#include "../Mount.h"
#include "../goto/Goto.h"
#include "../guide/Guide.h"
#include "../home/Home.h"
#include "../limits/Limits.h"
#include "../../../lib/sense/Sense.h"

void parkSignalWrapper() { park.signal(); }

void Park::init() {
  // confirm the data structure size
  if (ParkSettingsSize < sizeof(ParkSettings)) { nv.initError = true; DL("ERR: Park::Init(), ParkSettingsSize error"); }

  // write the default settings to NV
  if (!nv.hasValidKey() || nv.isNull(NV_MOUNT_PARK_BASE, sizeof(ParkSettings))) {
    VLF("MSG: Mount, park writing defaults to NV");
    nv.writeBytes(NV_MOUNT_PARK_BASE, &settings, sizeof(ParkSettings));
    // set the initial park position at home
    state = settings.state;
    set();
  }

  // read the settings
  nv.readBytes(NV_MOUNT_PARK_BASE, &settings, sizeof(ParkSettings));
  state = settings.state;

  // configure any associated sense/signal pins
  #if (PARK_SENSE) != OFF && (PARK_SENSE_PIN) != OFF
    VLF("MSG: Mount, park adding sense");
    parkSenseHandle = sense.add(PARK_SENSE_PIN, PARK_SENSE_INIT, PARK_SENSE);
  #endif

  #if (PARK_SIGNAL) != OFF && (PARK_SIGNAL_PIN) != OFF
    VLF("MSG: Mount, park adding signal");
    parkSignalHandle = sense.add(PARK_SIGNAL_PIN, PARK_SIGNAL_INIT, PARK_SIGNAL);
    
    VF("MSG: Mount, start park signal monitor task (rate 1000ms priority 4)... ");
    if (tasks.add(1000, 0, true, 4, parkSignalWrapper, "ParkSgl")) { VLF("success"); } else { VLF("FAILED!"); }
  #endif
}

// sets the park position
CommandError Park::set() {
  if (state == PS_PARK_FAILED) return CE_PARK_FAILED;
  if (state == PS_PARKED)      return CE_PARKED;
  if (goTo.state != GS_NONE)   return CE_SLEW_IN_MOTION;
  if (guide.state != GU_NONE)  return CE_SLEW_IN_MOTION;
  if (mount.motorFault())      return CE_SLEW_ERR_HARDWARE_FAULT;

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
    if (!settings.saved)         return CE_NO_PARK_POSITION_SET;
    if (state == PS_PARKED)      return CE_NONE;
    if (state == PS_PARKING)     return CE_PARK_FAILED;
    if (state == PS_PARK_FAILED) return CE_PARK_FAILED;
    if (!mount.isEnabled())      return CE_SLEW_ERR_IN_STANDBY;
    if (goTo.state != GS_NONE)   return CE_SLEW_IN_MOTION;
    if (guide.state != GU_NONE)  return CE_SLEW_IN_MOTION;
    if (mount.motorFault())      return CE_SLEW_ERR_HARDWARE_FAULT;

    CommandError e = goTo.validate();
    if (e != CE_NONE) return e;
    
    // stop tracking
    wasTracking = mount.isTracking();
    mount.tracking(false);
    mount.enable(true);
    goTo.firstGoto = false;

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
    if (transform.mountType == ALTAZM) transform.equToHor(&parkTarget);

    // goto the park (mount) target coordinate
    VLF("MSG: Mount, parking");
    if (parkTarget.pierSide == PIER_SIDE_EAST) e = goTo.request(parkTarget, PSS_EAST_ONLY, false); else
    if (parkTarget.pierSide == PIER_SIDE_WEST) e = goTo.request(parkTarget, PSS_WEST_ONLY, false);

    if (e != CE_NONE) {
      mount.tracking(wasTracking);

      state = priorParkState;
      settings.state = state;
      nv.updateBytes(NV_MOUNT_PARK_BASE, &settings, sizeof(ParkSettings));

      VF(": Mount::parkGoto(), Failed to start goto (CE "); V(e); VL(")");
      return e;
    }
  return CE_NONE;
}

// clear park state on abort
void Park::requestAborted() {
  state = PS_UNPARKED;
  settings.state = state;
  nv.updateBytes(NV_MOUNT_PARK_BASE, &settings, sizeof(ParkSettings));
  
  // restore backlash settings
  axis1.setBacklash(mount.settings.backlash.axis1);
  axis2.setBacklash(mount.settings.backlash.axis2);
  
  mount.tracking(wasTracking);
}

// once parked save the park state
void Park::requestDone() {

  #if (PARK_SENSE) != OFF && (PARK_SENSE_PIN) != OFF
    if (sense.isOn(parkSenseHandle)) {
      VLF("MSG: Mount, park sense state indicates success.");
    } else {
      DLF("WRN: Mount, park sense state failed!");
      state = PS_PARK_FAILED;
    }
  #endif

  if (state != PS_PARK_FAILED) {
    #if DEBUG == VERBOSE
      long index = axis1.getInstrumentCoordinateSteps() - axis1.getMotorPositionSteps();
      VF("MSG: Mount, park axis1 motor target   "); VL(axis1.getTargetCoordinateSteps() - index);
      VF("MSG: Mount, park axis1 motor position "); VL(axis1.getMotorPositionSteps());
      index = axis2.getInstrumentCoordinateSteps() - axis2.getMotorPositionSteps();
      VF("MSG: Mount, park axis2 motor target   "); VL(axis2.getTargetCoordinateSteps() - index);
      VF("MSG: Mount, park axis2 motor position "); VL(axis2.getMotorPositionSteps());
    #endif

    // save the axis state
    state = PS_PARKED;
    settings.state = state;
    nv.updateBytes(NV_MOUNT_PARK_BASE, &settings, sizeof(ParkSettings));

    #if ALIGN_MAX_NUM_STARS > 1  
      transform.align.modelWrite();
    #endif

    VLF("MSG: Mount, parking done");
  } else { DLF("ERR: Mount::parkFinish(), Parking failed"); }

  mount.enable(MOUNT_ENABLE_IN_STANDBY == ON);
}

// returns a parked telescope to operation
CommandError Park::restore(bool withTrackingOn) {
  if (!settings.saved) return CE_NO_PARK_POSITION_SET;
  if (state != PS_PARKED) {
    #if PARK_STRICT == ON
      VLF("MSG: Mount, unpark ignored not parked");
      return CE_NOT_PARKED;
    #else
      if (!mount.isHome()) {
        VLF("MSG: Mount, unpark when not parked allowed at home only");
        return CE_NOT_PARKED;
      }
    #endif
  }
  if (!site.isDateTimeReady()) {
    VLF("MSG: Mount, unpark postponed no date/time");
    return CE_PARKED;
  }
  if (mount.motorFault()) return CE_SLEW_ERR_HARDWARE_FAULT;

  if (withTrackingOn) {
    VLF("MSG: Mount, unparking");
  } else {
    VLF("MSG: Mount, recovering unpark position");
  }

  #if AXIS1_PEC == ON
    wormSenseSteps = settings.wormSensePositionSteps;
  #endif

  if (!goTo.absoluteEncodersPresent) {

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
    if (parkTarget.pierSide == PIER_SIDE_EAST && parkTarget.h < -limits.settings.pastMeridianE) parkTarget.h += PI*2.0;
    if (parkTarget.pierSide == PIER_SIDE_WEST && parkTarget.h > limits.settings.pastMeridianW) parkTarget.h -= PI*2.0;

    // set the mount target
    double a1, a2;
    if (transform.mountType == ALTAZM) transform.equToHor(&parkTarget);
    transform.mountToInstrument(&parkTarget, &a1, &a2);
    axis1.setInstrumentCoordinatePark(a1);
    axis2.setInstrumentCoordinatePark(a2);

    VF("MSG: Mount, unpark axis1 motor position "); VL(axis1.getMotorPositionSteps());
    VF("MSG: Mount, unpark axis2 motor position "); VL(axis2.getMotorPositionSteps());

    // restore backlash settings
    axis1.setBacklash(mount.settings.backlash.axis1);
    axis2.setBacklash(mount.settings.backlash.axis2);
  }
  
  limits.enabled(true);
  if (!goTo.absoluteEncodersPresent) mount.syncFromOnStepToEncoders = true;

  if (withTrackingOn) {
    state = PS_UNPARKED;
    settings.state = state;
    nv.updateBytes(NV_MOUNT_PARK_BASE, &settings, sizeof(ParkSettings));
    mount.tracking(true);
    VLF("MSG: Mount, unparking done");
  } else {
    VLF("MSG: Mount, recovering unpark position done");
  }

  return CE_NONE;
}

// check input pin to initiate a park operation, if allowed
void Park::signal() {
  #if PARK_SIGNAL != OFF && PARK_SIGNAL_PIN != OFF
    if (sense.isOn(parkSignalHandle) && state == PS_UNPARKED && !mount.isSlewing() && !mount.isHome()) {
      request();
    }
  #endif
}

Park park;

#endif
