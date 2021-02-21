//--------------------------------------------------------------------------------------------------
// observatory site and time
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;
#include "../coordinates/Convert.h"
extern Convert convert;
#include "../coordinates/Transform.h"
extern Transform transform;
#include "../commands/ProcessCmds.h"
#include "Clock.h"
extern Clock clock;
#include "Mount.h"
#include "Telescope.h"

Telescope telescope;

void Telescope::init() {
  // Site
  site.latitude.value = degToRad(40.0);
  site.longitude      = degToRad(75.2);
  updateSite();

  transform.init(MOUNT_TYPE);
  clock.init();
  #if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF
    mount.init(MOUNT_TYPE);
  #endif
}

void Telescope::updateSite() {
  site.latitude.cosine = cos(site.latitude.value);
  site.latitude.sine   = sin(site.latitude.value);
  site.latitude.absval = fabs(site.latitude.value);
  if (site.latitude.value >= 0) site.latitude.sign = 1; else site.latitude.sign = -1;
  site.longitude = site.longitude;
  transform.site = site;
  clock.updateSite();
}
    
bool Telescope::command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError) {

  if (clock.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;

  #if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF
    if (mount.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;
  #endif
  
  int i;
  double value;
  PrecisionMode precisionMode = PM_LOW;

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
  } else

  // :Gt#       Get current site Latitude, positive for North latitudes
  //            Returns: sDD*MM#
  // :GtH#      Get current site Latitude, positive for North latitudes
  //            Returns: sDD*MM:SS.SSS# (high precision)
  if (cmdH("Gt")) {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(site.latitude.value), false, true, precisionMode);
    *numericReply = false;
  } else 

  // :Gg#       Get Current Site Longitude, east is negative
  //            Returns: sDDD*MM#
  // :GgH#      Get current site Longitude
  //            Returns: sDD*MM:SS.SSS# (high precision)
  if (cmdH("Gg"))  {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(site.longitude), true, true, precisionMode);
    *numericReply = false;
  } else 

  //  :St[sDD*MM]# or :St[sDD*MM:SS]# or :St[sDD*MM:SS.SSS]#
  //            Set current site latitude
  //            Return: 0 failure, 1 success
  if (cmdP("St"))  {
    if (convert.dmsToDouble(&value, parameter, true)) {
      site.latitude.value = degToRad(value);
      updateSite();
    } else *commandError = CE_PARAM_FORM;
  } else 
    
  //  :Sg[(s)DDD*MM]# or :Sg[(s)DDD*MM:SS]# or :Sg[(s)DDD*MM:SS.SSS]#
  //            Set current site longitude, east longitudes can be negative or > 180 degrees
  //            Return: 0 failure, 1 success
  if (cmdP("Sg"))  {
    if (parameter[0] == '-' || parameter[0] == '+') i = 1; else i = 0;
    if (convert.dmsToDouble(&value, (char *)&parameter[i], false)) {
      if (parameter[0] == '-') site.longitude = -site.longitude;
      if (value >= -180.0 && value <= 360.0) {
        if (value >= 180.0) value -= 360.0;
        site.longitude = degToRad(value);
        updateSite(); 
        // nv.writeFloat(EE_sites+currentSite*25+4,longitude);
      } else *commandError = CE_PARAM_RANGE;
    } else *commandError = CE_PARAM_FORM;
  } else return false;

  return true;
}

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF
  void mountMonitorWrapper() {
    telescope.mount.monitor();
  }
#endif
#if AXIS3_DRIVER_MODEL != OFF
  void rotatorMonitorWrapper() {
    telescope.rotator.monitor();
  }
#endif
#if AXIS4_DRIVER_MODEL != OFF
  void focuser1MonitorWrapper() {
    telescope.focuser1.monitor();
  }
#endif
#if AXIS5_DRIVER_MODEL != OFF
  void focuser1MonitorWrapper() {
    telescope.focuser2.monitor();
  }
#endif
#if AXIS6_DRIVER_MODEL != OFF
  void focuser1MonitorWrapper() {
    telescope.focuser3.monitor();
  }
#endif
