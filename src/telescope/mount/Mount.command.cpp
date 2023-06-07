//--------------------------------------------------------------------------------------------------
// telescope mount control, commands

#include "Mount.h"

#ifdef MOUNT_PRESENT

#include "../../lib/tasks/OnTask.h"

#include "site/Site.h"
#include "coordinates/Transform.h"
#include "goto/Goto.h"
#include "guide/Guide.h"
#include "limits/Limits.h"
#include "park/Park.h"

bool Mount::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  char *conv_end;
  PrecisionMode precisionMode = PM_HIGH;

  if (command[0] == 'G') {
    // :GA#       Get Mount Altitude
    //            Returns: sDD*MM# or sDD*MM'SS# (based on precision setting)
    // :GAH#      High precision
    //            Returns: sDD*MM'SS.SSS# (high precision)
    if (command[1] == 'A' && (parameter[0] == 0 || parameter[1] == 0)) {
      if (parameter[0] == 'H') precisionMode = PM_HIGHEST; else if (parameter[0] != 0) { *commandError = CE_PARAM_FORM; return true; }
      convert.doubleToDms(reply, radToDeg(getPosition(CR_MOUNT_ALT).a), false, true, precisionMode);
      *numericReply = false;
    } else

    // :GD#       Get Mount Declination
    //            Returns: sDD*MM# or sDD*MM:SS# (based on precision setting)
    // :GDH#      Returns: sDD*MM:SS.SSS# (high precision)
    if (command[1] == 'D' && (parameter[0] == 0 || parameter[1] == 0)) {
      if (parameter[0] == 'H' || parameter[0] == 'e') precisionMode = PM_HIGHEST; else if (parameter[0] != 0) { *commandError = CE_PARAM_FORM; return true; }
      convert.doubleToDms(reply, radToDeg(getPosition().d), false, true, precisionMode);
      *numericReply = false;
    } else

    // :GR#       Get Mount Right Ascension
    //            Returns: HH:MM.T# or HH:MM:SS# (based on precision setting)
    // :GRH#      Returns: HH:MM:SS.SSSS# (high precision)
    if (command[1] == 'R' && (parameter[0] == 0 || parameter[1] == 0)) {
      if (parameter[0] == 'H' || parameter[0] == 'a') precisionMode = PM_HIGHEST; else if (parameter[0] != 0) { *commandError = CE_PARAM_FORM; return true; }
      convert.doubleToHms(reply, radToHrs(getPosition().r), false, precisionMode);
      *numericReply = false;
    } else

    // :GT#         Get tracking rate, 0.0 unless TrackingSidereal
    //              Returns: n.n# (OnStep returns more decimal places than LX200 standard)
    if (command[1] == 'T' && parameter[0] == 0)  {
      if (trackingState == TS_NONE) strcpy(reply,"0"); else sprintF(reply, "%0.5f", siderealToHz(trackingRate));
      *numericReply = false;
    } else 

    // :GX          OnStep extended command
    if (command[1] == 'X' && parameter[2] == 0)  {

      // :GX4[n]#   Get angles [n]
      //            (0 and 1) Returns instrument angle in: DDD:MM:SS#
      //            (2 and 3) Returns instrument angle in: n.nnnnnn#
      //            (4 and 5) Returns encoder angle in counts: n#
      if (parameter[0] == '4')  {
        *numericReply = false;
        switch (parameter[1]) {
          case '0': convert.doubleToDms(reply, radToDeg(axis1.getInstrumentCoordinate()), true, true, PM_HIGH); break;
          case '1': convert.doubleToDms(reply, radToDeg(axis2.getInstrumentCoordinate()), true, true, PM_HIGH); break; 
          case '2': sprintF(reply, "%0.6f", radToDeg(axis1.getInstrumentCoordinate())); break;
          case '3': sprintF(reply, "%0.6f", radToDeg(axis2.getInstrumentCoordinate())); break;
          case '4': sprintf(reply, "%d", axis1.motor->getEncoderCount()); break;
          case '5': sprintf(reply, "%d", axis2.motor->getEncoderCount()); break;
          default:  *numericReply = true; *commandError = CE_CMD_UNKNOWN;
        }
      } else

      // :GXE[m]#   Get mount setting
      //            Returns: n#
      if (parameter[0] == 'E')  {
        uint16_t axesToRevert;
        switch (parameter[1]) {
          case '4': sprintf(reply, "%ld", lround(axis1.getStepsPerMeasure()/RAD_DEG_RATIO)); *numericReply = false; break;
          case '5': sprintf(reply, "%ld", lround(axis2.getStepsPerMeasure()/RAD_DEG_RATIO)); *numericReply = false; break;
          case 'E': reply[0] = '0' + (MOUNT_COORDS - 1); *supressFrame = true; *numericReply = false; break;
          case 'F': if (AXIS2_TANGENT_ARM != ON) *commandError = CE_0; break;
          case 'M':
            axesToRevert = nv.readUI(NV_AXIS_SETTINGS_REVERT);
            if (axesToRevert & 1) sprintf(reply, "%d", (int)nv.readUC(NV_MOUNT_TYPE_BASE)); else strcpy(reply, "0");
            *numericReply = false;
          break;
        default:
          return false;
        }
      } else

      // :GXFn#     Get frequency setting and workload
      //            Returns: Value
      if (parameter[0] == 'F')  {
        switch (parameter[1]) {
          case '3': sprintF(reply, "%0.6f", (axis1.getDirection() == DIR_FORWARD) ? axis1.getFrequencySteps() : -axis1.getFrequencySteps()); *numericReply = false; break;
          case '4': sprintF(reply, "%0.6f", (axis2.getDirection() == DIR_FORWARD) ? axis2.getFrequencySteps() : -axis2.getFrequencySteps()); *numericReply = false; break;
          case 'A': sprintf(reply, "%d%%", 50); *numericReply = false; break; // workload
          case 'G': // index position for Axis2
            sprintF(reply, "%0.6f", radToDeg(transform.instrumentToMount(0.0, axis2.getIndexPosition()).a2));
            *numericReply = false;
          break;
        default:
          return false;
        }
      } else

      // :GXTD#     Get tracking rate offset Dec in arc-seconds/sidereal second
      //            Returns: n.nnnnnn#
      if (parameter[0] == 'T' && parameter[1] == 'D' && parameter[2] == 0) {
        sprintF(reply, "%0.8f", trackingRateOffsetDec*15.0F);
        *numericReply = false;
      } else

      // :GXTR#     Get tracking rate offset RA in arc-seconds/sidereal second
      //            Returns: n.nnnnnn#
      if (parameter[0] == 'T' && parameter[1] == 'R' && parameter[2] == 0) {
        sprintF(reply, "%0.8f", trackingRateOffsetRA*15.0F);
        *numericReply = false;
      } else return false;

    } else

    // :GZ#       Get Mount Azimuth
    //            Returns: DDD*MM# or DDD*MM'SS# (based on precision setting)
    // :GZH#      High precision
    //            Returns: DDD*MM'SS.SSS# (high precision)
    if (command[1] == 'Z' && (parameter[0] == 0 || parameter[1] == 0)) {
      if (parameter[0] == 'H') precisionMode = PM_HIGHEST; else if (parameter[0] != 0) { *commandError = CE_PARAM_FORM; return true; }
      convert.doubleToDms(reply, NormalizeAzimuth(radToDeg(getPosition(CR_MOUNT_HOR).z)), true, false, precisionMode);
      *numericReply = false;
    } else return false;
  } else

  if (command[0] == 'S') {
    //  :ST[H.H]# Set Tracking Rate in Hz where 60.0 is the solar rate
    //            Return: 0 on failure
    //                    1 on success
    if (command[1] == 'T') {
      double f = strtod(parameter,&conv_end);
      if (&parameter[0] != conv_end && ((f >= 30.0 && f < 90.0) || fabs(f) < 0.1)) {
        if (fabs(f) < 0.1) tracking(false); else {
          #if GOTO_FEATURE == ON
            if (park.state != PS_PARKED) {
              trackingRate = hzToSidereal(f);
              tracking(true);
            }
          #endif
        }
      } else *commandError = CE_PARAM_RANGE;
    } else

    //  :SEO#         Set encoder origin (for Encoder Bridge) also resets OnStep
    //                Return: 0 on failure
    //                        1 on success
    if (command[1] == 'E' && parameter[0] == 'O' && parameter[1] == 0) {
      #ifdef SERVO_MOTOR_PRESENT
        #if AXIS1_ENCODER == SERIAL_BRIDGE && AXIS2_ENCODER == SERIAL_BRIDGE && defined(SERIAL_ENCODER)
          if (!mount.isTracking() && !mount.isSlewing()) {
            VLF("MSG: Mount, setting absolute encoder origin");
            SERIAL_ENCODER.print(":SO#");
            #ifdef HAL_RESET
              delay(100);
              enable(false);
              VLF("MSG: Mount, resetting OnStep...");
              nv.wait();
              tasks.yield(1000);
              HAL_RESET();
            #endif
          } else {
            *commandError = CE_0;
            DLF("MSG: Mount, setting absolute encoder origin failed; the mount is in motion!");
          }
        #endif
      #endif
    } else

    if (command[1] == 'X') {
      if (parameter[2] != ',') { *commandError = CE_PARAM_FORM; return true; }

      // :SX4[m],[n]#   Set encoder axis [m] to value [n]
      //                Return: 0 on failure
      //                        1 on success
      if (parameter[0] == '4') {
        double d;
        static double encoderAxis1 = NAN;
        static double encoderAxis2 = NAN;

        switch (parameter[1]) {

          // set encoder Axis1 value
          case '0':
            d = strtod(&parameter[3], &conv_end);
            if (&parameter[3] != conv_end && fabs(d) <= 360.0L) { encoderAxis1 = degToRad(d); } else { encoderAxis1 = NAN; *commandError = CE_PARAM_RANGE; }
          break;

          // set encoder Axis2 value
          case '1':
            d = strtod(&parameter[3], &conv_end);
            if (&parameter[3] != conv_end && fabs(d) <= 360.0L) { encoderAxis2 = degToRad(d); } else { encoderAxis2 = NAN; *commandError = CE_PARAM_RANGE; }
          break;

          // sync from encoder values
          case '2':
            if (parameter[3] == '1' && parameter[4] == 0) {
              #if GOTO_FEATURE == ON
                CommandError e = goTo.validate();
                if (e != CE_NONE) { *commandError = e; return true; }
              #endif
              if (isnan(encoderAxis1) || isnan(encoderAxis2) || syncFromOnStepToEncoders) { *commandError = CE_0; return true; }
              axis1.setInstrumentCoordinate(encoderAxis1);
              axis2.setInstrumentCoordinate(encoderAxis2);
            }
          break;

          // allow sws to control sync mode
          case '3': syncFromOnStepToEncoders = false; break;

          // set and sync encoder Axis1 and Axis2 values
          case '4': {
            d = strtod(&parameter[3], &conv_end);
            if (&parameter[3] != conv_end && fabs(d) <= 360.0L) { encoderAxis1 = degToRad(d); } else { encoderAxis1 = NAN; }

            char *parameter2 = strchr(&parameter[3], ','); parameter2++;
            d = strtod(parameter2, &conv_end);
            if (parameter2 != conv_end && fabs(d) <= 360.0L) { encoderAxis2 = degToRad(d); } else { encoderAxis2 = NAN; }

            #if GOTO_FEATURE == ON
              CommandError e = goTo.validate();              
              if (e != CE_NONE && e != CE_SLEW_ERR_IN_STANDBY && e != CE_SLEW_IN_SLEW) { *commandError = e; return true; }
            #endif

            if ( isnan(encoderAxis1) ||
                 isnan(encoderAxis2) ||
                 syncFromOnStepToEncoders ||
                 (goTo.state != GS_NONE && goTo.stage != GG_NEAR_DESTINATION_WAIT) ||
                 guide.state != GU_NONE) { *commandError = CE_0; return true; }

            axis1.setInstrumentCoordinate(encoderAxis1);
            axis2.setInstrumentCoordinate(encoderAxis2);
          break; }

          default: *commandError = CE_CMD_UNKNOWN; break;
        }
      } else

      // :SXEM,[n]#   Set mount type (for next restart) where n=0 for default
      //              Return: 0 on failure
      //                      1 on success
      if (parameter[0] == 'E' && parameter[1] == 'M') {
        long l = atol(&parameter[3]);
        if (l == 0 ||
            (l == GEM && AXIS1_WRAP == OFF) ||
            (l == FORK && AXIS1_WRAP == OFF) ||
            (l == ALTAZM && AXIS2_TANGENT_ARM == OFF)) {
          nv.write(NV_MOUNT_TYPE_BASE, (uint8_t)l);
        } else *commandError = CE_PARAM_RANGE;
      } else

      // :SXTD,n.n#   Set tracking rate offset Dec in arc-seconds/sidereal second
      //              Return: 0 on failure
      //                      1 on success
      if (parameter[0] == 'T' && parameter[1] == 'D') {
        float f = strtod(&parameter[3], &conv_end);
        if (f < -1800.0F) f = -1800.0F;
        if (f > 1800.0F) f = 1800.0F;
        trackingRateOffsetDec = f/15.0F;
      } else

      // :SXTR,n.n# Set tracking rate offset RA in arc-seconds/sidereal second
      //            Return: 0 failure, 1 success
      if (parameter[0] == 'T' && parameter[1] == 'R') {
        float f = strtod(&parameter[3], &conv_end);
        if (f < -1800.0F) f = -1800.0F;
        if (f > 1800.0F) f = 1800.0F;
        trackingRateOffsetRA = f/15.0F;
      } else return false;

    } else return false;
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
    if (command[1] == 'o') { settings.rc = RC_MODEL; } else
    if (command[1] == 'r') { settings.rc = RC_REFRACTION; } else
    if (command[1] == 'n') { settings.rc = RC_NONE; } else
    if (command[1] == '1') {
      if (settings.rc == RC_REFRACTION_DUAL) settings.rc = RC_REFRACTION; else
      if (settings.rc == RC_MODEL_DUAL) settings.rc = RC_MODEL;
    } else
    if (command[1] == '2') {
      if (settings.rc == RC_REFRACTION) settings.rc = RC_REFRACTION_DUAL; else
      if (settings.rc == RC_MODEL) settings.rc = RC_MODEL_DUAL;
    } else
    if (command[1] == 'S') { settings.rc = RC_NONE; trackingRate = hzToSidereal(60.0F); } else
    if (command[1] == 'K') { settings.rc = RC_NONE; trackingRate = hzToSidereal(60.136F); } else
    if (command[1] == 'L') { settings.rc = RC_NONE; trackingRate = hzToSidereal(57.9F); } else
    if (command[1] == 'Q') { trackingRate = hzToSidereal(SIDEREAL_RATE_HZ); } else
    if (command[1] == '+') { site.setSiderealPeriod(site.getSiderealPeriod() - hzToSubMicros(0.02F)); } else
    if (command[1] == '-') { site.setSiderealPeriod(site.getSiderealPeriod() + hzToSubMicros(0.02F)); } else
    if (command[1] == 'R') { site.setSiderealPeriod(SIDEREAL_PERIOD); } else
    if (command[1] == 'e') {
      #if GOTO_FEATURE == ON
        if (park.state != PS_PARKED) tracking(true); else *commandError = CE_PARKED;
      #else
        tracking(true);
      #endif
    } else
    if (command[1] == 'd') {
      tracking(false);
    } else *commandError = CE_CMD_UNKNOWN;

    if (transform.mountType == ALTAZM) {
      if (settings.rc == RC_MODEL) settings.rc = RC_MODEL_DUAL;
      if (settings.rc == RC_REFRACTION) settings.rc = RC_REFRACTION_DUAL;
    }

    if (*commandError == CE_NONE) {
      switch (command[1]) { case 'S': case 'K': case 'L': case 'Q': case '+': case '-': case 'R': *numericReply = false; }
      switch (command[1]) { case 'o': case 'r': case 'n': trackingRate = hzToSidereal(SIDEREAL_RATE_HZ); }
      nv.updateBytes(NV_MOUNT_SETTINGS_BASE, &settings, sizeof(MountSettings));
      update();
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
  if (command[0] == '$' && command[1] == 'B') {
    int16_t arcSecs;
    if (convert.atoi2((char*)&parameter[1], &arcSecs)) {
      if (arcSecs >= 0 && arcSecs <= 3600) {
        if (parameter[0] == 'D') {
          settings.backlash.axis2 = arcsecToRad(arcSecs);
          axis2.setBacklash(settings.backlash.axis2);
          nv.updateBytes(NV_MOUNT_SETTINGS_BASE, &settings, sizeof(MountSettings));
        } else
        if (parameter[0] == 'R') {
          settings.backlash.axis1 = arcsecToRad(arcSecs);
          axis1.setBacklash(settings.backlash.axis1);
          nv.updateBytes(NV_MOUNT_SETTINGS_BASE, &settings, sizeof(MountSettings));
        } else *commandError = CE_CMD_UNKNOWN;
      } else *commandError = CE_PARAM_RANGE;
    } else *commandError = CE_PARAM_FORM;
  } else

  //  % - Return parameter
  // :%BD#      Get Dec/Alt Antibacklash value in arc-seconds
  //            Return: n#
  // :%BR#      Get RA/Azm Antibacklash value in arc-seconds
  //            Return: n#
  if (command[0] == '%' && command[1] == 'B' && parameter[1] == 0) {
    if (parameter[0] == 'D') {
        int arcSec = round(radToArcsec(settings.backlash.axis2));
        if (arcSec < 0) arcSec = 0;
        if (arcSec > 3600) arcSec = 3600;
        sprintf(reply,"%d", arcSec);
        *numericReply = false;
    } else
    if (parameter[0] == 'R') {
        int arcSec = round(radToArcsec(settings.backlash.axis1));
        if (arcSec < 0) arcSec = 0;
        if (arcSec > 3600) arcSec = 3600;
        sprintf(reply,"%d", arcSec);
        *numericReply = false;
    } else *commandError = CE_CMD_UNKNOWN;
  } else return false;

  return true;
}

#endif