//--------------------------------------------------------------------------------------------------
// telescope mount control
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#if STEPS_PER_WORM_ROTATION == 0
  #define AXIS1_PEC ON
#else
  #define AXIS1_PEC OFF
#endif

#include "../debug/Debug.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;
#include "../coordinates/Convert.h"
extern Convert convert;
#include "../coordinates/Transform.h"
extern Transform transform;
#include "../commands/ProcessCmds.h"
extern GeneralErrors generalErrors;
#include "../StepDrivers/StepDrivers.h"
#include "Axis.h"
extern Axis axis1;
extern Axis axis2;
#include "Clock.h"
extern Clock clock;
#include "Mount.h"

#if AXIS1_DRIVER_MODEL != OFF
  AxisSettings         axis1Settings           = {AXIS1_STEPS_PER_DEGREE*RAD,AXIS1_DRIVER_REVERSE,degToRad(AXIS1_LIMIT_MIN),degToRad(AXIS1_LIMIT_MAX)};
#endif
#if AXIS2_DRIVER_MODEL != OFF
  AxisSettings         axis2Settings           = {AXIS2_STEPS_PER_DEGREE*RAD,AXIS2_DRIVER_REVERSE,degToRad(AXIS2_LIMIT_MIN),degToRad(AXIS2_LIMIT_MAX)};
#endif

