//--------------------------------------------------------------------------------------------------
// telescope mount control
#pragma once

#include "../../../Common.h"

#ifdef MOUNT_PRESENT

#include "../guide/Guide.h"

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

    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // constrain meridian limits to the allowed range
    void constrainMeridianLimits();

    // target coordinate check ahead of sync, goto, etc.
    CommandError validateTarget(Coordinate *coords, bool isGoto);
    CommandError validateTarget(Coordinate *coords, bool *eastReachable, bool *westReachable, double *eastCorrection, double *westCorrection, bool isGoto);

    // true if an limit related error is exists
    bool isError();

    // true if an error exists that impacts goto safety
    bool isGotoError();

    // true if an above overhead limit
    bool isAboveOverhead() { return error.altitude.max; }

    // return general error code
    uint8_t errorCode();

    // enable or disable limit enforcement
    inline void enabled(bool state) { limitsEnabled = state; }

    // check if limits are being enforced
    inline bool isEnabled() { return limitsEnabled; }

    void poll();

    LimitSettings settings = { { degToRadF(-10.0F), degToRadF(80.0F) }, degToRadF(15.0F), degToRadF(15.0F) };

  private:
    void stop();
    void stopAxis1(GuideAction stopDirection = GA_BREAK);
    void stopAxis2(GuideAction stopDirection = GA_BREAK);
    // get least distance between coordinates
    inline double dist(double a, double b) { if (a > b) return a - b; else return b - a; }

    bool limitsEnabled = false;
    LimitsError error;
};

extern Limits limits;

#endif
