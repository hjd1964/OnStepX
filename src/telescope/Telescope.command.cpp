//--------------------------------------------------------------------------------------------------
// OnStepX telescope control

#include "../Common.h"
#include "../lib/tasks/OnTask.h"

#include "../lib/convert/Convert.h"
#include "../libApp/commands/ProcessCmds.h"
#include "../libApp/weather/Weather.h"
#include "Telescope.h"

#include "addonFlasher/AddonFlasher.h"
#include "mount/Mount.h"
#include "mount/goto/Goto.h"
#include "mount/guide/Guide.h"
#include "mount/home/Home.h"
#include "mount/library/Library.h"
#include "mount/limits/Limits.h"
#include "mount/park/Park.h"
#include "mount/pec/Pec.h"
#include "mount/site/Site.h"
#include "mount/status/Status.h"
#include "rotator/Rotator.h"
#include "focuser/Focuser.h"
#include "auxiliary/Features.h"

// possible reset function for this MCU
#ifdef HAL_RESET_FUNC
  HAL_RESET_FUNC;
#endif

bool Telescope::command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError) {

  #ifdef MOUNT_PRESENT
    if (mount.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (guide.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (gpio.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (mountStatus.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (goTo.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (park.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (library.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (site.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (limits.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (home.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (pec.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (axis1.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
    if (axis2.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
  #endif

  #ifdef ROTATOR_PRESENT
    if (rotator.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
  #endif

  #ifdef FOCUSER_PRESENT
    if (focuser.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
  #endif

  #ifdef FEATURES_PRESENT
    if (features.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
  #endif

  //  B - Reticle/Accessory Control
  // :B+#       Increase reticle Brightness
  //            Returns: Nothing
  // :B-#       Decrease Reticle Brightness
  //            Returns: Nothing
  if (command[0] == 'B' && (command[1] == '+' || command[1] == '-') && parameter[0] == 0)  {
    #if RETICLE_LED_DEFAULT >= 0 && RETICLE_LED_PIN != OFF
      int scale;
      if (reticleBrightness > 255 - 8) scale = 1; else
      if (reticleBrightness > 255 - 32) scale = 4; else
      if (reticleBrightness > 255 - 64) scale = 12; else
      if (reticleBrightness > 255 - 128) scale = 32; else scale = 64;
      if (command[1] == '-') reticleBrightness += scale;
      if (reticleBrightness > 255) reticleBrightness = 255;
      if (command[1] == '+') reticleBrightness -= scale;
      if (reticleBrightness < 0)   reticleBrightness = 0;
      analogWrite(RETICLE_LED_PIN, analog8BitToAnalogRange(RETICLE_LED_INVERT == ON ? 255 - reticleBrightness : reticleBrightness));
      #if RETICLE_LED_MEMORY == ON
        nv.write(NV_TELESCOPE_SETTINGS_BASE, reticleBrightness);
      #endif
    #endif
    *numericReply = false;
  } else 

  //  E - Enter special mode
  if (command[0] == 'E') {
    // :EC[s]# Echo string [s] as a debug message
    //            Return: Nothing
    if (command[1] == 'C') {
      // spaces are encoded as '_'
      for (unsigned int i = 0; i < strlen(parameter); i++) if (parameter[i] == '_') parameter[i] = ' ';
      // a newline is encoded as '&' in the last char of message
      int l = strlen(parameter);
      if (l > 0 && parameter[l - 1] == '&') { parameter[l - 1] = 0; DL(parameter); } else { D(parameter); }
      *numericReply = false;
    } else

    // :ERESET#   Reset the MCU
    //            Returns: Nothing
    if (command[1] == 'R' && parameter[0] == 'E' && parameter[1] == 'S' && parameter[2] == 'E' && parameter[3] == 'T' && parameter[4] == 0) {
      #ifdef HAL_RESET
        nv.wait();
        tasks.yield(1000);
        HAL_RESET();
      #endif
      *numericReply = false;
    } else

    // :ENVRESET# Wipe flash
    if (command[1] == 'N' && parameter[0] == 'V' && parameter[1] == 'R' && parameter[2] == 'E' && parameter[3] == 'S' && parameter[4] == 'E' && parameter[5] == 'T' && parameter[6] == 0) {
      nv.writeKey(0);
      strcpy(reply, "NV memory will be cleared on the next boot.");
      *numericReply = false;
    } else

    // :ESPFLASH# ESP8266 device flash mode
    //            Return: 1 on completion (after up to one minute from start of command.)
    #if SERIAL_B_ESP_FLASHING == ON
      if (command[1] == 'S' && parameter[0] == 'P' && parameter[1] == 'F' && parameter[2] == 'L' && parameter[3] == 'A' && parameter[4] == 'S' && parameter[5] == 'H' && parameter[6] == 0) {
        SERIAL_A.println("The SWS microcontroller will now be placed in flash upload mode (at 115200 Baud.)");
        SERIAL_A.println("Arduino's 'Tools -> Upload Speed' should be set to 115200 Baud.");
        SERIAL_A.println("Waiting for data, you have one minute to start the upload.");
        tasks.yield(1000);
        addonFlasher.go();
        SERIAL_A.println("SWS microcontroller reset and in run mode, resuming operation...");
        tasks.yield(1000);
      } else
    #elif ADDON_SELECT_PIN != OFF
      if (command[1] == 'S' && parameter[0] == 'P' && parameter[1] == 'F' && parameter[2] == 'L' && parameter[3] == 'A' && parameter[4] == 'S' && parameter[5] == 'H' && parameter[6] == 0) {
        SERIAL_A.println("The SWS microcontroller will now be connected to USB for flashing firmware.");
        SERIAL_A.println("Once done uploading power cycle or reboot to restore normal operation.");
        tasks.yield(1000);
        pinModeEx(ADDON_SELECT_PIN, OUTPUT);
        digitalWriteEx(ADDON_SELECT_PIN, LOW);
      } else
    #endif
    *commandError = CE_CMD_UNKNOWN;
  } else

  if (command[0] == 'G') {
    // :GVD#      Get OnStepX Firmware Date
    //            Returns: MTH DD YYYY#
    // :GVM#      General Message
    //            Returns: s# (where s is a string up to 16 chars)
    // :GVN#      Get Firmware Number
    //            Returns: M.mp#
    // :GVP#      Get Product Name
    //            Returns: s#
    // :GVT#      Get Firmware Time
    //            Returns: HH:MM:SS#
    // :GVC#      Get Firmware Config Name
    //            Returns: s#
    if (command[1] == 'V' && parameter[1] == 0) {
      if (parameter[0] == 'D') strcpy(reply, firmware.date); else
      if (parameter[0] == 'M') sprintf(reply, "%s %i.%02i%s", firmware.name, firmware.version.major, firmware.version.minor, firmware.version.patch); else
      if (parameter[0] == 'N') sprintf(reply, "%i.%02i%s", firmware.version.major, firmware.version.minor, firmware.version.patch); else
      if (parameter[0] == 'P') strcpy(reply, firmware.name); else
      if (parameter[0] == 'T') strcpy(reply, firmware.time); else
      if (parameter[0] == 'C') strncpy(reply, CONFIG_NAME, 40); else *commandError = CE_CMD_UNKNOWN;
      *numericReply = false;
    } else

    if (command[1] == 'X' && parameter[2] == 0) {
      if (parameter[0] == '9') {
        // :GX9A#     temperature in deg. C
        //            Returns: +/-n.n
        if (parameter[1] == 'A') {
          sprintF(reply, "%3.1f", weather.getTemperature());
          *numericReply = false;
        } else

        // :GX9B#     pressure in mb
        //            Returns: +/-n.n
        if (parameter[1] == 'B') {
          sprintF(reply, "%3.1f", weather.getPressure());
          *numericReply = false;
        } else

        // :GX9C#     relative humidity in %
        //            Returns: +/-n.n
        if (parameter[1] == 'C') {
          sprintF(reply, "%3.1f", weather.getHumidity());
          *numericReply = false;
        } else

        // :GX9E#     dew point in deg. C
        //            Returns: +/-n.n
        if (parameter[1] == 'E') {
          sprintF(reply, "%3.1f", weather.getDewPoint());
          *numericReply = false;
        } else

        // :GX9F#     MCU temperature in deg. C
        //            Returns: +/-n.n
        if (parameter[1] == 'F') {
          if (!isnan(telescope.mcuTemperature)) {
            sprintF(reply, "%1.0f", telescope.mcuTemperature);
            *numericReply = false;
          } else {
            *numericReply = true;
            *commandError = CE_0;
          }
        } else return false;
      } else

      if (parameter[0] == 'A') {
        // :GXA0#     Get axis/driver revert all state
        //            Returns: Value
        if (parameter[1] == '0') {
          uint16_t axesToRevert = nv.readUI(NV_AXIS_SETTINGS_REVERT);
          if (!(axesToRevert & 1)) *commandError = CE_0;
        } else return false;
      } else return false;

    } else return false;
  } else

  if (command[0] == 'S' && command[1] == 'X' && parameter[2] == ',') {
    if (parameter[0] == '9') {
      char *conv_end;
      float f = strtod(&parameter[3], &conv_end);
      if (&parameter[3] == conv_end) f = NAN;
      // :SX9A,[sn.n]#
      //            Set temperature in deg. C
      //            Return: 0 failure, 1 success
      if (parameter[1] == 'A') {
        if (!weather.setTemperature(f)) *commandError = CE_PARAM_RANGE;
      } else
      // :SX9B,[n.n]#
      //            Set pressure in mb
      //            Return: 0 failure, 1 success
      if (parameter[1] == 'B') {
        if (!weather.setPressure(f)) *commandError = CE_PARAM_RANGE;
      } else
      // :SX9C,[n.n]#
      //            relative humidity in %
      //            Return: 0 failure, 1 success
      if (parameter[1] == 'C') {
        if (!weather.setHumidity(f)) *commandError = CE_PARAM_RANGE;
      } else return false;
    } else

    if (parameter[0] == 'A') {
      // :SXAC,0#   for run-time NV (EEPROM) axis settings
      // :SXAC,1#   for compile-time Config.h axis settings
      //            Return: 0 failure, 1 success
      if (parameter[1] == 'C' && parameter[4] == 0) {
        if (parameter[3] == '0' || parameter[3] == '1') {
          uint16_t axesToRevert = nv.readUI(NV_AXIS_SETTINGS_REVERT);
          if (parameter[3] == '0') {
             VLF("MSG: Using Axes settings from NV (EEPROM)");
             bitSet(axesToRevert, 0);
           } else {
             VLF("MSG: Using Axes settings from Config.h");
             bitClear(axesToRevert, 0);
           }
          nv.update(NV_AXIS_SETTINGS_REVERT, axesToRevert);
        } else *commandError = CE_PARAM_RANGE;
      }
    } else return false;
  } else return false;

  return true;
}
