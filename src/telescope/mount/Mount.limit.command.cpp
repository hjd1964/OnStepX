//--------------------------------------------------------------------------------------------------
// telescope mount control, commands

#include "Mount.h"

#ifdef MOUNT_PRESENT

#include "../../tasks/OnTask.h"
extern Tasks tasks;
#include "site/Site.h"

extern unsigned long periodSubMicros;

bool Mount::commandLimit(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  *supressFrame = false;
  
  // :Gh#       Get Horizon Limit, the minimum elevation of the mount relative to the horizon
  //            Returns: sDD*#
  if (cmd("Gh"))  { sprintf(reply,"%+02ld*", lroundf(radToDegF(limits.altitude.min))); *numericReply = false; } else

  // :Go#       Get Overhead Limit
  //            Returns: DD*#
  //            The highest elevation above the horizon that the telescope will goto
  if (cmd("Go"))  { sprintf(reply,"%02ld*", lroundf(radToDegF(limits.altitude.max))); *numericReply=false; } else

  // :GXE[L]#   Get Other Limit [L]
  //            Returns: n#
  if (cmdGX("GXE")) {
    *numericReply = false;
    switch (parameter[1]) {
      case '9': sprintf(reply,"%ld",lroundf(radToDegF(limits.pastMeridianE)*4.0F)); break;       // minutes past meridianE
      case 'A': sprintf(reply,"%ld",lroundf(radToDegF(limits.pastMeridianW)*4.0F)); break;       // minutes past meridianW
      case 'e': sprintf(reply,"%ld",lroundf(radToDegF(axis1.settings.limits.min))); break;       // RA east or -Az limit, in degrees
      case 'w': sprintf(reply,"%ld",lroundf(radToDegF(axis1.settings.limits.max))); break;       // RA west or +Az limit, in degrees
      case 'B': sprintf(reply,"%ld",lroundf(radToDegF(axis1.settings.limits.max)/15.0F)); break; // RA west or +Az limit, in hours
      case 'C': sprintf(reply,"%ld",lroundf(radToDegF(axis2.settings.limits.min))); break;       // Dec south or -Alt limit, in degrees
      case 'D': sprintf(reply,"%ld",lroundf(radToDegF(axis2.settings.limits.max))); break;       // Dec north or +Alt limit, in degrees
      default: return false;
    }
  } else
  
  //  :Sh[sDD]#
  //            Set the elevation lower limit
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("Sh")) {
    int16_t deg;
    if (convert.atoi2(parameter, &deg)) {
      if (deg >= -30.0F && deg <= 30.0F) {
        limits.altitude.min = degToRadF(deg);
        nv.updateBytes(NV_MOUNT_LIMITS_BASE, &limits, LimitsSize);
      } else *commandError = CE_PARAM_RANGE;
    } else *commandError = CE_PARAM_FORM;
  } else

  //  :So[DD]#
  //            Set the overhead elevation limit in degrees relative to the horizon
  //            Return: 0 on failure
  //                    1 on success
  if (cmdP("So"))  {
    int16_t deg;
    if (convert.atoi2(parameter, &deg)) {
      if (deg >= 60.0F && deg <= 90.0F) {
        limits.altitude.max = degToRadF(deg);
        if (transform.mountType == ALTAZM && limits.altitude.max > 87) limits.altitude.max = 87;
        nv.updateBytes(NV_MOUNT_LIMITS_BASE, &limits, LimitsSize);
      } else *commandError = CE_PARAM_RANGE;
    } else *commandError = CE_PARAM_FORM;
  } else

  //  :SXE9,[M]#
  //  :SXEA,[M]#
  //            Set meridian limit east (9) or west (A) to value [M] in minutes
  //            Return: 0 on failure or 1 on success
  if (cmdSX("SXE")) {
    long l = atol(&parameter[3]); float degs = l/4.0;
    switch (parameter[1]) {
      case '9':
        if (degs >= -270.0F && degs <= 270.0F) {
          limits.pastMeridianE = degToRadF(degs);
          if (limits.pastMeridianE < -axis1.settings.limits.max) limits.pastMeridianE = -axis1.settings.limits.max;
          if (limits.pastMeridianE > -axis1.settings.limits.min) limits.pastMeridianE = -axis1.settings.limits.min;
          nv.updateBytes(NV_MOUNT_LIMITS_BASE, &limits, LimitsSize);
        } else *commandError = CE_PARAM_RANGE;
      break;
      case 'A':
        if (degs >= -270.0F && degs <= 270.0F) {
          limits.pastMeridianW = degToRadF(degs);
          if (limits.pastMeridianW < axis1.settings.limits.min) limits.pastMeridianW = axis1.settings.limits.min;
          if (limits.pastMeridianW > axis1.settings.limits.max) limits.pastMeridianW = axis1.settings.limits.max;
          nv.updateBytes(NV_MOUNT_LIMITS_BASE, &limits, LimitsSize);
        } else *commandError = CE_PARAM_RANGE;
      break;
      default: return false;
    }
  } else return false;

  return true;

}

#endif