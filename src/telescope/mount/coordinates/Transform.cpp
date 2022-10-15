//--------------------------------------------------------------------------------------------------
// coordinate transformation

#include "Transform.h"

#ifdef MOUNT_PRESENT

#include "../../../libApp/weather/Weather.h"
#include "../../Telescope.h"

extern volatile unsigned long fracLAST;

#define fsToRad(x) ((x)/(13750.98708313976*FRACTIONAL_SEC))
#define radToFs(x) ((x)*(13750.98708313976*FRACTIONAL_SEC))

#if DEBUG != OFF
  void Transform::print(Coordinate *coord) {
    VF("(a="); V(radToDeg(coord->a)); VF(", z="); V(radToDeg(coord->z));
    VF("), (r="); V(radToDeg(coord->r)); VF(", h="); V(radToDeg(coord->h)); VF(", d="); V(radToDeg(coord->d));
    VF("), pierSide="); VL(coord->pierSide);
  }
#endif

void Transform::init() {
  // NV_AXIS_SETTINGS_REVERT bit 0 = settings at compile (0) or run time (1), bits 1 to 9 = reset axis n on next boot
  bool revert = !(nv.readUI(NV_AXIS_SETTINGS_REVERT) & 1);

  // write axis settings to NV
  if (!nv.hasValidKey() || revert) {
    nv.write(NV_MOUNT_TYPE_BASE, (uint8_t)MOUNT_SUBTYPE);
  }
  mountType = nv.readUC(NV_MOUNT_TYPE_BASE);
  if (mountType == 0) {
    mountType = MOUNT_SUBTYPE;
    nv.write(NV_MOUNT_TYPE_BASE, (uint8_t)MOUNT_SUBTYPE);
    VLF("MSG: Transform, revert mount type to default");
  } else
  if (mountType < GEM || mountType > ALTAZM) {
    mountType = MOUNT_SUBTYPE;
    initError.value = true;
    VLF("WRN: Transform, unknown mount type reverting to default");
  }

  #if DEBUG == VERBOSE
    const char* MountTypeStr[4] = {"", "GEM", "FORK", "ALTAZM"};
    VF("MSG: Mount, type "); VL(MountTypeStr[mountType]);
  #endif

  if (mountType == ALTAZM) meridianFlips = false; else meridianFlips = true;

  #if ALIGN_MAX_NUM_STARS > 1
    align.init(mountType, site.location.latitude);
  #endif
}

Coordinate Transform::mountToNative(Coordinate *coord, bool returnHorizonCoords) {
  Coordinate result = *coord;
  
  #if MOUNT_COORDS == OBSERVED_PLACE
    mountToObservedPlace(&result);
  #elif MOUNT_COORDS == TOPOCENTRIC || MOUNT_COORDS == TOPO_STRICT
    mountToTopocentric(&result);
  #else
    #error "Configuration (Config.h): MOUNT_COORDS, Unknown native mount coordinate system!"
  #endif

  if (mountType == ALTAZM) {
    horToEqu(&result);
  } else {
    if (returnHorizonCoords) equToHor(&result);
  }

  hourAngleToRightAscension(&result, true);
  return result;
}

void Transform::nativeToMount(Coordinate *coord, double *a1, double *a2) {
  rightAscensionToHourAngle(coord, true);

  if (mountType == ALTAZM) equToHor(coord);

  #if MOUNT_COORDS == OBSERVED_PLACE
    observedPlaceToMount(coord);
  #elif MOUNT_COORDS == TOPOCENTRIC || MOUNT_COORDS == TOPO_STRICT
    topocentricToMount(coord);
  #else
    #error "Configuration (Config.h): MOUNT_COORDS, Unknown native mount coordinate system!"
  #endif

  if (a1 != NULL && a2 != NULL) {
    if (mountType == ALTAZM) { *a1 = coord->z; *a2 = coord->a; } else { *a1 = coord->h; *a2 = coord->d; }
  }
}

