//--------------------------------------------------------------------------------------------------
// telescope mount goto, path planning to keep slews clear of the overhead limit

#include "GotoPlanner.h"

#if defined(MOUNT_PRESENT) && GOTO_AVOID_OVERHEAD == ON

#include "../../../lib/tasks/OnTask.h"

#include "../Mount.h"
#include "../site/Site.h"
#include "../limits/Limits.h"
#include "Goto.h"

static_assert(GOTO_AVOID_TOLERANCE >= 0.1 && GOTO_AVOID_TOLERANCE <= 10.0,
  "Configuration (Config.h): Setting GOTO_AVOID_TOLERANCE unknown, use a value between 0.1 and 10 (degrees.)");
static_assert(GOTO_AVOID_GRID >= 10.0 && GOTO_AVOID_GRID <= 45.0,
  "Configuration (Config.h): Setting GOTO_AVOID_GRID unknown, use a value between 10 and 45 (degrees.)");
static_assert(GOTO_AVOID_REFINE >= 0.1 && GOTO_AVOID_REFINE <= GOTO_AVOID_GRID/2.0,
  "Configuration (Config.h): Setting GOTO_AVOID_REFINE unknown, use a value between 0.1 and GOTO_AVOID_GRID/2 (degrees.)");
static_assert(GOTO_AVOID_FLIP_OVERSHOOT >= 0.0 && GOTO_AVOID_FLIP_OVERSHOOT <= 2.0,
  "Configuration (Config.h): Setting GOTO_AVOID_FLIP_OVERSHOOT unknown, use a value between 0 and 2 (degrees.)");

// clearance a sample may use up before the next sample, as distance moved in radians
#define PLAN_SLACK           degToRad(0.05)
#define PLAN_SLACK_MIN       (2.0*OneArcSec)
// clearance from the horizon slowdown band where predicted timing stops being used
#define PLAN_BAND_SWITCH     degToRad(1.0)
#define PLAN_LEG_SAMPLES_MAX 20000
#define PLAN_WORK_UNITS      8
#define PLAN_PAIR_CHUNK      32
#define PLAN_PAIR_ROUND      1.25F
#define PLAN_REFINE_MOVES    16
#define PLAN_COST_NONE       1.0e9F
// samples spent trying to clear the region holding a whole slew before its timing is used
#define PLAN_BOX_TRIAL       1
// grid point flags
#define PLAN_GRID_TESTED     1
#define PLAN_GRID_END_TESTED 2

void GotoPlanner::begin(PlanPoint start, PlanPoint end, bool search) {
  this->start = start;
  this->end = end;
  this->search = search;

  northern = site.location.latitude >= 0.0;
  pole = northern ? Deg90 : -Deg90;
  tolerance = degToRad(GOTO_AVOID_TOLERANCE);

  // mount axis2 folds back at the poles, so a limit there can't be passed
  axis2MinActive = axis2.getLimitMin() > -Deg90 + OneArcSec;
  axis2MaxActive = axis2.getLimitMax() < Deg90 - OneArcSec;

  float rate = goTo.getRadsPerSecond();
  model[0] = axisModel(&axis1, rate);
  model[1] = axisModel(&axis2, rate*((float)(AXIS2_SLEW_RATE_PERCENT)/100.0F));

  // matches the conditions for goto rate scaling near the horizon
  bandEnabled = limits.hasSoftwareLimits() && transform.isEquatorial() && MOUNT_HORIZON_AVOIDANCE == ON &&
                site.locationEx.latitude.absval > degToRad(10.0);
  bandTop = limits.settings.altitude.min + degToRad(SLEW_ACCELERATION_DIST*2.0);

  // instrument coordinates of the zenith for each pier side
  double a2 = transform.mountType == ALTALT ? 0.0 : site.location.latitude;
  zenith[0].a1 = 0.0;
  zenith[0].a2 = a2;
  zenith[1].a1 = Deg180;
  zenith[1].a2 = northern ? Deg180 - a2 : (-Deg180) - a2;

  clearance(start, startClearance);
  clearance(end, endClearance);

  waypointCount = 0;
  counters = {0, 0};
  legActive = false;
  phase = PP_DIRECT;
  state = PLAN_BUSY;

  VF("MSG: Mount, goto plan from a1="); V(radToDeg(start.a1)); VF(" a2="); V(radToDeg(start.a2));
  VF(" to a1="); V(radToDeg(end.a1)); VF(" a2="); V(radToDeg(end.a2)); VLF(" deg");
}

