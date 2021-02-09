//--------------------------------------------------------------------------------------------------
// telescope control
#pragma once

typedef struct Limits {
  double horizon;
  double overhead;
  double pastMeridianE;
  double pastMeridianW;
} Limits;

enum MeridianFlip {MeridianFlipNever,  MeridianFlipAlign, MeridianFlipAlways};

class Telescope {
  public:
    // handle telescope commands
    bool command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandErrors *commandError);

    CommandErrors validateGoto();
    CommandErrors validateGotoCoords(Coordinate coords);

    CommandErrors syncEqu(Coordinate target);

  private:
    void updatePosition();
    Coordinate position, target;

    Limits limits = {degToRad(-10), degToRad(85), degToRad(15), degToRad(15)};
    bool tracking = false;
    bool atHome = true;
    bool safetyLimitsOn = false;
    bool syncToEncodersOnly = false;
    MeridianFlip meridianFlip = MeridianFlipAlways;
};

bool Telescope::command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandErrors *commandError) {
  tasks_mutex_enter(MX_TELESCOPE_CMD);
  PrecisionMode precisionMode = convert.precision;
  
  // :GR#       Get Telescope RA
  //            Returns: HH:MM.T# or HH:MM:SS# (based on precision setting)
  // :GRH#      Returns: HH:MM:SS.SSSS# (high precision)
  if (cmdH("GR")) {
    updatePosition();
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToHms(reply, radToHrs(transform.mountToNative(&position).r), false, precisionMode);
    *numericReply = false;
  } else

  // :GD#       Get Telescope Declination
  //            Returns: sDD*MM# or sDD*MM:SS# (based on precision setting)
  // :GDH#      Returns: sDD*MM:SS.SSS# (high precision)
  if (cmdH("GD"))  {
    updatePosition();
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(transform.mountToNative(&position).d), false, true, precisionMode);
    *numericReply = false;
  } else 

  // :Gr#       Get Target RA
  //            Returns: HH:MM.T# or HH:MM:SS (based on precision setting)
  // :GrH#      Returns: HH:MM:SS.SSSS# (high precision)
  if (cmdH("Gr"))  {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToHms(reply, radToHrs(target.r), false, precisionMode);
    *numericReply = false;
  } else

  // :Gd#       Get Target Dec
  //            Returns: HH:MM.T# or HH:MM:SS (based on precision setting)
  // :GdH#      High precision
  //            Returns: HH:MM:SS.SSS# (high precision)
  if (cmdH("Gd"))  {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(target.d), false, true, precisionMode);
    *numericReply = false;
  } else

  //  :Sd[sDD*MM]# or :Sd[sDD*MM:SS]# or :Sd[sDD*MM:SS.SSS]#
  //            Set target object declination
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sd"))  {
    if (!convert.dmsToDouble(&target.d, parameter, true)) *commandError = CE_PARAM_RANGE;
    target.d = degToRad(target.d);
  } else

  //  :Sr[HH:MM.T]# or :Sr[HH:MM:SS]# or :Sr[HH:MM:SS.SSSS]#
  //            Set target object RA
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sr"))  {
    if (!convert.hmsToDouble(&target.r, parameter)) *commandError = CE_PARAM_RANGE;
    target.r = hrsToRad(target.r);
  } else

  { tasks_mutex_exit(MX_TELESCOPE_CMD); return false; }
  tasks_mutex_exit(MX_TELESCOPE_CMD); return true;
}

// check if goto/sync is valid
CommandErrors Telescope::validateGoto() {
  // Check state
//if (parkStatus != NotParked)            return CE_SLEW_ERR_IN_PARK;
  if (!axis1.isEnabled())                 return CE_SLEW_ERR_IN_STANDBY;
//if (syncing())                          return CE_MOUNT_IN_MOTION;
//if (moving())                           return CE_GOTO_ERR_GOTO;
//if (guide.moving())                     return CE_MOUNT_IN_MOTION;
  if (axis1.fault() || axis2.fault())     return CE_SLEW_ERR_HARDWARE_FAULT;
  return CE_NONE;
}

