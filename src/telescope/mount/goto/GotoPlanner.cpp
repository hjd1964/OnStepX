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
static_assert(GOTO_AVOID_STEP >= 5.0 && GOTO_AVOID_STEP <= 45.0,
  "Configuration (Config.h): Setting GOTO_AVOID_STEP unknown, use a value between 5 and 45 (degrees.)");
static_assert(GOTO_AVOID_REFINE >= 0.1 && GOTO_AVOID_REFINE <= GOTO_AVOID_STEP/2.0,
  "Configuration (Config.h): Setting GOTO_AVOID_REFINE unknown, use a value between 0.1 and GOTO_AVOID_STEP/2 (degrees.)");
static_assert(GOTO_AVOID_FLIP_OVERSHOOT >= 0.0 && GOTO_AVOID_FLIP_OVERSHOOT <= 2.0,
  "Configuration (Config.h): Setting GOTO_AVOID_FLIP_OVERSHOOT unknown, use a value between 0 and 2 (degrees.)");

// clearance a sample may use up before the next sample, as distance moved in radians
#define PLAN_SLACK           degToRad(0.05)
#define PLAN_SLACK_MIN       (2.0*OneArcSec)
// clearance from the horizon slowdown band where predicted timing stops being used
#define PLAN_BAND_SWITCH     degToRad(1.0)
#define PLAN_LEG_SAMPLES_MAX 20000
#define PLAN_WORK_UNITS      8
// samples spent trying to clear the region holding a whole slew before its timing is used
#define PLAN_BOX_TRIAL       1
// furthest a waypoint is moved from the blocked slew
#define PLAN_PUSH_MAX        degToRad(360.0)

void GotoPlanner::begin(PlanPoint start, PlanPoint end, bool search) {
  this->start = start;
  this->end = end;
  this->search = search;

  northern = site.location.latitude >= 0.0;
  pole = northern ? Deg90 : -Deg90;
  tolerance = degToRad(GOTO_AVOID_TOLERANCE);

  // mount axis2 folds back at the pole, so a limit there can't be passed
  axis2MinActive = northern || axis2.getLimitMin() > -Deg90 + OneArcSec;
  axis2MaxActive = !northern || axis2.getLimitMax() < Deg90 - OneArcSec;

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
          if (search) surveyBegin(); else finish(PLAN_FAILED);
        break;
      }
    break;
    case PP_SURVEY: surveyStep(); break;
    case PP_PUSH: pushStep(); break;
    case PP_BISECT: bisectStep(); break;
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

// the blocked slew is surveyed for its highest point and where it comes within the tolerance of the overhead limit
void GotoPlanner::surveyBegin() {
  legBegin(start, end, true);
  surveyTime = 0.0F;
  surveyInterval = (float)(degToRad(GOTO_AVOID_STEP)/10.0/legRate);
  highestAltitude = -Deg90;
  highest = start;
  zoneEntry = start;
  zoneExit = end;
  zoneFound = false;
  phase = PP_SURVEY;
}

void GotoPlanner::surveyStep() {
  PlanPoint p = legPosition(surveyTime);
  double c[PC_COUNT], altitude;
  clearanceSide(p, isWest(p), c, &altitude);
  counters.samples++;

  if (altitude > highestAltitude) { highestAltitude = altitude; highest = p; }
  if (c[PC_ALTITUDE_MAX] < tolerance) {
    if (!zoneFound) zoneEntry = p;
    zoneExit = p;
    zoneFound = true;
  }

  if (surveyTime >= legDuration) { pushBegin(); return; }
  surveyTime += surveyInterval;
  if (surveyTime > legDuration) surveyTime = legDuration;
}

// waypoints are moved in steps away from the nearest image of the zenith, then to either side of the slew,
// a single waypoint from the highest point first then a pair from where the slew enters and leaves the zone
void GotoPlanner::pushBegin() {
  double d1 = zoneExit.a1 - zoneEntry.a1;
  double d2 = zoneExit.a2 - zoneEntry.a2;
  double length = sqrt(d1*d1 + d2*d2);
  if (length < OneArcSec) {
    d1 = end.a1 - start.a1;
    d2 = end.a2 - start.a2;
    length = sqrt(d1*d1 + d2*d2);
  }
  pushDirection[1].a1 = -d2/length;
  pushDirection[1].a2 = d1/length;
  pushDirection[2].a1 = d2/length;
  pushDirection[2].a2 = -d1/length;

  PlanPoint z = nearestZenith(highest);
  d1 = highest.a1 - z.a1;
  d2 = highest.a2 - z.a2;
  length = sqrt(d1*d1 + d2*d2);
  if (length < OneArcSec) pushDirection[0] = pushDirection[1]; else {
    pushDirection[0].a1 = d1/length;
    pushDirection[0].a2 = d2/length;
  }

  pushIndex = 0;
  pushPair = false;
  pushDistance = 0.0;
  candidateActive = false;
  phase = PP_PUSH;
}

