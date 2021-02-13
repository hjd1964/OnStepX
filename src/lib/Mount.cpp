//--------------------------------------------------------------------------------------------------
// telescope mount control
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../coordinates/Convert.h"
extern Convert convert;
#include "../coordinates/Transform.h"
extern Transform transform;
#include "../commands/ProcessCmds.h"
extern GeneralErrors generalErrors;
#include "../StepDrivers/StepDrivers.h"
#include "Axis.h"
#include "Clock.h"
extern Clock clock;
#include "Mount.h"

extern Axis axis1;
extern Axis axis2;

void Mount::init(int8_t mountType) {
  this->mountType = mountType;
  if (mountType == GEM) meridianFlip = MF_ALWAYS;

  // setup axis1
  VLF("MSG: Mount::init, starting telescope mount axis1");
  axis1.init(1);
  axis1.setStepsPerMeasure(radToDeg(AXIS1_STEPS_PER_DEGREE));
  axis1.setMinCoordinate(degToRad(-180.0));
  axis1.setMaxCoordinate(degToRad(180.0));
  axis1.setInstrumentCoordinate(degToRad(90.0));
  axis1.enable(true);

  // setup axis2
  VLF("MSG: Mount::init, starting telescope mount axis2");
  axis2.init(2);
  axis2.setStepsPerMeasure(radToDeg(AXIS2_STEPS_PER_DEGREE));
  axis2.setMinCoordinate(degToRad(-90.0));
  axis2.setMaxCoordinate(degToRad(90.0));
  axis2.setInstrumentCoordinate(degToRad(90.0));
  axis2.enable(true);

  // ------------------------------------------------------------------------------------------------
  // move in measures (radians) per second, tracking_enabled
  VLF("MSG: Mount::init, starting tracking");
  axis1.setFrequencyMax(degToRad(4.0));
  axis1.setFrequency(arcsecToRad(15.0*SIDEREAL_RATIO));
  axis1.setTracking(true);

}

