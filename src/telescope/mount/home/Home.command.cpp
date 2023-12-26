//--------------------------------------------------------------------------------------------------
// telescope mount control, home commands

#include "Home.h"

#ifdef MOUNT_PRESENT

#include "../park/Park.h"

bool Home::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  UNUSED(reply);
  UNUSED(supressFrame);
  if (command[0] == 'h') {
    // :hC#       Moves mount to the home position
    //            Returns: Nothing
    if (command[1] == 'C' && parameter[0] == 0) {
      *commandError = request();
      *numericReply = false;
    } else

    // :hF#       Reset mount at the home position.  This position is required for a cold Start.
    //            Point to the celestial pole.  GEM w/counterweights pointing downwards (CWD position).  Equatorial fork mounts at HA = 0.
    //            Returns: Nothing
    if (command[1] == 'F' && parameter[0] == 0) {
      *commandError = reset(true);
      park.reset();
      *numericReply = false;
    } else

    // :hO#       Get home offset status and offsets for axis1 and axis2 in arcseconds.
    //            Returns: n,n,n#
    if (command[1] == 'O' && parameter[0] == 0) {
      sprintf(reply, "%d,%ld,%ld", useOffset(), senseOffset.axis1, senseOffset.axis2);
      *numericReply = false;
    } else

    // :hOn,n#    Set home offsets for axis1 and axis2 in arcseconds.
    //            Returns: Nothing
    if (command[1] == 'O' && parameter[0] != 0) {
      if (!sscanf(parameter, "%ld,%ld" ,senseOffset.axis1, senseOffset.axis2)) *commandError = CE_PARAM_FORM;     
      *numericReply = false;
    } else return false; 

  } else return false;

  return true;
}

#endif