PlanState GotoPlanner::poll(unsigned long microseconds) {
  unsigned long startTime = micros();
  while (state == PLAN_BUSY && (long)(micros() - startTime) < (long)microseconds) {
    for (int i = 0; i < PLAN_WORK_UNITS && state == PLAN_BUSY; i++) work();
    Y;
  }
  return state;
}

void GotoPlanner::abort() {
  if (state == PLAN_BUSY) { VLF("MSG: Mount, goto plan aborted"); }
  state = PLAN_IDLE;
  legActive = false;
}

void GotoPlanner::work() {
  switch (phase) {
    case PP_DIRECT:
      // other limits are left to the existing behaviour on the direct path
      if (!legActive) { legBegin(start, end, true); legActive = true; }
      switch (legStep()) {
        case PL_RUNNING: break;
        case PL_PASS: finish(PLAN_DIRECT); break;
        case PL_FAIL:
          legActive = false;
          if (search) gridBegin(); else finish(PLAN_FAILED);
        break;
      }
    break;
    case PP_GRID: gridStep(); break;
    case PP_PAIR: pairStep(); break;
    case PP_REFINE: refineStep(); break;
  }
}

void GotoPlanner::finish(PlanState result) {
  state = result;
  legActive = false;

  #if DEBUG == VERBOSE
    VF("MSG: Mount, goto plan ");
    if (result == PLAN_DIRECT) { VF("direct"); } else
    if (result == PLAN_WAYPOINTS) { VF("with waypoints"); } else { VF("FAILED"); }
    VF(" (legs "); V(counters.legs); VF(", samples "); V(counters.samples); VLF(")");
    for (uint8_t i = 0; i < waypointCount; i++) {
      VF("MSG: Mount, goto plan waypoint a1="); V(radToDeg(waypoint[i].a1)); VF(" a2="); V(radToDeg(waypoint[i].a2)); VLF(" deg");
    }
  #endif
}

// single waypoint search, also finds which slews from the start and to the end are clear for the pair search
void GotoPlanner::gridBegin() {
  // the axis limits apply to mount coordinates, so cover their instrument coordinates for both pier sides
  double a1Min = axis1.getLimitMin();
  double a1Max = axis1.getLimitMax();
  double a2Min = axis2.getLimitMin();
  double a2Max = axis2.getLimitMax();
  double westA2Min = northern ? Deg180 - a2Max : (-Deg180) - a2Max;
  double westA2Max = northern ? Deg180 - a2Min : (-Deg180) - a2Min;

  double westA1Max = a1Max + Deg180;
  double low[2] = { a1Min, a2Min < westA2Min ? a2Min : westA2Min };
  double high[2] = { westA1Max, a2Max > westA2Max ? a2Max : westA2Max };
  int countMax[2] = { PLAN_GRID_AXIS1_MAX, PLAN_GRID_AXIS2_MAX };
  int count[2];
  double spacing[2];
  for (int axisNum = 0; axisNum < 2; axisNum++) {
    low[axisNum] += tolerance;
    double range = (high[axisNum] - tolerance) - low[axisNum];
    if (range < 0.0) count[axisNum] = 0; else {
      count[axisNum] = (int)ceil(range/degToRad(GOTO_AVOID_GRID)) + 1;
      if (count[axisNum] > countMax[axisNum]) count[axisNum] = countMax[axisNum];
    }
    spacing[axisNum] = count[axisNum] > 1 ? range/(count[axisNum] - 1) : 0.0;
  }

  gridAxis1Count = count[0];
  gridCount = count[0]*count[1];
  gridAxis1Low = low[0];
  gridAxis2Low = low[1];
  gridAxis1Spacing = spacing[0];
  gridAxis2Spacing = spacing[1];
  gridIndex = 0;
  gridLeg = 0;
  phase = PP_GRID;
}

