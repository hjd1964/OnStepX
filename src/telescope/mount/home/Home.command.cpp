//--------------------------------------------------------------------------------------------------
// telescope mount control, home commands

#include "Home.h"

#ifdef MOUNT_PRESENT

#include "../../../lib/nv/Nv.h"

#include "../limits/Limits.h"
#include "../park/Park.h"
#include "../site/Site.h"

bool Home::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  UNUSED(reply);
  UNUSED(supressFrame);
  if (command[0] == 'h') {
    // :h?#       Get home status - has sense, auto home enabled, offset axis1, offset axis2 (in arcseconds.)
    //            Returns: n,n,n,n#
    if (command[1] == '?' && parameter[0] == 0) {
      sprintf(reply, "%d,%ld,%ld",  (int)hasSense, settings.axis1.senseOffset, settings.axis2.senseOffset);
      *numericReply = false;
    } else

    // :hAn#      Set auto home state.
    //            Returns: Nothing
    if (command[1] == 'A' && parameter[1] == 0) {
      switch (parameter[0]) {
        case '0': settings.automaticAtBoot = false; break;
        case '1': settings.automaticAtBoot = true; break;
        default: *commandError = CE_PARAM_RANGE; break;
      }
      nv.writeBytes(NV_MOUNT_HOME_BASE, &settings, sizeof(Settings));
      *numericReply = false;
    } else

    // :hC#       Moves mount to the home position
    //            Returns: Nothing
    if (command[1] == 'C' && parameter[0] == 0) {
      *commandError = request();
      *numericReply = false;
    } else

    // :hC1,n#    Set home sense direction and home offset for axis1 arcseconds.
    //            Returns: Nothing
    if (command[1] == 'C' && parameter[0] == '1' && parameter[1] == ',') {
      if (parameter[2] == 'R' && parameter[3] == 0) {
        settings.axis1.senseReverse = !settings.axis1.senseReverse;
        V("MSG: Set Axis1 home reverse "); VL(settings.axis1.senseReverse);
        setReversal();
      } else {
        long l = atol(&parameter[2]);
        if (l >= -648000 || l <= 648000) {
          settings.axis1.senseOffset = l;
        } else *commandError = CE_PARAM_RANGE;
      }
      nv.writeBytes(NV_MOUNT_HOME_BASE, &settings, sizeof(Settings));
      *numericReply = false;
    } else

    // :hC2,n#    Set home sense direction and home offset for axis2 arcseconds.
    //            Returns: Nothing
    if (command[1] == 'C' && parameter[0] == '2' && parameter[1] == ',') {
      if (parameter[2] == 'R' && parameter[3] == 0) {
        settings.axis2.senseReverse = !settings.axis2.senseReverse;
        V("MSG: Set Axis2 home reverse "); VL(settings.axis2.senseReverse);
        setReversal();
      } else {
        long l = atol(&parameter[2]);
        if (l >= -648000 || l <= 648000) {
          settings.axis2.senseOffset = l;
        } else *commandError = CE_PARAM_RANGE;
      }
      nv.writeBytes(NV_MOUNT_HOME_BASE, &settings, sizeof(Settings));
      *numericReply = false;
    } else

    // :hF#       Reset mount at the home position.  This position is required for a cold Start.
    //            Point to the celestial pole.  GEM w/counterweights pointing downwards (CWD position).  Equatorial fork mounts at HA = 0.
    //            Returns: Nothing
    if (command[1] == 'F' && parameter[0] == 0) {
      *commandError = reset(true);
      park.reset();
      limits.enabled(site.isDateTimeReady());
      *numericReply = false;
    } else return false;

  } else return false;

  return true;
}

#endif
