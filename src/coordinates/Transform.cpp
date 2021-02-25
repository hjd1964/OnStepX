//--------------------------------------------------------------------------------------------------
// coordinate transformation
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;
#include "Transform.h"

Transform transform;
extern volatile unsigned long centisecondLAST;

void Transform::init(int mountType) {
  this->mountType = mountType;
}

Coordinate Transform::mountToNative(Coordinate *coord, bool returnHorizonCoords) {
  Coordinate result = *coord;
  #if MOUNT_COORDS == OBSERVED
    mountToObservedPlace(&result);
  #elif MOUNT_COORDS == TOPOCENTRIC || MOUNT_COORDS == TOPO_STRICT
    mountToTopocentric(&result);
  #else
    #error "Configuration (ConfigX.h): MOUNT_COORDS, Unknown native mount coordinate system!"
  #endif
  hourAngleToRightAscension(&result);
  if (returnHorizonCoords) equToHor(&result);
  return result;
}

void Transform::nativeToMount(Coordinate *coord, double *a1, double *a2) {
  rightAscensionToHourAngle(coord);
  #if MOUNT_COORDS == OBSERVED
    observedPlaceToMount(coord);
  #elif MOUNT_COORDS == TOPOCENTRIC || MOUNT_COORDS == TOPO_STRICT
    topocentricToMount(coord);
  #else
    #error "Configuration (ConfigX.h): MOUNT_COORDS, Unknown native mount coordinate system!"
  #endif
  if (a1 != NULL && a2 != NULL) {
    if (mountType == ALTAZM) { *a1 = coord->z; *a2 = coord->a; } else { *a1 = coord->h; *a2 = coord->d; }
  }
}

void Transform::mountToTopocentric(Coordinate *coord) {
  mountToObservedPlace(coord);
  observedPlaceToTopocentric(coord);
}

void Transform::topocentricToMount(Coordinate *coord) {
  topocentricToObservedPlace(coord);
  observedPlaceToMount(coord);
}

void Transform::mountToObservedPlace(Coordinate *coord) {
  // apply the pointing model
  if (mountType == ALTAZM) horToEqu(coord);
}

void Transform::observedPlaceToMount(Coordinate *coord) {
  if (mountType == ALTAZM) equToHor(coord);
  // de-apply the pointing model
}

// converts from instrument to mount coordinates
Coordinate Transform::instrumentToMount(double a1, double a2) {
  Coordinate mount;

  if (a2 < -Deg90 || a2 > Deg90) {
    mount.pierSide = PIER_SIDE_WEST;
    a1 -= Deg180;
    a2  = Deg180 - a2;
  } else mount.pierSide = PIER_SIDE_EAST;

  if (a2 > Deg180) a2 -= Deg360; else if (a2 <= -Deg180) a2 += Deg360;

  if (mountType == ALTAZM) { mount.z = a1; mount.a = a2; } else { mount.h = a1; mount.d = a2; }
  return mount;
}

void Transform::mountToInstrument(Coordinate *coord, double *a1, double *a2) {
  if (mountType == ALTAZM) { *a1 = coord->z; *a2 = coord->a; } else { *a1 = coord->h; *a2 = coord->d; }
  
  if (coord->pierSide == PIER_SIDE_WEST) *a1 += Deg180;
  if (site.latitude.value >= 0.0) {
    if (coord->pierSide == PIER_SIDE_WEST) *a2 = Deg180 - *a2;
  } else {
    if (coord->pierSide == PIER_SIDE_WEST) *a2 = (-Deg180) - *a2;
  }
  if (*a2 >  Deg360) *a2 -= Deg360; else if (*a2 < -Deg360) *a2 += Deg360;
}

void Transform::topocentricToObservedPlace(Coordinate *coord) {
  if (mountType != ALTAZM) {
    // within about 1/20 arc-second of NCP or SCP
    #if MOUNT_COORDS == TOPO_STRICT
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
    #if MOUNT_COORDS == TOPO_STRICT
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
  coord->h = backInRads2(coord->h);
}

void Transform::equToHor(Coordinate *coord) {
  double cosHA  = cos(coord->h);
  double sinAlt = sin(coord->d)*site.latitude.sine + cos(coord->d)*site.latitude.cosine*cosHA;  
  coord->a      = asin(sinAlt);
  double t1     = sin(coord->h);
  double t2     = cosHA*site.latitude.sine - tan(coord->d)*site.latitude.cosine;
  coord->z      = atan2(t1,t2);
  coord->z      += PI;
  if (coord->z > PI) coord->z -= TWO_PI;
}

void Transform::horToEqu(Coordinate *coord) { 
  double cosAzm = cos(coord->z);
  double sinDec = sin(coord->a)*site.latitude.sine + cos(coord->a)*site.latitude.cosine*cosAzm;  
  coord->d      = asin(sinDec); 
  double t1     = sin(coord->z);
  double t2     = cosAzm*site.latitude.sine - tan(coord->a)*site.latitude.cosine;
  coord->h      = atan2(t1,t2);
  coord->h     += PI;
  if (coord->h > PI) coord->h -= TWO_PI;
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

double Transform::backInRads2(double angle) {
  while (angle >= Deg180) angle -= Deg360;
  while (angle < -Deg180) angle += Deg360;
  return angle;
}
