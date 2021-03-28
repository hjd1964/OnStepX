//--------------------------------------------------------------------------------------------------
// OnStepX telescope control
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../lib/nv/NV.h"
extern NVS nv;
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;

#include "../commands/ProcessCmds.h"
#include "../mount/Mount.h"
#include "Telescope.h"

Telescope telescope;
InitError error;

void Telescope::init() {
  if (nv.readUL(NV_KEY) != INIT_NV_KEY) {
    VF("MSG: Telescope, Wipe NV "); V(nv.size); VLF(" Bytes (please wait)");
    for (int i = 0; i < nv.size; i++) nv.write(i, (char)0);
    while (!nv.committed()) nv.poll();

    nv.write(NV_KEY, (uint32_t)INIT_NV_KEY);

    VLF("MSG: Telescope, NV reset to defaults");
  } else VLF("MSG: Telescope, correct NV key found");

  #if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF
    mount.init();
  #endif
}

bool Telescope::command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError) {

  #if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF
    if (mount.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (mount.commandGoto(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (mount.commandGuide(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (mount.commandLimit(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (mount.commandPec(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
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
    if (parameter[0] == 'M') sprintf(reply,"%s %i.%02i%s", FirmwareName, FirmwareVersionMajor, FirmwareVersionMinor, FirmwareVersionPatch); else
    if (parameter[0] == 'N') sprintf(reply,"%i.%02i%s", FirmwareVersionMajor, FirmwareVersionMinor, FirmwareVersionPatch); else
    if (parameter[0] == 'P') strcpy(reply,FirmwareName); else
    if (parameter[0] == 'T') strcpy(reply,FirmwareTime); else *commandError = CE_CMD_UNKNOWN;
    *numericReply = false;
  } else

  if (cmdGX("GX9")) {

    // :GX9A#     temperature in deg. C
    //            Returns: +/-n.n
    if (parameter[1] == 'A') {
      sprintF(reply, "%3.1f", ambient.temperature);
      *numericReply = false;
     } else

    // :GX9B#     pressure in mb
    //            Returns: +/-n.n
    if (parameter[1] == 'B') {
      sprintF(reply, "%3.1f", ambient.pressure);
      *numericReply = false;
    } else

    // :GX9C#     relative humidity in %
    //            Returns: +/-n.n
    if (parameter[1] == 'C') {
      sprintF(reply, "%3.1f", ambient.humidity);
      *numericReply = false;
    } else

    // :GX9D#     altitude in meters
    //            Returns: +/-n.n
    if (parameter[1] == 'D') {
      sprintF(reply, "%3.1f", ambient.altitude);
      *numericReply = false;
    } else

    // :GX9E#     dew point in deg. C
    //            Returns: +/-n.n
    if (parameter[1] == 'E') {
      sprintF(reply, "%3.1f", dewPoint(ambient));
      *numericReply = false;
    } else return false;
  } else
 
  if (cmdSX("SX9")) {
    char *conv_end;
    float f = strtod(&parameter[3], &conv_end);
    if (&parameter[3] == conv_end) f = NAN;
    // :SX9A,[sn.n]#
    //            Set temperature in deg. C
    //            Return: 0 failure, 1 success
    if (parameter[1] == 'A') {
      if (f >= -100.0 && f < 100.0) ambient.temperature = f; else *commandError = CE_PARAM_RANGE;
    } else
    // :SX9B,[n.n]#
    //            Set pressure in mb
    //            Return: 0 failure, 1 success
    if (parameter[1] == 'B') {
      if (f >= 500.0 && f < 1500.0) ambient.pressure = f; else *commandError = CE_PARAM_RANGE;
    } else
    // :SX9C,[n.n]#
    //            relative humidity in %
    //            Return: 0 failure, 1 success
    if (parameter[1] == 'C') {
      if (f >= 0.0 && f < 100.0) ambient.humidity = f; else *commandError = CE_PARAM_RANGE;
    } else
    // :SX9D,[sn.n]#
    //            altitude
    //            Return: 0 failure, 1 success
    if (parameter[1] == 'D') {
      if (f >= -100.0 && f < 20000.0) ambient.altitude = f; else *commandError = CE_PARAM_RANGE;
    } else return false;
  } else return false;

  return true;
}
