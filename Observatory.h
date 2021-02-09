//--------------------------------------------------------------------------------------------------
// observatory site and time
#pragma once

#include "Transform.h"

class Observatory {
  public:
    // setup the location, time keeping, and coordinate converson
    void init();

    // update the location for time keeping and coordinate conversion
    void setSite(LI site);
    void updateSite();

    // handle observatory commands
    bool command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandErrors *commandError);

  private:
    LI site;
};

void Observatory::init() {
  LI newSite;
  newSite.latitude.value = degToRad(40.0);
  newSite.longitude      = degToRad(75.2);
  setSite(newSite);
  clock.init(site);
}

void Observatory::setSite(LI site) {
  this->site = site;
  updateSite();
}

void Observatory::updateSite() {
  this->site.latitude.cosine = cos(site.latitude.value);
  this->site.latitude.sine   = sin(site.latitude.value);
  this->site.latitude.absval = fabs(site.latitude.value);
  if (this->site.latitude.value >= 0) this->site.latitude.sign = 1; else this->site.latitude.sign = -1;
  this->site.longitude = site.longitude;
  transform.setSite(site);
  clock.setSite(site);
}

bool Observatory::command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandErrors *commandError) {

  if (clock.command(reply, command, parameter, supressFrame, numericReply, commandError)) return true;

  int i;
  double value;
  PrecisionMode precisionMode = PM_LOW;

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
    if (convert.degToDouble(&value, parameter, true)) {
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
    if (convert.degToDouble(&value, (char *)&parameter[i], false)) {
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

Observatory observatory;