CommandErrors Telescope::validateGotoCoords(Coordinate coords) {
  transform.equToHor(&coords);
  // Check coordinates
//if (coords.a < limits.getMinAltitude())    return CE_GOTO_ERR_BELOW_HORIZON;
//if (coords.a > limits.getMaxAltitude())    return CE_GOTO_ERR_ABOVE_OVERHEAD;
  if (AXIS2_TANGENT_ARM == OFF && mountType != ALTAZM) {
    if (coords.d < axis2.getMinCoordinate()) return CE_SLEW_ERR_OUTSIDE_LIMITS;
    if (coords.d > axis2.getMaxCoordinate()) return CE_SLEW_ERR_OUTSIDE_LIMITS;
  }
  if (coords.h < axis1.getMinCoordinate())   return CE_SLEW_ERR_OUTSIDE_LIMITS;
  if (coords.h > axis1.getMaxCoordinate())   return CE_SLEW_ERR_OUTSIDE_LIMITS;
  return CE_NONE;
}

// syncs the telescope/mount to the sky
CommandErrors Telescope::syncEqu(Coordinate target) {
  // validate
  CommandErrors e = validateGoto();
  if (e == CE_SLEW_ERR_IN_STANDBY && atHome) { tracking = true; axis1.enable(true); axis2.enable(true); e = validateGoto(); }
  if (e != CE_NONE) return e;
  e = validateGotoCoords(target);
  if (e != CE_NONE) return e;

  double a1, a2;
  if (mountType == ALTAZM) {
    transform.equToHor(&target);
    // AlignH.horToInstr(Axis2, Axis1, &Axis2, &Axis1, getInstrPierSide());
    a1 = target.z;
    a2 = target.a;
    // Axis1 = haRange(Axis1);
  } else {
    // AlignE.equToInstr(HA, Dec, &Axis1, &Axis2, getInstrPierSide());
    a1 = target.h;
    a2 = target.d;
  }

  // west side of pier - we're in the eastern sky and the HA's are negative
  // east side of pier - we're in the western sky and the HA's are positive
  updatePosition();
  PierSide newPierSide = position.p;
  if (meridianFlip != MeridianFlipNever) {
    // best side of pier decided based on meridian
    if (atHome) { if (a1 < 0) newPierSide = PIER_SIDE_WEST; else newPierSide = PIER_SIDE_EAST; } else
#if PIER_SIDE_SYNC_CHANGE_SIDES == ON
    if (preferredPierSideDefault == WEST) { newPierSide = PIER_SIDE_WEST; if (a1 >  limits.pastMeridianW) newPierSide = PIER_SIDE_EAST; } else
    if (preferredPierSideDefault == EAST) { newPierSide = PIER_SIDE_EAST; if (a1 < -limits.pastMeridianE) newPierSide = PIER_SIDE_WEST; } else
#endif
    {
      if ((position.p == PIER_SIDE_WEST) && (a1 >  limits.pastMeridianW)) newPierSide = PIER_SIDE_EAST;
      if ((position.p == PIER_SIDE_EAST) && (a1 < -limits.pastMeridianE)) newPierSide = PIER_SIDE_WEST;
    }

#if PIER_SIDE_SYNC_CHANGE_SIDES == OFF
    if (!atHome && newPierSide != position.p) return CE_SLEW_ERR_OUTSIDE_LIMITS;
#endif

  } else {
    // always on the "east" side of pier - we're in the western sky and the HA's are positive
    // this is the default in the polar-home position and also for MOUNT_TYPE FORK and MOUNT_TYPE ALTAZM.
    newPierSide = PIER_SIDE_EAST;
  }

  //setIndexAxis1(Axis1, newPierSide);
  //setIndexAxis2(Axis2, newPierSide);
  safetyLimitsOn = true;
  syncToEncodersOnly = true;

  VLF("MSG: Sync, indices set");

  return CE_NONE;
}

void Telescope::updatePosition() {
  position = transform.instrumentToMount(axis1.getInstrumentCoordinate(), axis2.getInstrumentCoordinate());
}

Telescope telescope;
