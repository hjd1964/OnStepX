//--------------------------------------------------------------------------------------------------
// telescope control
#pragma once

class Telescope {
  public:
    // handle telescope commands
    bool command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandErrors *commandError);

  private:
    EquCoordinate native, instrument, target;
};

bool Telescope::command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandErrors *commandError) {
  tasks_mutex_enter(MX_TELESCOPE_CMD);
  PrecisionMode precisionMode = convert.precision;
  
  // :GR#       Get Telescope RA
  //            Returns: HH:MM.T# or HH:MM:SS# (based on precision setting)
  // :GRH#      Returns: HH:MM:SS.SSSS# (high precision)
  if (cmdH("GR")) {
    instrument.h = axis1.getInstrumentCoordinate(); instrument.d = axis2.getInstrumentCoordinate();
    native = transform.equInstrumentToNative(instrument);
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToHms(reply, native.r, precisionMode);
    *numericReply=false;
  } else

  // :GD#       Get Telescope Declination
  //            Returns: sDD*MM# or sDD*MM:SS# (based on precision setting)
  // :GDH#      Returns: sDD*MM:SS.SSS# (high precision)
  if (cmdH("GD"))  {
    instrument.h = axis1.getInstrumentCoordinate(); instrument.d = axis2.getInstrumentCoordinate();
    native = transform.equInstrumentToNative(instrument);
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, native.d, false, true, precisionMode);
    *numericReply = false;
  } else 

  // :Gr#       Get Target RA
  //            Returns: HH:MM.T# or HH:MM:SS (based on precision setting)
  // :GrH#      Returns: HH:MM:SS.SSSS# (high precision)
  if (cmdH("Gr"))  {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToHms(reply, radToHrs(target.r), precisionMode);
    *numericReply = false;
  } else

  // :Gd#       Get Target Dec
  //            Returns: HH:MM.T# or HH:MM:SS (based on precision setting)
  // :GdH#      High precision
  //            Returns: HH:MM:SS.SSS# (high precision)
  if (cmdH("Gd"))  {
    if (parameter[0] == 'H') precisionMode = PM_HIGHEST;
    convert.doubleToDms(reply, radToDeg(target.d), false, true, precisionMode);
    *numericReply = false;
  } else

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

Telescope telescope;
