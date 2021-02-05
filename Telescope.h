//--------------------------------------------------------------------------------------------------
// telescope control
#pragma once

class Telescope {
  public:
  
    bool command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandErrors *commandError) {
// the following are just here for now, will end up in above...

// :GVD#      Get Telescope Firmware Date
//            Returns: MTH DD YYYY#
// :GVM#      General Message
//            Returns: s# (where s is a string up to 16 chars)
// :GVN#      Get Telescope Firmware Number
//            Returns: M.mp#
// :GVP#      Get Telescope Product Name
//            Returns: s#
// :GVT#      Get Telescope Firmware Time
//            Returns: HH:MM:SS#
      if (command[0] == 'G' && command[1] == 'V' && parameter[1] == 0) {
        if (parameter[0] == 'D') strcpy(reply,FirmwareDate); else
        if (parameter[0] == 'M') sprintf(reply,"OnStepX %i.%i%s",FirmwareVersionMajor,FirmwareVersionMinor,FirmwareVersionPatch); else
        if (parameter[0] == 'N') sprintf(reply,"%i.%i%s",FirmwareVersionMajor,FirmwareVersionMinor,FirmwareVersionPatch); else
        if (parameter[0] == 'P') strcpy(reply,FirmwareName); else
        if (parameter[0] == 'T') strcpy(reply,FirmwareTime); else return CE_CMD_UNKNOWN;
        *numericReply = false;
      } else

//  :Sd[sDD*MM]# or :Sd[sDD*MM:SS]# or :Sd[sDD*MM:SS.SSS]#
//            Set target object declination
//            Return: 0 on failure
//                    1 on success
      if (command[0] == 'S' && command[1] == 'd' && parameter[0] == 0)  {
        if (!transform.dmsTodouble(&target.d, parameter, true)) return CE_PARAM_RANGE;
        target.d = degToRad(target.d);
      } else

//  :Sr[HH:MM.T]# or :Sr[HH:MM:SS]# or :Sr[HH:MM:SS.SSSS]#
//            Set target object RA
//            Return: 0 on failure
//                    1 on success
      if (command[0] == 'S' && command[1] == 'r' && parameter[0] == 0)  {
        if (!transform.hmsTodouble(&target.r, parameter)) return CE_PARAM_RANGE;
        target.r = hrsToRad(target.r);
      } else return false;

      return true;
    }

  private:
    EquCoordinate target;

};

Telescope telescope;