void GotoPlanner::gridStep() {
  // the estimated cost is the cost used to rank waypoints
  if (gridIndex < gridCount) {
    PlanPoint p = gridPoint(gridIndex);
    bool valid = isValid(p);
    gridFlags[gridIndex] = valid ? 0 : PLAN_GRID_TESTED;
    costStart[gridIndex] = valid ? legCost(start, p) : PLAN_COST_NONE;
    costEnd[gridIndex] = valid ? legCost(p, end) : PLAN_COST_NONE;
    gridIndex++;
    return;
  }

  switch (gridLeg) {
    case 0: {
      // points are tested in order of cost so the first to pass is the best
      float best = PLAN_COST_NONE;
      gridSelected = -1;
      for (int i = 0; i < gridCount; i++) {
        if (!(gridFlags[i] & PLAN_GRID_TESTED) && costStart[i] + costEnd[i] < best) { best = costStart[i] + costEnd[i]; gridSelected = i; }
      }
      if (gridSelected < 0) { pairBegin(); return; }
      gridFlags[gridSelected] |= PLAN_GRID_TESTED;
      legBegin(start, gridPoint(gridSelected));
      gridLeg = 1;
    } break;
    case 1:
      switch (legStep()) {
        case PL_RUNNING: return;
        case PL_PASS: break;
        // the slew to the end is left untested until the pair search needs it
        case PL_FAIL: costStart[gridSelected] = PLAN_COST_NONE; gridLeg = 0; return;
      }
      gridFlags[gridSelected] |= PLAN_GRID_END_TESTED;
      legBegin(gridPoint(gridSelected), end);
      gridLeg = 2;
    break;
    case 2:
      switch (legStep()) {
        case PL_RUNNING: return;
        case PL_PASS: break;
        case PL_FAIL: costEnd[gridSelected] = PLAN_COST_NONE; break;
      }
      gridLeg = 0;
      if (costStart[gridSelected] < PLAN_COST_NONE && costEnd[gridSelected] < PLAN_COST_NONE) {
        waypoint[0] = gridPoint(gridSelected);
        waypointCount = 1;
        refineBegin();
      }
    break;
  }
}

PlanPoint GotoPlanner::gridPoint(int index) {
  return {gridAxis1Low + (index % gridAxis1Count)*gridAxis1Spacing, gridAxis2Low + (index / gridAxis1Count)*gridAxis2Spacing};
}

// least cost pair of waypoints, middle slews are tested in rounds of increasing lower bound cost
void GotoPlanner::pairBegin() {
  pairI = 0;
  pairJ = 0;
  pairMinimumScan = true;
  pairMore = false;
  pairLow = PLAN_COST_NONE;
  pairBest = PLAN_COST_NONE;
  phase = PP_PAIR;
}

void GotoPlanner::pairStep() {
  if (legActive) {
    PlanLegResult result = legStep();
    if (result == PL_RUNNING) return;
    legActive = false;
    if (pairEndLeg) {
      // the same pair is considered again now its slew to the end is known
      if (result == PL_FAIL) costEnd[pairJ] = PLAN_COST_NONE;
      return;
    }
    if (result == PL_PASS) {
      pairBest = pairCandidate;
      pairBestI = pairI;
      pairBestJ = pairJ;
    }
    pairAdvance();
    return;
  }

  for (int n = 0; n < PLAN_PAIR_CHUNK; n++) {
    if (pairI >= gridCount) { pairRoundEnd(); return; }
    if (costStart[pairI] >= PLAN_COST_NONE) { pairI++; pairJ = 0; continue; }

    if (pairI != pairJ && costEnd[pairJ] < PLAN_COST_NONE) {
      PlanPoint p = gridPoint(pairI);
      PlanPoint q = gridPoint(pairJ);
      float lower = costStart[pairI] + legCostLowerBound(p, q) + costEnd[pairJ];
      if (pairMinimumScan) {
        if (lower < pairLow) pairLow = lower;
      } else
      if (lower >= pairHigh) pairMore = true; else
      if (lower >= pairLow) {
        float total = costStart[pairI] + legCost(p, q) + costEnd[pairJ];
        if (total < pairBest) {
          pairEndLeg = !(gridFlags[pairJ] & PLAN_GRID_END_TESTED);
          if (pairEndLeg) {
            gridFlags[pairJ] |= PLAN_GRID_END_TESTED;
            legBegin(q, end);
          } else {
            pairCandidate = total;
            legBegin(p, q);
          }
          legActive = true;
          return;
        }
      }
    }
    pairAdvance();
  }
}

