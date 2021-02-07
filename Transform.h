//--------------------------------------------------------------------------------------------------
// coordinate transformation
#pragma once

// MOTOR      <--> apply index offset and backlash        <--> INSTRUMENT  (Axis)
// INSTRUMENT <--> apply celestial coordinate conventions <--> MOUNT       (Transform)
// MOUNT      <--> apply pointing model                   <--> OBSERVED    (Transform)
// OBSERVED   <--> apply refraction                       <--> TOPOCENTRIC (Transform)

enum PierSide {PIER_SIDE_NONE, PIER_SIDE_EAST, PIER_SIDE_WEST};

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

class Transform {
  public:

    void setSite(LI site) {
      this->site = site;
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
      // convert HA into RA
      return equ;
    }

    EquCoordinate observedPlaceToEquMount(EquCoordinate equ) {
      // convert RA into HA
      // de-apply the pointing model
      return equ;
    }

    // converts from instrument to mount coordinates
    EquCoordinate equInstrumentToMount(EquCoordinate equ) {
      if (equ.d < -PI/2 || equ.d > PI/2) {
        equ.r += (PI - degToRad(360.0));
        equ.d =   PI - equ.d;
      }
      if (equ.d >  PI) equ.d -= PI*2; else
      if (equ.d < -PI) equ.d += PI*2;
      return equ;
    }

    // converts from mount to instrument coordinates
    EquCoordinate equMountToInstrument(EquCoordinate equ) {
      if (equ.p == PIER_SIDE_WEST) equ.r += PI;
      if (site.latitude.value >= 0.0) {
        if (equ.p == PIER_SIDE_WEST) equ.d =   PI  - equ.d;
      } else {
        if (equ.p == PIER_SIDE_WEST) equ.d = (-PI) - equ.d;
      }
      if (equ.d >  PI*2) equ.d -= PI*2; else
      if (equ.d < -PI*2) equ.d += PI*2;
      return equ;
    }

    PierSide mountToPierSide(EquCoordinate equ) {
      if (equ.d < -PI/2 || equ.d > PI/2) return PIER_SIDE_WEST; else return PIER_SIDE_EAST;
    }

    // converts from the topocentric to observed
    EquCoordinate topocentricToObservedPlace(EquCoordinate equ) {
      HorCoordinate hor;

      // within about 1/20 arc-second of NCP or SCP
    #if TOPOCENTRIC_STRICT == ON
      if (fabs(equ.d - PI/2) < SmallestRad) { hor.z = 0.0; hor.a =  site.latitude.value; } else
      if (fabs(equ.d + PI/2) < SmallestRad) { hor.z = PI;  hor.a = -site.latitude.value; } else hor = equToHor(equ);
    #else
      if (fabs(equ.d - PI/2) < SmallestRad || fabs(equ.d + PI/2) < SmallestRad) return equ; else hor = equToHor(equ);
    #endif

      hor.a += trueRefrac(hor.a);
      return horToEqu(hor);
    }
    
    // converts from the "Observed" to "Topocentric"
    EquCoordinate observedPlaceToTopocentric(EquCoordinate equ) {
      HorCoordinate hor;
      
      // within about 1/20 arc-second of the "refracted" NCP or SCP
    #if TOPOCENTRIC_STRICT == ON
      if (fabs(equ.d - PI/2) < SmallestRad) { hor.z = 0.0;   hor.a =  site.latitude.value; } else
      if (fabs(equ.d + PI/2) < SmallestRad) { hor.z = 180.0; hor.a = -site.latitude.value; } else hor = equToHor(equ);
    #else  
      if (fabs(equ.d - PI/2) < SmallestRad || fabs(equ.d + PI/2) < SmallestRad) return equ; else hor = equToHor(equ);
    #endif

      hor.a -= apparentRefrac(hor.a);
      return horToEqu(hor);
    }
    
    void hourAngleToRightAscension(EquCoordinate *equ) {
      noInterrupts();
      unsigned long cs = centisecondLAST;
      interrupts();
      equ->r = csToRad(cs) - equ->h;
      equ->r = backInRads(equ->r);
    }
    
    void rightAscensionToHourAngle(EquCoordinate *equ) {
      noInterrupts();
      unsigned long cs = centisecondLAST;
      interrupts();
      equ->h = csToRad(cs) - equ->r;
      equ->h = backInRads(equ->h);
    }

    // convert equatorial coordinates to horizon
    HorCoordinate equToHor(EquCoordinate equ) {
      HorCoordinate hor;
      double cosHA  = cos(equ.h);
      double sinAlt = (sin(equ.d)*site.latitude.sine) + (cos(equ.d)*site.latitude.cosine*cosHA);  
      hor.a         = asin(sinAlt);
      double t1     = sin(equ.h);
      double t2     = cosHA*site.latitude.sine - tan(equ.d)*site.latitude.cosine;
      hor.z         = atan2(t1,t2);
      hor.z        += PI;
      return hor;
    }
          
    // convert horizon coordinates to equatorial
    EquCoordinate horToEqu(HorCoordinate hor) { 
      EquCoordinate equ;
      double cosAzm = cos(hor.z);
      double sinDec = (sin(hor.a)*site.latitude.sine) + (cos(hor.a)*site.latitude.cosine*cosAzm);  
      equ.d         = asin(sinDec); 
      double t1     = sin(hor.z);
      double t2     = cosAzm*site.latitude.sine - tan(hor.a)*site.latitude.cosine;
      equ.h         = atan2(t1,t2);
      equ.h        += PI;
      return equ;
    }

    // refraction, returns the amount of refraction at the given true altitude (radians), pressure (millibars), and temperature (celsius)
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
    
    // refraction, returns the amount of refraction at the given apparent altitude (radians), pressure (millibars), and temperature (celsius)
    double apparentRefrac(double altitude, double pressure = 1010.0, double temperature = 10.0) {
      if (isnan(pressure)) pressure = 1010.0;
      if (isnan(temperature)) temperature = 10.0;
      double r = trueRefrac(altitude, pressure, temperature);
      return trueRefrac(altitude - r, pressure, temperature);
    }

  private:
    LI site;

    double cot(double n) {
      return 1.0/tan(n);
    }

    double backInRads(double angle) {
      double PI2 = PI*2.0;
      while (angle >= PI2) angle -= PI2;
      while (angle < 0.0)  angle += PI2;
      return angle;
    }
};

// instantiate
Transform transform;
