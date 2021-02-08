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

    // setup for coordinate transformation
    void setSite(LI site);

    // converts between Instrument and Native coordinates, including Pier side
    EquCoordinate equInstrumentToNative(EquCoordinate equ);
    EquCoordinate nativeToEquInstrument(EquCoordinate equ);
    PierSide mountToPierSide(EquCoordinate equ);

    // converts between Mount and Topocentric coordinates
    EquCoordinate equMountToTopocentric(EquCoordinate equ);
    EquCoordinate topocentricToEquMount(EquCoordinate equ);

    // converts between Mount and Observed coordinates
    EquCoordinate equMountToObservedPlace(EquCoordinate equ);
    EquCoordinate observedPlaceToEquMount(EquCoordinate equ);

    // converts between Mount and Instrument coordinates
    EquCoordinate equInstrumentToMount(EquCoordinate equ);
    EquCoordinate equMountToInstrument(EquCoordinate equ);

    // converts between Topocentric and Observed coordinates
    EquCoordinate topocentricToObservedPlace(EquCoordinate equ);
    EquCoordinate observedPlaceToTopocentric(EquCoordinate equ);
    
    // converts between Hour Angle and Right Ascension coordinates
    void hourAngleToRightAscension(EquCoordinate *equ);
    void rightAscensionToHourAngle(EquCoordinate *equ);

    // convert between Equatorial and Horizon coordinates
    HorCoordinate equToHor(EquCoordinate equ);
    EquCoordinate horToEqu(HorCoordinate hor);

    // refraction at altitude (radians), pressure (millibars), and temperature (celsius)
    // returns amount of refraction at the true altitude
    double trueRefrac(double altitude, double pressure = 1010.0, double temperature = 10.0);
    // returns the amount of refraction at the apparent altitude
    double apparentRefrac(double altitude, double pressure = 1010.0, double temperature = 10.0);

  private:
    LI site;

    double cot(double n);

    double backInRads(double angle);
};

void Transform::setSite(LI site) {
  this->site = site;
}

EquCoordinate Transform::equInstrumentToNative(EquCoordinate equ) {
  EquCoordinate mount = equInstrumentToMount(equ);
  #if TELESCOPE_COORDINATES == TOPOCENTRIC
    return equMountToObservedPlace(mount);
  #elif TELESCOPE_COORDINATES == OBSERVED
    return equMountToTopocentric(mount);
  #else
    #error "Configuration (Constants.h): TELESCOPE_COORDINATES invalid!"
  #endif
}

EquCoordinate Transform::nativeToEquInstrument(EquCoordinate equ) {
  #if TELESCOPE_COORDINATES == TOPOCENTRIC
    EquCoordinate mount = observedPlaceToEquMount(equ);
  #elif TELESCOPE_COORDINATES == OBSERVED
    EquCoordinate mount =  topocentricToEquMount(equ);
  #endif
  return equMountToInstrument(mount);
}

PierSide Transform::mountToPierSide(EquCoordinate equ) {
  if (equ.d < -PI/2 || equ.d > PI/2) return PIER_SIDE_WEST; else return PIER_SIDE_EAST;
}

EquCoordinate Transform::equMountToTopocentric(EquCoordinate equ) {
  EquCoordinate obs=equMountToObservedPlace(equ);
  return observedPlaceToTopocentric(obs);
}

EquCoordinate Transform::topocentricToEquMount(EquCoordinate equ) {
  EquCoordinate obs=topocentricToObservedPlace(equ);
  return observedPlaceToEquMount(obs);
}

EquCoordinate Transform::equMountToObservedPlace(EquCoordinate equ) {
  // apply the pointing model
  // convert HA into RA
  return equ;
}

EquCoordinate Transform::observedPlaceToEquMount(EquCoordinate equ) {
  // convert RA into HA
  // de-apply the pointing model
  return equ;
}

// converts from instrument to mount coordinates
EquCoordinate Transform::equInstrumentToMount(EquCoordinate equ) {
  if (equ.d < -PI/2 || equ.d > PI/2) {
    equ.r += (PI - degToRad(360.0));
    equ.d =   PI - equ.d;
  }
  if (equ.d >  PI) equ.d -= PI*2; else
  if (equ.d < -PI) equ.d += PI*2;
  return equ;
}

EquCoordinate Transform::equMountToInstrument(EquCoordinate equ) {
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

EquCoordinate Transform::topocentricToObservedPlace(EquCoordinate equ) {
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

EquCoordinate Transform::observedPlaceToTopocentric(EquCoordinate equ) {
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

void Transform::hourAngleToRightAscension(EquCoordinate *equ) {
  noInterrupts();
  unsigned long cs = centisecondLAST;
  interrupts();
  equ->r = csToRad(cs) - equ->h;
  equ->r = backInRads(equ->r);
}

void Transform::rightAscensionToHourAngle(EquCoordinate *equ) {
  noInterrupts();
  unsigned long cs = centisecondLAST;
  interrupts();
  equ->h = csToRad(cs) - equ->r;
  equ->h = backInRads(equ->h);
}

HorCoordinate Transform::equToHor(EquCoordinate equ) {
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
      
EquCoordinate Transform::horToEqu(HorCoordinate hor) { 
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

double Transform::trueRefrac(double altitude, double pressure, double temperature) {
  if (isnan(pressure)) pressure = 1010.0;
  if (isnan(temperature)) temperature = 10.0;

//    double TPC = (pressure/1010.0)*(283.0/(273.0+temperature));
//    double r   = 1.02*cot( (Alt+(10.3/(Alt+5.11)))/Rad ) * TPC;  // r is in arc-minutes

  double TPC = (pressure/1010.0)*(283.0/(273.0 + temperature));
  double r   = 2.96705972855e-4*cot(altitude + 0.17977/(altitude + 0.08919))*TPC;

  if (r < 0.0) r=0.0;
  return r;
}

double Transform::apparentRefrac(double altitude, double pressure, double temperature) {
  if (isnan(pressure)) pressure = 1010.0;
  if (isnan(temperature)) temperature = 10.0;
  double r = trueRefrac(altitude, pressure, temperature);
  return trueRefrac(altitude - r, pressure, temperature);
}

double Transform::cot(double n) {
  return 1.0/tan(n);
}

double Transform::backInRads(double angle) {
  double PI2 = PI*2.0;
  while (angle >= PI2) angle -= PI2;
  while (angle < 0.0)  angle += PI2;
  return angle;
}

Transform transform;
