//--------------------------------------------------------------------------------------------------
// coordinate transformation
#pragma once

// MOTOR      <--> apply index offset and backlash        <--> INSTRUMENT  (Axis)
// INSTRUMENT <--> apply celestial coordinate conventions <--> MOUNT       (Transform)
// MOUNT      <--> apply pointing model                   <--> OBSERVED    (Transform)
// OBSERVED   <--> apply refraction                       <--> TOPOCENTRIC (Transform)

enum PierSide {PIER_SIDE_NONE, PIER_SIDE_EAST, PIER_SIDE_WEST};

typedef struct Coordinate {
    double r;
    double h;
    double d;
    double a;
    double z;
    PierSide p;
} Coordinate;

class Transform {
  public:

    // setup for coordinate transformation
    void setSite(LI site);

    // converts between Mount and Native coordinates
    Coordinate mountToNative(Coordinate *coord);
    
    // converts between Mount and Topocentric coordinates
    void equMountToTopocentric(Coordinate *coord);
    void topocentricToEquMount(Coordinate *coord);

    // converts between Mount and Observed coordinates
    void equMountToObservedPlace(Coordinate *coord);
    void observedPlaceToEquMount(Coordinate *coord);

    // converts between Mount and Instrument coordinates
    Coordinate instrumentToMount(double a1, double a2);
    void mountToInstrument(Coordinate *coord, double *a1, double *a2);

    // converts between Topocentric and Observed coordinates
    void topocentricToObservedPlace(Coordinate *coord);
    void observedPlaceToTopocentric(Coordinate *coord);

    // converts between Hour Angle and Right Ascension coordinates
    void hourAngleToRightAscension(Coordinate *coord);
    void rightAscensionToHourAngle(Coordinate *coord);

