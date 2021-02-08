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

  return false;
}

Observatory observatory;
