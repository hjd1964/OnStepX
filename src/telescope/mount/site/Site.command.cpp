//--------------------------------------------------------------------------------------------------
// telescope mount time and location, commands

#include "Site.h"

#ifdef MOUNT_PRESENT

#include "../../../libApp/commands/ProcessCmds.h"
#include "../../Telescope.h"
#include "../home/Home.h"
#include "../Mount.h"

bool Site::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  *supressFrame = false;
  PrecisionMode precisionMode = convert.precision;

  if (command[0] == 'G') {
    // :Ga#       Get standard time in 12 hour format
    //            Returns: HH:MM:SS#
    if (command[1] == 'a' && parameter[0] == 0) {
      double time = rangeAmPm(getTime() - location.timezone);
      convert.doubleToHms(reply, time, false, PM_HIGH);
      *numericReply = false;
    } else

    // :GC#       Get standard calendar date
    //            Returns: MM/DD/YY#
    if (command[1] == 'C' && parameter[0] == 0) {
      GregorianDate local = calendars.julianToGregorian(UT1ToLocal(getDateTime()));
      sprintf(reply,"%02d/%02d/%02d", (int)local.month, (int)local.day, (int)local.year % 100);
      *numericReply = false;
    } else

    // :Gc#       Get the Local Standard Time format
    //            Returns: 24#
    if (command[1] == 'c' && parameter[0] == 0) {
      strcpy(reply, "24");
      *numericReply = false;
    } else

    // :GG#       Get UTC offset time, hours and minutes to add to local time to convert to UTC
    //            Returns: [s]HH:MM#
    if (command[1] == 'G' && parameter[0] == 0) {
      convert.doubleToHms(reply, location.timezone, true, PM_LOWEST);
      *numericReply = false;
    } else

    // :Gg#       Get Current Site Longitude, east is negative
    //            Returns: sDDD*MM#
    // :GgH#      Get current site Longitude
    //            Returns: sDD*MM:SS.SSS# (high precision)
    if (command[1] == 'g' && (parameter[0] == 0 || parameter[1] == 0)) {
      precisionMode = PM_LOW;
      if (parameter[0] == 'H') precisionMode = PM_HIGHEST; else if (parameter[0] != 0) { *commandError = CE_PARAM_FORM; return true; }
      convert.doubleToDms(reply, radToDeg(location.longitude), true, true, precisionMode);
      *numericReply = false;
    } else 

    // :GL#       Get Local Standard Time in 24 hour format
    //            Returns: HH:MM:SS#
    // :GLH#      Returns: HH:MM:SS.SSSS# (high precision)
    if (command[1] == 'L' && (parameter[0] == 0 || parameter[1] == 0)) {
      if (parameter[0] == 'H') precisionMode = PM_HIGHEST; else if (parameter[0] != 0) { *commandError = CE_PARAM_FORM; return true; }
      convert.doubleToHms(reply, rangeHours(getTime() - location.timezone), false, precisionMode);
      *numericReply = false;
    } else

    // :GM#       Get site 1 name
    // :GN#       Get site 2 name
    // :GO#       Get site 3 name
    // :GP#       Get site 4 name
    //            Returns: s#
    if ((command[1] == 'M' || command[1] == 'N' || command[1] == 'O' || command[1] == 'P') && parameter[0] == 0)  {
      Location tempLocation;
      uint8_t locationNumber = command[1] - 'M';
      nv.readBytes(NV_SITE_BASE + locationNumber*LocationSize, &tempLocation, LocationSize);
      strcpy(reply, tempLocation.name);
      if (reply[0] == 0) { strcat(reply,"None"); }
      *numericReply = false; 
    } else

    // :GS#       Get the Sidereal Time as sexagesimal value in 24 hour format
    //            Returns: HH:MM:SS#
    // :GSH#      Returns: HH:MM:SS.ss# (high precision)
    if (command[1] == 'S' && (parameter[0] == 0 || parameter[1] == 0)) {
      if (parameter[0] == 'H') precisionMode = PM_HIGHEST; else if (parameter[0] != 0) { *commandError = CE_PARAM_FORM; return true; }
      convert.doubleToHms(reply, getSiderealTime(), false, precisionMode);
      *numericReply = false;
    } else

    // :Gt#       Get current site Latitude, positive for North latitudes
    //            Returns: sDD*MM#
    // :GtH#      Get current site Latitude, positive for North latitudes
    //            Returns: sDD*MM:SS.SSS# (high precision)
    if (command[1] == 't' && (parameter[0] == 0 || parameter[1] == 0)) {
      precisionMode = PM_LOW;
      if (parameter[0] == 'H') precisionMode = PM_HIGHEST; else if (parameter[0] != 0) { *commandError = CE_PARAM_FORM; return true; }
      convert.doubleToDms(reply, radToDeg(location.latitude), false, true, precisionMode);
      *numericReply = false;
    } else 

    // :Gv#       Get the site eleVation in meters
    //            Returns: +/-n.n
    if (command[1] == 'v' && parameter[0] == 0) {
      sprintF(reply, "%3.1f", location.elevation);
      *numericReply = false;
    } else

    if (command[1] == 'X' && parameter[0] == '8' && parameter[2] == 0) {
      // :GX80#     Get the UT1 Time as sexagesimal value in 24 hour format
      //            Returns: HH:MM:SS.ss#
      if (parameter[1] == '0') {
        convert.doubleToHms(reply, rangeHours(getTime()), false, PM_HIGH);
        *numericReply = false;
      } else

      // :GX81#     Get the UT1 Date
      //            Returns: MM/DD/YY#
      if (parameter[1] == '1') {
        JulianDate julianDay = ut1;
        double hour = getTime();
        while (hour >= 24.0) { hour -= 24.0; julianDay.day += 1.0; }
        if    (hour < 0.0)   { hour += 24.0; julianDay.day -= 1.0; }
        GregorianDate date = calendars.julianDayToGregorian(julianDay);
        sprintf(reply,"%02d/%02d/%02d", (int)date.month, (int)date.day, (int)date.year % 100);
        *numericReply = false;
      } else

      // :GX89#     Date/time ready status
      //            Return: 0 ready, 1 not ready
      if (parameter[1] == '9') {
        if (dateIsReady && timeIsReady) *commandError = CE_0;
      } else return false;

    } else return false;
  } else

  if (command[0] == 'S') {
    // :SC[MM/DD/YY]# or :SC[MM/DD/YYYY]#
    //            Change local standard date
    //            Return: 0 on failure, 1 on success
    if (command[1] == 'C') {
      GregorianDate local = calendars.julianToGregorian(UT1ToLocal(getDateTime()));
      if (strToDate(parameter, &local)) {
        dateIsReady = true;
        setDateTime(localToUT1(calendars.gregorianToJulian(local)));        
        updateTLS();
      } else *commandError = CE_PARAM_FORM;
    } else

    //  :SG[sHH]# or :SG[sHH:MM]# (where MM is 00, 30, or 45)
    //            Set the number of hours added to local time to yield UT1
    //            Return: 0 failure, 1 success
    if (command[1] == 'G') {
      double hour;
      if (convert.tzToDouble(&hour, parameter)) {
        if (hour >= -13.75 || hour <= 12.0) {
          location.timezone = hour;
          nv.updateBytes(NV_SITE_BASE + locationNumber*LocationSize, &location, LocationSize);
        } else *commandError = CE_PARAM_RANGE;
      } else *commandError = CE_PARAM_FORM;
    } else

    //  :Sg[(s)DDD*MM]# or :Sg[(s)DDD*MM:SS]# or :Sg[(s)DDD*MM:SS.SSS]#
    //            Set current site longitude, east longitudes can be negative or > 180 degrees
    //            Return: 0 failure, 1 success
    if (command[1] == 'g') {
      double degs;
      int i;
      if (parameter[0] == '-' || parameter[0] == '+') i = 1; else i = 0;
      if (convert.dmsToDouble(&degs, (char *)&parameter[i], false)) {
        if (degs >= -180.0 && degs <= 360.0) {
          if (degs >= 180.0) degs -= 360.0;
          location.longitude = degToRad(degs);
          if (parameter[0] == '-') location.longitude = -location.longitude;
          updateLocation();
          nv.updateBytes(NV_SITE_BASE + locationNumber*LocationSize, &location, LocationSize);
        } else *commandError = CE_PARAM_RANGE;
      } else *commandError = CE_PARAM_FORM;
    } else

    //  :SL[HH:MM:SS]# or :SL[HH:MM:SS.SSS]#
    //            Set the local Time
    //            Return: 0 failure, 1 success
    if (command[1] == 'L') {
      GregorianDate local = calendars.julianToGregorian(UT1ToLocal(getDateTime()));
      if (convert.hmsToDouble(&local.hour, parameter, PM_HIGH) || convert.hmsToDouble(&local.hour, parameter, PM_HIGHEST)) {
        timeIsReady = true;
        setDateTime(localToUT1(calendars.gregorianToJulian(local)));
        updateTLS();
      } else *commandError = CE_PARAM_FORM;
    } else

    //  :SM[s]# or :SN[s]# or :SO[s]# or :SP[s]#
    //            Set site name, string may be up to 15 characters
    //            Return: 0 on failure
    //                    1 on success
    if ((command[1] == 'M' || command[1] == 'N' || command[1] == 'O' || command[1] == 'P')) {
      uint8_t locationNumber = command[1] - 'M';
      if (strlen(parameter) <= 15) {
        Location tempLocation;
        nv.readBytes(NV_SITE_BASE + locationNumber*LocationSize, &tempLocation, LocationSize);
        strcpy(tempLocation.name, parameter);
        nv.updateBytes(NV_SITE_BASE + locationNumber*LocationSize, &tempLocation, LocationSize);
        strcpy(location.name, parameter);
      } else *commandError = CE_PARAM_RANGE;
    } else

    //  :St[sDD*MM]# or :St[sDD*MM:SS]# or :St[sDD*MM:SS.SSS]#
    //            Set current site latitude in degrees
    //            Return: 0 failure, 1 success
    if (command[1] == 't') {
      double degs;
      if (convert.dmsToDouble(&degs, parameter, true)) {
        location.latitude = degToRad(degs);
        updateLocation();
        nv.updateBytes(NV_SITE_BASE + locationNumber*LocationSize, &location, LocationSize);
        if (mount.isHome()) home.init();
      } else *commandError = CE_PARAM_FORM;
    } else 

    //  :SU[s.s]#
    //            Set the number of seconds (up to +/-0.9) added to UTC to yield UT1 (DUT1)
    //            presently only applied to GPS and NTP UTC time for UT1 conversion
    //            see: https://datacenter.iers.org/data/latestVersion/bulletinB.txt
    //            Return: 0 failure, 1 success
    if (command[1] == 'U') {
      char *conv_end;
      double dut1 = strtod(parameter, &conv_end);
      if (dut1 >= -0.9 && dut1 <= 0.9) {
        #if TIME_LOCATION_SOURCE != OFF
          tls->DUT1 = dut1;
        #endif
        #if TIME_LOCATION_SOURCE_FALLBACK != OFF
          tlsFallback->DUT1 = dut1;
        #endif
      } else *commandError = CE_PARAM_FORM;
    } else

    // :Sv[sn.n]#
    //            Sets current site eleVation in meters
    //            Return: 0 failure, 1 success
    if (command[1] == 'v') {
      char *conv_end;
      float f = strtod(&parameter[0], &conv_end);
      if (&parameter[0] == conv_end) f = NAN;
      if (!setElevation(f)) *commandError = CE_PARAM_RANGE;
      nv.updateBytes(NV_SITE_BASE + locationNumber*LocationSize, &location, LocationSize);
    } else return false;
  } else

  if (command[0] == 'W') {
    // :W[n]#     Sets current site to n, where n = 0..3
    //            Returns: Nothing
    if ((command[1] >= '0' && command[1] <= '3') && parameter[0] == 0) {
      locationNumber = command[1] - '0';
      nv.update(NV_SITE_NUMBER, locationNumber);
      readLocation(locationNumber);
      updateLocation();
      if (mount.isHome()) home.init();
      *numericReply = false;
    } else

    // :W?#       Queries current site
    //            Returns: n#
    if (command[1] == '?' && parameter[0] == 0) {
      sprintf(reply, "%d", (int)nv.read(NV_SITE_NUMBER));
      *numericReply = false;
    } else return false;
  } else return false;

  return true;
}

#endif
