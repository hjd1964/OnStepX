//--------------------------------------------------------------------------------------------------
// telescope mount control, commands
#include "../OnStepX.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../tasks/OnTask.h"
extern Tasks tasks;

#include "../coordinates/Convert.h"
#include "../coordinates/Transform.h"
#include "../coordinates/Site.h"
#include "../commands/ProcessCmds.h"
#include "../motion/StepDrivers.h"
#include "../motion/Axis.h"
#include "Mount.h"

extern unsigned long periodSubMicros;

bool Mount::commandGuide(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  *supressFrame = false;
  
  // :GX90#     Get setting pulse guide rate
  //            Returns: n.nn#
  if (cmd2("GX90")) {
    sprintF(reply,"%0.2f",guideRateSelectToRate(misc.pulseGuideRateSelect));
  } else

  // M - Telescope Movement (Guiding) Commands
  if (command[0] == 'M') {

    // :Mgd[n]#   Pulse guide command where n is the guide time in milliseconds
    //            Returns: Nothing
    // :MGd[n]#   Pulse guide command where n is the guide time in milliseconds
    //            Return: 0 on failure
    //                    1 on success
    if (command[1] == 'g' || command[1] == 'G') {
      int16_t timeMs;
      if (transform.site.convert.atoi2(&parameter[1], &timeMs)) {
        if (timeMs >= 0 && timeMs <= 16399) {
          GuideRateSelect rateSelect = guideRateSelect;
          if (SEPARATE_PULSE_GUIDE_RATE == ON) rateSelect = misc.pulseGuideRateSelect;
          if (parameter[0] == 'w') {
            *commandError = guideStartAxis1(GA_FORWARD, rateSelect, timeMs);
          } else
          if (parameter[0] == 'e') {
            *commandError = guideStartAxis1(GA_REVERSE, rateSelect, timeMs);
          } else
          if (parameter[0] == 'n') {
            *commandError = guideStartAxis2(GA_FORWARD, rateSelect, timeMs);
          } else
          if (parameter[0] == 's') { 
            *commandError = guideStartAxis2(GA_REVERSE, rateSelect, timeMs);
          } else *commandError = CE_CMD_UNKNOWN;
          if (command[1] == 'g') *numericReply = false;
        } else *commandError = CE_PARAM_RANGE;
      } else *commandError = CE_PARAM_FORM;
    } else

    // ::Mw#      Move Telescope West at current guide rate
    //            Returns: Nothing
    if (command[1] == 'w' && parameter[0] == 0) {
      *commandError = guideStartAxis1(GA_FORWARD, guideRateSelect, GUIDE_TIME_LIMIT*1000);
      *numericReply = false;
    } else
    // :Me#       Move Telescope East at current guide rate
    //            Returns: Nothing
    if (command[1] == 'e' && parameter[0] == 0) {
      *commandError = guideStartAxis1(GA_REVERSE, guideRateSelect, GUIDE_TIME_LIMIT*1000);
      *numericReply = false;
    } else
    // :Mn#       Move Telescope North at current guide rate
    //            Returns: Nothing
    if (command[1] == 'n' && parameter[0] == 0) {
      *commandError = guideStartAxis2(GA_FORWARD, guideRateSelect, GUIDE_TIME_LIMIT*1000);
      *numericReply = false;
    } else
    // :Ms#       Move Telescope South at current guide rate
    //            Returns: Nothing
    if (command[1] == 's' && parameter[0] == 0) {
      *commandError = guideStartAxis2(GA_REVERSE, guideRateSelect, GUIDE_TIME_LIMIT*1000);
      *numericReply = false;
    } else
    // :Mp#       Move Telescope for sPiral search at current guide rate
    //            Returns: Nothing
    if (command[1] == 'p' && parameter[0] == 0) {
      *commandError = guideSpiralStart(guideRateSelect, GUIDE_SPIRAL_TIME_LIMIT*1000);
      *numericReply = false;
    } else return false;
  } else

  // Q - Movement Commands
  // :Q#        Halt all slews, stops goto
  //            Returns: Nothing
  if (command[0] == 'Q') {
    if (command[1] == 0) {
        gotoStop();
        guideStopAxis1(GA_BREAK);
        guideStopAxis2(GA_BREAK);
        *numericReply = false; 
    } else
    // :Qe# Qw#   Halt east/westward Slews
    //            Returns: Nothing
    if ((command[1] == 'e' || command[1] == 'w') && parameter[0] == 0) {
      guideStopAxis1(GA_BREAK);
      *numericReply = false;
    } else
    // :Qn# Qs#   Halt north/southward Slews
    //            Returns: Nothing
    if ((command[1] == 'n' || command[1] == 's') && parameter[0] == 0) {
      guideStopAxis2(GA_BREAK);
      *numericReply = false;
    } else return false;
  } else

  // R - Guide Rate Commands
  if (command[0] == 'R') {

    // :RA[n.n]#  Set Axis1 Guide rate to n.n degrees per second
    //            Returns: Nothing
    if (command[1] == 'A') {
      char* conv_end;
      float f = strtod(parameter, &conv_end);
      if (&parameter[0] != conv_end) {
        float maxDegsPerSec = radToDeg(radsPerSecondCurrent);
        if (f < 0.001/3600.0) f = 0.001/3600.0;
        if (f > maxDegsPerSec) f = maxDegsPerSec;
        customGuideRateAxis1 = f*240.0;
        guideRateSelectAxis1 = GR_CUSTOM;
      }
      *numericReply=false; 
    } else

    // :RE[n.n]#  Set Axis2 Guide rate to n.n degrees per second
    //            Returns: Nothing
    if (command[1] == 'E') {
      char* conv_end;
      float f = strtod(parameter, &conv_end);
      if (&parameter[0] != conv_end) {
        float maxDegsPerSec = radToDeg(radsPerSecondCurrent);
        if (f < 0.001/3600.0) f = 0.001/3600.0;
        if (f > maxDegsPerSec) f = maxDegsPerSec;
        customGuideRateAxis2 = f*240.0;
        guideRateSelectAxis2 = GR_CUSTOM;
      }
      *numericReply = false;
    } else

    // :RG#       Set guide rate: Guiding        1X
    // :RC#       Set guide rate: Centering      8X
    // :RM#       Set guide rate: Find          20X
    // :RF#       Set guide rate: Fast          48X
    // :RS#       Set guide rate: Slew           ?X (1/2 of current goto rate)
    // :Rn#       Set guide rate to n, where n = 0..9
    //            Returns: Nothing
    if (strchr("GCMFS0123456789", command[1]) && parameter[0] == 0) {
      int r;
      if (command[1] == 'G') r = 2; else if (command[1] == 'C') r = 5; else
      if (command[1] == 'M') r = 6; else if (command[1] == 'F') r = 7; else
      if (command[1] == 'S') r = 8; else r = command[1] - '0';
      guideRateSelect = (GuideRateSelect)r;
      if (SEPARATE_PULSE_GUIDE_RATE == ON && guideRateSelect <= GR_1X) misc.pulseGuideRateSelect = guideRateSelect;
      *numericReply = false; 
    } else return false;
  } else return false;

  return true;
}

#endif