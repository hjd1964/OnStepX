//--------------------------------------------------------------------------------------------------
// telescope mount control, commands
#include "../OnStepX.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../tasks/OnTask.h"
extern Tasks tasks;

#include "../coordinates/Convert.h"
#include "../coordinates/Site.h"
#include "Mount.h"

extern unsigned long periodSubMicros;

bool Mount::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {

  char *conv_end;

  // process any date/time/location commands
  if (transform.site.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;

  // process any axis1 commands
  if (axis1.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;

  // process any axis2 commands
  if (axis2.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;

  // :GT#         Get tracking rate, 0.0 unless TrackingSidereal
  //              Returns: n.n# (OnStep returns more decimal places than LX200 standard)
  if (cmd("GT"))  {
    if (trackingState == TS_NONE) strcpy(reply,"0"); else sprintF(reply,"%0.5f",siderealToHz(trackingRate));
    *numericReply = false;
  } else 

  // :GU#       Get telescope Status
  //            Returns: s#
  if (cmd("GU"))  {
    int i = 0;
    if (trackingState == TS_NONE)            reply[i++]='n';                     // [n]ot tracking
    if (gotoState == GS_NONE)                reply[i++]='N';                     // [N]o goto
    if (park.state == PS_UNPARKED)           reply[i++]='p'; else                // not [p]arked
    if (park.state == PS_PARKING)            reply[i++]='I'; else                // parking [I]n-progress
    if (park.state == PS_PARKED)             reply[i++]='P'; else                // [P]arked
    if (park.state == PS_PARK_FAILED)        reply[i++]='F';                     // park [F]ailed
    if (misc.syncToEncodersOnly)             reply[i++]='e';                     // sync to [e]ncoders only
    if (atHome)                              reply[i++]='H';                     // at [H]ome
  //if (ppsSynced)                           reply[i++]='S';                     // PPS [S]ync
    if (guideState != GU_NONE)               reply[i++]='g';                     // [g]uide active
    if (guideState == GU_PULSE_GUIDE)        reply[i++]='G';                     // pulse [G]uide active
//    if (transform.mountType != ALTAZM) {
      if (rateCompensation == RC_REFR_RA)  { reply[i++]='r'; reply[i++]='s'; }   // [r]efr enabled [s]ingle axis
      if (rateCompensation == RC_REFR_BOTH){ reply[i++]='r'; }                   // [r]efr enabled
      if (rateCompensation == RC_FULL_RA)  { reply[i++]='t'; reply[i++]='s'; }   // on[t]rack enabled [s]ingle axis
      if (rateCompensation == RC_FULL_BOTH){ reply[i++]='t'; }                   // on[t]rack enabled
//    }
    if (waitingHome)                         reply[i++]='w';                     // [w]aiting at home 
    if (misc.meridianFlipPause)              reply[i++]='u';                     // pa[u]se at home enabled?
    if (misc.buzzer)                         reply[i++]='z';                     // bu[z]zer enabled?
    if (transform.mountType==GEM && misc.meridianFlipAuto) reply[i++]='a';       // [a]uto meridian flip
    #if AXIS1_PEC == ON
      if (pec.recorded)                      reply[i++]='R';                     // PEC data has been [R]ecorded
      if (transform.mountType != ALTAZM)     reply[i++]="/,~;^"[(int)pec.state]; // PEC State (/)gnore, ready (,)lay, (~)laying, ready (;)ecord, (^)ecording
    #endif
    if (transform.mountType == GEM)          reply[i++]='E'; else                // GEM
    if (transform.mountType == FORK)         reply[i++]='K'; else                // FORK
    if (transform.mountType == ALTAZM)       reply[i++]='A';                     // ALTAZM

    updatePosition(CR_MOUNT);
    if (current.pierSide == PIER_SIDE_NONE)  reply[i++]='o'; else                // pier side n[o]ne
    if (current.pierSide == PIER_SIDE_EAST)  reply[i++]='T'; else                // pier side eas[T]
    if (current.pierSide == PIER_SIDE_WEST)  reply[i++]='W';                     // pier side [W]est

    reply[i++]='0'+misc.pulseGuideRateSelect;                                    // provide pulse-guide rate
    reply[i++]='0'+guideRateSelect;                                              // provide guide rate

    reply[i++]='0' + errorNumber();                                              // provide general error code
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
//    if (transform.mountType != ALTAZM) {
      if (rateCompensation == RC_REFR_RA)        reply[0]|=0b11010000;           // Refr enabled Single axis
      if (rateCompensation == RC_REFR_BOTH)      reply[0]|=0b10010000;           // Refr enabled
      if (rateCompensation == RC_FULL_RA)        reply[0]|=0b11100000;           // OnTrack enabled Single axis
      if (rateCompensation == RC_FULL_BOTH)      reply[0]|=0b10100000;           // OnTrack enabled
//    }
    if (rateCompensation == RC_NONE) {
      double r = siderealToHz(trackingRate);
      if (fequal(r, 57.900))                     reply[1]|=0b10000001; else      // Lunar rate selected
      if (fequal(r, 60.000))                     reply[1]|=0b10000010; else      // Solar rate selected
      if (fequal(r, 60.136))                     reply[1]|=0b10000011;           // King rate selected
    }
    
    if (misc.syncToEncodersOnly)                 reply[1]|=0b10000100;           // sync to encoders only
    if (guideState != GU_NONE)                   reply[1]|=0b10001000;           // guide active
    if (atHome)                                  reply[2]|=0b10000001;           // At home
    if (waitingHome)                             reply[2]|=0b10000010;           // Waiting at home
    if (misc.meridianFlipPause)                  reply[2]|=0b10000100;           // Pause at home enabled?
    if (misc.buzzer)                             reply[2]|=0b10001000;           // Buzzer enabled?
    if (transform.mountType == GEM && misc.meridianFlipAuto)
                                                 reply[2]|=0b10010000;           // Auto meridian flip

    if (transform.mountType == GEM)              reply[3]|=0b10000001; else      // GEM
    if (transform.mountType == FORK)             reply[3]|=0b10000010; else      // FORK
    if (transform.mountType == ALTAZM)           reply[3]|=0b10001000;           // ALTAZM

    updatePosition(CR_MOUNT);
    if (current.pierSide == PIER_SIDE_NONE)      reply[3]|=0b10010000; else      // Pier side none
    if (current.pierSide == PIER_SIDE_EAST)      reply[3]|=0b10100000; else      // Pier side east
    if (current.pierSide == PIER_SIDE_WEST)      reply[3]|=0b11000000;           // Pier side west

    #if AXIS1_PEC == ON
      if (transform.mountType != ALTAZM)
        reply[4] = (int)pec.state|0b10000000;                                    // PEC state: 0 ignore, 1 ready play, 2 playing, 3 ready record, 4 recording
      if (pec.recorded)                          reply[4]|=0b11000000;           // PEC state: data has been recorded
    #endif
    reply[5] = (int)park.state|0b10000000;                                       // Park state: 0 not parked, 1 parking in-progress, 2 parked, 3 park failed
    reply[6] = (int)misc.pulseGuideRateSelect|0b10000000;                        // Pulse-guide selection
    reply[7] = (int)guideRateSelect|0b10000000;                                  // Guide selection
    reply[8] = errorNumber()|0b10000000;                                         // General error
    reply[9] = 0;
    *numericReply=false;
  } else

  // :GX0[p]#   Get align setting [p]
  //            Returns: Value
  if (cmdGX("GX0")) {
    switch (parameter[1]) {
      case '2': sprintf(reply,"%ld",(long)(0*3600.0)); *numericReply = false; break; // altCor
      case '3': sprintf(reply,"%ld",(long)(0*3600.0)); *numericReply = false; break; // azmCor
//    case '2': sprintf(reply,"%ld",(long)(AlignH.altCor*3600.0)); *numericReply = false; break; // altCor
//    case '3': sprintf(reply,"%ld",(long)(AlignH.azmCor*3600.0)); *numericReply = false; break; // azmCor
    default: return false;
    }
  } else

  // :GX9[p]#   Get setting [p]
  //            Returns: Value
  if (cmdGX("GX9")) {
    *numericReply = false;
    switch (parameter[1]) {
      case '2': sprintF(reply, "%0.3f", misc.usPerStepCurrent); break;          // current
      case '3': sprintF(reply, "%0.3f", usPerStepBase); break;                  // default base
      case '4': sprintf(reply, "%d%s", (int)current.pierSide,(meridianFlip == MF_NEVER)?" N":""); break; // pierSide (N if never)
      case '5': sprintf(reply, "%d", (int)misc.meridianFlipAuto); break;        // autoMeridianFlip
      case '6': reply[0] = "EWB"[preferredPierSide-10]; reply[1] = 0; break;    // preferred pier side
      case '7': sprintF(reply, "%0.1f", (1000000.0/misc.usPerStepCurrent)/degToRad(axis1.getStepsPerMeasure())); break; // slew speed
      case '8':                                                                 // rotator availablity 2=rotate/derotate, 1=rotate, 0=off
        if (AXIS3_DRIVER_MODEL != OFF) {
          if (transform.mountType == ALTAZM) strcpy(reply, "D"); else strcpy(reply, "R");
        } else strcpy(reply, "N");
      break;
      case '9': sprintF(reply, "%0.3f",usPerStepLowerLimit()); break;           // fastest step rate in microseconds
      default: return false;
    }
  } else

  // :GXE[M]#   Get [M]ount setting
  //            Returns: n# or 
  if (cmdGX("GXE")) {
    switch (parameter[1]) {
//    case '1': dtostrf((double)usPerStepBaseActual, 3, 3, reply); break;
//    case '2': dtostrf(SLEW_ACCELERATION_DIST, 2, 1, reply); break;
//    case '3': sprintf(reply, "%ld", lround(TRACK_BACKLASH_RATE)); break;
      case '4': sprintf(reply, "%ld", lround(axis1.getStepsPerMeasure()/RAD_DEG_RATIO)); *numericReply = false; break;
      case '5': sprintf(reply, "%ld", lround(axis2.getStepsPerMeasure()/RAD_DEG_RATIO)); *numericReply = false; break;
      case 'E': reply[0] = '0' + (MOUNT_COORDS - 1); *supressFrame = true; *numericReply = false; break;
      case 'F': if (AXIS2_TANGENT_ARM != ON) *commandError = CE_0; break;
//    case 'M': if (!runtimeSettings()) strcpy(reply, "0"); else sprintf(reply, "%d", (int)nv.read(EE_mountType)); break;
    default:
      return false;
    }
  } else

  // :GXFn#     Get frequency setting and workload
  //            Returns: Value
  if (cmdGX("GXF")) {
    switch (parameter[1]) {
      case '3': sprintF(reply, "%0.6f", axis1.getFrequencySteps()); *numericReply = false; break; // Axis1 final tracking rate Hz
      case '4': sprintF(reply, "%0.6f", axis2.getFrequencySteps()); *numericReply = false; break; // Axis2 final tracking rate Hz
      case 'A': sprintf(reply, "%d%%", 50); *numericReply = false; break;                         // Workload
    default:
      return false;
    }
  } else

  // :hC#       Moves telescope to the home position
  //            Returns: Nothing
  if (cmd("hC")) {
    trackingState = TS_NONE;
    if (transform.mountType == ALTAZM) transform.horToEqu(&home);
    // flag to use the hour angle directly, not RA
    home.r = NAN;
    *commandError = gotoEqu(&home, PSS_EAST);
    *numericReply = false;
  } else

  // :hF#       Reset telescope at the home position.  This position is required for a cold Start.
  //            Point to the celestial pole.  GEM w/counterweights pointing downwards (CWD position).  Equatorial fork mounts at HA = 0.
  //            Returns: Nothing
  if (cmd("hF")) {
    // setup where the home position is
    updateHomePosition();
    *commandError = resetHome();
    *numericReply = false;
  } else 

  // :hP#       Goto the Park Position
  //            Return: 0 on failure
  //                    1 on success
  if (cmd("hP")) *commandError = parkGoto(); else 

  // :hQ#       Set the park position
  //            Return: 0 on failure
  //                    1 on success
  if (cmd("hQ")) *commandError = parkSet(); else 

  // :hR#       Restore parked telescope to operation
  //            Return: 0 on failure
  //                    1 on success
  if (cmd("hR")) *commandError = parkRestore(true); else

  //  :ST[H.H]# Set Tracking Rate in Hz where 60.0 is the solar rate
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("ST"))  {
    double f = strtod(parameter,&conv_end);
    if (&parameter[0] != conv_end && ((f >= 30.0 && f < 90.0) || fabs(f) < 0.1)) {
      if (fabs(f) < 0.1) setTrackingState(TS_NONE); else {
        if (park.state != PS_PARKED) {
          trackingRate = hzToSidereal(f);
          setTrackingState(TS_SIDEREAL);
        }
      }
    } else *commandError = CE_PARAM_RANGE;
  } else

  if (cmdSX("SX9")) {
    switch (parameter[1]) {
      case '2': // set new slew rate (returns 1 success or 0 failure)
        if (gotoState == GS_NONE && guideState == GU_NONE) {
          misc.usPerStepCurrent = strtod(&parameter[3],&conv_end);
          if (misc.usPerStepCurrent < usPerStepBase/2.0) misc.usPerStepCurrent = usPerStepBase/2.0;
          if (misc.usPerStepCurrent > usPerStepBase*2.0) misc.usPerStepCurrent = usPerStepBase*2.0;
          if (misc.usPerStepCurrent < usPerStepLowerLimit()) misc.usPerStepCurrent = usPerStepLowerLimit();
          nv.updateBytes(NV_MOUNT_MISC_BASE, &misc, MiscSize);
          updateAccelerationRates();
        } else *commandError = CE_SLEW_IN_MOTION;
      break;
      case '3': // slew rate preset (returns nothing)
        *numericReply = false;
        if (gotoState == GS_NONE && guideState == GU_NONE) {
          switch (parameter[3]) {
            case '5': misc.usPerStepCurrent = usPerStepBase*2.0; break; // 50%
            case '4': misc.usPerStepCurrent = usPerStepBase*1.5; break; // 75%
            case '3': misc.usPerStepCurrent = usPerStepBase;     break; // 100%
            case '2': misc.usPerStepCurrent = usPerStepBase/1.5; break; // 150%
            case '1': misc.usPerStepCurrent = usPerStepBase/2.0; break; // 200%
            default:  misc.usPerStepCurrent = usPerStepBase;
          }
          if (misc.usPerStepCurrent < usPerStepLowerLimit()) misc.usPerStepCurrent = usPerStepLowerLimit();
          nv.updateBytes(NV_MOUNT_MISC_BASE, &misc, MiscSize);
          updateAccelerationRates();
        } else *commandError = CE_SLEW_IN_MOTION;
      break;
      case '5': // autoMeridianFlip
        if (parameter[3] == '0' || parameter[3] == '1') {
          misc.meridianFlipAuto = parameter[3] - '0';
          nv.updateBytes(NV_MOUNT_MISC_BASE, &misc, MiscSize);
        } else *commandError = CE_PARAM_RANGE;
      break;
      case '6': // preferred pier side
        switch (parameter[3]) {
          case 'E': preferredPierSide = PSS_EAST; break;
          case 'W': preferredPierSide = PSS_WEST; break;
          case 'B': preferredPierSide = PSS_BEST; break;
          default: *commandError = CE_PARAM_RANGE;
        }
      break;
      case '7': // buzzer
        if (parameter[3] == '0' || parameter[3] == '1') {
          misc.buzzer = parameter[3] - '0';
          nv.updateBytes(NV_MOUNT_MISC_BASE, &misc, MiscSize);
        } else *commandError = CE_PARAM_RANGE;
      break;
      case '8': // pause at home on meridian flip
        if (parameter[3] == '0' || parameter[3] == '1') {
          misc.meridianFlipPause = parameter[3] - '0';
          nv.updateBytes(NV_MOUNT_MISC_BASE, &misc, MiscSize);
        } else *commandError = CE_PARAM_RANGE;
      break;
      case '9': // continue if paused at home
        if (parameter[3] == '1') { if (waitingHome) waitingHomeContinue = true; } else *commandError = CE_PARAM_RANGE;
      break;
      default: return false;
    }
  } else

  if (cmdP2("SXEM")) { // En: Setup value
    long l = strtol(&parameter[3],NULL,10); float degs = l/4.0;
  //if (l == 0 || l == GEM || l == FORK || l == ALTAZM) nv.write(EE_mountType, l); else *commandError = CE_PARAM_RANGE;
  } else

  // T - Tracking Commands
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
  //
  // :To#       Track full compensation on
  // :Tr#       Track refraction compensation on
  // :Tn#       Track compensation off
  // :T1#       Track dual axis off (disable Dec tracking on Eq mounts)
  // :T2#       Track dual axis on
  //            Return: 0 on failure
  //                    1 on success
  if (command[0] == 'T' && parameter[0] == 0) {
    if (command[1] == 'o') { rateCompensation = RC_FULL_RA; } else
    if (command[1] == 'r') { rateCompensation = RC_REFR_RA; } else
    if (command[1] == 'n') { rateCompensation = RC_NONE;    } else
    if (command[1] == '1') {
      if (rateCompensation == RC_REFR_BOTH) rateCompensation = RC_REFR_RA; else
      if (rateCompensation == RC_FULL_BOTH) rateCompensation = RC_FULL_RA;
    } else
    if (command[1] == '2') {
      if (rateCompensation == RC_REFR_RA) rateCompensation = RC_REFR_BOTH; else
      if (rateCompensation == RC_FULL_RA) rateCompensation = RC_FULL_BOTH;
    } else
    if (command[1] == 'S') { rateCompensation = RC_NONE; trackingRate = hzToSidereal(60);     } else
    if (command[1] == 'K') { rateCompensation = RC_NONE; trackingRate = hzToSidereal(60.136); } else
    if (command[1] == 'L') { rateCompensation = RC_NONE; trackingRate = hzToSidereal(57.9);   } else
    if (command[1] == 'Q') { trackingRate = hzToSidereal(SIDEREAL_RATE_HZ);                   } else
    if (command[1] == '+') { transform.site.setPeriodSubMicros(periodSubMicros - hzToSubMicros(0.02)); } else
    if (command[1] == '-') { transform.site.setPeriodSubMicros(periodSubMicros + hzToSubMicros(0.02)); } else
    if (command[1] == 'R') { transform.site.setPeriodSubMicros(SIDEREAL_PERIOD); } else
    if (command[1] == 'e') {
      if (park.state != PS_PARKED) {
        errorReset();
        setTrackingState(TS_SIDEREAL);
      } else *commandError = CE_PARKED;
    } else
    if (command[1] == 'd') {
      if (gotoState == GS_NONE && guideState == GU_NONE) trackingState = TS_NONE; else *commandError = CE_SLEW_IN_MOTION;
    } else *commandError = CE_CMD_UNKNOWN;

    if (transform.mountType == ALTAZM) {
      if (rateCompensation == RC_FULL_RA) rateCompensation = RC_FULL_BOTH;
      if (rateCompensation == RC_REFR_RA) rateCompensation = RC_REFR_BOTH;
    }

    if (*commandError == CE_NONE) {
      switch (command[1]) { case 'S': case 'K': case 'L': case 'Q': case '+': case '-': case 'R': *numericReply = false; }
      switch (command[1]) { case 'o': case 'r': case 'n': trackingRate = hzToSidereal(SIDEREAL_RATE_HZ); }
      updateTrackingRates();
    }
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
          misc.backlash.axis2 = arcsecToRad(arcSecs);
          axis2.setBacklash(misc.backlash.axis2);
          nv.updateBytes(NV_MOUNT_MISC_BASE, &misc, MiscSize);
        } else
        if (parameter[0] == 'R') {
          misc.backlash.axis1 = arcsecToRad(arcSecs);
          axis1.setBacklash(misc.backlash.axis1);
          nv.updateBytes(NV_MOUNT_MISC_BASE, &misc, MiscSize);
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
        int arcSec = radToArcsec(misc.backlash.axis2);
        if (arcSec < 0) arcSec = 0;
        if (arcSec > 3600) arcSec = 3600;
        sprintf(reply,"%d", arcSec);
        *numericReply = false;
    } else
    if (parameter[0] == 'R' && parameter[1] == 0) {
        int arcSec = radToArcsec(misc.backlash.axis1);
        if (arcSec < 0) arcSec = 0;
        if (arcSec > 3600) arcSec = 3600;
        sprintf(reply,"%d", arcSec);
        *numericReply = false;
    } else *commandError = CE_CMD_UNKNOWN;
  } else return false;

  return true;
}

#endif