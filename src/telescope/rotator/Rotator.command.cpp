//--------------------------------------------------------------------------------------------------
// telescope rotator control, commands

#include "../../lib/convert/Convert.h"
#include "../../lib/axis/Axis.h"

#include "../mount/Mount.h"
#include "Rotator.h"

#if AXIS3_DRIVER_MODEL != OFF

extern Axis axis3;

bool Rotator::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  *supressFrame = false;

  // process any rotator axis commands
  if (axis3.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;

  if (command[0] == 'h') {
    // :hP#       Moves rotator to the park position
    //            Return: 0 on failure
    //                    1 on success
    if (command[1] == 'P' && parameter[0] == 0) {
      CommandError e = park();
      if (e == CE_NONE) *commandError = CE_1; else { VF("MSG: Rotator, park error "); VL(e); *commandError = e; }
      return false;
    } else 

    // :hR#       Restore parked rotator to operation
    //            Return: 0 on failure
    //                    1 on success
    if (command[1] == 'R' && parameter[0] == 0) {
      CommandError e = unpark();
      if (e == CE_NONE) *commandError = CE_1; else { VF("MSG: Rotator, unpark error "); VL(e); *commandError = e; }
      return false;
    } else return false;
  } else

  // :rA#    rotator Active?
  //            Return: 0 on failure (no rotator)
  //                    1 on success
  if (command[0] == 'r' && command[1] == 'A' && parameter[0] == 0) {
    // empty command for success response 1
  } else

  // r - rotator Commands
  if (command[0] == 'r') {

    // :rT#       Get rotator sTatus
    //            Returns: s#
    if (command[1] == 'T') {
      if (axis3.isSlewing()) strcat(reply, "M"); else { // [M]oving
        strcat(reply, "S");                             // [S]topped)
        if (derotatorEnabled) strcat(reply, "D");       // [D]e-Rotate enabled
        if (derotatorReverse) strcat(reply, "R");       // De-Rotate [R]everse
      }
      char temp[2] = "0"; temp[0] = '0' + getGotoRate(); strcat(reply, temp); // [1] to [5] for 0.5x to 2x goto rate
      *numericReply = false;
    } else

    // :rI#       Get rotator mInimum position (in degrees)
    //            Returns: n#
    if (command[1] == 'I') {
      sprintf(reply,"%ld",(long)round(axis3.settings.limits.min));
      *numericReply = false;
    } else

    // :rM#       Get rotator Max position (in degrees)
    //            Returns: n#
    if (command[1] == 'M') {
      sprintf(reply,"%ld",(long)round(axis3.settings.limits.max));
      *numericReply = false;
    } else

    // :rD#       Get rotator degrees per step
    //            Returns: n.n#
    if (command[1] == 'D') {
      sprintF(reply, "%7.5f", 1.0/axis3.getStepsPerMeasure());
      *numericReply = false;
    } else

    // :rb#       Get rotator Backlash amount (in steps)
    //            Return: n#
    if (command[1] == 'b' && parameter[0] == 0) {
      sprintf(reply,"%ld",(long)round(getBacklash()));
      *numericReply = false;
    } else

    // :rb[n]#    Set rotator Backlash amount (in steps)
    //            Return: 0 on failure
    //                    1 on success
    if (command[1] == 'b') {
      *commandError = setBacklash(atol(parameter));
    } else

    // :rQ#       Stop (Quit) rotator movement
    //            Returns: Nothing
    if (command[1] == 'Q') {
      if (axis3.isHoming()) {
        axis3.autoSlewAbort();
        homing = false;
      } else axis3.autoSlewStop();
      *numericReply = false;
    } else

    // :r[n]#     Set rotator move or goto rate
    //            move where n = 1 for 0.01 deg/s, 2 for 0.1 deg/s, 3 for 1.0 deg/s, 4 for 0.5x goto rate
    //            goto rate where n = 5 for 0.5x, 6 for 0.66x, 7 for 1x, 8 for 1.5x, 9 for 2x AXISn_SLEW_RATE_BASE_DESIRED
    //            Returns: Nothing
    if (command[1] >= '1' && command[1] <= '9') {
      if (strlen(parameter) == 0) {
        int v = command[1] - '0';
        if (v < 5) setMoveRate(v); else setGotoRate(v - 4);
      } else *commandError = CE_PARAM_FORM;
      *numericReply = false;
    } else

    // :rW#       Get working slew rate in deg/s
    //            Returns: d.d#
    if (command[1] == 'W') {
      sprintF(reply, "%0.1f", settings.gotoRate);
      *numericReply = false;
    } else

    // :rc#       Set continious move
    //            Returns: Nothing
    if (command[1] == 'c') {
      // this command is ignored, all rotator movement in OnStepX is continious
      *numericReply = false;
    } else

    // :r>#       Move rotator CW
    //            Returns: Nothing
    if (command[1] == '>') {
      *commandError = move(DIR_FORWARD);
      *numericReply = false;
    } else

    // :r<#       Move rotator CCW
    //            Returns: Nothing
    if (command[1] == '<') {
      *commandError = move(DIR_REVERSE);
      *numericReply = false;
    } else

    // :rG#       Get rotator current angle
    //            Returns: sDD*MM#
    if (command[1] == 'G') {
      convert.doubleToDms(reply, axis3.getInstrumentCoordinate(), true, true, PM_LOW);
      *numericReply = false;
    } else

    // :rr[sDDD*MM, etc.]#
    //            Set rotator target angle relative (in degrees)
    //            Returns: Nothing
    if (command[1] == 'r') {
      double r, t;
      convert.dmsToDouble(&r, parameter, true);
      t = axis3.getTargetCoordinate();
      *commandError = gotoTarget(t + r);
      *numericReply = false;
    } else

    // :rS[sDDD*MM, etc.]#
    //            Move rotator to Set target angle (in degrees)
    //            Return: 0 on failure
    //                    1 on success
    if (command[1] == 'S') {
      int i = 0;
      double t, s = 1.0;
      if (parameter[0] == '-') s = -1.0;
      if (parameter[0] == '+' || parameter[0] == '-') i = 1;
      if (convert.dmsToDouble(&t, &parameter[i], false)) *commandError = gotoTarget(s*t); else *commandError = CE_PARAM_FORM;
    } else

    // :rZ#       Set rotator position to Zero degrees
    //            Returns: Nothing
    if (command[1] == 'Z') {
      settings.parkState = PS_UNPARKED;
      *commandError = axis3.resetPosition(0.0);
      axis3.setBacklashSteps(getBacklash());
      *numericReply = false;
    } else

    // :rF#       Set rotator position as Half-travel
    //            Returns: Nothing
    if (command[1] == 'F') {
      settings.parkState = PS_UNPARKED;
      *commandError = axis3.resetPosition((axis3.settings.limits.max + axis3.settings.limits.min)/2.0F);
      axis3.setBacklashSteps(getBacklash());
      *numericReply = false;
    } else

    // :rC#       Move rotator to half-travel target position
    //            Returns: Nothing
    if (command[1] == 'C') {
      if (AXIS3_SENSE_HOME != OFF) {
        if (settings.parkState == PS_UNPARKED) {
          axis3.setFrequencySlew(settings.gotoRate);
          *commandError = axis3.autoSlewHome();
          if (*commandError == CE_NONE) {
            homing = true;
          }
        } else *commandError = CE_PARKED;
      } else {
        *commandError = gotoTarget((axis3.settings.limits.max + axis3.settings.limits.min)/2.0F);
      }
      *numericReply = false;
    } else

    // :r+#       Derotator Enable
    //            Returns: Nothing
    if (command[1] == '+') {
      #ifdef MOUNT_PRESENT
        if (transform.mountType == ALTAZM) {
          if (settings.parkState == PS_UNPARKED) derotatorEnabled = true; else *commandError = CE_PARKED;
        }
      #endif
      *numericReply = false;
    } else

    // :r-#       Derotator Disable 
    //            Returns: Nothing
    if (command[1] == '-') {
      derotatorEnabled = false;
      axis3.setFrequencyBase(0.0F);
      *numericReply = false;
    } else

    // :rP#       Rotator move to parallactic angle
    //            Returns: Nothing
    if (command[1] == 'P') {
      #ifdef MOUNT_PRESENT
        Coordinate current = mount.getPosition();
        *commandError = gotoTarget(parallacticAngle(&current));
      #endif
      *numericReply = false;
    } else

    // :rR#       Derotator Reverse direction
    //            Returns: Nothing
    if (command[1] == 'R') {
      derotatorReverse = !derotatorReverse;
      *numericReply = false;
    } else *commandError = CE_CMD_UNKNOWN;

  } else

  // :GX98#     Get rotator availablity
  //            Returns: D for rotate/derotate
  //                     R for rotate only
  //                     N for none
  if (command[0] == 'G' && command[1] == 'X' && parameter[0] == '9' && parameter[1] == '8' && parameter[2] == 0) {
    *numericReply = false;
    if (AXIS3_DRIVER_MODEL != OFF) {
      #if defined(MOUNT_PRESENT)
        if (transform.mountType == ALTAZM) strcpy(reply, "D"); else
      #endif
      strcpy(reply, "R");
    } else strcpy(reply, "N");
  } else return false;

  return true;
}

#endif