bool Mount::command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError) {
  char *conv_end;
  PrecisionMode precisionMode = convert.precision;

  tasks_mutex_enter(MX_TELESCOPE_CMD);
  
  //  C - Sync Control
  // :CS#       Synchonize the telescope with the current right ascension and declination coordinates
  //            Returns: Nothing (Sync's fail silently)
  // :CM#       Synchonize the telescope with the current database object (as above)
  //            Returns: "N/A#" on success, "En#" on failure where n is the error code per the :MS# command
  if (cmd("CS") || cmd("CM")) {
    CommandError e;
    //if (alignActive()) { e = alignStar(); if (e != CE_NONE) { alignNumStars = 0; alignThisStar = 0; commandError = e; } } else e = syncEqu(syncTarget);
    e = syncEqu(target);
    if (command[1] == 'M') {
      if (e >= CE_GOTO_ERR_BELOW_HORIZON && e <= CE_GOTO_ERR_UNSPECIFIED) strcpy(reply,"E0"); reply[1] = (char)(e - CE_GOTO_ERR_BELOW_HORIZON) + '1';
      if (e == CE_NONE) strcpy(reply,"N/A");
    }
    *numericReply = false;
  } else

  // :GA#       Get Mount Altitude
  //            Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
  // :GAH#      High precision
  //            Returns: sDD*MM'SS.SSS# (high precision)
  if (cmdH("GA")) {
    updatePosition();
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(transform.mountToNative(&current, true).a), false, true, precisionMode);
    *numericReply = false;
  } else

  // :GZ#       Get Mount Azimuth
  //            Returns: DDD*MM# or DDD*MM'SS# (based on precision setting)
  // :GZH#      High precision
  //            Returns: DDD*MM'SS.SSS# (high precision)
  if (cmdH("GZ")) {
    updatePosition();
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(transform.mountToNative(&current, true).z), true, false, precisionMode);
    *numericReply = false;
  } else

  // :Ga#       Get Target Altitude
  //            Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
  // :GaH#      High precision
  //            Returns: sDD*MM'SS.SSS# (high precision)
  if (cmdH("Ga"))  {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToHms(reply, radToDeg(target.a), false, precisionMode);
    *numericReply = false;
  } else

  // :Gz#       Get Target Azimuth
  //            Returns: DDD*MM# or DDD*MM'SS# (based on precision setting)
  // :GzH#      High precision
  //            Returns: DDD*MM'SS.SSS# (high precision)
  if (cmdH("Gz"))  {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(target.z), false, true, precisionMode);
    *numericReply = false;
  } else

  //  :Sa[sDD*MM]# or :Sa[sDD*MM'SS]# or :Sa[sDD*MM'SS.SSS]#
  //            Set Target Altitude
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sa"))  {
    if (!convert.dmsToDouble(&target.a, parameter, true)) *commandError = CE_PARAM_RANGE;
    target.a = degToRad(target.a);
  } else

  //  :Sz[DDD*MM]# or :Sz[DDD*MM'SS]# or :Sz[DDD*MM'SS.SSS]#
  //            Set Target Azmuith
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sz"))  {
    if (!convert.dmsToDouble(&target.z, parameter, false)) *commandError = CE_PARAM_RANGE;
    target.z = degToRad(target.z);
  } else

  // :GR#       Get Mount Right Ascension
  //            Returns: HH:MM.T# or HH:MM:SS# (based on precision setting)
  // :GRH#      Returns: HH:MM:SS.SSSS# (high precision)
  if (cmdH("GR")) {
    updatePosition();
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToHms(reply, radToHrs(transform.mountToNative(&current).r), false, precisionMode);
    *numericReply = false;
  } else

  // :GD#       Get Mount Declination
  //            Returns: sDD*MM# or sDD*MM:SS# (based on precision setting)
  // :GDH#      Returns: sDD*MM:SS.SSS# (high precision)
  if (cmdH("GD"))  {
    updatePosition();
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(transform.mountToNative(&current).d), false, true, precisionMode);
    *numericReply = false;
  } else 

  // :Gr#       Get Target Right Ascension
  //            Returns: HH:MM.T# or HH:MM:SS (based on precision setting)
  // :GrH#      Returns: HH:MM:SS.SSSS# (high precision)
  if (cmdH("Gr"))  {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToHms(reply, radToHrs(target.r), false, precisionMode);
    *numericReply = false;
  } else

  // :Gd#       Get Target Declination
  //            Returns: HH:MM.T# or HH:MM:SS (based on precision setting)
  // :GdH#      High precision
  //            Returns: HH:MM:SS.SSS# (high precision)
  if (cmdH("Gd"))  {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(target.d), false, true, precisionMode);
    *numericReply = false;
  } else

  //  :Sd[sDD*MM]# or :Sd[sDD*MM:SS]# or :Sd[sDD*MM:SS.SSS]#
  //            Set Target Declination
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sd"))  {
    if (!convert.dmsToDouble(&target.d, parameter, true)) *commandError = CE_PARAM_RANGE;
    target.d = degToRad(target.d);
  } else

  //  :Sr[HH:MM.T]# or :Sr[HH:MM:SS]# or :Sr[HH:MM:SS.SSSS]#
  //            Set Target Right Ascension
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sr"))  {
    if (!convert.hmsToDouble(&target.r, parameter)) *commandError = CE_PARAM_RANGE;
    target.r = hrsToRad(target.r);
  } else

