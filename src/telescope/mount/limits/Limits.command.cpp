//--------------------------------------------------------------------------------------------------
// telescope mount limits commands

#include "Limits.h"

#ifdef MOUNT_PRESENT

#include "../../../lib/tasks/OnTask.h"
#include "../../../lib/nv/Nv.h"

#include "../../Telescope.h"
#include "../Mount.h"
#include "../site/Site.h"

bool Limits::command(char *reply, char *command, char *parameter, bool *suppressFrame, bool *numericReply, CommandError *commandError) {
  *suppressFrame = false;
  
  if (command[0] == 'G') {
    // :Gh#       Get Horizon Limit, the minimum elevation of the mount relative to the horizon
    //            Returns: sDD*#
    if (command[1] == 'h' && parameter[0] == 0) {
      sprintf(reply, "%+02ld*", lroundf(radToDegF(settings.altitude.min)));
      *numericReply = false;
    } else

    // :Go#       Get Overhead Limit
    //            Returns: DD*#
    //            The highest elevation above the horizon that the telescope will goto
    if (command[1] == 'o' && parameter[0] == 0) {
      sprintf(reply, "%02ld*", lroundf(radToDegF(settings.altitude.max)));
      *numericReply=false;
    } else

    // :GXE[m]#   Get Other Limit [m]
    //            Returns: n#
    if (command[1] == 'X' && parameter[0] == 'E' && parameter[2] == 0) {
      *numericReply = false;
      switch (parameter[1]) {
        case '9': sprintf(reply, "%ld",lroundf(radToDegF(settings.pastMeridianE)*4.0F)); break; // minutes past meridianE
        case 'A': sprintf(reply, "%ld",lroundf(radToDegF(settings.pastMeridianW)*4.0F)); break; // minutes past meridianW
        case 'e': sprintf(reply, "%ld",lroundf(radToDegF(axis1.getLimitMin()))); break;         // RA east or -Az limit, in degrees
        case 'w': sprintf(reply, "%ld",lroundf(radToDegF(axis1.getLimitMax()))); break;         // RA west or +Az limit, in degrees
        case 'B': sprintf(reply, "%ld",lroundf(radToDegF(axis1.getLimitMax())/15.0F)); break;   // RA west or +Az limit, in hours
        case 'C': sprintf(reply, "%ld",lroundf(radToDegF(axis2.getLimitMin()))); break;         // Dec south or -Alt limit, in degrees
        case 'D': sprintf(reply, "%ld",lroundf(radToDegF(axis2.getLimitMax()))); break;         // Dec north or +Alt limit, in degrees
        // Sync limits in degrees: H = maximum |Dec| (90 = OFF), I/J = axis1/2 (0 = OFF).
        case 'H': sprintF(reply, "%0.6f", getSyncPoleLimit()); break;
        case 'I': sprintF(reply, "%0.6f", syncSettings.axis1 == OFF ? 0.0F : syncSettings.axis1); break;
        case 'J': sprintF(reply, "%0.6f", syncSettings.axis2 == OFF ? 0.0F : syncSettings.axis2); break;
        default: return false;
      }
    } else return false;
  } else
  
  if (command[0] == 'S') {
    //  :Sh[sDD]#
    //            Set the elevation lower limit
    //            Return: 0 on failure
    //                    1 on success
    if (command[1] == 'h') {
      int16_t deg;
      if (convert.atoi2(parameter, &deg)) {
        if (deg >= -30.0F && deg <= 30.0F) {
          settings.altitude.min = degToRadF(deg);
          nv().kv().put(nvKey, settings);
        } else *commandError = CE_PARAM_RANGE;
      } else *commandError = CE_PARAM_FORM;
    } else

    //  :So[DD]#
    //            Set the overhead elevation limit in degrees relative to the horizon
    //            Return: 0 on failure
    //                    1 on success
    if (command[1] == 'o') {
      int16_t deg;
      if (convert.atoi2(parameter, &deg)) {
        if (deg >= 60.0F && deg <= 90.0F) {
          settings.altitude.max = degToRadF(deg);
          nv().kv().put(nvKey, settings);
        } else *commandError = CE_PARAM_RANGE;
      } else *commandError = CE_PARAM_FORM;
    } else

    // :SXE[m],n# Set Other Limit [m]
    //           Return: 0 on failure
    //                   1 on success
    if (command[1] == 'X' && parameter[0] == 'E') {
      if (parameter[2] != ',') { *commandError = CE_PARAM_FORM; return true; }
      double degrees;
      if (!convert.atof2(&parameter[3], &degrees)) { *commandError = CE_PARAM_FORM; return true; }
      switch (parameter[1]) {
        case 'A': // west meridian limit in minutes
          degrees /= 4.0;
          if (degrees >= -360.0F && degrees <= 360.0F) {
            settings.pastMeridianW = degToRadF(degrees);
            nv().kv().put(nvKey, settings);
          } else *commandError = CE_PARAM_RANGE;
        break;
        case '9': // east meridian limit in minutes
          degrees /= 4.0;
          if (degrees >= -360.0F && degrees <= 360.0F) {
            settings.pastMeridianE = degToRadF(degrees);
            nv().kv().put(nvKey, settings);
          } else *commandError = CE_PARAM_RANGE;
        break;
        case 'H': // maximum |Dec| for Sync/align: [60,90] degrees, 90 = OFF
          if (degrees >= 60.0 && degrees <= 90.0) {
            SyncLimitSettings updated = syncSettings;
            updated.pole = degrees == 90.0 ? OFF : degrees;
            if (nv().kv().put(syncNvKey, updated) == KvPartition::Status::Ok) {
              syncSettings = updated;
            } else *commandError = CE_0;
          } else *commandError = CE_PARAM_RANGE;
        break;
        case 'I': // axis1 sync limit: [0,30] degrees, 0 = OFF
          if (degrees >= 0.0 && degrees <= 30.0) {
            SyncLimitSettings updated = syncSettings;
            updated.axis1 = degrees == 0.0 ? OFF : degrees;
            if (nv().kv().put(syncNvKey, updated) == KvPartition::Status::Ok) {
              syncSettings = updated;
            } else *commandError = CE_0;
          } else *commandError = CE_PARAM_RANGE;
        break;
        case 'J': // axis2 sync limit: [0,30] degrees, 0 = OFF
          if (degrees >= 0.0 && degrees <= 30.0) {
            SyncLimitSettings updated = syncSettings;
            updated.axis2 = degrees == 0.0 ? OFF : degrees;
            if (nv().kv().put(syncNvKey, updated) == KvPartition::Status::Ok) {
              syncSettings = updated;
            } else *commandError = CE_0;
          } else *commandError = CE_PARAM_RANGE;
        break;
        default: return false;
      }
    } else return false;
  } else return false;

  return true;
}

#endif
