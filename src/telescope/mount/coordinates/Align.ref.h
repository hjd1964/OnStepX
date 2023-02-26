// -----------------------------------------------------------------------------------
// GOTO ASSIST GEOMETRIC ALIGN FOR ALT/AZM AND EQ MOUNTS (REFERENCE DESIGN)
//
// by Howard Dutton
//
// Copyright (C) 2012 to 2021 Howard Dutton
//
#pragma once

#include "../../../Common.h"

#if defined(MOUNT_PRESENT) && !defined(HIGH_SPEED_ALIGN)

#include "../../../libApp/commands/ProcessCmds.h"

#if defined(ALIGN_MAX_STARS) && ALIGN_MAX_STARS != AUTO
  #if (ALIGN_MAX_STARS < 3 || ALIGN_MAX_STARS > 9) && ALIGN_MAX_STARS != 1
    #error "ALIGN_MAX_STARS must be 1, or in the range of 3 to 9"
  #endif
  #define ALIGN_MAX_NUM_STARS ALIGN_MAX_STARS
#else
  #if defined(HAL_FAST_PROCESSOR)
    #define ALIGN_MAX_NUM_STARS 9
  #else
    #define ALIGN_MAX_NUM_STARS 6
  #endif
#endif

enum PierSide: uint8_t {PIER_SIDE_NONE, PIER_SIDE_EAST, PIER_SIDE_WEST};

typedef struct Coordinate {
  double r;
  double h;
  double d;
  double a;
  double z;
  double a1;
  double a2;
  PierSide pierSide;
} Coordinate;

#if ALIGN_MAX_NUM_STARS > 1

// -----------------------------------------------------------------------------------
// ADVANCED GEOMETRIC ALIGN FOR EQUATORIAL MOUNTS (GOTO ASSIST)

typedef struct AlignCoordinate {
  float ax1;
  float ax2;
  float h;
  float d;
  int side;
} AlignCoordinate;

#define AlignModelSize 32
typedef struct AlignModel {
  float ax1Cor;
  float ax2Cor;
  float altCor;
  float azmCor;
  float doCor;
  float pdCor;
  float dfCor;
  float tfCor;
} AlignModel;

class GeoAlign
{
  public:
    // prepare goto assist for operation, also clears the alignment model;
    void init(int8_t mountType, float latitude);

    // reads the last saved alignment model from NV
    void modelRead();
    // writes the alignment model to NV
    void modelWrite();
    // clear the alignment model
    void modelClear();
    // reports if ready for operation
    bool modelReady();

    // add a star to an alignment model
    // thisStar: 1 for 1st star, 2 for 2nd star, etc. up to numberStars (at which point the mount model is calculated)
    // numberStars: total number of stars for this align (1 to 9)
    // actual: equatorial or horizon coordinate (depending on the mount type) for where the star should be (in mount coordinates)
    // mount:  equatorial or horizon coordinate (depending on the mount type) for where the star is (in mount coordinates)
    CommandError addStar(int thisStar, int numberStars, Coordinate *actual, Coordinate *mount);

    void createModel(int numberStars);
    
    // convert equatorial (h,d) or horizon (a,z) coordinate from observed place to mount
    void observedPlaceToMount(Coordinate *coord);
    // convert equatorial (h,d) or horizon (a,z) coordinate from mount to observed place
    void mountToObservedPlace(Coordinate *coord);

    void autoModel(int n);

    AlignCoordinate mount[ALIGN_MAX_NUM_STARS];
    AlignCoordinate actual[ALIGN_MAX_NUM_STARS];
    AlignCoordinate delta[ALIGN_MAX_NUM_STARS];
    AlignModel model;

  private:
    void correct(float ha, float dec, float pierSide, float sf, float _deo, float _pd, float _pz, float _pe, float _da, float _ff, float _tf, float *h1, float *d1);
    void doSearch(float sf, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9);

    bool modelIsReady = false;
    int8_t mountType;
    float cosLat, sinLat;

    long num, l;
    long Ff, Df;
    float best_deo, best_pd, best_pz, best_pe, best_ohw, best_odw, best_ohe, best_ode, best_tf, best_df, best_ff;
    float avg_ha, avg_dec;
    float dist, sumd, rms;
    float best_dist;
    float ohe, ode, ohw, odw, dh;
    float sum1;
    float max_dist;

    uint8_t autoModelTask = 0;
};

#endif
#endif
