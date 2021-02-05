//--------------------------------------------------------------------------------------------------
// observatory site and time
#pragma once

volatile unsigned long centisecondLAST;
#include "transform.h"

class Observatory {
  public:
    void init(LI site, TI ut1, uint8_t handle) {
      transform.init(site);

      this->site = site;
      setLatitude(site.latitude.value);
      setLongitude(site.longitude);

      this->ut1  = ut1;
      this->ut1.timeReady = false;
      this->ut1.dateReady = false;
      ut1.julianDay = transform.gregorianToJulian(ut1.year, ut1.month, ut1.day);
      updateLAST(julianToLAST(ut1.julianDay, ut1.hour + (ut1.minute + (ut1.second + (ut1.centisecond/100.0))/60.0)/60.0));
    }

    bool command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandErrors *commandError) {
      // :SC[MM/DD/YY]#
      // Change Date to MM/DD/YY
      // Return: 0 on failure
      //         1 on success
      if (command[0] == 'S' && command[1] == 'C')  {
        double jd;
        if (transform.dateToDouble(&jd, parameter)) {
          // nv.writeFloat(EE_JD, JD);
          ut1.julianDay = jd; // <-- watch this for truncating the fraction, Julian days start at Noon
          adjustLAST(ut1.julianDay);
          // if (generalError == ERR_SITE_INIT && observatory.ut1.timeReady) generalError = ERR_NONE;
        } else *commandError = CE_PARAM_FORM;
      } else return false;
      return true;
    }

    void setLatitude(double latitude) {
      site.latitude.value  = latitude;
      site.latitude.cosine = cos(latitude);
      site.latitude.sine   = sin(latitude);
      site.latitude.absval = fabs(latitude);
      if (latitude >= 0) site.latitude.sign = 1; else site.latitude.sign = -1;
    }

    void setLongitude(double longitude) {
      site.longitude = longitude;
      adjustLAST(ut1.julianDay);
    }

    // gets local apparent sidereal time in hours
    double getLAST() {
      unsigned long cs;
      noInterrupts(); 
      cs = centisecondLAST;
      interrupts();
      return backInHours(radToHrs(csToRad(cs)));
    }

    // callback to tick the centisecond sidereal clock
    void clockTick() {
      centisecondLAST++;
    }

    TI                     ut1;
    LI                     site;

  private:

    // passes local apparent sidereal time to the centisecond sidereal timer
    // and marks the date/time this occured
    void updateLAST(double hours) {
      long cs = lround((hours/24.0)*8640000.0);
      ut1.centisecondLASTstart = cs;
      noInterrupts();
      centisecondLAST = cs;
      interrupts();
    }

    // adjusts local apparent sidereal time for the new Julian Day
    void adjustLAST(long julianDay) {
      updateLAST(julianToLAST(julianDay, backInHours(getHour())));
    }

    // gets the current Julian Day (including the time)
    double getJulianDay() {
      return ut1.julianDay + getHour();
    }

    // gets the current Julian Hour
    double getHour() {
      unsigned long cs;
      noInterrupts(); 
      cs = centisecondLAST;
      interrupts();
      double gregorianSeconds = (double)((cs - ut1.centisecondLASTstart)/100.0)/SIDEREAL_RATIO;
      return gregorianSeconds/3600.0;
    }

    // convert julian date/time to greenwich apparent sidereal time
    double julianToGAST(double julianDay, double ut1) {
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
    double julianToLAST(double julianDay, double hours) {
      double gast = julianToGAST(julianDay, hours);
      return backInHours(radToHrs(gast - site.longitude));
    }

    double backInHours(double time) {
      while (time >= 24.0) time -= 24.0;
      while (time < 0.0)   time += 24.0;
      return time;
    }
};

// instantiate and callback wrappers
Observatory observatory;
void clockTick() { observatory.clockTick(); }