// :GT#         Get tracking rate, 0.0 unless TrackingSidereal
//              Returns: n.n# (OnStep returns more decimal places than LX200 standard)
      if (cmd("GT"))  {
        dtostrf(siderealToHz(trackingRate), 0, 5, reply);
        *numericReply = false;
      } else 
      
  //  :ST[H.H]# Set Tracking Rate in Hz where 60.0 is solar rate
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("ST"))  {
    double f = strtod(parameter,&conv_end);
    if (&parameter[0] != conv_end && ((f >= 30.0 && f < 90.0) || fabs(f) < 0.1)) {
      if (fabs(f) < 0.1) trackingState = TS_NONE; else {
        if (trackingState == TS_NONE) { trackingState = TS_SIDEREAL; axis1.enable(true); axis2.enable(true); }
        trackingRate = hzToSidereal(f);
      }
      updateTrackingRates();
    } else *commandError = CE_PARAM_RANGE;
  } else

  // T - Tracking Commands
  //
  // :T+#       Master sidereal clock faster by 0.02 Hertz (stored in EEPROM)
  // :T-#       Master sidereal clock slower by 0.02 Hertz (stored in EEPROM)
  // :TS#       Track rate solar
  // :TL#       Track rate lunar
  // :TQ#       Track rate sidereal
  // :TR#       Master sidereal clock reset (to calculated sidereal rate, stored in EEPROM)
  // :TK#       Track rate king
  //            Returns: Nothing
  //
  // :Te#       Tracking enable
  // :Td#       Tracking disable
  // :To#       OnTrack enable
  // :Tr#       Track refraction enable
  // :Tn#       Track refraction disable
  // :T1#       Track single axis (disable Dec tracking on Eq mounts)
  // :T2#       Track dual axis
  //            Return: 0 on failure
  //                    1 on success
  if (command[0] == 'T' && parameter[0] == 0) {
    if (command[1] == '1' && mountType != ALTAZM) { rateCompensationDualAxis = false; } else                                // dual axis tracking off
    if (command[1] == '2' && mountType != ALTAZM) { rateCompensationDualAxis = true;  } else                                // dual axis tracking on
    if (command[1] == 'o' && mountType != ALTAZM) { rateCompensation = RC_FULL_RA; trackingRate = 1.0; } else               // full compensation on
    if (command[1] == 'r' && mountType != ALTAZM) { rateCompensation = RC_REFR_RA; trackingRate = 1.0; } else               // refraction compensation on
    if (command[1] == 'n' && mountType != ALTAZM) { rateCompensation = RC_NONE;    trackingRate = 1.0; }                    // compensation off
    if (command[1] == 'S') { rateCompensation = RC_NONE; trackingRate = hzToSidereal(60);     *numericReply = false; } else // solar tracking rate 60Hz
    if (command[1] == 'K') { rateCompensation = RC_NONE; trackingRate = hzToSidereal(60.136); *numericReply = false; } else // king tracking rate 60.136Hz
    if (command[1] == 'L') { rateCompensation = RC_NONE; trackingRate = hzToSidereal(57.9);   *numericReply = false; } else // lunar tracking rate 57.9Hz
    if (command[1] == 'Q') { trackingRate = hzToSidereal(SIDEREAL_RATE_HZ);                   *numericReply = false; } else // sidereal tracking rate
    if (command[1] == '+') { clock.setPeriodSubMicros((clock.getPeriodSubMicros() - hzToSubMicros(0.02))); *numericReply = false; } else
    if (command[1] == '-') { clock.setPeriodSubMicros((clock.getPeriodSubMicros() + hzToSubMicros(0.02))); *numericReply = false; } else
    if (command[1] == 'R') { clock.setPeriodSubMicros(SIDEREAL_PERIOD); *numericReply = false; } else                       // reset master sidereal clock interval
    if (command[1] == 'e') {
      if (parkState == PS_PARKED) *commandError = CE_PARKED; else { resetGeneralErrors(); trackingState = TS_SIDEREAL; axis1.enable(true); axis2.enable(true); }
    } else
    if (command[1] == 'd') {
      if (gotoState != GS_NONE || guideState != GU_NONE) *commandError = CE_MOUNT_IN_MOTION; else trackingState = TS_NONE;
    } else *commandError = CE_CMD_UNKNOWN;

    if (mountType != ALTAZM) {
      if (rateCompensation == RC_NONE) rateCompensationDualAxis = false; else
      if (rateCompensation == RC_REFR_RA   &&  rateCompensationDualAxis) rateCompensation = RC_REFR_BOTH; else
      if (rateCompensation == RC_REFR_BOTH && !rateCompensationDualAxis) rateCompensation = RC_REFR_RA;   else
      if (rateCompensation == RC_FULL_RA   &&  rateCompensationDualAxis) rateCompensation = RC_FULL_BOTH; else
      if (rateCompensation == RC_FULL_BOTH && !rateCompensationDualAxis) rateCompensation = RC_FULL_RA;
    }
    updateTrackingRates();

  } else

  { tasks_mutex_exit(MX_TELESCOPE_CMD); return false; }
  tasks_mutex_exit(MX_TELESCOPE_CMD); return true;
}

// check if goto/sync is valid
CommandError Mount::validateGoto() {
  // Check state
  if ( axis1.fault()     ||  axis2.fault())     return CE_SLEW_ERR_HARDWARE_FAULT;
  if (!axis1.isEnabled() || !axis2.isEnabled()) return CE_SLEW_ERR_IN_STANDBY;
  if (parkState != PS_UNPARKED)                 return CE_SLEW_ERR_IN_PARK;
  if (guideState != GU_NONE)                    return CE_MOUNT_IN_MOTION;
  if (gotoState == GS_GOTO_SYNC)                return CE_MOUNT_IN_MOTION;
  if (gotoState != GS_NONE)                     return CE_SLEW_IN_SLEW;
  return CE_NONE;
}

