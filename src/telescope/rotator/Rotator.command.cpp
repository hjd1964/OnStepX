//--------------------------------------------------------------------------------------------------
// OnStepX rotator control

#include "../../lib/convert/Convert.h"
#include "../mount/Mount.h"
#include "Rotator.h"

#if AXIS3_DRIVER_MODEL != OFF

bool Rotator::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  *supressFrame = false;

  // process any rotator axis commands
  if (axis.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;

  if (command[0] == 'h') {
    // :hP#       Moves rotator to the park position
    //            Return: 0 on failure
    //                    1 on success
    if (command[1] == 'P' && parameter[0] == 0) {
      CommandError e = CE_NONE;
      park();
      if (e != CE_NONE) *commandError = e; else *commandError = CE_1;
      return false;
    } else 

    // :hR#       Restore parked rotator to operation
    //            Return: 0 on failure
    //                    1 on success
    if (command[1] == 'R' && parameter[0] == 0) {
      CommandError e = CE_NONE;
      unpark();
      if (e != CE_NONE) *commandError = e; else *commandError = CE_1;
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
    //            Returns: M# (for moving) or S# (for stopped)
    if (command[1] == 'T') {
      if (axis.isSlewing()) strcpy(reply,"M"); else strcpy(reply,"S");
      *numericReply = false;
    } else

    // :rI#       Get rotator mInimum position (in degrees)
    //            Returns: n#
    if (command[1] == 'I') {
      sprintf(reply,"%ld",(long)round(axis.settings.limits.min));
      *numericReply = false;
    } else

    // :rM#       Get rotator Max position (in degrees)
    //            Returns: n#
    if (command[1] == 'M') {
      sprintf(reply,"%ld",(long)round(axis.settings.limits.max));
      *numericReply = false;
    } else

    // :rD#       Get rotator degrees per step
    //            Returns: n.n#
    if (command[1] == 'u') {
      sprintF(reply, "%7.5f", 1.0/axis.getStepsPerMeasure());
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
      setBacklash(atol(parameter));
      axis.setBacklashSteps(getBacklash());
    } else

    // :rQ#       Stop (Quit) rotator movement
    //            Returns: Nothing
    if (command[1] == 'Q') {
      axis.autoSlewStop();
      *numericReply = false;
    } else

    // :r[n]#     Set rotator move rate where n = 1 for .01 deg/s, 2 for 0.1 deg/s, 3 for 1.0 deg/s, 4 for 10.0 deg/s
    //            Returns: Nothing
    if (command[1] >= '1' && command[1] <= '4') {
      float p[] = {0.01F, 0.1F, 1.0F, 10.0F};
      slewRate = p[command[1] - '1'];
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
      *commandError = axis.autoSlew(DIR_FORWARD, slewRate);
      *numericReply = false;
    } else

    // :r<#       Move rotator CCW
    //            Returns: Nothing
    if (command[1] == '<') {
      *commandError = axis.autoSlew(DIR_REVERSE, slewRate);
      *numericReply = false;
    } else

    // :rG#       Get rotator current angle
    //            Returns: sDD*MM#
    if (command[1] == 'G') {
      convert.doubleToDms(reply, axis.getInstrumentCoordinate(), true, true, PM_LOW);
      *numericReply = false;
    } else

    // :rR[sDDD*MM, etc.]#
    //            Set rotator target angle Relative (in degrees)
    //            Returns: Nothing
    if (command[1] == 'R') {
      double r, t;
      convert.dmsToDouble(&r, parameter, true);
      t = axis.getTargetCoordinate();
      *commandError = gotoTarget(t + r);
      *numericReply = false;
    } else

    // :rS[sDDD*MM, etc.]#
    //            Move rotator to Set target angle (in degrees)
    //            Return: 0 on failure
    //                    1 on success
    if (command[1] == 'S') {
      double t;
      convert.dmsToDouble(&t, parameter, true);
      *commandError = gotoTarget(t);
    } else

    // :rZ#       Set rotator position to Zero degrees
    //            Returns: Nothing
    if (command[1] == 'Z') {
      *commandError = axis.resetPosition(0.0);
      axis.setBacklashSteps(getBacklash());
      *numericReply = false;
    } else

    // :rF#       Set rotator position as Half-travel
    //            Returns: Nothing
    if (command[1] == 'F') {
      double t = round((axis.settings.limits.max + axis.settings.limits.min)/2.0);
      *commandError = axis.resetPosition(t);
      axis.setBacklashSteps(getBacklash());
      *numericReply = false;
    } else

    // :rC#       Move rotator to half-travel target position
    //            Returns: Nothing
    if (command[1] == 'C') {
      *commandError = gotoTarget(round((axis.settings.limits.max + axis.settings.limits.min)/2.0));
      *numericReply = false;
    } else

    // :r+#       Derotator Enable
    //            Returns: Nothing
    if (command[1] == '+') {
      #ifdef MOUNT_PRESENT
        if (transform.mountType == ALTAZM) derotatorEnabled = true;
      #endif
      *numericReply = false;
    } else

    // :r-#       Derotator Disable 
    //            Returns: Nothing
    if (command[1] == '-') {
      derotatorEnabled = false;
      axis.setFrequencyBase(0.0F);
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

  } else return false;

  return true;
}

#endif
