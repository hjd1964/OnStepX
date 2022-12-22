//--------------------------------------------------------------------------------------------------
// telescope mount control, park commands

#include "Park.h"

#if defined(MOUNT_PRESENT)

bool Park::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  UNUSED(reply);
  UNUSED(supressFrame);
  UNUSED(numericReply);
  if (command[0] == 'h') {
    // :hP#       Moves mount to the park position
    //            Return: 0 on failure
    //                    1 on success
    if (command[1] == 'P' && parameter[0] == 0) {
      CommandError e = request();
      if (e == CE_NONE) *commandError = CE_1; else { VF("MSG: Mount, park error "); VL(e); *commandError = e; } 
    } else 

    // :hQ#       Set the mount park position
    //            Return: 0 on failure
    //                    1 on success
    if (command[1] == 'Q' && parameter[0] == 0) *commandError = set(); else 

    // :hR#       Restore parked mount to operation
    //            Return: 0 on failure
    //                    1 on success
    if (command[1] == 'R' && parameter[0] == 0) {
      CommandError e = restore(true);
      if (e == CE_NONE) *commandError = CE_1; else { VF("MSG: Mount, unpark error "); VL(e); *commandError = e; }
    } else return false;
  } else return false;

  return true;
}
#endif