CommandError Mount::validateGotoCoords(Coordinate coords) {
  transform.equToHor(&coords);
  // Check coordinates
//if (coords.a < limits.getMinAltitude())       return CE_GOTO_ERR_BELOW_HORIZON;
//if (coords.a > limits.getMaxAltitude())       return CE_GOTO_ERR_ABOVE_OVERHEAD;
  if (AXIS2_TANGENT_ARM == OFF && mountType != ALTAZM) {
    if (coords.d < axis2.getMinCoordinate())    return CE_SLEW_ERR_OUTSIDE_LIMITS;
    if (coords.d > axis2.getMaxCoordinate())    return CE_SLEW_ERR_OUTSIDE_LIMITS;
  }
  if (coords.h < axis1.getMinCoordinate())      return CE_SLEW_ERR_OUTSIDE_LIMITS;
  if (coords.h > axis1.getMaxCoordinate())      return CE_SLEW_ERR_OUTSIDE_LIMITS;
  return CE_NONE;
}

// syncs the telescope/mount to the sky
CommandError Mount::syncEqu(Coordinate target) {
  CommandError e = validateGoto();
  if (e == CE_SLEW_ERR_IN_STANDBY && atHome) { tracking = true; axis1.enable(true); axis2.enable(true); e = validateGoto(); }
  if (e != CE_NONE) return e;

  double a1, a2;
  transform.nativeToMount(&target, &a1, &a2);

  e = validateGotoCoords(target);
  if (e != CE_NONE) return e;

  // east side of pier - we're in the western sky and the HA's are positive
  // west side of pier - we're in the eastern sky and the HA's are negative
  updatePosition();
  target.pierSide = current.pierSide;
  if (meridianFlip == MF_ALWAYS) {
    if (atHome) { if (a1 < 0) target.pierSide = PIER_SIDE_WEST; else target.pierSide = PIER_SIDE_EAST; } else
    #if PIER_SIDE_SYNC_CHANGE_SIDES == ON
      if (preferredPierSideDefault == WEST) { newPierSide = PIER_SIDE_WEST; if (a1 >  limits.pastMeridianW) target.pierSide = PIER_SIDE_EAST; } else
      if (preferredPierSideDefault == EAST) { newPierSide = PIER_SIDE_EAST; if (a1 < -limits.pastMeridianE) target.pierSide = PIER_SIDE_WEST; } else
    #endif
      {// preferredPierSideDefault == BEST
        if (current.pierSide == PIER_SIDE_WEST && a1 >  limits.pastMeridianW) target.pierSide = PIER_SIDE_EAST;
        if (current.pierSide == PIER_SIDE_EAST && a1 < -limits.pastMeridianE) target.pierSide = PIER_SIDE_WEST;
      }
    #if PIER_SIDE_SYNC_CHANGE_SIDES == OFF
      if (!atHome && target.pierSide != current.pierSide) return CE_SLEW_ERR_OUTSIDE_LIMITS;
    #endif
  } else {
    // east side of pier is always the default polar-home position
    target.pierSide = PIER_SIDE_EAST;
  }

  transform.mountToInstrument(&target, &a1, &a2);
  axis1.setInstrumentCoordinate(a1);
  axis2.setInstrumentCoordinate(a2);

  safetyLimitsOn = true;
  syncToEncodersOnly = true;

  VLF("MSG: Mount::syncEqu, instrument coordinates updated");

  return CE_NONE;
}

void Mount::updatePosition() {
  current = transform.instrumentToMount(axis1.getInstrumentCoordinate(), axis2.getInstrumentCoordinate());
}

void Mount::updateTrackingRates() {
  if (trackingState == TS_NONE) trackingRate = 0;
  if (mountType != ALTAZM) {
    trackingRateAxis1 = trackingRate;
    if (rateCompensation != RC_REFR_BOTH && rateCompensation != RC_FULL_BOTH) trackingRateAxis2 = 0;
  }
  axis1.setFrequency(siderealToRad(trackingRateAxis1 + guideRateAxis1 + deltaRateAxis1));
  axis2.setFrequency(siderealToRad(trackingRateAxis2 + guideRateAxis2 + deltaRateAxis2));
}

void Mount::resetGeneralErrors() {
  generalErrors.altitudeMin = false;
  generalErrors.limitSense = false;
  generalErrors.decMinMax = false;
  generalErrors.azmMinMax = false;
  generalErrors.raMinMax  = false;
  generalErrors.raMeridian = false;
  generalErrors.sync = false;
  generalErrors.altitudeMax = false;
  generalErrors.park = false;
}

#endif