void Mount::init(int8_t mountType) {
  VF("MSG: Mount::init, type "); VL(mountType);
  this->mountType = mountType;
  if (mountType == GEM) meridianFlip = MF_ALWAYS;

  radsPerCentisecond = degToRad(15.0/3600.0)/100.0;

  home.h = degToRad(90.0);
  home.d = degToRad(90.0);
  home.pierSide = PIER_SIDE_EAST;

  // setup axis1 and axis2
  axis1.init(1, axis1Settings);
  axis1.setInstrumentCoordinate(home.h);
  axis2.init(2, axis2Settings);
  axis2.setInstrumentCoordinate(home.d);

  // ------------------------------------------------------------------------------------------------
  // move in measures (radians) per second, tracking_enabled
  VLF("MSG: Mount::init, starting tracking");
  setTrackingState(TS_SIDEREAL);
  axis1.setFrequencyMax(degToRad(4.0));
  trackingRate = hzToSidereal(SIDEREAL_RATE_HZ);
  updateTrackingRates();

  // automatic movement for axis1 and axis2
  axis1.setTracking(true);
  axis2.setTracking(true);
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
    CommandError e;
    //if (alignActive()) { e = alignStar(); if (e != CE_NONE) { alignNumStars = 0; alignThisStar = 0; commandError = e; } } else e = syncEqu(gotoTarget);
    e = syncEqu(&gotoTarget);
    if (command[1] == 'M') {
      if (e >= CE_GOTO_ERR_BELOW_HORIZON && e <= CE_GOTO_ERR_UNSPECIFIED) strcpy(reply,"E0"); reply[1] = (char)(e - CE_GOTO_ERR_BELOW_HORIZON) + '1';
      if (e == CE_NONE) strcpy(reply,"N/A");
    }
    *numericReply = false;
  } else

  // :MS#       Goto the Target Object
  //            Returns:
  //              0=Goto is possible
  //              1=below the horizon limit
  //              2=above overhead limit
  //              3=controller in standby
  //              4=mount is parked
  //              5=Goto in progress
  //              6=outside limits (AXIS2_LIMIT_MAX, AXIS2_LIMIT_MIN, AXIS1_LIMIT_MIN/MAX, MERIDIAN_E/W)
  //              7=hardware fault
  //              8=already in motion
  //              9=unspecified error
  if (cmd("MS"))  {
    CommandError e = gotoEqu(&gotoTarget);
    strcpy(reply,"0");
    if (e >= CE_GOTO_ERR_BELOW_HORIZON && e <= CE_GOTO_ERR_UNSPECIFIED) reply[0] = (char)(e - CE_GOTO_ERR_BELOW_HORIZON) + '1';
    if (e == CE_NONE) reply[0] = '0';
    *numericReply = false;
    *supressFrame = true;
    *commandError = e;
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

  // :Ga#       Get Target Altitude
  //            Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
  // :GaH#      High precision
  //            Returns: sDD*MM'SS.SSS# (high precision)
  if (cmdH("Ga")) {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToHms(reply, radToDeg(gotoTarget.a), false, precisionMode);
    *numericReply = false;
  } else

  // :GD#       Get Mount Declination
  //            Returns: sDD*MM# or sDD*MM:SS# (based on precision setting)
  // :GDH#      Returns: sDD*MM:SS.SSS# (high precision)
  if (cmdH("GD")) {
    updatePosition();
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(transform.mountToNative(&current).d), false, true, precisionMode);
    *numericReply = false;
  } else

  // :Gd#       Get Target Declination
  //            Returns: HH:MM.T# or HH:MM:SS (based on precision setting)
  // :GdH#      High precision
  //            Returns: HH:MM:SS.SSS# (high precision)
  if (cmdH("Gd")) {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(gotoTarget.d), false, true, precisionMode);
    *numericReply = false;
  } else

  // :Gh#       Get Horizon Limit, the minimum elevation of the mount relative to the horizon
  //            Returns: sDD*#
  if (cmd("Gh"))  { sprintf(reply,"%+02ld*", lround(radToDeg(limits.minAltitude))); *numericReply = false; } else

  // :Go#       Get Overhead Limit
  //            Returns: DD*#
  //            The highest elevation above the horizon that the telescope will goto
  if (cmd("Go"))  { sprintf(reply,"%02ld*", lround(radToDeg(limits.maxAltitude))); *numericReply=false; } else

  // :GR#       Get Mount Right Ascension
  //            Returns: HH:MM.T# or HH:MM:SS# (based on precision setting)
  // :GRH#      Returns: HH:MM:SS.SSSS# (high precision)
  if (cmdH("GR")) {
    updatePosition();
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToHms(reply, radToHrs(transform.mountToNative(&current).r), false, precisionMode);
    *numericReply = false;
  } else

  // :Gr#       Get Target Right Ascension
  //            Returns: HH:MM.T# or HH:MM:SS (based on precision setting)
  // :GrH#      Returns: HH:MM:SS.SSSS# (high precision)
  if (cmdH("Gr")) {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToHms(reply, radToHrs(gotoTarget.r), false, precisionMode);
    *numericReply = false;
  } else

  // :GT#         Get tracking rate, 0.0 unless TrackingSidereal
  //              Returns: n.n# (OnStep returns more decimal places than LX200 standard)
  if (cmd("GT"))  {
    if (trackingState == TS_NONE) strcpy(reply,"0"); else dtostrf(siderealToHz(trackingRate), 0, 5, reply);
    *numericReply = false;
  } else 

  // :GU#       Get telescope Status
  //            Returns: s#
  if (cmd("GU"))  {
    int i = 0;
    if (trackingState == TS_NONE)            reply[i++]='n';                     // [n]ot tracking
    if (gotoState == GS_NONE)                reply[i++]='N';                     // [N]o goto
    if (parkState == PS_UNPARKED)            reply[i++]='p'; else
    if (parkState == PS_PARKING)             reply[i++]='I'; else
    if (parkState == PS_PARKED)              reply[i++]='P'; else
    if (parkState == PS_PARK_FAILED)         reply[i++]='F';                     // not [p]arked, parking [I]n-progress, [P]arked, Park [F]ailed
    if (pecRecorded)                         reply[i++]='R';                     // PEC data has been [R]ecorded
  //if (syncToEncodersOnly)                  reply[i++]='e';                     // sync to [e]ncoders only
    if (atHome)                              reply[i++]='H';                     // at [H]ome
  //if (ppsSynced)                           reply[i++]='S';                     // PPS [S]ync
    if (guideState != GU_NONE)               reply[i++]='g';                     // [g]uide active
    if (guideState != GU_PULSE_GUIDE)        reply[i++]='G';                     // pulse [G]uide active
    if (mountType != ALTAZM) {
      if (rateCompensation == RC_REFR_RA)  { reply[i++]='r'; reply[i++]='s'; }   // [r]efr enabled [s]ingle axis
      if (rateCompensation == RC_REFR_BOTH){ reply[i++]='r'; }                   // [r]efr enabled
      if (rateCompensation == RC_FULL_RA)  { reply[i++]='t'; reply[i++]='s'; }   // on[t]rack enabled [s]ingle axis
      if (rateCompensation == RC_FULL_BOTH){ reply[i++]='t'; }                   // on[t]rack enabled
    }
    if (waitingHome)                         reply[i++]='w';                     // [w]aiting at home 
    if (pauseHome)                           reply[i++]='u';                     // pa[u]se at home enabled?
    if (soundEnabled)                        reply[i++]='z';                     // bu[z]zer enabled?
    if (mountType==GEM && autoMeridianFlip)  reply[i++]='a';                     // [a]uto meridian flip
    #if AXIS1_PEC == ON
      if (mountType != ALTAZM)               reply[i++]="/,~;^"[(int)pecState];  // PEC Status (/)gnore, ready (,)lay, (~)laying, ready (;)ecord, (^)ecording
    #endif
    // provide mount type
    if (mountType == GEM)                    reply[i++]='E'; else
    if (mountType == FORK)                   reply[i++]='K'; else
    if (mountType == ALTAZM)                 reply[i++]='A';

    // provide pier side info.
    if (current.pierSide == PIER_SIDE_NONE)  reply[i++]='o'; else                // pier side n[o]ne
    if (current.pierSide == PIER_SIDE_EAST)  reply[i++]='T'; else                // pier side eas[T]
    if (current.pierSide == PIER_SIDE_WEST)  reply[i++]='W';                     // pier side [W]est

    // provide pulse-guide rate
    reply[i++]='0'+pulseGuideRate;

    // provide guide rate
    reply[i++]='0'+guideRate;

    // provide general error
    reply[i++]='0'; //+generalError;
    reply[i++]=0;

    *numericReply = false;
  } else

  // :Gu#       Get bit packed telescope status
  //            Returns: s#
  if (cmd("Gu")) {
    memset(reply, (char)0b10000000, 9);
    if (trackingState == TS_NONE)                reply[0]|=0b10000001;           // Not tracking
    if (gotoState == GS_NONE)                    reply[0]|=0b10000010;           // No goto
//  if (ppsSynced)                               reply[0]|=0b10000100;           // PPS sync
    if (guideState == GU_PULSE_GUIDE)            reply[0]|=0b10001000;           // pulse guide active
    if (mountType != ALTAZM) {
      if (rateCompensation == RC_REFR_RA)        reply[0]|=0b11010000;           // Refr enabled Single axis
      if (rateCompensation == RC_REFR_BOTH)      reply[0]|=0b10010000;           // Refr enabled
      if (rateCompensation == RC_FULL_RA)        reply[0]|=0b11100000;           // OnTrack enabled Single axis
      if (rateCompensation == RC_FULL_BOTH)      reply[0]|=0b10100000;           // OnTrack enabled
    }
    if (rateCompensation == RC_NONE) {
      double r = siderealToHz(trackingRate);
      if (fequal(r, 57.900))                     reply[1]|=0b10000001; else      // Lunar rate selected
      if (fequal(r, 60.000))                     reply[1]|=0b10000010; else      // Solar rate selected
      if (fequal(r, 60.136))                     reply[1]|=0b10000011;           // King rate selected
    }
    
    if (syncToEncodersOnly)                      reply[1]|=0b10000100;           // sync to encoders only
    if (guideState != GU_NONE)                   reply[1]|=0b10001000;           // guide active
    if (atHome)                                  reply[2]|=0b10000001;           // At home
    if (waitingHome)                             reply[2]|=0b10000010;           // Waiting at home
    if (pauseHome)                               reply[2]|=0b10000100;           // Pause at home enabled?
    if (soundEnabled)                            reply[2]|=0b10001000;           // Buzzer enabled?
    if (mountType == GEM && autoMeridianFlip)    reply[2]|=0b10010000;           // Auto meridian flip
    if (pecRecorded)                             reply[2]|=0b10100000;           // PEC data has been recorded

    // provide mount type
    if (mountType == GEM)                        reply[3]|=0b10000001; else      // GEM
    if (mountType == FORK)                       reply[3]|=0b10000010; else      // FORK
    if (mountType == ALTAZM)                     reply[3]|=0b10001000;           // ALTAZM

    // provide pier side info.
    updatePosition();
    if (current.pierSide == PIER_SIDE_NONE)      reply[3]|=0b10010000; else      // Pier side none
    if (current.pierSide == PIER_SIDE_EAST)      reply[3]|=0b10100000; else      // Pier side east
    if (current.pierSide == PIER_SIDE_WEST)      reply[3]|=0b11000000;           // Pier side west

#if AXIS1_PEC == ON
    if (mountType != ALTAZM) {
      reply[4] = (int)pecState|0b10000000;                                       // PEC status: 0 ignore, 1 ready play, 2 playing, 3 ready record, 4 recording
    }
#endif
    reply[5] = (int)parkState|0b10000000;                                        // Park status: 0 not parked, 1 parking in-progress, 2 parked, 3 park failed
    reply[6] = (int)pulseGuideRate|0b10000000;                                   // Pulse-guide rate
    reply[7] = (int)guideRate|0b10000000;                                        // Guide rate
//  reply[8] = generalError|0b10000000;                                          // General error
    reply[9] = 0;
    *numericReply=false;
  } else

  if (cmdP("GX") && parameter[0] == '9') { // 9n: Misc.
    *numericReply = false;
    switch (parameter[1]) {
//    case '0': dtostrf(guideRates[getPulseGuideRateSelection()]/15.0,2,2,reply); break; // pulse-guide rate
//    case '1': sprintf(reply,"%d",pecValue); break;                             // pec analog value
      case '2': dtostrf(usPerStepCurrent,3,3,reply); break;                      // MaxRate (current)
      case '3': dtostrf(usPerStepDefault,3,3,reply); break;                      // MaxRate (default)
      case '4': sprintf(reply,"%d%s",(int)current.pierSide,(meridianFlip == MF_NEVER)?" N":""); break; // pierSide (N if never)
      case '5': sprintf(reply,"%d",(int)limits.autoMeridianFlip); break;         // autoMeridianFlip
      case '6': reply[0] = "EWB"[preferredPierSide-10]; reply[1] = 0; break;     // preferred pier side
      case '7': dtostrf((1000000.0/usPerStepCurrent)/degToRad(axis1.getStepsPerMeasure()),3,1,reply); break; // slew speed
      case '8':                                                                  // rotator availablity 2=rotate/derotate, 1=rotate, 0=off
        if (ROTATOR == ON) {
          if (mountType == ALTAZM) strcpy(reply,"D"); else strcpy(reply,"R");
        } else strcpy(reply,"N");
      break;
//    case '9': dtostrf(maxRateLowerLimit()/16.0,3,3,reply); break;              // MaxRate (fastest/lowest)
//    case 'A': dtostrf(ambient.getTemperature(),3,1,reply); break;              // temperature in deg. C
//    case 'B': dtostrf(ambient.getPressure(),3,1,reply); break;                 // pressure in mb
//    case 'C': dtostrf(ambient.getHumidity(),3,1,reply); break;                 // relative humidity in %
//    case 'D': dtostrf(ambient.getAltitude(),3,1,reply); break;                 // altitude in meters
//    case 'E': dtostrf(ambient.getDewPoint(),3,1,reply); break;                 // dew point in deg. C
//    case 'F':                                                                  // internal MCU temperature in deg. C
//      float t=HAL_MCU_Temperature();
//      if (t > -999) dtostrf(t,1,0,reply); else { *numericReply = true; commandError=CE_0; }
//    break;
    default:
      *numericReply = true;
      *commandError = CE_CMD_UNKNOWN;
    }
  } else

  if (cmdP("GX") && parameter[0] == 'E') { // En: Get settings
    *numericReply = false;
    switch (parameter[1]) {
//    case '1': dtostrf((double)usPerStepBaseActual,3,3,reply); break;
//    case '2': dtostrf(SLEW_ACCELERATION_DIST,2,1,reply); break;
//    case '3': sprintf(reply,"%ld",lround(TRACK_BACKLASH_RATE)); break;
      case '4': sprintf(reply,"%ld",lround(axis1Settings.stepsPerMeasure/RAD)); break;
      case '5': sprintf(reply,"%ld",lround(axis2Settings.stepsPerMeasure/RAD)); break;
//    case '6': dtostrf(stepsPerSecondAxis1,3,6,reply); break;
//    case '7': sprintf(reply,"%ld",nv.readLong(EE_stepsPerWormRotAxis1)); break;
//    case '8': sprintf(reply,"%ld",lround(pecBufferSize)); break;
      case '9': sprintf(reply,"%ld",lround(radToDeg(limits.pastMeridianE)*4)); break; // minutes past meridianE
      case 'A': sprintf(reply,"%ld",lround(radToDeg(limits.pastMeridianW)*4)); break; // minutes past meridianW
      case 'e': sprintf(reply,"%ld",lround(radToDeg(axis1Settings.min))); break;      // RA east or -Az limit, in degrees
      case 'w': sprintf(reply,"%ld",lround(radToDeg(axis1Settings.max))); break;      // RA west or +Az limit, in degrees
      case 'B': sprintf(reply,"%ld",lround(radToDeg(axis1Settings.max)/15.0)); break; // RA west or +Az limit, in hours
      case 'C': sprintf(reply,"%ld",lround(radToDeg(axis2Settings.min))); break;
      case 'D': sprintf(reply,"%ld",lround(radToDeg(axis2Settings.max))); break;
      case 'E': reply[0] = '0' + (MOUNT_COORDS - 1); *supressFrame = true; break;
      case 'F': if (AXIS2_TANGENT_ARM == ON) reply[0] = '1'; else reply[0] = '0'; *supressFrame = true; break;
//    case 'M': if (!runtimeSettings()) strcpy(reply, "0"); else sprintf(reply,"%d",(int)nv.read(EE_mountType)); break; // return the mount type
    default:
      *numericReply = true;
      *commandError = CE_CMD_UNKNOWN;
    }
  } else

  if (parameter[0] == 'F') { // Fn: Debug
    *numericReply = false;
    switch (parameter[1]) {
      case '3': dtostrf(axis1.getFrequencySteps(),3,6,reply); break;                  // Axis1 final tracking rate Hz
      case '4': dtostrf(axis2.getFrequencySteps(),3,6,reply); break;                  // Axis2 final tracking rate Hz
    default:
      *numericReply = true;
      *commandError = CE_CMD_UNKNOWN;
    }
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

  // :Gz#       Get Target Azimuth
  //            Returns: DDD*MM# or DDD*MM'SS# (based on precision setting)
  // :GzH#      High precision
  //            Returns: DDD*MM'SS.SSS# (high precision)
  if (cmdH("Gz")) {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(gotoTarget.z), false, true, precisionMode);
    *numericReply = false;
  } else

  //  :Sa[sDD*MM]# or :Sa[sDD*MM'SS]# or :Sa[sDD*MM'SS.SSS]#
  //            Set Target Altitude
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sa")) {
    if (!convert.dmsToDouble(&gotoTarget.a, parameter, true)) *commandError = CE_PARAM_RANGE; else
    gotoTarget.a = degToRad(gotoTarget.a);
  } else

  //  :Sd[sDD*MM]# or :Sd[sDD*MM:SS]# or :Sd[sDD*MM:SS.SSS]#
  //            Set Target Declination
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sd")) {
    if (!convert.dmsToDouble(&gotoTarget.d, parameter, true)) *commandError = CE_PARAM_RANGE; else
    gotoTarget.d = degToRad(gotoTarget.d);
  } else

  //  :Sh[sDD]#
  //            Set the elevation lower limit
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sh")) {
    int16_t deg;
    if (convert.atoi2(parameter, &deg)) {
      if (deg >= -30 && deg <= 30) {
        limits.minAltitude = degToRad(deg);
//      nv.update(EE_minAlt, minAlt+128);
      } else *commandError = CE_PARAM_RANGE;
    } else *commandError = CE_PARAM_FORM;
  } else

  //  :So[DD]#
  //            Set the overhead elevation limit in degrees relative to the horizon
  //            Return: 0 on failure
  //                    1 on success
  if (command[1] == 'o')  {
    int16_t deg;
    if (convert.atoi2(parameter, &deg)) {
      if (deg >= 60 && deg <= 90) {
        limits.maxAltitude = degToRad(deg);
        if (mountType == ALTAZM && limits.maxAltitude > 87) limits.maxAltitude = 87;
//      nv.update(EE_maxAlt,maxAlt); 
      } else *commandError = CE_PARAM_RANGE;
    } else *commandError = CE_PARAM_FORM;
  } else

  //  :Sr[HH:MM.T]# or :Sr[HH:MM:SS]# or :Sr[HH:MM:SS.SSSS]#
  //            Set Target Right Ascension
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sr")) {
    if (!convert.hmsToDouble(&gotoTarget.r, parameter)) *commandError = CE_PARAM_RANGE; else
    gotoTarget.r = hrsToRad(gotoTarget.r);
  } else

  //  :ST[H.H]# Set Tracking Rate in Hz where 60.0 is solar rate
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("ST"))  {
    double f = strtod(parameter,&conv_end);
    if (&parameter[0] != conv_end && ((f >= 30.0 && f < 90.0) || fabs(f) < 0.1)) {
      if (fabs(f) < 0.1) trackingState = TS_NONE; else {
        if (trackingState == TS_NONE) setTrackingState(TS_SIDEREAL);
        trackingRate = hzToSidereal(f);
      }
      updateTrackingRates();
    } else *commandError = CE_PARAM_RANGE;
  } else

  if (cmdP("SX") && parameter[0] == '9') { // 9n: Misc.
    switch (parameter[1]) {
      case '2': // set new slew rate (returns 1 success or 0 failure)
        if (gotoState == GS_NONE && guideState == GU_NONE) {
          usPerStepCurrent = strtod(&parameter[3],&conv_end);
          if (usPerStepCurrent < usPerStepBase/2.0) usPerStepCurrent = usPerStepBase/2.0;
          if (usPerStepCurrent > usPerStepBase*2.0) usPerStepCurrent = usPerStepBase*2.0;
          if (usPerStepCurrent < usPerStepLowerLimit()) usPerStepCurrent = usPerStepLowerLimit();
          //nv.writeLong(EE_maxRateL,maxRate);
          //setAccelerationRates(maxRate);
        } else *commandError = CE_MOUNT_IN_MOTION;
      break;
      case '3': // slew rate preset (returns nothing)
        *numericReply = false;
        if (gotoState == GS_NONE && guideState == GU_NONE) {
          switch (parameter[3]) {
            case '5': usPerStepCurrent = usPerStepBase*2.0; break; // 50%
            case '4': usPerStepCurrent = usPerStepBase*1.5; break; // 75%
            case '3': usPerStepCurrent = usPerStepBase;     break; // 100%
            case '2': usPerStepCurrent = usPerStepBase/1.5; break; // 150%
            case '1': usPerStepCurrent = usPerStepBase/2.0; break; // 200%
            default:  usPerStepCurrent = usPerStepBase;
          }
          if (usPerStepCurrent < usPerStepLowerLimit()) usPerStepCurrent = usPerStepLowerLimit();

          //nv.writeLong(EE_maxRateL,maxRate);
          //setAccelerationRates(maxRate);
        } else *commandError = CE_MOUNT_IN_MOTION;
      break;
      case '5': // autoMeridianFlip
        if (parameter[3] == '0' || parameter[3] == '1') {
          autoMeridianFlip = parameter[3]-'0';
          //nv.write(EE_autoMeridianFlip, autoMeridianFlip);
        } else *commandError = CE_PARAM_RANGE;
      break;
      case '6': // preferred pier side 
        switch (parameter[3]) {
          case 'E': preferredPierSide = EAST; break;
          case 'W': preferredPierSide = WEST; break;
          case 'B': preferredPierSide = BEST; break;
          default: *commandError = CE_PARAM_RANGE;
        }
      break;
      case '7': // buzzer
        if (parameter[3] == '0' || parameter[3] == '1') {
          soundEnabled = parameter[3] - '0';
        } else *commandError = CE_PARAM_RANGE;
      break;
      case '8': // pause at home on meridian flip
        if (parameter[3] == '0' || parameter[3] == '1') {
          pauseHome = parameter[3]-'0';
          //nv.write(EE_pauseHome, pauseHome);
        } else *commandError = CE_PARAM_RANGE;
      break;
      case '9': // continue if paused at home
        if (parameter[3] == '1') { if (waitingHome) waitingHomeContinue = true; } *commandError = CE_PARAM_RANGE;
      break;
/*
      case 'A': // temperature in deg. C
        float f = strtod(&parameter[3], &conv_end);
        if (&parameter[3] != conv_end && f >= -100.0 && f < 100.0) {
          ambient.setTemperature(f);
        } else *commandError = CE_PARAM_RANGE;
      break;
      case 'B': // pressure in mb
        f=strtod(&parameter[3],&conv_end);
        if (&parameter[3] != conv_end && f >= 500.0 && f < 1500.0) {
          ambient.setPressure(f);
        } else *commandError = CE_PARAM_RANGE;
      break;
      case 'C': // relative humidity in % 
        f=strtod(&parameter[3],&conv_end);
        if (&parameter[3] != conv_end && f >= 0.0 && f < 100.0) {
          ambient.setHumidity(f);
        } else *commandError = CE_PARAM_RANGE;
      break;
      case 'D': // altitude 
        f = strtod(&parameter[3], &conv_end);
        if (&parameter[3] != conv_end && f >= -100.0 && f < 20000.0) {
          ambient.setAltitude(f);
        } else *commandError = CE_PARAM_RANGE;
      break;
  */
      default: *commandError = CE_CMD_UNKNOWN;
    }
  } else

  if (cmdP("SX") && parameter[0] == 'E') { // En: Setup value
    long l = strtol(&parameter[3],NULL,10); float degs = l/4.0;
    switch (parameter[1]) {
      case '7': // stepsPerWormRotation
        if (AXIS1_PEC != ON) l = 0;
        //if (l >= 0 && l < 129600000) nv.writeLong(EE_stepsPerWormRotAxis1,l); else *commandError = CE_PARAM_RANGE;
      break;
      case '9': // minutes past meridianE (up to +/- 270 degrees range, within min/max)
        if (degs >= -270 && degs <= 270) {
          limits.pastMeridianE = degToRad(degs);
          if (limits.pastMeridianE < -axis1Settings.max) limits.pastMeridianE = -axis1Settings.max;
          if (limits.pastMeridianE > -axis1Settings.min) limits.pastMeridianE = -axis1Settings.min;
          //nv.writeFloat(EE_dpmE, limits.pastMeridianE);
        } else *commandError = CE_PARAM_RANGE;
      break;
      case 'A': // minutes past meridianW (up to +/- 270 degrees range, within min/max)
        if (degs >= -270 && degs <= 270) {
          limits.pastMeridianW = degToRad(degs);
          if (limits.pastMeridianW < axis1Settings.min) limits.pastMeridianW = axis1Settings.min;
          if (limits.pastMeridianW > axis1Settings.max) limits.pastMeridianW = axis1Settings.max;
          //nv.writeFloat(EE_dpmW,limits.pastMeridianW);
        } else *commandError = CE_PARAM_RANGE;
      break;
      default: *commandError = CE_CMD_UNKNOWN;
      case 'M': // mount type
        //if (l == 0 || l == GEM || l == FORK || l == ALTAZM) nv.write(EE_mountType,l); else *commandError = CE_PARAM_RANGE;
      break;
    }
  } else

  //  :Sz[DDD*MM]# or :Sz[DDD*MM'SS]# or :Sz[DDD*MM'SS.SSS]#
  //            Set Target Azmuith
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sz")) {
    if (!convert.dmsToDouble(&gotoTarget.z, parameter, false)) *commandError = CE_PARAM_RANGE; else
    gotoTarget.z = degToRad(gotoTarget.z);
  } else

  // T - Tracking Commands
  //
  // :To#       Track full compensation on
  // :Tr#       Track refraction compensation on
  // :Tn#       Track compensation off
  // :T1#       Track dual axis off (disable Dec tracking on Eq mounts)
  // :T2#       Track dual axis on
  //
  // :TS#       Track rate solar    60Hz
  // :TK#       Track rate king     60.136Hz
  // :TL#       Track rate lunar    57.9Hz
  // :TQ#       Track rate sidereal 60.164Hz
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
    if (command[1] == 'o' && mountType != ALTAZM) { rateCompensation = RC_FULL_RA; } else
    if (command[1] == 'r' && mountType != ALTAZM) { rateCompensation = RC_REFR_RA; } else
    if (command[1] == 'n' && mountType != ALTAZM) { rateCompensation = RC_NONE;    }
    if (command[1] == '1' && mountType != ALTAZM) {
      if (rateCompensation == RC_REFR_BOTH) rateCompensation = RC_REFR_RA; else
      if (rateCompensation == RC_FULL_BOTH) rateCompensation = RC_FULL_RA;
    } else
    if (command[1] == '2' && mountType != ALTAZM) {
      if (rateCompensation == RC_REFR_RA) rateCompensation = RC_REFR_BOTH; else
      if (rateCompensation == RC_FULL_RA) rateCompensation = RC_FULL_BOTH;
    } else
    if (command[1] == 'S') { rateCompensation = RC_NONE; trackingRate = hzToSidereal(60);     } else
    if (command[1] == 'K') { rateCompensation = RC_NONE; trackingRate = hzToSidereal(60.136); } else
    if (command[1] == 'L') { rateCompensation = RC_NONE; trackingRate = hzToSidereal(57.9);   } else
    if (command[1] == 'Q') { trackingRate = hzToSidereal(SIDEREAL_RATE_HZ);                   } else 
    if (command[1] == '+') { clock.setPeriodSubMicros(clock.getPeriodSubMicros() - hzToSubMicros(0.02)); } else
    if (command[1] == '-') { clock.setPeriodSubMicros(clock.getPeriodSubMicros() + hzToSubMicros(0.02)); } else
    if (command[1] == 'R') { clock.setPeriodSubMicros(SIDEREAL_PERIOD); } else
    if (command[1] == 'e') {
      if (parkState != PS_PARKED) {
        resetGeneralErrors();
        setTrackingState(TS_SIDEREAL);
      } else *commandError = CE_PARKED;
    } else
    if (command[1] == 'd') {
      if (gotoState == GS_NONE && guideState == GU_NONE) trackingState = TS_NONE; else *commandError = CE_MOUNT_IN_MOTION;
    } else *commandError = CE_CMD_UNKNOWN;

    if (*commandError == CE_NONE) {
      switch (command[1]) { case 'S': case 'K': case 'L': case 'Q': case '+': case '-': case 'R': *numericReply = false; }
      switch (command[1]) { case 'o': case 'r': case 'n': trackingRate = hzToSidereal(SIDEREAL_RATE_HZ); }
      updateTrackingRates();
    }
  } else

  // :VS#       PEC number of steps per second of worm rotation
  //            Returns: n.n#
  if (cmd("VS")) {
    char temp[12];
    dtostrf(stepsPerSecondAxis1, 0, 6, temp);
    strcpy(reply, temp);
    *numericReply = false;
  } else

  //  $ - Set parameter
  // :$BD[n]#   Set Dec/Alt backlash in arc-seconds
  //            Return: 0 on failure
  //                    1 on success
  // :$BR[n]#   Set RA/Azm backlash in arc-seconds
  //            Return: 0 on failure
  //                    1 on success
  //        Set the Backlash values.  Units are arc-seconds
  if (cmdP("$B")) {
    int16_t arcSecs;
    if (convert.atoi2((char*)&parameter[1], &arcSecs)) {
      if (arcSecs >= 0 && arcSecs <= 3600) {
        if (parameter[0] == 'D') {
          axis2.setBacklash(arcsecToRad(arcSecs));
//        nv.writeInt(EE_backlashAxis2,backlashAxis2);
        } else
        if (parameter[0] == 'R') {
          axis1.setBacklash(arcsecToRad(arcSecs));
//        nv.writeInt(EE_backlashAxis1,backlashAxis1);
        } else *commandError = CE_CMD_UNKNOWN;
      } else *commandError = CE_PARAM_RANGE;
    } else *commandError = CE_PARAM_FORM;
  } else

  //  % - Return parameter
  // :%BD#      Get Dec/Alt Antibacklash value in arc-seconds
  //            Return: n#
  // :%BR#      Get RA/Azm Antibacklash value in arc-seconds
  //            Return: n#
  if (cmdP("%B")) {
    if (parameter[0] == 'D' && parameter[1] == 0) {
        int arcSec = radToArcsec(axis2.getBacklash());
        if (arcSec < 0) arcSec = 0; if (arcSec > 3600) arcSec = 3600;
        sprintf(reply,"%d", arcSec);
        *numericReply = false;
    } else
    if (parameter[0] == 'R' && parameter[1] == 0) {
        int arcSec = radToArcsec(axis2.getBacklash());
        if (arcSec < 0) arcSec = 0; if (arcSec > 3600) arcSec = 3600;
        sprintf(reply,"%d", arcSec);
        *numericReply = false;
    } else *commandError = CE_CMD_UNKNOWN;
  } else return false;

  return true;
}