    // convert between Equatorial and Horizon coordinates
    void equToHor(Coordinate *coord);
    void horToEqu(Coordinate *coord);

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

Coordinate Transform::mountToNative(Coordinate *coord) {
  Coordinate result = *coord;
  if (mountType == ALTAZM) horToEqu(&result);
#if TELESCOPE_COORDINATES == OBSERVED
  equMountToObservedPlace(&result);
#elif TELESCOPE_COORDINATES == TOPOCENTRIC
  equMountToTopocentric(&result);
#else
  #error "Configuration (Constants.h): Unknown TELESCOPE_COORDINATES!"
#endif
  hourAngleToRightAscension(&result);
  return result;
}

void Transform::equMountToTopocentric(Coordinate *coord) {
  equMountToObservedPlace(coord);
  observedPlaceToTopocentric(coord);
}

void Transform::topocentricToEquMount(Coordinate *coord) {
  topocentricToObservedPlace(coord);
  observedPlaceToEquMount(coord);
}

void Transform::equMountToObservedPlace(Coordinate *coord) {
  // apply the pointing model
  // convert HA into RA
}

void Transform::observedPlaceToEquMount(Coordinate *coord) {
  // convert RA into HA
  // de-apply the pointing model
}

// converts from instrument to mount coordinates
Coordinate Transform::instrumentToMount(double a1, double a2) {
  Coordinate mount;

  if (a2 < -Deg90 || a2 > Deg90) {
    a1 -= Deg180;
    a2  = Deg180 - a2;
  }
  if (a2 >  Deg180) a2 -= Deg360; else
  if (a2 < -Deg180) a2 += Deg360;

  if (a2 < -Deg90 || a2 > Deg90) mount.p = PIER_SIDE_WEST; else mount.p = PIER_SIDE_EAST;

  if (mountType == ALTAZM) { mount.z = a1; mount.a = a2; } else { mount.h = a1; mount.d = a2; }
  return mount;
}

void Transform::mountToInstrument(Coordinate *coord, double *a1, double *a2) {
  if (mountType == ALTAZM) { *a1 = coord->z; *a2 = coord->a; } else { *a1 = coord->h; *a2 = coord->d; }
  
  if (coord->p == PIER_SIDE_WEST) *a1 += PI;
  if (site.latitude.value >= 0.0) {
    if (coord->p == PIER_SIDE_WEST) *a2 =   Deg180  - *a2;
  } else {
    if (coord->p == PIER_SIDE_WEST) *a2 = (-Deg180) - *a2;
  }
  if (*a2 >  Deg360) *a2 -= Deg360; else
  if (*a2 < -Deg360) *a2 += Deg360;
}

void Transform::topocentricToObservedPlace(Coordinate *coord) {
  if (mountType != ALTAZM) {
    // within about 1/20 arc-second of NCP or SCP
    #if TOPOCENTRIC_STRICT == ON
      if (fabs(coord->d - Deg90) < SmallestRad) { coord->z = 0.0;    coord->a =  site.latitude.value; } else
      if (fabs(coord->d + Deg90) < SmallestRad) { coord->z = Deg180; coord->a = -site.latitude.value; } else equToHor(coord);
    #else
      if (fabs(coord->d - Deg90) < SmallestRad || fabs(coord->d + Deg90) < SmallestRad) return; else equToHor(coord);
    #endif
  }
  coord->a += trueRefrac(coord->a);
  horToEqu(coord);
}

void Transform::observedPlaceToTopocentric(Coordinate *coord) {
  if (mountType != ALTAZM) {
    // within about 1/20 arc-second of the "refracted" NCP or SCP
    #if TOPOCENTRIC_STRICT == ON
      if (fabs(coord->d - Deg90) < SmallestRad) { coord->z = 0.0;    coord->a =  site.latitude.value; } else
      if (fabs(coord->d + Deg90) < SmallestRad) { coord->z = Deg180; coord->a = -site.latitude.value; } else equToHor(coord);
    #else  
      if (fabs(coord->d - Deg90) < SmallestRad || fabs(coord->d + Deg90) < SmallestRad) return; else equToHor(coord);
    #endif
  }
  coord->a -= apparentRefrac(coord->a);
  horToEqu(coord);
}

void Transform::hourAngleToRightAscension(Coordinate *coord) {
  noInterrupts();
  unsigned long cs = centisecondLAST;
  interrupts();
  coord->r = csToRad(cs) - coord->h;
  coord->r = backInRads(coord->r);
}

void Transform::rightAscensionToHourAngle(Coordinate *coord) {
  noInterrupts();
  unsigned long cs = centisecondLAST;
  interrupts();
  coord->h = csToRad(cs) - coord->r;
  coord->h = backInRads(coord->h);
}

void Transform::equToHor(Coordinate *coord) {
  rightAscensionToHourAngle(coord);
  double cosHA  = cos(coord->h);
  double sinAlt = sin(coord->d)*site.latitude.sine + cos(coord->d)*site.latitude.cosine*cosHA;  
  coord->a      = asin(sinAlt);
  double t1     = sin(coord->h);
  double t2     = cosHA*site.latitude.sine - tan(coord->d)*site.latitude.cosine;
  coord->z      = atan2(t1,t2);
  coord->z      += PI;
}

void Transform::horToEqu(Coordinate *coord) { 
  double cosAzm = cos(coord->z);
  double sinDec = sin(coord->a)*site.latitude.sine + cos(coord->a)*site.latitude.cosine*cosAzm;  
  coord->d      = asin(sinDec); 
  double t1     = sin(coord->z);
  double t2     = cosAzm*site.latitude.sine - tan(coord->a)*site.latitude.cosine;
  coord->h      = atan2(t1,t2);
  coord->h     += PI;
  hourAngleToRightAscension(coord);
}

double Transform::trueRefrac(double altitude, double pressure, double temperature) {
  if (isnan(pressure)) pressure = 1010.0;
  if (isnan(temperature)) temperature = 10.0;
  double TPC = (pressure/1010.0)*(283.0/(273.0 + temperature));
  double r   = 2.96705972855e-4*cot(altitude + 0.17977/(altitude + 0.08919))*TPC;
  if (r < 0.0) r = 0.0;
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
  while (angle >= Deg360) angle -= Deg360;
  while (angle < 0.0)     angle += Deg360;
  return angle;
}

Transform transform;
