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
  float min;
  float max;
} AltitudeLimits;

#define LimitSettingsSize 16
typedef struct LimitSettings {
  AltitudeLimits altitude;
  float pastMeridianE;
  float pastMeridianW;
} LimitSettings;
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

class Limits {
  public:
    void init();

    bool command(char *reply, char *command, char *parameter, bool *suppressFrame, bool *numericReply, CommandError *commandError);

    // constrain meridian limits to the allowed range
    void constrainMeridianLimits();

    // target coordinate check ahead of sync, goto, etc.
    CommandError validateTarget(Coordinate *coords, bool isGoto);
    CommandError validateTarget(Coordinate *coords, bool *eastReachable, bool *westReachable, double *eastCorrection, double *westCorrection, bool isGoto);

    // true if an limit related error is exists
    bool isError();

    // true if an error exists that impacts goto safety
    bool isGotoError();

    // true if an below horizon limit
    inline bool isBelowHorizon() { return error.altitude.min; }

    // true if an above overhead limit
    inline bool isAboveOverhead() { return error.altitude.max; }

    // true if past meridian limit west
    inline bool isPastMeridianW() { return error.meridian.west; }

    // true if past the axis1 max limit
    inline bool isPastAxis1Max() { return error.limit.axis1.max; } 

    // return general error code
    uint8_t errorCode();

    // enable or disable limit enforcement
    inline void enabled(bool state) { limitsEnabled = state; }

    // check if limits are being enforced
    inline bool isEnabled() { return limitsEnabled; }

    // disable meridian limits for the specified period in seconds
    void limitsDisablePeriod(float seconds) { limitsDisablePeriodDs = lroundf(seconds*10.0F); }

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
};

extern Limits limits;

#endif