void Mount::setTrackingState(TrackingState state) {
  if (trackingState == TS_NONE) { axis1.enable(true); axis2.enable(true); }
  trackingState = state;
  if (trackingState == TS_SIDEREAL) atHome = false;
}

void Mount::updatePosition() {
  current = transform.instrumentToMount(axis1.getInstrumentCoordinate(), axis2.getInstrumentCoordinate());
}

void Mount::updateTrackingRates() {
  if (mountType != ALTAZM) {
    trackingRateAxis1 = trackingRate;
    if (rateCompensation != RC_REFR_BOTH && rateCompensation != RC_FULL_BOTH) trackingRateAxis2 = 0;
  }
  if (trackingState != TS_SIDEREAL || gotoState != GS_NONE) { trackingRateAxis1 = 0; trackingRateAxis2 = 0; }
  axis1.setFrequency(siderealToRad(trackingRateAxis1 + guideRateAxis1 + deltaRateAxis1 + gotoRateAxis1));
  axis2.setFrequency(siderealToRad(trackingRateAxis2 + guideRateAxis2 + deltaRateAxis2 + gotoRateAxis2));
}

// check for platform rate limit (lowest maxRate) in us units
double Mount::usPerStepLowerLimit() {
  // for example 16us, this basis is platform/clock-rate specific (for square wave)
  double r_us = HAL_MAXRATE_LOWER_LIMIT;
  
  // higher speed ISR code path?
  #if STEP_WAVE_FORM == PULSE || STEP_WAVE_FORM == DEDGE
    r_us=r_us/1.6; // about 1.6x faster than SQW mode in my testing
  #endif
  
  // on-the-fly mode switching used?
  #if MODE_SWITCH_BEFORE_SLEW == OFF
    // if this code is enabled, r_us == 27us
    if (axis1StepsGoto != 1 || axis2StepsGoto != 1) r_us *= 1.7;
  #endif

  // average required goto us rates for each axis with any micro-step mode switching applied,
  // if tracking in 32X mode using 4X for gotos (32/4 = 8,) that's an 8x lower true rate so 27/8 = 3.4 is allowed
  double r_us_axis1 = r_us/axis1.getStepsPerStepGoto();
  double r_us_axis2 = r_us/axis2.getStepsPerStepGoto();
  
  // average in axis2 step rate scaling for drives where the reduction ratio isn't equal
  // if Axis1 is 10000 step/deg & Axis2 is 20000 steps/deg, Axis2 needs to run 2x speed so we must slow down.
  // 3.4 on one axis and 6.8 on the other for an average of 5.1
  r_us = (r_us_axis1 + r_us_axis2/timerRateRatio)/2.0;
 
  // the timer granulaity can start to make for some very abrupt rate changes below 0.25us
  if (r_us < 0.25) { r_us = 0.25; DLF("WRN, usPerStepLowerLimit(): r_us exceeds design limit"); }

  // return rate in us units
  return r_us;
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
