//--------------------------------------------------------------------------------------------------
// coordinate transformation

// MOTOR      <--> apply index offset and backlash        <--> INSTRUMENT  (Axis)
// INSTRUMENT <--> apply celestial coordinate conventions <--> MOUNT       (Transform)
// MOUNT      <--> apply pointing model                   <--> OBSERVED    (Transform)
// OBSERVED   <--> apply refraction                       <--> TOPOCENTRIC (Transform)

enum PierSide {PIER_SIDE_NONE, PIER_SIDE_EAST, PIER_SIDE_WEST};

#pragma once

typedef struct EquCoordinate {
    double r;
    double h;
    double d;
    PierSide p;
} EquCoordinate;

typedef struct HorCoordinate {
    double a;
    double z;
} HorCoordinate;

typedef struct TI {
  int16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t centisecond;
  long    julianDay;
} TI;

typedef struct Latitude {
  double value;
  double sine;
  double cosine;
  double absval;
  double sign;
} Latitude;

typedef struct LI {
  Latitude latitude;
  double longitude;
  double timezone;
} LI;

class Transform {
  public:
    void init(LI site, TI ut1, uint8_t handle) {
      this->site = site;
      updateLatitude(site.latitude.value);

      this->ut1  = ut1;
      ut1.julianDay=toJulian(ut1.year,ut1.month,ut1.day);
      updateLAST(julianToLAST(ut1.julianDay,ut1.hour));
    }

    EquCoordinate equMountToTopocentric(EquCoordinate equ) {
      EquCoordinate obs=equMountToObservedPlace(equ);
      return observedPlaceToTopocentric(obs);
    }
    EquCoordinate topocentricToEquMount(EquCoordinate equ) {
      EquCoordinate obs=topocentricToObservedPlace(equ);
      return observedPlaceToEquMount(obs);
    }

    EquCoordinate equMountToObservedPlace(EquCoordinate equ) {
      // apply the pointing model
      return equ;
    }
    EquCoordinate observedPlaceToEquMount(EquCoordinate equ) {
      // de-apply the pointing model
      return equ;
    }

    // converts from instrument to mount coordinates
    EquCoordinate equInstrumentToMount(EquCoordinate equ) {
      if ((equ.d < degToRad(-90.0)) || (equ.d > degToRad(90.0))) {
        equ.r += (degToRad(180.0) - degToRad(360.0));
        equ.d =   degToRad(180.0) - equ.d;
      }
      if (equ.d > degToRad(180.0))  equ.d -= degToRad(360.0); else
      if (equ.d < degToRad(-180.0)) equ.d += degToRad(360.0);
      return equ;
    }

    // converts from mount to instrument coordinates
    EquCoordinate equMountToInstrument(EquCoordinate equ) {
      if (equ.p == PIER_SIDE_WEST) equ.r += degToRad(180.0);
      if (site.latitude.value >= 0.0) {
        if (equ.p == PIER_SIDE_WEST) equ.d = degToRad( 180.0) - equ.d;
      } else {
        if (equ.p == PIER_SIDE_WEST) equ.d = degToRad(-180.0) - equ.d;
      }
      if (equ.d > degToRad(360.0))  equ.d -= degToRad(360.0); else
      if (equ.d < degToRad(-360.0)) equ.d += degToRad(360.0);
    }

    // converts from the topocentric to observed
    EquCoordinate topocentricToObservedPlace(EquCoordinate equ) {
      HorCoordinate hor;

      // within about 1/20 arc-second of NCP or SCP
    #if TOPOCENTRIC_STRICT == ON
      if (fabs(equ.d-PI/2) < SmallestRad) { hor.z=0.0;   hor.a= site.latitude.value; } else 
      if (fabs(equ.d+PI/2) < SmallestRad) { hor.z=180.0; hor.a=-site.latitude.value; } else hor=equToHor(equ);
    #else
      if (fabs(equ.d-PI/2) < SmallestRad || fabs(equ.d+PI/2) < SmallestRad) return equ; else hor=equToHor(equ);
    #endif

      hor.a += trueRefrac(hor.a);
      return horToEqu(hor);
    }
    
    // converts from the "Observed" to "Topocentric"
    EquCoordinate observedPlaceToTopocentric(EquCoordinate equ) {
      HorCoordinate hor;
      
      // within about 1/20 arc-second of the "refracted" NCP or SCP
    #if TOPOCENTRIC_STRICT == ON
      if (fabs(equ.d-PI/2) < SmallestRad) { hor.z=0.0;   hor.a= site.latitude.value; } else
      if (fabs(equ.d+PI/2) < SmallestRad) { hor.z=180.0; hor.a=-site.latitude.value; } else hor=equToHor(equ);
    #else  
      if (fabs(equ.d-PI/2) < SmallestRad || fabs(equ.d+PI/2) < SmallestRad) return equ; else hor=equToHor(equ);
    #endif

      hor.a -= apparentRefrac(hor.a);
      return horToEqu(hor);
    }
    
    void hourAngleToRightAscension(EquCoordinate *equ) {
      noInterrupts();
      long cs = centisecondLAST;
      interrupts();
      equ->r = csToRad(cs) - equ->h;
      equ->r = backInRads(equ->r);
    }
    