void GotoPlanner::pairRoundEnd() {
  if (pairMinimumScan) {
    if (pairLow >= PLAN_COST_NONE) { finish(PLAN_FAILED); return; }
    pairMinimumScan = false;
    pairHigh = pairLow*PLAN_PAIR_ROUND;
    if (pairHigh < pairLow + 1.0F) pairHigh = pairLow + 1.0F;
  } else {
    // any pair not yet tested costs at least pairHigh
    if (pairBest <= pairHigh || (!pairMore && pairBest < PLAN_COST_NONE)) {
      waypoint[0] = gridPoint(pairBestI);
      waypoint[1] = gridPoint(pairBestJ);
      waypointCount = 2;
      refineBegin();
      return;
    }
    if (!pairMore) { finish(PLAN_FAILED); return; }
    pairLow = pairHigh;
    pairHigh = pairHigh*PLAN_PAIR_ROUND;
  }
  pairI = 0;
  pairJ = 0;
  pairMore = false;
}

void GotoPlanner::pairAdvance() {
  if (++pairJ >= gridCount) { pairJ = 0; pairI++; }
}

// move each waypoint toward the overhead limit while the slews it joins stay clear
void GotoPlanner::refineBegin() {
  for (uint8_t i = 0; i < waypointCount; i++) { refineDone[i] = false; refineMoves[i] = 0; }
  refineIndex = 0;
  refineLeg = 0;
  phase = PP_REFINE;
}

void GotoPlanner::refineStep() {
  PlanPoint previous = refineIndex == 0 ? start : waypoint[refineIndex - 1];
  PlanPoint next = refineIndex == waypointCount - 1 ? end : waypoint[refineIndex + 1];

  switch (refineLeg) {
    case 0: {
      bool done = true;
      for (uint8_t i = 0; i < waypointCount; i++) if (!refineDone[i]) done = false;
      if (done) { finish(PLAN_WAYPOINTS); return; }

      if (refineDone[refineIndex]) { refineNext(); return; }
      if (refineMoves[refineIndex] >= PLAN_REFINE_MOVES || !refineCandidate(waypoint[refineIndex], &refinePoint)) {
        refineDone[refineIndex] = true;
        refineNext();
        return;
      }
      legBegin(previous, refinePoint);
      refineLeg = 1;
    } break;
    case 1:
      switch (legStep()) {
        case PL_RUNNING: return;
        case PL_PASS: legBegin(refinePoint, next); refineLeg = 2; return;
        case PL_FAIL: refineDone[refineIndex] = true; refineLeg = 0; refineNext(); return;
      }
    break;
    case 2:
      switch (legStep()) {
        case PL_RUNNING: return;
        case PL_PASS:
          waypoint[refineIndex] = refinePoint;
          refineMoves[refineIndex]++;
          for (uint8_t i = 0; i < waypointCount; i++) if (i != refineIndex) refineDone[i] = false;
        break;
        case PL_FAIL: refineDone[refineIndex] = true; break;
      }
      refineLeg = 0;
      refineNext();
    break;
  }
}

void GotoPlanner::refineNext() {
  if (++refineIndex >= waypointCount) refineIndex = 0;
}

