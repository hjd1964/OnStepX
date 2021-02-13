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
  VF("MSG: Mount::init, type "); VL(mountType);
  this->mountType = mountType;
  if (mountType == GEM) meridianFlip = MF_ALWAYS;

  // setup axis1
  axis1.init(1);
  axis1.setStepsPerMeasure(radToDeg(AXIS1_STEPS_PER_DEGREE));
  axis1.setMinCoordinate(degToRad(-180.0));
  axis1.setMaxCoordinate(degToRad(180.0));
  axis1.setInstrumentCoordinate(degToRad(90.0));
  axis1.enable(true);

  // setup axis2
  axis2.init(2);
  axis2.setStepsPerMeasure(radToDeg(AXIS2_STEPS_PER_DEGREE));
  axis2.setMinCoordinate(degToRad(-90.0));
  axis2.setMaxCoordinate(degToRad(90.0));
  axis2.setInstrumentCoordinate(degToRad(90.0));
  axis2.enable(true);

  // ------------------------------------------------------------------------------------------------
  // move in measures (radians) per second, tracking_enabled
  VLF("MSG: Mount::init, starting tracking");
  trackingState = TS_SIDEREAL;
  axis1.setFrequencyMax(degToRad(4.0));
  axis1.setTracking(true);
  trackingRate = hzToSidereal(SIDEREAL_RATE_HZ);
  updateTrackingRates();
}

