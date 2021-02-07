//--------------------------------------------------------------------------------------------------
// telescope control
#pragma once

class Telescope {
  public:
  
    bool command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandErrors *commandError) {
      tasks_mutex_enter(MX_TELESCOPE_CMD);

      //  :Sd[sDD*MM]# or :Sd[sDD*MM:SS]# or :Sd[sDD*MM:SS.SSS]#
      //            Set target object declination
      //            Return: 0 on failure
      //                    1 on success
      if (cmdp("Sd"))  {
        if (!convert.dmsTodouble(&target.d, parameter, true)) *commandError = CE_PARAM_RANGE;
        target.d = degToRad(target.d);
      } else

      //  :Sr[HH:MM.T]# or :Sr[HH:MM:SS]# or :Sr[HH:MM:SS.SSSS]#
      //            Set target object RA
      //            Return: 0 on failure
      //                    1 on success
      if (cmdp("Sr"))  {
        if (!convert.hmsToDouble(&target.r, parameter)) *commandError = CE_PARAM_RANGE;
        target.r = hrsToRad(target.r);
      } else

      { tasks_mutex_exit(MX_TELESCOPE_CMD); return false; }
      tasks_mutex_exit(MX_TELESCOPE_CMD); return true;
    }

  private:
    EquCoordinate target;

};

Telescope telescope;