// half the distance to the tolerance about the overhead limit, toward the nearest image of the zenith
bool GotoPlanner::refineCandidate(PlanPoint from, PlanPoint *to) {
  double c[PC_COUNT];
  clearance(from, c);
  if (c[PC_ALTITUDE_MAX] >= CLEARANCE_UNLIMITED) return false;
  double step = (c[PC_ALTITUDE_MAX] - tolerance)/2.0;
  if (step <= degToRad(GOTO_AVOID_REFINE)) return false;

  PlanPoint target = zenith[0];
  double best = CLEARANCE_UNLIMITED;
  for (int side = 0; side < 2; side++) {
    for (int turn = -1; turn <= 1; turn++) {
      PlanPoint z = zenith[side];
      z.a1 += turn*Deg360;
      double distance = fabs(z.a1 - from.a1) + fabs(z.a2 - from.a2);
      if (distance < best) { best = distance; target = z; }
    }
  }
  if (best <= step) return false;

  to->a1 = from.a1 + (target.a1 - from.a1)*(step/best);
  to->a2 = from.a2 + (target.a2 - from.a2)*(step/best);
  return isValid(*to);
}

// samples the predicted slew so no limit can be reached between samples, since altitude and the
// other limit coordinates change by no more than the combined axis motion
void GotoPlanner::legBegin(PlanPoint from, PlanPoint to, bool overheadOnly) {
  legFrom = from;
  legTo = to;
  float distance[2] = { (float)fabs(to.a1 - from.a1), (float)fabs(to.a2 - from.a2) };
  for (int i = 0; i < 2; i++) legProfile[i].init(distance[i], model[i].accel, model[i].vLow, model[i].vHigh);
  legDuration = legProfile[0].duration() > legProfile[1].duration() ? legProfile[0].duration() : legProfile[1].duration();
  legRate = legProfile[0].rateMax() + legProfile[1].rateMax();
  legTime = 0.0F;
  legSamples = 0;
  legBox = false;
  boxCount = 0;
  legResult = PL_RUNNING;
  counters.legs++;

  // a limit already exceeded at the start is left to the existing limit recovery behaviour
  double cFrom[PC_COUNT], cTo[PC_COUNT];
  pointClearance(from, cFrom);
  pointClearance(to, cTo);
  double requiredMin = tolerance;
  for (int k = 0; k < PC_COUNT; k++) {
    if (cFrom[k] <= 0.0 || (overheadOnly && k != PC_ALTITUDE_MAX)) { legRequired[k] = -CLEARANCE_UNLIMITED; continue; }
    if (cTo[k] < 0.0) legResult = PL_FAIL;
    double required = tolerance;
    if (cFrom[k] < required) required = cFrom[k];
    if (cTo[k] < required) required = cTo[k] > 0.0 ? cTo[k] : 0.0;
    legRequired[k] = required;
    if (required < requiredMin) requiredMin = required;
  }

  legSlack = requiredMin/2.0;
  if (legSlack > PLAN_SLACK) legSlack = PLAN_SLACK;
  if (legSlack < PLAN_SLACK_MIN) legSlack = PLAN_SLACK_MIN;

  legRequiredMax = -CLEARANCE_UNLIMITED;
  for (int k = 0; k < PC_COUNT; k++) {
    if (legRequired[k] <= -CLEARANCE_UNLIMITED) continue;
    legRequired[k] -= legSlack;
    if (legRequired[k] > legRequiredMax) legRequiredMax = legRequired[k];
  }

  // a clear region holding the whole slew passes it without the timing, but is only tried briefly
  boxPush(from, to);
  legBox = true;
  boxTrial = PLAN_BOX_TRIAL;
}

PlanLegResult GotoPlanner::legStep() {
  if (legResult != PL_RUNNING) return legResult;
  if (legBox) return boxStep();

  PlanPoint p;
  p.a1 = legTo.a1 >= legFrom.a1 ? legFrom.a1 + legProfile[0].position(legTime) : legFrom.a1 - legProfile[0].position(legTime);
  p.a2 = legTo.a2 >= legFrom.a2 ? legFrom.a2 + legProfile[1].position(legTime) : legFrom.a2 - legProfile[1].position(legTime);

  double altitude;
  double distance = spare(p, &altitude);
  counters.samples++;
  if (distance < legSlack || ++legSamples > PLAN_LEG_SAMPLES_MAX) return legResult = PL_FAIL;

  // horizon slowdown changes the timing, so from here on any path inside the region to the end must be clear
  if (bandEnabled) {
    double band = altitude - bandTop;
    if (band < PLAN_BAND_SWITCH) {
      boxPush(p, legTo);
      legBox = true;
      return PL_RUNNING;
    }
    if (band < distance) distance = band;
  }

  if (legTime >= legDuration) return legResult = PL_PASS;
  legTime += distance/legRate;
  if (legTime > legDuration) legTime = legDuration;
  return PL_RUNNING;
}

