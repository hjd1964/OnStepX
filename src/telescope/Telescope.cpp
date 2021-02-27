//--------------------------------------------------------------------------------------------------
// OnStepX telescope control
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;

#include "../commands/ProcessCmds.h"
#include "../mount/Mount.h"
#include "Telescope.h"

Telescope telescope;

void Telescope::init() {
  #if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF
    mount.init();
  #endif
}

bool Telescope::command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError) {

  #if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF
    if (mount.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
  #endif
  
  // :GVD#      Get OnStepX Firmware Date
  //            Returns: MTH DD YYYY#
  // :GVM#      General Message
  //            Returns: s# (where s is a string up to 16 chars)
  // :GVN#      Get OnStepX Firmware Number
  //            Returns: M.mp#
  // :GVP#      Get OnStepX Product Name
  //            Returns: s#
  // :GVT#      Get OnStepX Firmware Time
  //            Returns: HH:MM:SS#
  if (cmdP("GV")) {
    if (parameter[0] == 'D') strcpy(reply,FirmwareDate); else
    if (parameter[0] == 'M') sprintf(reply,"%s %i.%02i%s",FirmwareName, FirmwareVersionMajor, FirmwareVersionMinor, FirmwareVersionPatch); else
    if (parameter[0] == 'N') sprintf(reply,"%i.%02i%s", FirmwareVersionMajor, FirmwareVersionMinor, FirmwareVersionPatch); else
    if (parameter[0] == 'P') strcpy(reply,FirmwareName); else
    if (parameter[0] == 'T') strcpy(reply,FirmwareTime); else *commandError = CE_CMD_UNKNOWN;
    *numericReply = false;
    return *commandError;
  } else return false;

  return true;
}