    void rightAscensionToHourAngle(EquCoordinate *equ) {
      noInterrupts();
      long cs = centisecondLAST;
      interrupts();
      equ->h = csToRad(cs) - equ->r;
      equ->h = backInRads(equ->h);
    }

    // convert equatorial coordinates to horizon
    HorCoordinate equToHor(EquCoordinate equ) {
      HorCoordinate hor;
      double cosHA  = cos(equ.h);
      double sinAlt = (sin(equ.d) * site.latitude.sine) + (cos(equ.d) * site.latitude.cosine * cosHA);  
      hor.a         = asin(sinAlt);
      double t1     = sin(equ.h);
      double t2     = cosHA*site.latitude.sine-tan(equ.d)*site.latitude.cosine;
      hor.z         = atan2(t1,t2);
      hor.z        += PI;
      return hor;
    }
          
    // convert horizon coordinates to equatorial
    EquCoordinate horToEqu(HorCoordinate hor) { 
      EquCoordinate equ;
      double cosAzm = cos(hor.z);
      double sinDec = (sin(hor.a) * site.latitude.sine) + (cos(hor.a) * site.latitude.cosine * cosAzm);  
      equ.d         = asin(sinDec); 
      double t1     = sin(hor.z);
      double t2     = cosAzm*site.latitude.sine-tan(hor.a)*site.latitude.cosine;
      equ.h         = atan2(t1,t2);
      equ.h        += PI;
      return equ;
    }

    void updateLatitude(double lat) {
      site.latitude.value  = lat;
      site.latitude.cosine = cos(lat);
      site.latitude.sine   = sin(lat);
      site.latitude.absval = fabs(lat);
      if (lat >= 0) site.latitude.sign = 1; else site.latitude.sign = -1;
    }

    void updateLongitude(double latitude) {
    }

    // returns the amount of refraction at the given true altitude (radians), pressure (millibars), and temperature (celsius)
    double trueRefrac(double altitude, double pressure = 1010.0, double temperature = 10.0) {
      if (isnan(pressure)) pressure=1010.0;
      if (isnan(temperature)) temperature=10.0;

//    double TPC = (pressure/1010.0)*(283.0/(273.0+temperature));
//    double r   = 1.02*cot( (Alt+(10.3/(Alt+5.11)))/Rad ) * TPC;  // r is in arc-minutes

      double TPC = (pressure/1010.0)*(283.0/(273.0 + temperature));
      double r   = 2.96705972855e-4*cot(altitude + 0.17977/(altitude + 0.08919))*TPC;

      if (r < 0.0) r=0.0;
      return r;
    }
    
    // returns the amount of refraction at the given apparent altitude (radians), pressure (millibars), and temperature (celsius)
    double apparentRefrac(double altitude, double pressure = 1010.0, double temperature = 10.0) {
      if (isnan(pressure)) pressure = 1010.0;
      if (isnan(temperature)) temperature = 10.0;
      double r = trueRefrac(altitude,pressure,temperature);
      return trueRefrac(altitude-r,pressure,temperature);
    }

    // gets the local apparent sidereal time in hours
    double getLAST() {
      double cs;
      noInterrupts(); 
      cs = centisecondLAST;
      interrupts();
      return backInHours(radToHrs(csToRad(cs)));
    }

    void clockTick() {
      centisecondLAST++;
    }

    // convert gregorian date (year,month,day) to julian day number
    double toJulian(int year, int month, int day) {
      if (month == 1 || month == 2) { year--; month=month + 12; }
      double B = 2.0-floor(year/100.0) + floor(year/400.0);
      return (B + floor(365.25*year) + floor(30.6001*(month + 1.0)) + day + 1720994.5);
    }
    
  private:

    // passes local apparent sidereal time to centi-sidereal-second timer counter
    // and marks the date/time this occured
    void updateLAST(double t) {
      long cs = lround((t/24.0)*8640000.0);
      start_ut1 = ut1;
      start_ut1.centisecond = cs;
      noInterrupts();
      centisecondLAST = cs;
      interrupts();
    }

    // convert julian date/time to greenwich apparent sidereal time
    double julianToGAST(double julianDay, double ut1) {
      int y,m,d;
      toGregorian(julianDay,&y,&m,&d);
      double julianDay0=toJulian(y,m,d);
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
    double julianToLAST(double julianDay, double ut1) {
      double gast = julianToGAST(julianDay,ut1);
      return backInHours(radToHrs(gast-site.longitude));
    }

    
    // convert julian day number to gregorian date (year,month,day)
    void toGregorian(double julianDay, int *year, int *month, int *day) {
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

    double cot(double n) {
      return 1.0/tan(n);
    }

    double backInHours(double time) {
      while (time >= 24.0) time -= 24.0;
      while (time < 0.0)   time += 24.0;
      return time;
    }

    double backInRads(double angle) {
      double PI2 = PI*2.0;
      while (angle >= PI2) angle -= PI2;
      while (angle < 0.0)  angle += PI2;
      return angle;
    }

    uint32_t centisecondLAST;
    TI       start_ut1;
    TI       ut1;
    LI       site;
};

Transform transform;