PlanLegResult GotoPlanner::boxStep() {
  if (boxCount == 0) return legResult = PL_PASS;

  PlanBox b = box[--boxCount];
  double width1 = b.a1High - b.a1Low;
  double width2 = b.a2High - b.a2Low;
  double halfSize = (width1 + width2)/2.0;
  PlanPoint center = {b.a1Low + width1/2.0, b.a2Low + width2/2.0};

  counters.samples++;
  bool clear = spare(center, NULL) >= halfSize;
  bool divisible = halfSize >= legSlack && boxCount + 2 <= PLAN_BOX_STACK_SIZE;
  if (boxTrial > 0 && (--boxTrial == 0 || !(clear || divisible))) {
    if (clear && boxCount == 0) return legResult = PL_PASS;
    boxTrial = 0;
    legBox = false;
    return PL_RUNNING;
  }
  if (clear) return PL_RUNNING;
  if (!divisible) return legResult = PL_FAIL;

  PlanBox low = b, high = b;
  if (width1 >= width2) { low.a1High = center.a1; high.a1Low = center.a1; } else { low.a2High = center.a2; high.a2Low = center.a2; }
  box[boxCount++] = low;
  box[boxCount++] = high;
  return PL_RUNNING;
}

void GotoPlanner::boxPush(PlanPoint p1, PlanPoint p2) {
  PlanBox b;
  b.a1Low = p1.a1 < p2.a1 ? p1.a1 : p2.a1;
  b.a1High = p1.a1 < p2.a1 ? p2.a1 : p1.a1;
  b.a2Low = p1.a2 < p2.a2 ? p1.a2 : p2.a2;
  b.a2High = p1.a2 < p2.a2 ? p2.a2 : p1.a2;
  box[0] = b;
  boxCount = 1;
}

// estimated slew time in seconds
float GotoPlanner::legCost(PlanPoint from, PlanPoint to) {
  GotoProfile profile;
  profile.init(fabs(to.a1 - from.a1), model[0].accel, model[0].vLow, model[0].vHigh);
  float duration1 = profile.duration();
  profile.init(fabs(to.a2 - from.a2), model[1].accel, model[1].vLow, model[1].vHigh);
  float duration2 = profile.duration();
  return duration1 > duration2 ? duration1 : duration2;
}

float GotoPlanner::legCostLowerBound(PlanPoint from, PlanPoint to) {
  float duration1 = fabs(to.a1 - from.a1)/model[0].vHigh;
  float duration2 = fabs(to.a2 - from.a2)/model[1].vHigh;
  return duration1 > duration2 ? duration1 : duration2;
}

// slew rates and acceleration as applied by Axis::autoGoto() and Axis::poll()
PlanAxisModel GotoPlanner::axisModel(Axis *axis, float rate) {
  PlanAxisModel result;
  float minFreq = axis->getFrequencyMin();
  float maxFreq = axis->getFrequencyMax();

  result.vHigh = rate;
  if (minFreq != 0.0F && result.vHigh < minFreq) result.vHigh = minFreq;
  if (maxFreq != 0.0F && result.vHigh > maxFreq) result.vHigh = maxFreq;

  result.vLow = axis->getBacklashFrequency();
  if (minFreq != 0.0F && result.vLow < minFreq) result.vLow = minFreq;

  result.accel = axis->getSlewAccelerationRate();
  return result;
}

// Transform::instrumentToMount() with the pier side given
Coordinate GotoPlanner::mountCoordinate(PlanPoint p, bool west) {
  Coordinate mount = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, PIER_SIDE_EAST};
  double a1 = p.a1;
  double a2 = p.a2;
  if (west) {
    mount.pierSide = PIER_SIDE_WEST;
    a1 -= Deg180;
    a2 = northern ? Deg180 - a2 : (-Deg180) - a2;
  }
  if (transform.mountType == ALTALT) { mount.aa1 = a1; mount.aa2 = a2; } else { mount.h = a1; mount.d = a2; }
  mount.a1 = a1;
  mount.a2 = a2;
  return mount;
}

