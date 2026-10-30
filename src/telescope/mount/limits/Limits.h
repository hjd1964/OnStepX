//--------------------------------------------------------------------------------------------------
// telescope mount control
#pragma once

#include "../../../Common.h"

#ifdef MOUNT_PRESENT

#include "../guide/Guide.h"

// Limit defaults
#ifndef LIMIT_HORIZON
#define LIMIT_HORIZON -10.0F
#endif
#ifndef LIMIT_OVERHEAD
#define LIMIT_OVERHEAD 80.0F
#endif
#ifndef LIMIT_MERIDIAN_EAST
#define LIMIT_MERIDIAN_EAST 15.0F
#endif
#ifndef LIMIT_MERIDIAN_WEST
#define LIMIT_MERIDIAN_WEST 15.0F
#endif

#pragma pack(1)
typedef struct AltitudeLimits {
  float min; // in radians
  float max; // in radians
} AltitudeLimits;

typedef struct LimitSettings {
  AltitudeLimits altitude;
  float pastMeridianE; // in radians
  float pastMeridianW; // in radians
} LimitSettings;

typedef struct SyncLimitSettings {
  float pole;  // maximum |Dec| in degrees, or OFF; 90 is unlimited
  float axis1; // degrees, or OFF
  float axis2; // degrees, or OFF
} SyncLimitSettings;
#pragma pack()

typedef struct MerdianError {
  uint8_t east;
  uint8_t west;
} MerdianError;

typedef struct MinMaxError {
  uint8_t min;
  uint8_t max;
} MinMaxError;

typedef struct AxisMinMaxError {
  MinMaxError axis1;
  MinMaxError axis2;
} AxisMinMaxError;

typedef struct LimitsError {
  MinMaxError     altitude;
  AxisMinMaxError limit;
  AxisMinMaxError limitSense;
  MerdianError    meridian;
} LimitsError;

#define CLEARANCE_UNLIMITED 1.0e6

// signed distance (in radians) from each position limit, negative when the limit is exceeded
typedef struct LimitsClearance {
  double altitudeMin;
  double altitudeMax;
  double meridianEast;
  double meridianWest;
  double axis1Min;
  double axis1Max;
  double axis2Min;
  double axis2Max;
} LimitsClearance;

class Limits {
  public:
    void init();

    inline float getSyncPoleLimit() { return syncSettings.pole == OFF ? 90.0F : syncSettings.pole; }
    inline bool hasSyncAxisLimits() { return syncSettings.axis1 != OFF || syncSettings.axis2 != OFF; }

    bool command(char *reply, char *command, char *parameter, bool *suppressFrame, bool *numericReply, CommandError *commandError);

    // target coordinate check ahead of sync, goto, etc.
    CommandError validateTarget(Coordinate *coords, bool isGoto);
    CommandError validateTarget(Coordinate *coords, bool *eastReachable, bool *westReachable, double *eastCorrection, double *westCorrection, bool isGoto);

    // validate sync/align declination in radians against the pole limit
    CommandError validateSyncPole(double declination);

    // validate and apply an instrument coordinate change for a mount axis
    CommandError validateInstrumentCoordinate(uint8_t axisNumber, double value, bool bypass = false);
    CommandError setInstrumentCoordinate(uint8_t axisNumber, double value, bool bypass = false);

    // true if an limit related error is exists
    bool isError();

    // true if an error exists that impacts goto safety
    bool isGotoError();

    // true if an below horizon limit
    inline bool isBelowHorizon() { return error.altitude.min; }

    // true if an above overhead limit
    inline bool isAboveOverhead() { return error.altitude.max; }

    // true if past meridian limit east
    inline bool isPastMeridianE() { return error.meridian.east; }

    // true if past meridian limit west
    inline bool isPastMeridianW() { return error.meridian.west; }

    // true if past the axis1 min limit
    inline bool isPastAxis1Min() { return error.limit.axis1.min; }

    // true if past the axis1 max limit
    inline bool isPastAxis1Max() { return error.limit.axis1.max; } 

    // return general error code
    uint8_t errorCode();

    // enable or disable limit enforcement
    inline void enabled(bool state) { limitsEnabled = state; }

    // check if limits are enabled
    inline bool isEnabled() { return limitsEnabled; }

    // check if the limits state permits motion
    inline bool isMotionAllowed() { return limitsEnabled || !hasSoftwareLimits(); }

    // software limits apply to normal mounts; physical limit switches always apply
    inline bool hasSoftwareLimits() { return true; }

    // disable meridian limits for the specified period in seconds
    void limitsDisablePeriod(float seconds) { limitsDisablePeriodDs = lroundf(seconds*10.0F); }

    // clearance of a mount position (with altitude) from the horizon and overhead limits
    void altitudeClearance(Coordinate *position, LimitsClearance *clearance);

    // clearance of a mount position from the meridian limits
    void meridianClearance(Coordinate *position, LimitsClearance *clearance);

    // axis1 coordinate used for the min/max limits of a mount position
    double limitAxis1Coordinate(Coordinate *position);

    // clearance of a limit axis1 coordinate from the min/max limits
    void axis1Clearance(double a1, LimitsClearance *clearance);

    // clearance of a limit axis2 coordinate from the min/max limits
    void axis2Clearance(double a2, LimitsClearance *clearance);

    // clearance of a mount position (with altitude) from all position limits
    void positionClearance(Coordinate *position, LimitsClearance *clearance);

    void poll();

    LimitSettings settings = { { degToRadF(LIMIT_HORIZON), degToRadF(LIMIT_OVERHEAD) }, degToRadF(LIMIT_MERIDIAN_EAST), degToRadF(LIMIT_MERIDIAN_WEST) };

  private:
    void stop();
    void stopAxis1(GuideAction stopDirection = GA_BREAK);
    void stopAxis2(GuideAction stopDirection = GA_BREAK);
    // get least distance between coordinates
    inline double dist(double a, double b) { if (a > b) return a - b; else return b - a; }

    bool limitsEnabled = false;
    LimitsError error;

    int limitsDisablePeriodDs = 0; // in deciseconds (0.1s)

    uint32_t nvKey;
    SyncLimitSettings syncSettings = {SYNC_POLE_LIMIT, AXIS1_LIMIT_SYNC, AXIS2_LIMIT_SYNC};
    uint32_t syncNvKey;
};

extern Limits limits;

#endif
