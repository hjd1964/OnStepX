//--------------------------------------------------------------------------------------------------
// telescope mount control, commands

#include "Guide.h"

#ifdef MOUNT_PRESENT

#include "../../../lib/tasks/OnTask.h"

#include "../site/Site.h"
#include "../goto/Goto.h"

bool Guide::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  *supressFrame = false;
  
  // :GX90#     Get setting pulse guide rate
  //            Returns: n.nn#
  if (command[0] == 'G' && command[1] == 'X' && parameter[0] == '9' && parameter[1] == '0' && parameter[2] == 0) {
    sprintF(reply, "%0.2f", rateSelectToRate(settings.pulseRateSelect));
    *numericReply = false;
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
      if (convert.atoi2(&parameter[1], &timeMs)) {
        if (timeMs >= 0) {
          if (parameter[0] == 'w') {
            *commandError = startAxis1(GA_FORWARD, (GUIDE_SEPARATE_PULSE_RATE == ON) ? settings.pulseRateSelect : settings.axis1RateSelect, timeMs);
          } else
          if (parameter[0] == 'e') {
            *commandError = startAxis1(GA_REVERSE, (GUIDE_SEPARATE_PULSE_RATE == ON) ? settings.pulseRateSelect : settings.axis1RateSelect, timeMs);
          } else
          if (parameter[0] == 'n') {
            *commandError = startAxis2(GA_FORWARD, (GUIDE_SEPARATE_PULSE_RATE == ON) ? settings.pulseRateSelect : settings.axis2RateSelect, timeMs);
          } else
          if (parameter[0] == 's') {
            *commandError = startAxis2(GA_REVERSE, (GUIDE_SEPARATE_PULSE_RATE == ON) ? settings.pulseRateSelect : settings.axis2RateSelect, timeMs);
          } else *commandError = CE_CMD_UNKNOWN;
          if (command[1] == 'g') *numericReply = false;
        } else *commandError = CE_PARAM_RANGE;
      } else *commandError = CE_PARAM_FORM;
    } else

    // :Mw#       Move Telescope West at current guide rate
    //            Returns: Nothing
    if (command[1] == 'w' && parameter[0] == 0) {
      *commandError = startAxis1(GA_FORWARD, settings.axis1RateSelect, GUIDE_TIME_LIMIT*1000);
      *numericReply = false;
    } else
    // :Me#       Move Telescope East at current guide rate
    //            Returns: Nothing
    if (command[1] == 'e' && parameter[0] == 0) {
      *commandError = startAxis1(GA_REVERSE, settings.axis1RateSelect, GUIDE_TIME_LIMIT*1000);
      *numericReply = false;
    } else
    // :Mn#       Move Telescope North at current guide rate
    //            Returns: Nothing
    if (command[1] == 'n' && parameter[0] == 0) {
      *commandError = startAxis2(GA_FORWARD, settings.axis2RateSelect, GUIDE_TIME_LIMIT*1000);
      *numericReply = false;
    } else
    // :Ms#       Move Telescope South at current guide rate
    //            Returns: Nothing
    if (command[1] == 's' && parameter[0] == 0) {
      *commandError = startAxis2(GA_REVERSE, settings.axis2RateSelect, GUIDE_TIME_LIMIT*1000);
      *numericReply = false;
    } else
    // :Mp#       Move Telescope for sPiral search at current guide rate
    //            Returns: Nothing
    if (command[1] == 'p' && parameter[0] == 0) {
      *commandError = startSpiral(settings.axis1RateSelect, GUIDE_SPIRAL_TIME_LIMIT*1000);
      *numericReply = false;
    } else return false;
  } else

  // Q - Movement Commands
  // :Q#        Halt all slews, stops goto
  //            Returns: Nothing
  if (command[0] == 'Q') {
    if (command[1] == 0) {
        #if GOTO_FEATURE == ON
          goTo.stop();
        #endif
        stop();
        *numericReply = false;
    } else
    // :Qe# Qw#   Halt east/westward Slews
    //            Returns: Nothing
    if ((command[1] == 'e' || command[1] == 'w') && parameter[0] == 0) {
      stopAxis1();
      *numericReply = false;
    } else
    // :Qn# Qs#   Halt north/southward Slews
    //            Returns: Nothing
    if ((command[1] == 'n' || command[1] == 's') && parameter[0] == 0) {
      stopAxis2();
      *numericReply = false;
    } else return false;
  } else

  // R - Guide Rate Commands
  if (command[0] == 'R') {

    #if GOTO_FEATURE == ON
      float maxDegsPerSec = radToDegF(goTo.rate);
    #else
      float maxDegsPerSec = 0.083333333F;
    #endif

    // :RA[n.n]#  Set Axis1 Guide rate to n.n degrees per second
    //            Returns: Nothing
    //            Note: Tracking, if active, is suspended in this mode of operation
    if (command[1] == 'A') {
      char* conv_end;
      float f = strtod(parameter, &conv_end);
      if (&parameter[0] != conv_end) {
        if (f < 0.0001F/3600.0F) f = 0.0001F/3600.0F;
        if (f > maxDegsPerSec) f = maxDegsPerSec;
        VF("MSG: set Axis1 custom guide rate to "); V(f); VLF(" deg/s");
        settings.axis1RateSelect = GR_CUSTOM;
        customRateAxis1 = f*240.0F;
      }
      *numericReply = false;
    } else

    // :RE[n.n]#  Set Axis2 Guide rate to n.n degrees per second
    //            Returns: Nothing
    //            Note: Tracking, if active, is suspended in this mode of operation
    if (command[1] == 'E') {
      char* conv_end;
      float f = strtod(parameter, &conv_end);
      if (&parameter[0] != conv_end) {
        if (f < 0.0001F/3600.0F) f = 0.0001F/3600.0F;
        if (f > maxDegsPerSec) f = maxDegsPerSec;
        VF("MSG: set Axis2 custom guide rate to "); V(f); VLF(" deg/s");
        settings.axis2RateSelect = GR_CUSTOM;
        customRateAxis2 = f*240.0F;
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
    //            Note: Tracking, if active, is integrated in this mode of operation
    if (strchr("GCMFS0123456789", command[1]) && parameter[0] == 0) {
      int r;
      if (command[1] == 'G') r = 2; else
      if (command[1] == 'C') r = 5; else
      if (command[1] == 'M') r = 6; else
      if (command[1] == 'F') r = 7; else
      if (command[1] == 'S') r = 8; else r = command[1] - '0';
      #if GOTO_FEATURE != ON
        if (r > 6) r = 6;
      #endif
      settings.axis1RateSelect = (GuideRateSelect)r;
      settings.axis2RateSelect = (GuideRateSelect)r;
      VF("MSG: Guide rate "); V(r); VLF(" selected");
      if (GUIDE_SEPARATE_PULSE_RATE == ON && (GuideRateSelect)r <= GR_1X) {
        settings.pulseRateSelect = (GuideRateSelect)r;
        nv.updateBytes(NV_MOUNT_GUIDE_BASE, &settings, sizeof(GuideSettings));
      }
      *numericReply = false; 
    } else return false;
  } else return false;

  return true;
}

#endif