//--------------------------------------------------------------------------------------------------
// telescope mount goto, path planning to keep slews clear of the overhead limit
#pragma once

#include "../../../Common.h"

#if defined(MOUNT_PRESENT) && GOTO_AVOID_OVERHEAD == ON

#include "../coordinates/Transform.h"
#include "GotoProfile.h"

class Axis;

#define PLAN_WAYPOINTS_MAX   2

// the planner works in single precision, its arcsecond level rounding is far inside the tolerance
#define PLAN_DEG90           ((float)Deg90)
#define PLAN_DEG180          ((float)Deg180)
#define PLAN_DEG360          ((float)Deg360)
#define PLAN_BOX_STACK_SIZE  32

enum PlanState: uint8_t     {PLAN_IDLE, PLAN_BUSY, PLAN_DIRECT, PLAN_WAYPOINTS, PLAN_FAILED};
enum PlanPhase: uint8_t     {PP_DIRECT, PP_SURVEY, PP_PUSH, PP_BISECT};
enum PlanSurvey: uint8_t    {SS_WALK, SS_PEAK, SS_ENTRY, SS_EXIT};
enum PlanLegResult: uint8_t {PL_RUNNING, PL_PASS, PL_FAIL};
enum PlanClearance: uint8_t {PC_ALTITUDE_MIN, PC_ALTITUDE_MAX, PC_MERIDIAN_E, PC_MERIDIAN_W,
                             PC_AXIS1_MIN, PC_AXIS1_MAX, PC_AXIS2_MIN, PC_AXIS2_MAX, PC_COUNT};

// instrument coordinates in radians
typedef struct PlanPoint {
  float a1;
  float a2;
} PlanPoint;

// slew motion of an axis, in radians per second and radians per second per second
typedef struct PlanAxisModel {
  float accel;
  float vLow;
  float vHigh;
} PlanAxisModel;

// instrument coordinate region in radians
typedef struct PlanBox {
  float a1Low;
  float a1High;
  float a2Low;
  float a2High;
} PlanBox;

typedef struct PlanCounters {
  unsigned long legs;
  unsigned long samples;
} PlanCounters;

class GotoPlanner {
  public:
    // start planning a path between instrument coordinates
    // \param start: where the slew starts
    // \param end: where the slew ends
    // \param search: if false only the direct path is checked
    void begin(PlanPoint start, PlanPoint end, bool search = true);

    // continue planning for up to the given time
    PlanState poll(unsigned long microseconds);

    // stop planning
    void abort();

    inline PlanState getState() { return state; }
    inline uint8_t getWaypointCount() { return waypointCount; }
    inline PlanPoint getWaypoint(uint8_t index) { return waypoint[index]; }
    inline PlanCounters getCounters() { return counters; }

  private:
    void work();
    void finish(PlanState result);

    void surveyBegin(float time);
    void surveyStep();
    void surveyPeakBegin();
    float surveySample(float time, bool *inZone);
    void pushBegin();
    void pushStep();
    void bisectStep();
    bool pushBlocked();
    PlanPoint pushPoint(PlanPoint from, float distance);
    void candidateBegin(float distance);
    PlanLegResult candidateStep();
    PlanPoint nearestZenith(PlanPoint p);

    void legBegin(PlanPoint from, PlanPoint to, bool overheadOnly = false);
    PlanLegResult legStep();
    PlanLegResult boxStep();
    void boxPush(PlanPoint p1, PlanPoint p2);
    PlanPoint legPosition(float time);

    PlanAxisModel axisModel(Axis *axis, float rate);
    Coordinate mountCoordinate(PlanPoint p, bool west);
    void clearanceSide(PlanPoint p, bool west, float *clearance, float *altitude);
    void clearance(PlanPoint p, float *clearance);
    void pointClearance(PlanPoint p, float *clearance);
    float spare(PlanPoint p, float *altitude, float *step);
    float altitudeRate(PlanPoint p);
    inline bool isWest(PlanPoint p) { return northern ? p.a2 > PLAN_DEG90 : p.a2 < -PLAN_DEG90; }
    bool isValid(PlanPoint p);

    PlanState state = PLAN_IDLE;
    PlanPhase phase = PP_DIRECT;
    PlanCounters counters = {0, 0};
    bool search = true;

    PlanPoint start = {0.0, 0.0};
    PlanPoint end = {0.0, 0.0};
    PlanPoint waypoint[PLAN_WAYPOINTS_MAX];
    uint8_t waypointCount = 0;

    // fixed when planning begins
    PlanAxisModel model[2];
    float tolerance = 0.0;
    float pole = 0.0;
    bool northern = true;
    bool axis2MinActive = true;
    bool axis2MaxActive = true;
    bool bandEnabled = false;
    float bandTop = 0.0;
    PlanPoint zenith[2];
    float latitudeSine = 0.0F;
    float latitudeCosine = 0.0F;
    float startClearance[PC_COUNT];
    float endClearance[PC_COUNT];

    // leg being tested
    bool legActive = false;
    PlanPoint legFrom = {0.0, 0.0};
    PlanPoint legTo = {0.0, 0.0};
    GotoProfile legProfile[2];
    float legDuration = 0.0F;
    float legRate = 0.0F;
    float legRate1 = 0.0F;
    float legRate2 = 0.0F;
    float legTime = 0.0F;
    unsigned long legSamples = 0;
    float legRequired[PC_COUNT];
    float legRequiredMax = 0.0;
    float legSlack = 0.0;
    PlanLegResult legResult = PL_RUNNING;
    bool legBox = false;
    PlanBox box[PLAN_BOX_STACK_SIZE];
    uint8_t boxCount = 0;
    uint8_t boxTrial = 0;

    // highest point of the blocked slew and where it is within the tolerance of the overhead limit, times are
    // seconds from the start of the slew
    PlanSurvey surveyStage = SS_WALK;
    float surveyTime = 0.0F;
    float surveyInterval = 0.0F;
    float surveyResolution = 0.0F;
    float highestAltitude = 0.0;
    float highestTime = 0.0F;
    PlanPoint highest = {0.0, 0.0};
    PlanPoint zoneEntry = {0.0, 0.0};
    PlanPoint zoneExit = {0.0, 0.0};
    bool zoneEntered = false;
    bool zoneLeft = false;
    float entryLow = 0.0F;
    float entryHigh = 0.0F;
    float exitLow = 0.0F;
    float exitHigh = 0.0F;
    float goldenLow = 0.0F;
    float goldenHigh = 0.0F;
    float goldenC = 0.0F;
    float goldenD = 0.0F;
    float goldenAltitudeC = 0.0;
    float goldenAltitudeD = 0.0;
    uint8_t goldenNeed = 0;

    // waypoints moved away from the blocked slew, direction components are per radian moved
    PlanPoint pushDirection[3];
    uint8_t pushIndex = 0;
    bool pushPair = false;
    float pushDistance = 0.0;
    float bisectLow = 0.0;
    float bisectHigh = 0.0;

    // waypoints being tested
    PlanPoint candidate[PLAN_WAYPOINTS_MAX];
    uint8_t candidateCount = 0;
    uint8_t candidateLeg = 0;
    bool candidateActive = false;
};

extern GotoPlanner gotoPlanner;

#endif
