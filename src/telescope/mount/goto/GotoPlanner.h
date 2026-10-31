//--------------------------------------------------------------------------------------------------
// telescope mount goto, path planning to keep slews clear of the overhead limit
#pragma once

#include "../../../Common.h"

#if defined(MOUNT_PRESENT) && GOTO_AVOID_OVERHEAD == ON

#include "../coordinates/Transform.h"
#include "GotoProfile.h"

class Axis;

#define PLAN_GRID_AXIS1_MAX  ((int)(540.0/(GOTO_AVOID_GRID)) + 2)
#define PLAN_GRID_AXIS2_MAX  ((int)(360.0/(GOTO_AVOID_GRID)) + 2)
#define PLAN_GRID_POINTS_MAX (PLAN_GRID_AXIS1_MAX*PLAN_GRID_AXIS2_MAX)
#define PLAN_WAYPOINTS_MAX   2
#define PLAN_BOX_STACK_SIZE  32

enum PlanState: uint8_t     {PLAN_IDLE, PLAN_BUSY, PLAN_DIRECT, PLAN_WAYPOINTS, PLAN_FAILED};
enum PlanPhase: uint8_t     {PP_DIRECT, PP_GRID, PP_PAIR, PP_REFINE};
enum PlanLegResult: uint8_t {PL_RUNNING, PL_PASS, PL_FAIL};
enum PlanClearance: uint8_t {PC_ALTITUDE_MIN, PC_ALTITUDE_MAX, PC_MERIDIAN_E, PC_MERIDIAN_W,
                             PC_AXIS1_MIN, PC_AXIS1_MAX, PC_AXIS2_MIN, PC_AXIS2_MAX, PC_COUNT};

// instrument coordinates in radians
typedef struct PlanPoint {
  double a1;
  double a2;
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

    void gridBegin();
    void gridStep();
    PlanPoint gridPoint(int index);

    void pairBegin();
    void pairStep();
    void pairRoundEnd();
    void pairAdvance();

    void refineBegin();
    void refineStep();
    void refineNext();
    bool refineCandidate(PlanPoint from, PlanPoint *to);

    void legBegin(PlanPoint from, PlanPoint to, bool overheadOnly = false);
    PlanLegResult legStep();
    PlanLegResult boxStep();
    void boxPush(PlanPoint p1, PlanPoint p2);
    float legCost(PlanPoint from, PlanPoint to);
    float legCostLowerBound(PlanPoint from, PlanPoint to);

    PlanAxisModel axisModel(Axis *axis, float rate);
    Coordinate mountCoordinate(PlanPoint p, bool west);
    void clearanceSide(PlanPoint p, bool west, double *clearance, double *altitude);
    void clearance(PlanPoint p, double *clearance);
    double spare(PlanPoint p, double *altitude);
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
    double tolerance = 0.0;
    double pole = 0.0;
    bool northern = true;
    bool bandEnabled = false;
    double bandTop = 0.0;
    PlanPoint zenith[2];

    // leg being tested
    bool legActive = false;
    PlanPoint legFrom = {0.0, 0.0};
    PlanPoint legTo = {0.0, 0.0};
    GotoProfile legProfile[2];
    float legDuration = 0.0F;
    float legRate = 0.0F;
    float legTime = 0.0F;
    unsigned long legSamples = 0;
    double legRequired[PC_COUNT];
    double legRequiredMax = 0.0;
    double legSlack = 0.0;
    PlanLegResult legResult = PL_RUNNING;
    bool legBox = false;
    PlanBox box[PLAN_BOX_STACK_SIZE];
    uint8_t boxCount = 0;

    // waypoint grid, with the cost of the slews from the start to each point and each point to the end
    int gridAxis1Count = 0;
    int gridCount = 0;
    double gridAxis1Low = 0.0;
    double gridAxis2Low = 0.0;
    double gridAxis1Spacing = 0.0;
    double gridAxis2Spacing = 0.0;
    int gridIndex = 0;
    int gridSelected = 0;
    uint8_t gridLeg = 0;
    bool gridTested[PLAN_GRID_POINTS_MAX];
    float costStart[PLAN_GRID_POINTS_MAX];
    float costEnd[PLAN_GRID_POINTS_MAX];

    // pair search, in rounds of increasing lower bound cost
    int pairI = 0;
    int pairJ = 0;
    bool pairMinimumScan = true;
    bool pairMore = false;
    float pairLow = 0.0F;
    float pairHigh = 0.0F;
    float pairBest = 0.0F;
    float pairCandidate = 0.0F;
    int pairBestI = 0;
    int pairBestJ = 0;

    // refinement toward the overhead limit
    uint8_t refineIndex = 0;
    uint8_t refineLeg = 0;
    bool refineDone[PLAN_WAYPOINTS_MAX];
    uint8_t refineMoves[PLAN_WAYPOINTS_MAX];
    PlanPoint refinePoint = {0.0, 0.0};
};

extern GotoPlanner gotoPlanner;

#endif
