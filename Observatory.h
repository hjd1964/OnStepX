//--------------------------------------------------------------------------------------------------
// observatory site and time
#pragma once

typedef struct TI {
  int16_t  year;
  uint8_t  month;
  uint8_t  day;
  uint8_t  hour;
  uint8_t  minute;
  uint8_t  second;
  long     centisecond;
  long     lastCentisecondStart;
  long     julianDay;
  bool     dateReady;
  bool     timeReady;
} TI;

typedef struct Latitude {
  double   value;
  double   sine;
  double   cosine;
  double   absval;
  double   sign;
} Latitude;

typedef struct LI {
  Latitude latitude;
  double   longitude;
  double   timezone;
  bool     ready;
} LI;

class Observatory {
  public:
    void init(LI site, TI ut1, uint8_t handle) {
      this->site = site;
      setLatitude(site.latitude.value);
      setLongitude(site.longitude);

      this->ut1  = ut1;
      this->ut1.timeReady = false;
      this->ut1.dateReady = false;
      ut1.julianDay = gregorianToJulian(ut1.year, ut1.month, ut1.day);
      updateLAST(julianToLAST(ut1.julianDay, ut1.hour + (ut1.minute + (ut1.second + (ut1.centisecond/100.0))/60.0)/60.0));
    }

    void setLatitude(double latitude) {
      site.latitude.value  = latitude;
      site.latitude.cosine = cos(latitude);
      site.latitude.sine   = sin(latitude);
      site.latitude.absval = fabs(latitude);
      if (lat >= 0) site.latitude.sign = 1; else site.latitude.sign = -1;
    }

    void setLongitude(double longitude) {
      site.longitude = longitude;
      updateLST(julianToLAST(JD,UT1));
    }

    // sets a new date
    void setJulianDay(long julianDay) {
      ut1.julianDay = julianDay;
      updateLAST(julianToLAST(julianDay, backIn24(getJulianHours())));
    }

    // gets the UT1 time
    double getJulianHours() {
      unsigned long cs;
      noInterrupts(); 
      cs = centisecondLAST;
      interrupts();
      double gregorianSeconds = (double)((cs - ut1.centisecondLASTstart)/100.0)/SIDEREAL_RATIO;
      return ut1.julianDay + gregorianSeconds/3600.0;
    }
    
    // gets local apparent sidereal time in hours
    double getLAST() {
      unsigned long cs;
      noInterrupts(); 
      cs = centisecondLAST;
      interrupts();
      return backInHours(radToHrs(csToRad(cs)));
    }

    // convert gregorian date (year,month,day) to julian day number
    double gregorianToJulian(int year, int month, int day) {
      if (month == 1 || month == 2) { year--; month=month + 12; }
      double B = 2.0-floor(year/100.0) + floor(year/400.0);
      return (B + floor(365.25*year) + floor(30.6001*(month + 1.0)) + day + 1720994.5);
    }

    // callback to tick the centisecond sidereal clock
    void clockTick() {
      centisecondLAST++;
    }

    TI                     ut1;
    volatile unsigned long centisecondLAST;
    LI                     site;

  private:

    // passes local apparent sidereal time to centi-sidereal-second timer counter
    // and marks the date/time this occured
    void updateLAST(double t) {
      long cs = lround((t/24.0)*8640000.0);
      ut1.centisecondLASTstart = cs;
      noInterrupts();
      centisecondLAST = cs;
      interrupts();
    }

    // convert julian date/time to greenwich apparent sidereal time
    double julianToGAST(double julianDay, double ut1) {
      int y,m,d;
      julianToGregorian(julianDay,&y,&m,&d);
      double julianDay0 = gregorianToJulian(y,m,d);
      double D= (julianDay - 2451545.0)+(ut1/24.0);
      double D0=(julianDay0- 2451545.0);
      double H = ut1;
      double T = D/36525.0;
      double gmst = 6.697374558 + 0.06570982441908*D0;
      gmst=gmst + 1.00273790935*H + 0.000026*T*T;
    
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

    // convert julian day number to gregorian date (year,month,day)
    void julianToGregorian(double julianDay, int *year, int *month, int *day) {
      double A,B,C,D,D1,E,F,G,I;
    
      julianDay += 0.5;
      I=floor(julianDay);
     
      F=0.0;
      if (I > 2299160.0) {
        A=int((I-1867216.25)/36524.25);
        B=I+1.0+A-floor(A/4.0);
      } else B=I;
    
      C=B+1524.0;
      D=floor((C - 122.1)/365.25);
      E=floor(365.25*D);
      G=floor((C - E)/30.6001);
    
      D1=C-E+F-floor(30.6001*G);
      *day=floor(D1);
      if (G < 13.5)     *month=floor(G - 1.0);    else *month=floor(G - 13.0);
      if (*month > 2.5) *year =floor(D - 4716.0); else *year =floor(D - 4715.0);
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