void Transform::mountToTopocentric(Coordinate *coord) {
  mountToObservedPlace(coord);
  observedPlaceToTopocentric(coord);
}

void Transform::mountToObservedPlace(Coordinate *coord) {
  #if ALIGN_MAX_NUM_STARS > 1
    if (coord->pierSide != PIER_SIDE_NONE) align.mountToObservedPlace(coord);
  #else
    (void)(*coord);
  #endif
}

void Transform::observedPlaceToTopocentric(Coordinate *coord) {
  if (mountType != ALTAZM) {
    // within about 1/20 arc-second of the "refracted" NCP or SCP
    #if MOUNT_COORDS == TOPO_STRICT
      if (fabs(coord->d - Deg90) < OneArcSec) { coord->z = 0.0;    coord->a =  site.latitude.value; } else
      if (fabs(coord->d + Deg90) < OneArcSec) { coord->z = Deg180; coord->a = -site.latitude.value; } else equToHor(coord);
    #else  
      if (fabs(coord->d - Deg90) < OneArcSec || fabs(coord->d + Deg90) < OneArcSec) return; else equToHor(coord);
    #endif
    coord->a -= apparentRefrac(coord->a);
    horToEqu(coord);
  } else coord->a -= apparentRefrac(coord->a);
}

void Transform::topocentricToMount(Coordinate *coord) {
  topocentricToObservedPlace(coord);
  observedPlaceToMount(coord);
}

void Transform::observedPlaceToMount(Coordinate *coord) {
  #if ALIGN_MAX_NUM_STARS > 1
    if (coord->pierSide != PIER_SIDE_NONE) align.observedPlaceToMount(coord);
  #else
    (void)(*coord);
  #endif

  if (mountType == ALTAZM) {
    coord->a1 = coord->z;
    coord->a2 = coord->a;
  } else {
    coord->a1 = coord->h;
    coord->a2 = coord->d;
  }
}

void Transform::topocentricToObservedPlace(Coordinate *coord) {
  if (mountType != ALTAZM) {
    // within about 1/20 arc-second of NCP or SCP
    #if MOUNT_COORDS == TOPO_STRICT
      if (fabs(coord->d - Deg90) < OneArcSec) { coord->z = 0.0;    coord->a =  site.latitude.value; } else
      if (fabs(coord->d + Deg90) < OneArcSec) { coord->z = Deg180; coord->a = -site.latitude.value; } else equToHor(coord);
    #else
      if (fabs(coord->d - Deg90) < OneArcSec || fabs(coord->d + Deg90) < OneArcSec) return; else equToHor(coord);
    #endif
    coord->a += trueRefrac(coord->a);
    horToEqu(coord);
  } else coord->a += trueRefrac(coord->a);
}

Coordinate Transform::instrumentToMount(double a1, double a2) {
  Coordinate mount;

  #if AXIS2_TANGENT_ARM_CORRECTION == ON
    a2 = TANGENT_ARM_INSTRUMENT_TO_MOUNT(a2)
  #endif

  if (a2 < -Deg90 || a2 > Deg90) {
    mount.pierSide = PIER_SIDE_WEST;
    a1 -= Deg180;
    a2  = Deg180 - a2;
  } else mount.pierSide = PIER_SIDE_EAST;

  if (a2 > Deg180) a2 -= Deg360; else if (a2 <= -Deg180) a2 += Deg360;

  if (mountType == ALTAZM) {
    mount.z = a1;
    mount.a = a2;
  } else {
    mount.h = a1;
    mount.d = a2;
  }

  mount.a1 = a1;
  mount.a2 = a2;

  return mount;
}