bool Mount::command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError) {
  char *conv_end;
  PrecisionMode precisionMode = convert.precision;

  //  C - Sync Control
  // :CS#       Synchonize the telescope with the current right ascension and declination coordinates
  //            Returns: Nothing (Sync's fail silently)
  // :CM#       Synchonize the telescope with the current database object (as above)
  //            Returns: "N/A#" on success, "En#" on failure where n is the error code per the :MS# command
  if (cmd("CS") || cmd("CM")) {
    tasks_mutex_enter(MX_TELESCOPE_CMD);
    CommandError e;
    //if (alignActive()) { e = alignStar(); if (e != CE_NONE) { alignNumStars = 0; alignThisStar = 0; commandError = e; } } else e = syncEqu(syncTarget);
    e = syncEqu(target);
    if (command[1] == 'M') {
      if (e >= CE_GOTO_ERR_BELOW_HORIZON && e <= CE_GOTO_ERR_UNSPECIFIED) strcpy(reply,"E0"); reply[1] = (char)(e - CE_GOTO_ERR_BELOW_HORIZON) + '1';
      if (e == CE_NONE) strcpy(reply,"N/A");
    }
    *numericReply = false;
    tasks_mutex_exit(MX_TELESCOPE_CMD);
  } else return false;

  // :GA#       Get Mount Altitude
  //            Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
  // :GAH#      High precision
  //            Returns: sDD*MM'SS.SSS# (high precision)
  if (cmdH("GA")) {
    tasks_mutex_enter(MX_TELESCOPE_CMD);
    updatePosition();
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(transform.mountToNative(&current, true).a), false, true, precisionMode);
    *numericReply = false;
    tasks_mutex_exit(MX_TELESCOPE_CMD);
  } else

  // :Ga#       Get Target Altitude
  //            Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
  // :GaH#      High precision
  //            Returns: sDD*MM'SS.SSS# (high precision)
  if (cmdH("Ga")) {
    tasks_mutex_enter(MX_TELESCOPE_CMD);
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToHms(reply, radToDeg(target.a), false, precisionMode);
    *numericReply = false;
    tasks_mutex_exit(MX_TELESCOPE_CMD);
  } else

  // :GD#       Get Mount Declination
  //            Returns: sDD*MM# or sDD*MM:SS# (based on precision setting)
  // :GDH#      Returns: sDD*MM:SS.SSS# (high precision)
  if (cmdH("GD")) {
    tasks_mutex_enter(MX_TELESCOPE_CMD);
    updatePosition();
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(transform.mountToNative(&current).d), false, true, precisionMode);
    *numericReply = false;
    tasks_mutex_exit(MX_TELESCOPE_CMD);
  } else 

  // :Gd#       Get Target Declination
  //            Returns: HH:MM.T# or HH:MM:SS (based on precision setting)
  // :GdH#      High precision
  //            Returns: HH:MM:SS.SSS# (high precision)
  if (cmdH("Gd")) {
    tasks_mutex_enter(MX_TELESCOPE_CMD);
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(target.d), false, true, precisionMode);
    *numericReply = false;
    tasks_mutex_exit(MX_TELESCOPE_CMD);
  } else

  // :GR#       Get Mount Right Ascension
  //            Returns: HH:MM.T# or HH:MM:SS# (based on precision setting)
  // :GRH#      Returns: HH:MM:SS.SSSS# (high precision)
  if (cmdH("GR")) {
    tasks_mutex_enter(MX_TELESCOPE_CMD);
    updatePosition();
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToHms(reply, radToHrs(transform.mountToNative(&current).r), false, precisionMode);
    *numericReply = false;
    tasks_mutex_exit(MX_TELESCOPE_CMD);
  } else

  // :Gr#       Get Target Right Ascension
  //            Returns: HH:MM.T# or HH:MM:SS (based on precision setting)
  // :GrH#      Returns: HH:MM:SS.SSSS# (high precision)
  if (cmdH("Gr")) {
    tasks_mutex_enter(MX_TELESCOPE_CMD);
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToHms(reply, radToHrs(target.r), false, precisionMode);
    *numericReply = false;
    tasks_mutex_exit(MX_TELESCOPE_CMD);
  } else

  // :GT#         Get tracking rate, 0.0 unless TrackingSidereal
  //              Returns: n.n# (OnStep returns more decimal places than LX200 standard)
  if (cmd("GT"))  {
    if (trackingState == TS_NONE) strcpy(reply,"0"); else dtostrf(siderealToHz(trackingRate), 0, 5, reply);
    *numericReply = false;
  } else 

  // :Gu#       Get bit packed telescope status
  //            Returns: s#
  if (cmd("Gu")) {
    tasks_mutex_enter(MX_TELESCOPE_CMD);
    memset(reply, (char)0b10000000, 9);
    if (trackingState == TS_NONE)                reply[0]|=0b10000001;      // Not tracking
    if (gotoState == GS_NONE)                    reply[0]|=0b10000010;      // No goto
//  if (ppsSynced)                               reply[0]|=0b10000100;      // PPS sync
    if (guideState == GU_PULSE_GUIDE)            reply[0]|=0b10001000;      // pulse guide active
    if (mountType != ALTAZM) {
      if (rateCompensation == RC_REFR_RA)        reply[0]|=0b11010000;      // Refr enabled Single axis
      if (rateCompensation == RC_REFR_BOTH)      reply[0]|=0b10010000;      // Refr enabled
      if (rateCompensation == RC_FULL_RA)        reply[0]|=0b11100000;      // OnTrack enabled Single axis
      if (rateCompensation == RC_FULL_BOTH)      reply[0]|=0b10100000;      // OnTrack enabled
    }
    if (rateCompensation == RC_NONE) {
      double r = siderealToHz(trackingRate);
      if (fequal(r, 57.900))                     reply[1]|=0b10000001; else // Lunar rate selected
      if (fequal(r, 60.000))                     reply[1]|=0b10000010; else // Solar rate selected
      if (fequal(r, 60.136))                     reply[1]|=0b10000011;      // King rate selected
    }
    
    if (syncToEncodersOnly)                      reply[1]|=0b10000100;      // sync to encoders only
    if (guideState != GU_NONE)                   reply[1]|=0b10001000;      // guide active
    if (atHome)                                  reply[2]|=0b10000001;      // At home
    if (waitingHome)                             reply[2]|=0b10000010;      // Waiting at home
    if (pauseHome)                               reply[2]|=0b10000100;      // Pause at home enabled?
    if (soundEnabled)                            reply[2]|=0b10001000;      // Buzzer enabled?
    if (mountType == GEM && autoMeridianFlip)    reply[2]|=0b10010000;      // Auto meridian flip
    if (pecRecorded)                             reply[2]|=0b10100000;      // PEC data has been recorded

    // provide mount type
    if (mountType == GEM)                        reply[3]|=0b10000001; else // GEM
    if (mountType == FORK)                       reply[3]|=0b10000010; else // FORK
    if (mountType == ALTAZM)                     reply[3]|=0b10001000;      // ALTAZM

    // provide pier side info.
    updatePosition();
    if (current.pierSide == PIER_SIDE_NONE)      reply[3]|=0b10010000; else // Pier side none
    if (current.pierSide == PIER_SIDE_EAST)      reply[3]|=0b10100000; else // Pier side east
    if (current.pierSide == PIER_SIDE_WEST)      reply[3]|=0b11000000;      // Pier side west

#if AXIS1_PEC == ON
    if (mountType != ALTAZM) {
      reply[4] = (int)pecState|0b10000000;                                  // PEC status: 0 ignore, 1 ready play, 2 playing, 3 ready record, 4 recording
    }
#endif
    reply[5] = (int)parkState|0b10000000;                                   // Park status: 0 not parked, 1 parking in-progress, 2 parked, 3 park failed
    reply[6] = (int)pulseGuideRate|0b10000000;                              // Pulse-guide rate
    reply[7] = (int)guideRate|0b10000000;                                   // Guide rate
//  reply[8] = generalError|0b10000000;                                     // General error
    reply[9] = 0;
    *numericReply=false;
    tasks_mutex_exit(MX_TELESCOPE_CMD);
  } else

  // :GZ#       Get Mount Azimuth
  //            Returns: DDD*MM# or DDD*MM'SS# (based on precision setting)
  // :GZH#      High precision
  //            Returns: DDD*MM'SS.SSS# (high precision)
  if (cmdH("GZ")) {
    tasks_mutex_enter(MX_TELESCOPE_CMD);
    updatePosition();
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(transform.mountToNative(&current, true).z), true, false, precisionMode);
    *numericReply = false;
    tasks_mutex_exit(MX_TELESCOPE_CMD);
  } else

  // :Gz#       Get Target Azimuth
  //            Returns: DDD*MM# or DDD*MM'SS# (based on precision setting)
  // :GzH#      High precision
  //            Returns: DDD*MM'SS.SSS# (high precision)
  if (cmdH("Gz")) {
    tasks_mutex_enter(MX_TELESCOPE_CMD);
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(target.z), false, true, precisionMode);
    *numericReply = false;
    tasks_mutex_exit(MX_TELESCOPE_CMD);
  } else

  //  :Sa[sDD*MM]# or :Sa[sDD*MM'SS]# or :Sa[sDD*MM'SS.SSS]#
  //            Set Target Altitude
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sa")) {
    tasks_mutex_enter(MX_TELESCOPE_CMD);
    if (!convert.dmsToDouble(&target.a, parameter, true)) *commandError = CE_PARAM_RANGE;
    target.a = degToRad(target.a);
    tasks_mutex_exit(MX_TELESCOPE_CMD);
  } else

  //  :Sd[sDD*MM]# or :Sd[sDD*MM:SS]# or :Sd[sDD*MM:SS.SSS]#
  //            Set Target Declination
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sd")) {
    tasks_mutex_enter(MX_TELESCOPE_CMD);
    if (!convert.dmsToDouble(&target.d, parameter, true)) *commandError = CE_PARAM_RANGE;
    target.d = degToRad(target.d);
    tasks_mutex_exit(MX_TELESCOPE_CMD);
  } else

  //  :Sr[HH:MM.T]# or :Sr[HH:MM:SS]# or :Sr[HH:MM:SS.SSSS]#
  //            Set Target Right Ascension
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sr")) {
    tasks_mutex_enter(MX_TELESCOPE_CMD);
    if (!convert.hmsToDouble(&target.r, parameter)) *commandError = CE_PARAM_RANGE;
    target.r = hrsToRad(target.r);
    tasks_mutex_exit(MX_TELESCOPE_CMD);
  } else

  //  :ST[H.H]# Set Tracking Rate in Hz where 60.0 is solar rate
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("ST"))  {
    tasks_mutex_enter(MX_TELESCOPE_CMD);
    double f = strtod(parameter,&conv_end);
    if (&parameter[0] != conv_end && ((f >= 30.0 && f < 90.0) || fabs(f) < 0.1)) {
      if (fabs(f) < 0.1) trackingState = TS_NONE; else {
        if (trackingState == TS_NONE) { trackingState = TS_SIDEREAL; axis1.enable(true); axis2.enable(true); }
        trackingRate = hzToSidereal(f);
      }
      updateTrackingRates();
    } else *commandError = CE_PARAM_RANGE;
    tasks_mutex_exit(MX_TELESCOPE_CMD);
  } else

  //  :Sz[DDD*MM]# or :Sz[DDD*MM'SS]# or :Sz[DDD*MM'SS.SSS]#
  //            Set Target Azmuith
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sz")) {
    tasks_mutex_enter(MX_TELESCOPE_CMD);
    if (!convert.dmsToDouble(&target.z, parameter, false)) *commandError = CE_PARAM_RANGE;
    target.z = degToRad(target.z);
    tasks_mutex_exit(MX_TELESCOPE_CMD);
  } else

  // T - Tracking Commands
  //
  // :To#       Track full compensation on
  // :Tr#       Track refraction compensation on
  // :Tn#       Track compensation off
  // :T1#       Track dual axis off (disable Dec tracking on Eq mounts)
  // :T2#       Track dual axis on
  //
  // :TS#       Track rate solar
  // :TK#       Track rate king
  // :TL#       Track rate lunar
  // :TQ#       Track rate sidereal
  //
  // :T+#       Master sidereal clock faster by 0.02 Hertz (stored in EEPROM)
  // :T-#       Master sidereal clock slower by 0.02 Hertz (stored in EEPROM)
  // :TR#       Master sidereal clock reset (to calculated sidereal rate, stored in EEPROM)
  //            Returns: Nothing
  //
  // :Te#       Tracking enable
  // :Td#       Tracking disable
  //            Return: 0 on failure
  //                    1 on success
  if (command[0] == 'T' && parameter[0] == 0) {
    tasks_mutex_enter(MX_TELESCOPE_CMD);
    if (command[1] == 'o' && mountType != ALTAZM) { rateCompensation = RC_FULL_RA; } else            // full compensation on
    if (command[1] == 'r' && mountType != ALTAZM) { rateCompensation = RC_REFR_RA; } else            // refraction compensation on
    if (command[1] == 'n' && mountType != ALTAZM) { rateCompensation = RC_NONE;    }                 // compensation off
    if (command[1] == '1' && mountType != ALTAZM) {                                                  // dual axis tracking off
      if (rateCompensation == RC_REFR_BOTH) rateCompensation = RC_REFR_RA; else if (rateCompensation == RC_FULL_BOTH) rateCompensation = RC_FULL_RA;
    } else
    if (command[1] == '2' && mountType != ALTAZM) {                                                  // dual axis tracking on
      if (rateCompensation == RC_REFR_RA) rateCompensation = RC_REFR_BOTH; else if (rateCompensation == RC_FULL_RA) rateCompensation = RC_FULL_BOTH;
    } else
    if (command[1] == 'S') { rateCompensation = RC_NONE; trackingRate = hzToSidereal(60);     } else // solar tracking rate 60Hz
    if (command[1] == 'K') { rateCompensation = RC_NONE; trackingRate = hzToSidereal(60.136); } else // king  tracking rate 60.136Hz
    if (command[1] == 'L') { rateCompensation = RC_NONE; trackingRate = hzToSidereal(57.9);   } else // lunar tracking rate 57.9Hz
    if (command[1] == 'Q') { trackingRate = hzToSidereal(SIDEREAL_RATE_HZ);                   } else // sidereal tracking rate 60.164Hz
    if (command[1] == '+') { clock.setPeriodSubMicros(clock.getPeriodSubMicros() - hzToSubMicros(0.02)); } else
    if (command[1] == '-') { clock.setPeriodSubMicros(clock.getPeriodSubMicros() + hzToSubMicros(0.02)); } else
    if (command[1] == 'R') { clock.setPeriodSubMicros(SIDEREAL_PERIOD); } else                       // reset master sidereal clock interval
    if (command[1] == 'e') {
      if (parkState != PS_PARKED) { resetGeneralErrors(); trackingState = TS_SIDEREAL; axis1.enable(true); axis2.enable(true); } else *commandError = CE_PARKED;
    } else
    if (command[1] == 'd') {
      if (gotoState == GS_NONE && guideState == GU_NONE) trackingState = TS_NONE; else *commandError = CE_MOUNT_IN_MOTION;
    } else *commandError = CE_CMD_UNKNOWN;

    if (*commandError == CE_NONE) {
      switch (command[1]) { case 'S': case 'K': case 'L': case 'Q': case '+': case '-': case 'R': *numericReply = false; }
      switch (command[1]) { case 'o': case 'r': case 'n': hzToSidereal(SIDEREAL_RATE_HZ); }
      updateTrackingRates();
    }
    tasks_mutex_exit(MX_TELESCOPE_CMD);
  } else

  return true;
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
  if (mountType != ALTAZM) {
    trackingRateAxis1 = trackingRate;
    if (rateCompensation != RC_REFR_BOTH && rateCompensation != RC_FULL_BOTH) trackingRateAxis2 = 0;
  }
  if (trackingState == TS_NONE) { trackingRateAxis1 = 0; trackingRateAxis2 = 0; }
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
