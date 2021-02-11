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
#include "../coordinates/Transform.h"
#include "../commands/ProcessCmds.h"
#include "Clock.h"
#include "Mount.h"

#include "Telescope.h"

// instantiate and callback wrapper
Clock clock;
void clockTickWrapper() { clock.tick(); }

void Telescope::init() {
  // Site
  Site newSite;
  newSite.latitude.value = degToRad(40.0);
  newSite.longitude      = degToRad(75.2);
  setSite(newSite);

  // Coordinate transformation
  transform.init(MOUNT_TYPE);

  // Clock
  clock.init(site);
  // period ms (0=idle), duration ms (0=forever), repeat, priority (highest 0..7 lowest), task_handle
  uint8_t handle = tasks.add(0, 0, true, 0, clockTickWrapper, "ClkTick");
  if (!tasks.requestHardwareTimer(handle, 3, 1)) VLF("MSG: Warning, didn't get h/w timer for Clock (using s/w timer)");
  
  tasks.setPeriodSubMicros(handle, lround(160000.0/SIDEREAL_RATIO));

  // Mount
  mount.init(MOUNT_TYPE);
}

void Telescope::setSite(Site site) {
  this->site = site;
  updateSite();
}

void Telescope::updateSite() {
  this->site.latitude.cosine = cos(site.latitude.value);
  this->site.latitude.sine   = sin(site.latitude.value);
  this->site.latitude.absval = fabs(site.latitude.value);
  if (this->site.latitude.value >= 0) this->site.latitude.sign = 1; else this->site.latitude.sign = -1;
  this->site.longitude = site.longitude;

  transform.setSite(site);
  clock.setSite(site);
}
    
bool Telescope::command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError) {

  if (clock.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;

  #if (defined(AXIS1_DRIVER_MODEL) && AXIS1_DRIVER_MODEL != OFF) && (defined(AXIS2_DRIVER_MODEL) && AXIS2_DRIVER_MODEL != OFF)
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
    if (parameter[0] == 'M') sprintf(reply,"%s %i.%i%s",FirmwareName, FirmwareVersionMajor, FirmwareVersionMinor, FirmwareVersionPatch); else
    if (parameter[0] == 'N') sprintf(reply,"%i.%i%s", FirmwareVersionMajor, FirmwareVersionMinor, FirmwareVersionPatch); else
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
  if (cmd("St"))  {
    tasks_mutex_enter(MX_CLOCK_CMD);
    if (convert.dmsToDouble(&value, parameter, true)) {
      site.latitude.value = degToRad(value);
      updateSite();
    } else *commandError = CE_PARAM_FORM;
    tasks_mutex_exit(MX_CLOCK_CMD);
  } else 
    
  //  :Sg[(s)DDD*MM]# or :Sg[(s)DDD*MM:SS]# or :Sg[(s)DDD*MM:SS.SSS]#
  //            Set current site longitude, east longitudes can be negative or > 180 degrees
  //            Return: 0 failure, 1 success
  if (cmd("Sg"))  {
    tasks_mutex_enter(MX_CLOCK_CMD);
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
    tasks_mutex_exit(MX_CLOCK_CMD);
  } else return false;

  return true;
}