void GotoPlanner::pushStep() {
  if (!candidateActive) {
    pushDistance += degToRad(GOTO_AVOID_STEP);
    if (pushDistance > PLAN_PUSH_MAX || pushBlocked()) {
      pushDistance = 0.0;
      if (++pushIndex >= 3) {
        if (pushPair) { finish(PLAN_FAILED); return; }
        pushIndex = 0;
        pushPair = true;
      }
      return;
    }
    candidateBegin(pushDistance);
    return;
  }

  PlanLegResult result = candidateStep();
  if (result == PL_RUNNING) return;
  if (result == PL_PASS) {
    bisectLow = pushDistance - degToRad(GOTO_AVOID_STEP);
    bisectHigh = pushDistance;
    phase = PP_BISECT;
  }
}

// back toward the blocked slew while the waypoints stay clear
void GotoPlanner::bisectStep() {
  if (!candidateActive) {
    if (bisectHigh - bisectLow <= degToRad(GOTO_AVOID_REFINE)) { finish(PLAN_WAYPOINTS); return; }
    candidateBegin((bisectLow + bisectHigh)/2.0);
    return;
  }

  PlanLegResult result = candidateStep();
  if (result == PL_RUNNING) return;
  if (result == PL_PASS) bisectHigh = (bisectLow + bisectHigh)/2.0; else bisectLow = (bisectLow + bisectHigh)/2.0;
}

// a waypoint can move past the overhead limit but stops at any other limit
bool GotoPlanner::pushBlocked() {
  for (uint8_t i = 0; i < (pushPair ? 2 : 1); i++) {
    PlanPoint p = pushPoint(pushPair ? (i == 0 ? zoneEntry : zoneExit) : highest, pushDistance);
    if (!transform.meridianFlips && fabs(p.a2) > Deg90) return true;
    double c[PC_COUNT];
    clearance(p, c);
    for (int k = 0; k < PC_COUNT; k++) if (k != PC_ALTITUDE_MAX && c[k] < tolerance) return true;
  }
  return false;
}

PlanPoint GotoPlanner::pushPoint(PlanPoint from, double distance) {
  PlanPoint p;
  p.a1 = from.a1 + pushDirection[pushIndex].a1*distance;
  p.a2 = from.a2 + pushDirection[pushIndex].a2*distance;
  return p;
}

void GotoPlanner::candidateBegin(double distance) {
  candidateCount = pushPair ? 2 : 1;
  if (pushPair) {
    candidate[0] = pushPoint(zoneEntry, distance);
    candidate[1] = pushPoint(zoneExit, distance);
  } else candidate[0] = pushPoint(highest, distance);
  candidateLeg = 0;
  candidateActive = true;
  legActive = false;
}

// tests the slews through the candidate waypoints, which become the plan if clear
PlanLegResult GotoPlanner::candidateStep() {
  if (!legActive) {
    if (candidateLeg == 0) {
      for (uint8_t i = 0; i < candidateCount; i++) if (!isValid(candidate[i])) { candidateActive = false; return PL_FAIL; }
    }
    PlanPoint from = candidateLeg == 0 ? start : candidate[candidateLeg - 1];
    PlanPoint to = candidateLeg == candidateCount ? end : candidate[candidateLeg];
    legBegin(from, to);
    legActive = true;
  }

  PlanLegResult result = legStep();
  if (result == PL_RUNNING) return PL_RUNNING;
  legActive = false;
  if (result == PL_PASS && ++candidateLeg <= candidateCount) return PL_RUNNING;

  candidateActive = false;
  if (result == PL_PASS) {
    for (uint8_t i = 0; i < candidateCount; i++) waypoint[i] = candidate[i];
    waypointCount = candidateCount;
  }
  return result;
}

// nearest image of the zenith in instrument coordinates, for each pier side used and axis1 turn
PlanPoint GotoPlanner::nearestZenith(PlanPoint p) {
  PlanPoint target = zenith[0];
  double best = CLEARANCE_UNLIMITED;
  int sides = transform.meridianFlips ? 2 : 1;
  for (int side = 0; side < sides; side++) {
    for (int turn = -1; turn <= 1; turn++) {
      PlanPoint z = zenith[side];
      z.a1 += turn*Deg360;
      double distance = fabs(z.a1 - p.a1) + fabs(z.a2 - p.a2);
      if (distance < best) { best = distance; target = z; }
    }
  }
  return target;
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

  PlanPoint p = legPosition(legTime);

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

PlanPoint GotoPlanner::legPosition(float time) {
  PlanPoint p;
  p.a1 = legTo.a1 >= legFrom.a1 ? legFrom.a1 + legProfile[0].position(time) : legFrom.a1 - legProfile[0].position(time);
  p.a2 = legTo.a2 >= legFrom.a2 ? legFrom.a2 + legProfile[1].position(time) : legFrom.a2 - legProfile[1].position(time);
  return p;
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
  bool west = isWest(p);
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
  bool west = isWest(p);
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
  if (!transform.meridianFlips && fabs(p.a2) > Deg90) return false;
  double c[PC_COUNT];
  clearance(p, c);
  for (int k = 0; k < PC_COUNT; k++) if (c[k] < tolerance) return false;
  return true;
}

GotoPlanner gotoPlanner;

#endif
