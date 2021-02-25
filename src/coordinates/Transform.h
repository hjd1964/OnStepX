//--------------------------------------------------------------------------------------------------
// coordinate transformation
#pragma once
#include "Convert.h"

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
    PierSide pierSide;
} Coordinate;

typedef struct Latitude {
  double   value;
  double   sine;
  double   cosine;
  double   absval;
  double   sign;
} Latitude;

typedef struct Site {
  Latitude latitude;
  double   longitude;
  bool     ready;
} Site;

class Transform {
  public:
    Site site;

    // setup for coordinate transformation
    void init(int mountType);

    // converts between Mount (equatorial or horizon) and Native (equatorial and optionally horizon also) coordinates
    // returns equatorial RA
    Coordinate mountToNative(Coordinate *coord, bool returnHorizonCoords = false);
    // converts between Native (equatorial) and Mount (equatorial or horizon) coordinates (optional: a1 is h or z, a2 is d or a)
    // accepts equatorial RA
    void nativeToMount(Coordinate *coord, double *a1 = NULL, double *a2 = NULL);

    // converts between Mount and Topocentric coordinates (removes pointing model and refraction from equatorial coordinates)
    void equMountToTopocentric(Coordinate *coord);
    // converts between Topocentric and Mount coordinates (adds pointing model and refraction to equatorial coordinates)
    void topocentricToEquMount(Coordinate *coord);

    // converts between Mount and Observed coordinates (removes pointing model from equatorial coordinates)
    void equMountToObservedPlace(Coordinate *coord);
    // converts between Observed and Mount coordinates (adds pointing model to equatorial coordinates)
    void observedPlaceToEquMount(Coordinate *coord);

    // converts between Instrument (angular) and Mount (equatorial or horizon) coordinates
    Coordinate instrumentToMount(double a1, double a2);
    // converts between Mount (equatorial or horizon) and Instrument (angular) coordinates
    void mountToInstrument(Coordinate *coord, double *a1, double *a2);

    // converts between Topocentric and Observed coordinates (removes refraction effects from equatorial coordinates)
    void topocentricToObservedPlace(Coordinate *coord);
    // converts between Observed and Topocentric coordinates (adds refraction effects to equatorial coordinates)
    void observedPlaceToTopocentric(Coordinate *coord);

    // converts between Hour Angle (h) and Right Ascension (r) coordinates
    void hourAngleToRightAscension(Coordinate *coord);
    // converts between Right Ascension (r) and Hour Angle (h) coordinates
    void rightAscensionToHourAngle(Coordinate *coord);

    // convert between Equatorial (h,d) and Horizon (a,z) coordinates
    void equToHor(Coordinate *coord);
    // convert between Equatorial (h,d) and Horizon (a,z) coordinates
    void horToEqu(Coordinate *coord);

    // refraction at altitude, pressure (millibars), and temperature (celsius)
    // returns amount of refraction at the true altitude
    double trueRefrac(double altitude, double pressure = 1010.0, double temperature = 10.0);
    // refraction at altitude, pressure (millibars), and temperature (celsius)
    // returns the amount of refraction at the apparent altitude
    double apparentRefrac(double altitude, double pressure = 1010.0, double temperature = 10.0);

  private:
    int mountType;

    double cot(double n);
    
    // adjust coordinate back into 0 to 360 "degrees" range (in radians)
    double backInRads(double angle);
    // adjust coordinate back into -180 to 180 "degrees" range (in radians)
    double backInRads2(double angle);

    Convert convert;
};
