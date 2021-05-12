//--------------------------------------------------------------------------------------------------
// OnStepX telescope control
#include "../OnStepX.h"

#include "../coordinates/Convert.h"
#include "../commands/ProcessCmds.h"
#include "Telescope.h"

Telescope telescope;
InitError initError;

void Telescope::init(const char *fwName, int fwMajor, int fwMinor, char fwPatch, int fwConfig) {

  strcpy(telescope.firmware.name, fwName);
  telescope.firmware.version.major = fwMajor;
  telescope.firmware.version.minor = fwMinor;
  telescope.firmware.version.patch = fwPatch;
  telescope.firmware.version.config = fwConfig;
  strcpy(telescope.firmware.date, __DATE__);
  strcpy(telescope.firmware.time, __TIME__);

  bool validKey = true;
  if (nv.readUL(NV_KEY) != INIT_NV_KEY) {
    validKey = false;

    VF("MSG: Telescope, Wipe NV "); V(nv.size); VLF(" Bytes");
    for (int i = 0; i < nv.size; i++) nv.write(i, (char)0);
    VLF("MSG: Telescope, Wipe NV waiting for commit");
    while (!nv.committed()) { nv.poll(false); delay(10); }

    VLF("MSG: Telescope, NV reset to defaults");
  } else { VLF("MSG: Telescope, correct NV key found"); }

  #if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF
    mount.init(validKey);
  #endif

  if (!validKey) {
    while (!nv.committed()) nv.poll();
    nv.write(NV_KEY, (uint32_t)INIT_NV_KEY);
    while (!nv.committed()) { nv.poll(false); delay(10); }
    nv.ignoreCache(true);
    uint32_t key = nv.readUL(NV_KEY);
    if (key != (uint32_t)INIT_NV_KEY) { DLF("ERR: Telescope, NV reset failed to read back key!"); } else { VLF("MSG: Telescope, NV reset complete"); }
    nv.ignoreCache(false);
  }
}

bool Telescope::command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError) {

  #if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF
    if (mount.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (mount.commandGoto(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (mount.commandGuide(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (mount.commandLimit(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (mount.commandPec(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
  #endif
  
  //  E - Enter special mode
  if (command[0] == 'E') {
    // :EC[s]# Echo string [s] on DebugSer.
    //            Return: Nothing
    if (command[1] == 'C') {
      // spaces are encoded as '_'
      for (unsigned int i = 0; i < strlen(parameter); i++) if (parameter[i] == '_') parameter[i] = ' ';
      // a newline is encoded as '&' in the last char of message
      int l = strlen(parameter);
      if (l > 0 && parameter[l - 1] == '&') { parameter[l - 1] = 0; DL(parameter); } else { D(parameter); }
      *numericReply = false;
    } else

    // :ERESET#   Reset the MCU.  OnStep must be at home and tracking turned off for this command to work.
    //            Returns: Nothing
    if (command[1] == 'R' && parameter[0] == 'E' && parameter[1] == 'S' && parameter[2] == 'E' && parameter[3] == 'T' && parameter[4] == 0) {
      #ifdef HAL_RESET
        HAL_RESET;
      #endif
      *numericReply = false;
    } else

    // :ENVRESET# Wipe flash.  OnStep must be at home and tracking turned off for this command to work.
    if (command[1] == 'N' && parameter[0] == 'V' && parameter[1] == 'R' && parameter[2] == 'E' && parameter[3] == 'S' && parameter[4] == 'E' && parameter[5] == 'T' && parameter[6] == 0) {
      nv.write(NV_KEY, (uint32_t)0);
      strcpy(reply, "NV memory will be cleared on the next boot.");
      *numericReply = false;
    } else

    // :ESPFLASH# ESP8266 device flash mode.  OnStep must be at home and tracking turned off for this command to work.
    //            Return: 1 on completion (after up to one minute from start of command.)
    #if SERIAL_B_ESP_FLASHING == ON
      if (command[1] == 'S' && parameter[0] == 'P' && parameter[1] == 'F' && parameter[2] == 'L' && parameter[3] == 'A' && parameter[4] == 'S' && parameter[5] == 'H' && parameter[6] == 0) {
        SerialA.println("The ESP8266 will now be placed in flash upload mode (at 115200 Baud.)");
        SerialA.println("Arduino's 'Tools -> Upload Speed' should be set to 115200 Baud.");
        SerialA.println("Waiting for data, you have one minute to start the upload.");
        delay(1000);
        fa.go(false); // flash the addon
        SerialA.println("ESP8266 reset and in run mode, resuming OnStep operation...");
        delay(1000);
      } else
    #endif
    *commandError = CE_CMD_UNKNOWN;
  } else

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
    if (parameter[0] == 'D') strcpy(reply, firmware.date); else
    if (parameter[0] == 'M') sprintf(reply, "%s %i.%02i%c", firmware.name, firmware.version.major, firmware.version.minor, firmware.version.patch); else
    if (parameter[0] == 'N') sprintf(reply, "%i.%02i%c", firmware.version.major, firmware.version.minor, firmware.version.patch); else
    if (parameter[0] == 'P') strcpy(reply, firmware.name); else
    if (parameter[0] == 'T') strcpy(reply, firmware.time); else *commandError = CE_CMD_UNKNOWN;
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
 
  // :GXA0#     Get axis/driver revert all state
  //            Returns: Value
  if (cmdGX("GXA") && parameter[1] == '0') {
    uint16_t axesToRevert = nv.readUI(NV_REVERT_AXIS_SETTINGS);
    if (!(axesToRevert & 1)) *commandError = CE_0;
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
  } else

  if (cmdSX("SXA")) {
    // :SXAC,0#   for compile-time Config.h axis settings
    // :SXAC,1#   for run-time NV (EEPROM) axis settings
    //            Return: 0 failure, 1 success
    if (parameter[1] == 'C' && parameter[4] == 0) {
      if (parameter[3] == '0' || parameter[3] == '1') {
        uint16_t axesToRevert = nv.readUI(NV_REVERT_AXIS_SETTINGS);
        if (parameter[3] == '0') axesToRevert = 1; else axesToRevert = 0;
        nv.update(NV_REVERT_AXIS_SETTINGS, axesToRevert);
        return false; // pretend this command wasn't processed so other devices can respond
      } else *commandError = CE_PARAM_RANGE;
    }
  } else return false;

  return true;
}
