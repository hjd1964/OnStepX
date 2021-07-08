//--------------------------------------------------------------------------------------------------
// OnStepX focuser control

#include "../lib/convert/Convert.h"
#include "Focuser.h"

#if AXIS4_DRIVER_MODEL != OFF || AXIS5_DRIVER_MODEL != OFF || AXIS6_DRIVER_MODEL != OFF || AXIS7_DRIVER_MODEL != OFF || AXIS8_DRIVER_MODEL != OFF || AXIS9_DRIVER_MODEL != OFF

bool Focuser::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  static int active = 0;
  static int index = 0;

  // :FA#    Focuser Active?
  //            Return: 0 on failure (no focusers)
  //                    1 on success
  if (cmd("FA")) {
    if (focuserAxis[active] == NULL) *commandError = CE_0;
  } else

  // :FA[n]#    Select primary focuser where [n] = 1 to 6
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("FA") && parameter[1] == 0) {
    int i = parameter[0] - '1';
    if (i >= 0 && i <= 5) active = i; else *commandError = CE_PARAM_RANGE;
  } else

  // F, F1, F2, etc. - Focuser Commands
  if (command[0] == 'F') {
    // check that the requested focuser is active
    int i = command[1] - '1';
    if (i >= 0 && i <= 5) {
      // if not active return false so other focuser devices may process the command
      if (focuserAxis[i] == NULL) return false;
      index = i;
      command[1] = parameter[0];
      char temp[32];
      strcpy(temp, &parameter[1]);
      strcpy(&parameter[0], temp);
    } else index = active;

    // check for commands that shouldn't have a parameter
    if (strchr("TpIMtuQF1234+-GZHh", command[1]) && parameter[0] != 0) { *commandError = CE_PARAM_FORM; return true; }

    // get ready for commands that convert to microns or steps (these commands are upper-case for microns OR lower-case for steps)
    const float MicronsToSteps = focuserAxis[index]->getStepsPerMeasure();
    const float StepsToMicrons = 1.0F/MicronsToSteps;
    float MicronsToUnits = 1.0F;
    float UnitsToMicrons = 1.0F;
    float StepsToUnits  = StepsToMicrons;
    float UnitsToSteps  = MicronsToSteps;
    if (strchr("bdgimrs",command[1])) {
      MicronsToUnits = MicronsToSteps;
      UnitsToMicrons = StepsToMicrons;
      StepsToUnits = 1.0F;
      UnitsToSteps = 1.0F;
    }

    // :Fa#       Get primary focuser
    //            Returns: 1 if primary focuser is focuser 1, 0 otherwise
    if (command[1] == 'a') {
      if (index != 0) *commandError = CE_0;
    } else

    // :FT#       Get status
    //            Returns: M# (for moving) or S# (for stopped)
    if (command[1] == 'T') {
      if (focuserAxis[index]->autoSlewActive()) strcpy(reply,"M"); else strcpy(reply,"S");
      *numericReply = false;
    } else

    // :Fp#       Get mode
    //            Return: 0 for absolute
    //                    1 for pseudo absolute
    if (command[1] == 'p') {
      if (!isDC(index)) *commandError = CE_0;
    } else

    // :FI#       Get full in position (in microns or steps)
    //            Returns: n#
    if (toupper(command[1]) == 'I') {
      sprintf(reply,"%ld",(long)round(focuserAxis[index]->settings.limits.min*MicronsToUnits));
      *numericReply = false;
    } else

    // :FM#       Get max position (in microns or steps)
    //            Returns: n#
    if (toupper(command[1]) == 'M') {
      sprintf(reply,"%ld",(long)round(focuserAxis[index]->settings.limits.max*MicronsToUnits));
      *numericReply = false;
    } else

    // :Fe#       Get focuser temperature differential
    //            Returns: n# temperature in deg. C
    if (command[1] == 'e') {
      if (getTcfEnable(index)) sprintF(reply, "%3.1f", getTemperature() - getTcfT0(index)); else sprintF(reply, "%3.1f", 0.0);
      *numericReply = false;
    } else

    // :Ft#       Get focuser temperature
    //            Returns: n# temperature in deg. C
    if (command[1] == 't') {
      sprintF(reply, "%3.1f", getTemperature());
      *numericReply = false;
    } else

    // :Fu#       Get focuser microns per step
    //            Returns: n.n#
    if (command[1] == 'u') {
      sprintF(reply, "%7.5f", 1.0/focuserAxis[index]->getStepsPerMeasure());
      *numericReply = false;
    } else

    // :FB#       Get focuser backlash amount (in steps or microns)
    //            Return: n#
    if (toupper(command[1]) == 'B' && parameter[0] == 0) {
      sprintf(reply,"%ld",(long)round(getBacklash(index)*MicronsToUnits));
      *numericReply = false;
    } else

    // :FB[n]#    Set focuser backlash amount (in steps or microns)
    //            Return: 0 on failure
    //                    1 on success
    if (toupper(command[1]) == 'B') {
      setBacklash(index, round(atol(parameter)*UnitsToMicrons));
      focuserAxis[index]->setBacklash(getBacklash(index));
    } else

    // :FC#       Get focuser temperature compensation coefficient
    //            Return: n.n#
    if (command[1] == 'C' && parameter[0] == 0) {
      sprintF(reply, "%7.5f", getTcfCoef(index));
      *numericReply = false;
    } else

    // :FC[sn.n]# Set focuser temperature compensation coefficient in um per deg. C (+ moves out as temperature falls)
    //            Return: 0 on failure
    //                    1 on success
    if (command[1] == 'C') {
      float value = atof(parameter); if (!setTcfCoef(index, value))
      *commandError = CE_PARAM_RANGE;
    } else

    // :Fc#       Get focuser temperature compensation enable status
    //            Return: 0 if disabled
    //                    1 if enabled
    if (command[1] == 'c' && parameter[0] == 0) {
      if (!getTcfEnable(index)) *commandError = CE_0;
    } else

    // :Fc[n]#    Enable/disable focuser temperature compensation where [n] = 0 or 1
    //            Return: 0 on failure
    //                    1 on success
    if (command[1] == 'c' && parameter[1] == 0) {
      setTcfEnable(index, parameter[0] != '0');
    } else

    // :FD#       Get focuser temperature compensation deadband amount (in steps or microns)
    //            Return: n#
    if (toupper(command[1]) == 'D' && parameter[0] == 0) {
      sprintf(reply,"%ld",(long)round(getTcfDeadband(index)*MicronsToUnits));
      *numericReply = false;
    } else

    // :FD[n]#    Set focuser temperature compensation deadband amount (in steps or microns)
    //            Return: 0 on failure
    //                    1 on success
    if (toupper(command[1]) == 'D') {
      if (!setTcfDeadband(index, round(atol(parameter)*UnitsToMicrons))) *commandError = CE_PARAM_RANGE;
    } else

    // :FP#       Get focuser DC Motor Power Level (in %)
    //            Returns: n#
    // :FP[n]#    Set focuser DC Motor Power Level (in %)
    //            Return: 0 on failure
    //                    1 on success
    if (command[1] == 'P') {
      if (isDC(index)) {
        if (parameter[0] == 0) {
          sprintf(reply,"%d",(int)getDcPower(index)); *numericReply = false; 
        } else {
          int value = atol(parameter);
          if (!setDcPower(index, value)) *commandError = CE_PARAM_RANGE; 
        }
      } else *commandError = CE_CMD_UNKNOWN;
    } else

    // :FQ#       Stop the focuser
    //            Returns: Nothing
    if (command[1] == 'Q') {
      focuserAxis[index]->autoSlewStop();
      *numericReply = false;
    } else

    // :FF#       Set focuser for fast motion (1mm/s)
    //            Returns: Nothing
    if (command[1] == 'F') {
      focuserAxis[index]->setFrequencySlew(1000);
      *numericReply = false;
    } else

    // :FS#       Set focuser for slow motion (0.01mm/s)
    //            Returns: Nothing
    if (command[1] == 'S' && parameter[0] == 0) {
      focuserAxis[index]->setFrequencySlew(10);
      *numericReply = false;
    } else

    // :F[n]#     Set focuser move rate, where n = 1 for finest, 2 for 0.01mm/second, 3 for 0.1mm/second, 4 for 1mm/second
    //            Returns: Nothing
    if (command[1] >= '1' && command[1] <= '4') {
      int p[] = {1,10,100,1000};
      moveRate[index] = p[command[1] - '1'];
      *numericReply = false;
    } else

    // :F+#       Move focuser in (toward objective)
    //            Returns: Nothing
    if (command[1] == '+') {
      focuserAxis[index]->setFrequencySlew(moveRate[index]);
      focuserAxis[index]->autoSlew(DIR_FORWARD);
      *numericReply = false;
    } else

    // :F-#       Move focuser out (away from objective)
    //            Returns: Nothing
    if (command[1] == '-') {
      focuserAxis[index]->setFrequencySlew(moveRate[index]);
      focuserAxis[index]->autoSlew(DIR_REVERSE);
      *numericReply = false;
    } else

    // :FG#       Get focuser current position (in microns or steps)
    //            Returns: sn#
    if (toupper(command[1]) == 'G') {
      sprintf(reply,"%ld",(long)round(focuserAxis[index]->getInstrumentCoordinateSteps()*StepsToUnits));
      *numericReply = false;
    } else

    // :FR[sn]#   Set focuser target position relative (in microns or steps)
    //            Returns: Nothing
    if (toupper(command[1]) == 'R') {
      long t = focuserAxis[index]->getTargetCoordinateSteps();
      focuserAxis[index]->setTargetCoordinateSteps(t + atol(parameter)*UnitsToSteps);
      focuserAxis[index]->setFrequencySlew(slewRateDesired[index]);
      focuserAxis[index]->autoSlewRateByDistance(slewRateDesired[index]);
      *numericReply = false;
    } else

    // :FS[n]#    Set focuser target position (in microns or steps)
    //            Return: 0 on failure
    //                    1 on success
    if (toupper(command[1]) == 'S') {
      focuserAxis[index]->setTargetCoordinateSteps(atol(parameter)*UnitsToSteps);
      focuserAxis[index]->setFrequencySlew(slewRateDesired[index]);
      focuserAxis[index]->autoSlewRateByDistance(slewRateDesired[index]);
    //  *commandError = CE_SLEW_ERR_IN_STANDBY;
    } else

    // :FZ#       Set focuser position as zero
    //            Returns: Nothing
    if (command[1] == 'Z') {
      focuserAxis[index]->setMotorCoordinateSteps(0);
      *numericReply = false;
    } else

    // :FH#       Set focuser position as half-travel
    //            Returns: Nothing
    if (command[1] == 'H') {
      long p = round((focuserAxis[index]->settings.limits.max + focuserAxis[index]->settings.limits.min)/2.0F)*MicronsToSteps;
      focuserAxis[index]->setMotorCoordinateSteps(p);
      *numericReply = false;
    } else

    // :Fh#       Set focuser target position at half-travel
    //            Returns: Nothing
    if (command[1] == 'h') {
      long t = round((focuserAxis[index]->settings.limits.max + focuserAxis[index]->settings.limits.min)/2.0F)*MicronsToSteps;
      focuserAxis[index]->setTargetCoordinateSteps(t);
      focuserAxis[index]->setFrequencySlew(slewRateDesired[index]);
      focuserAxis[index]->autoSlewRateByDistance(slewRateDesired[index]);
      *numericReply = false;
    } else *commandError = CE_CMD_UNKNOWN;

  } else return false;

  return true;
}

#endif