// clearance from each limit as Limits::poll() tests them
void GotoPlanner::clearanceSide(PlanPoint p, bool west, double *clearance, double *altitude) {
  Coordinate mount = mountCoordinate(p, west);
  if (transform.mountType == ALTALT) transform.aaToEqu(&mount); else transform.equToAlt(&mount);

  LimitsClearance mountClearance;
  limits.positionClearance(&mount, &mountClearance);

  clearance[PC_ALTITUDE_MIN] = mountClearance.altitudeMin;
  clearance[PC_ALTITUDE_MAX] = mountClearance.altitudeMax;
  clearance[PC_MERIDIAN_E] = mountClearance.meridianEast;
  clearance[PC_MERIDIAN_W] = mountClearance.meridianWest;
  clearance[PC_AXIS1_MIN] = mountClearance.axis1Min;
  clearance[PC_AXIS1_MAX] = mountClearance.axis1Max;
  clearance[PC_AXIS2_MIN] = axis2MinActive ? mountClearance.axis2Min : CLEARANCE_UNLIMITED;
  clearance[PC_AXIS2_MAX] = axis2MaxActive ? mountClearance.axis2Max : CLEARANCE_UNLIMITED;

  if (altitude != NULL) *altitude = mount.a;
}

// clearance valid for any position within that distance, the pier side changes at the pole
// so the other side only applies beyond the distance to the pole
// clearance beyond the tolerance is only exact for the overhead and horizon limits
void GotoPlanner::clearance(PlanPoint p, double *clearance) {
  double other[PC_COUNT];
  bool west = northern ? p.a2 > Deg90 : p.a2 < -Deg90;
  double toPole = fabs(p.a2 - pole);
  clearanceSide(p, west, clearance, NULL);
  if (toPole >= tolerance) return;
  clearanceSide(p, !west, other, NULL);
  for (int k = 0; k < PC_COUNT; k++) {
    double c = other[k] > toPole ? other[k] : toPole;
    if (c < clearance[k]) clearance[k] = c;
  }
}

void GotoPlanner::pointClearance(PlanPoint p, double *clearance) {
  double *known = NULL;
  if (p.a1 == start.a1 && p.a2 == start.a2) known = startClearance; else
  if (p.a1 == end.a1 && p.a2 == end.a2) known = endClearance;
  if (known == NULL) { this->clearance(p, clearance); return; }
  for (int k = 0; k < PC_COUNT; k++) clearance[k] = known[k];
}

// least clearance beyond that required for the leg being tested
double GotoPlanner::spare(PlanPoint p, double *altitude) {
  double c[PC_COUNT];
  bool west = northern ? p.a2 > Deg90 : p.a2 < -Deg90;
  clearanceSide(p, west, c, altitude);

  double result = CLEARANCE_UNLIMITED;
  for (int k = 0; k < PC_COUNT; k++) if (c[k] - legRequired[k] < result) result = c[k] - legRequired[k];

  // the other pier side can only lower the result when the pole is near enough
  double toPole = fabs(p.a2 - pole);
  if (toPole < result + legRequiredMax) {
    double other[PC_COUNT];
    clearanceSide(p, !west, other, NULL);
    result = CLEARANCE_UNLIMITED;
    for (int k = 0; k < PC_COUNT; k++) {
      double o = other[k] > toPole ? other[k] : toPole;
      double combined = o < c[k] ? o : c[k];
      if (combined - legRequired[k] < result) result = combined - legRequired[k];
    }
  }
  return result;
}

bool GotoPlanner::isValid(PlanPoint p) {
  double c[PC_COUNT];
  clearance(p, c);
  for (int k = 0; k < PC_COUNT; k++) if (c[k] < tolerance) return false;
  return true;
}

GotoPlanner gotoPlanner;

#endif
