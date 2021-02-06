//--------------------------------------------------------------------------------------------------
// observatory site and time
#pragma once

volatile unsigned long centisecondLAST;
GeneralErrors generalError = ERR_NONE;

#include "transform.h"

class Observatory {
  public:
    void init();

    bool command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandErrors *commandError);

    void setLatitude(double latitude);

    void setLongitude(double longitude);

    // gets local apparent sidereal time in hours
    double getLAST();

    // callback to tick the centisecond sidereal clock
    void clockTick();

    TI ut1;
    LI site;

  private:

    // passes local apparent sidereal time to the centisecond sidereal timer
    // and marks the date/time this occured
    void updateLAST(double hours);

    // adjusts local apparent sidereal time for the new Julian Day
    void adjustLAST(long julianDay);

    // gets the current Julian Day (including the time)
    double getJulianDay();

    // gets the current Julian Hour
    double getHour();

    // convert julian date/time to greenwich apparent sidereal time
    double julianToGAST(double julianDay, double ut1);

    // convert julian date/time to local apparent sidereal time
    double julianToLAST(double julianDay, double hours);

    double backInHours(double time);

    bool dateIsReady = false;
    bool timeIsReady = false;
};

// instantiate and callback wrappers
Observatory observatory;
void clockTickWrapper() { observatory.clockTick(); }

void Observatory::init() {
  // setup the location and date/time for time keeping and coordinate converson
  site.latitude.value = degToRad(40.1); site.longitude = degToRad(76.0);
  setLatitude(site.latitude.value);
  setLongitude(site.longitude);
  transform.init(site);

  // add an event to tick the centisecond sidereal clock
  // period ms (0=idle), duration ms (0=forever), repeat, priority (highest 0..7 lowest), task_handle
  uint8_t handle = tasks.add(0, 0, true, 0, clockTickWrapper, "ClkTick");
  tasks.requestHardwareTimer(handle, 3, 1);
  tasks.setPeriodSubMicros(handle, lround(160000.0/SIDEREAL_RATIO));

  ut1.year = 2020; ut1.month  = 1;  ut1.day = 20;
  ut1.hour = 12;   ut1.minute = 11; ut1.second = 9; ut1.centisecond = 252;
  ut1.timezone = 5;

  ut1.julianDay = transform.gregorianToJulian(ut1.year, ut1.month, ut1.day);
  updateLAST(julianToLAST(ut1.julianDay, ut1.hour + (ut1.minute + (ut1.second + (ut1.centisecond/100.0))/60.0)/60.0));
}

bool Observatory::command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandErrors *commandError) {
  // :Ga#       Get Local Time in 12 hour format
  //            Returns: HH:MM:SS#
  if (command[0] == 'G' && command[1] == 'a' && parameter[0] == 0)  {
    double localStandardTime = backInHours(getHour() - ut1.timezone); if (localStandardTime > 12.0) localStandardTime -= 12.0;
    transform.doubleToHms(reply,&localStandardTime, PM_HIGH);
    *numericReply = false;
  } else
  
  // :GC#       Get the current local calendar date
  //            Returns: MM/DD/YY#
  if (command[0] == 'G' && command[1] == 'C' && parameter[0] == 0) {
    int y, m, d;
    transform.julianToGregorian(getJulianDay() - ut1.timezone, &y, &m, &d); y -= 2000; if (y >= 100) y -= 100;
    sprintf(reply,"%02d/%02d/%02d", m, d, y);
    *numericReply = false;
  } else

  // :Gc#       Get the current local time format
  //            Returns: 24#
  if (command[0] == 'G' && command[1] == 'c' && parameter[0] == 0) {
    strcpy(reply, "24");
    *numericReply = false;
  } else

  // :SC[MM/DD/YY]#
  //            Change Date to MM/DD/YY
  //            Return: 0 on failure
  //            1 on success
  if (command[0] == 'S' && command[1] == 'C')  {
    double jd;
    if (transform.dateToDouble(&jd, parameter)) {
      // nv.writeFloat(EE_JD, JD);
      ut1.julianDay = jd; // <-- watch this for truncating the fraction, Julian days start at Noon
      adjustLAST(ut1.julianDay);
      if (generalError == ERR_SITE_INIT && dateIsReady && timeIsReady) generalError = ERR_NONE;
    } else *commandError = CE_PARAM_FORM;
  } else return false;
  
  return true;
}

void Observatory::setLatitude(double latitude) {
  site.latitude.value  = latitude;
  site.latitude.cosine = cos(latitude);
  site.latitude.sine   = sin(latitude);
  site.latitude.absval = fabs(latitude);
  if (latitude >= 0) site.latitude.sign = 1; else site.latitude.sign = -1;
}

void Observatory::setLongitude(double longitude) {
  site.longitude = longitude;
  adjustLAST(ut1.julianDay);
}

// gets local apparent sidereal time in hours
double Observatory::getLAST() {
  unsigned long cs;
  noInterrupts(); 
  cs = centisecondLAST;
  interrupts();
  return backInHours(radToHrs(csToRad(cs)));
}

// callback to tick the centisecond sidereal clock
void Observatory::clockTick() {
  centisecondLAST++;
}

// passes local apparent sidereal time to the centisecond sidereal timer
// and marks the date/time this occured
void Observatory::updateLAST(double hours) {
  long cs = lround((hours/24.0)*8640000.0);
  ut1.centisecond = cs;
  noInterrupts();
  centisecondLAST = cs;
  interrupts();
}

// adjusts local apparent sidereal time for the new Julian Day
void Observatory::adjustLAST(long julianDay) {
  updateLAST(julianToLAST(julianDay, backInHours(getHour())));
}

// gets the current Julian Day (including the time)
double Observatory::getJulianDay() {
  return ut1.julianDay + getHour();
}

// gets the current Julian Hour
double Observatory::getHour() {
  unsigned long cs;
  noInterrupts(); 
  cs = centisecondLAST;
  interrupts();
  double gregorianSeconds = (double)((cs - ut1.centisecond)/100.0)/SIDEREAL_RATIO;
  return gregorianSeconds/3600.0;
}

// convert julian date/time to greenwich apparent sidereal time
double Observatory::julianToGAST(double julianDay, double ut1) {
  int y, m, d;
  transform.julianToGregorian(julianDay,&y,&m,&d);
  double julianDay0 = transform.gregorianToJulian(y,m,d);
  double D= (julianDay - 2451545.0)+(ut1/24.0);
  double D0=(julianDay0- 2451545.0);
  double H = ut1;
  double T = D/36525.0;
  double gmst = 6.697374558 + 0.06570982441908*D0;
  gmst = gmst + SIDEREAL_RATIO*H + 0.000026*T*T;

  // equation of the equinoxes
  double O = 125.04  - 0.052954 *D;
  double L = 280.47  + 0.98565  *D;
  double E = 23.4393 - 0.0000004*D;
  double W = -0.000319*sin(degToRad(O)) - 0.000024*sin(degToRad(2*L));
  double eqeq = W*cos(degToRad(E));
  double gast = gmst + eqeq;
  return backInHours(gast);
}

// convert julian date/time to local apparent sidereal time
double Observatory::julianToLAST(double julianDay, double hours) {
  double gast = julianToGAST(julianDay, hours);
  return backInHours(radToHrs(gast - site.longitude));
}

double Observatory::backInHours(double time) {
  while (time >= 24.0) time -= 24.0;
  while (time < 0.0)   time += 24.0;
  return time;
}
