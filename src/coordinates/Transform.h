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

    // converts between Mount and Native coordinates
    Coordinate mountToNative(Coordinate *coord, bool returnHorizonCoords = false);
    void nativeToMount(Coordinate *coord, double *a1 = NULL, double *a2 = NULL);
    
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
    int mountType;

    double cot(double n);
    
    double backInRads(double angle);

    Convert convert;
};