void Transform::mountToInstrument(Coordinate *coord, double *a1, double *a2) {
  if (mountType == ALTAZM) { *a1 = coord->z; *a2 = coord->a; } else { *a1 = coord->h; *a2 = coord->d; }

  if (coord->pierSide == PIER_SIDE_WEST) *a1 += Deg180;
  if (site.location.latitude >= 0.0) {
    if (coord->pierSide == PIER_SIDE_WEST) *a2 = Deg180 - *a2;
  } else {
    if (coord->pierSide == PIER_SIDE_WEST) *a2 = (-Deg180) - *a2;
  }
  if (*a2 >  Deg360) *a2 -= Deg360; else if (*a2 < -Deg360) *a2 += Deg360;

  #if AXIS2_TANGENT_ARM_CORRECTION == ON
    *a2 = TANGENT_ARM_MOUNT_TO_INSTRUMENT(*a2)
  #endif
}

void Transform::hourAngleToRightAscension(Coordinate *coord, bool native) {
  noInterrupts();
  unsigned long fs = fracLAST;
  interrupts();
  coord->r = fsToRad(fs) - coord->h;
  if (native) coord->r = backInRads(coord->r);
}

void Transform::rightAscensionToHourAngle(Coordinate *coord, bool native) {
  if (isnan(coord->r)) return; // NAN flags mount coordinates
  noInterrupts();
  unsigned long fs = fracLAST;
  interrupts();
  coord->h = fsToRad(fs) - coord->r;
  if (native) coord->h = backInRads2(coord->h);
}

void Transform::equToHor(Coordinate *coord) {
  double cosHA  = cos(coord->h);
  double sinAlt = sin(coord->d)*site.locationEx.latitude.sine + cos(coord->d)*site.locationEx.latitude.cosine*cosHA;  
  coord->a      = asin(sinAlt);
  double t1     = sin(coord->h);
  double t2     = cosHA*site.locationEx.latitude.sine - tan(coord->d)*site.locationEx.latitude.cosine;
  // handle degenerate coordinates near the poles
  if (fabs(coord->d - Deg90) < TenthArcSec) coord->z = 0.0; else
  if (fabs(coord->d + Deg90) < TenthArcSec) coord->z = Deg180; else {
    coord->z = atan2(t1, t2);
    coord->z += Deg180;
  }
  if (coord->z > Deg180) coord->z -= Deg360;
}

void Transform::equToAlt(Coordinate *coord) {
  double cosHA  = cos(coord->h);
  double sinAlt = sin(coord->d)*site.locationEx.latitude.sine + cos(coord->d)*site.locationEx.latitude.cosine*cosHA;  
  coord->a      = asin(sinAlt);
}

void Transform::horToEqu(Coordinate *coord) { 
  double cosAzm = cos(coord->z);
  double sinDec = sin(coord->a)*site.locationEx.latitude.sine + cos(coord->a)*site.locationEx.latitude.cosine*cosAzm;  
  coord->d      = asin(sinDec); 
  double t1     = sin(coord->z);
  double t2     = cosAzm*site.locationEx.latitude.sine - tan(coord->a)*site.locationEx.latitude.cosine;
  coord->h      = atan2(t1,t2);
  coord->h     += Deg180;
  if (coord->h > Deg180) coord->h -= Deg360;
}

double Transform::trueRefrac(double altitude) {
  float pressure = 1010.0F;
  float temperature = 10.0F;
  if (!isnan(weather.getPressure())) pressure = weather.getPressure();
  if (!isnan(weather.getTemperature())) temperature = weather.getTemperature();
  float TPC = (pressure/1010.0F)*(283.0F/(273.0F + temperature));
  float r   = 2.9670597e-4F*cotf(altitude + 0.0031375594F/(altitude + 0.089186324F))*TPC;
  if (r < 0.0F) r = 0.0F;
  return r;
}

double Transform::apparentRefrac(double altitude) {
  double r = trueRefrac(altitude);
  return trueRefrac(altitude - r);
}

float Transform::cotf(float n) {
  return 1.0F/tanf(n);
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

Transform transform;

#endif