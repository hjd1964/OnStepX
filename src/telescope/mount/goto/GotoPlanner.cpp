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
#define PLAN_SLACK           ((float)degToRad(0.05))
#define PLAN_SLACK_MIN       ((float)(2.0*OneArcSec))
// clearance allowed for single precision rounding, a slew may end exactly at its required clearance
#define PLAN_ROUNDING        ((float)(2.0*OneArcSec))
// clearance from the horizon slowdown band where predicted timing stops being used
#define PLAN_BAND_SWITCH     ((float)degToRad(1.0))
// samples allowed for one slew and for a whole plan, bounding planning time, a slew or plan needing more is treated as blocked
#define PLAN_LEG_SAMPLES_MAX 1000
#define PLAN_SAMPLES_MAX     5000
#define PLAN_WORK_UNITS      8
#define PLAN_ARCSEC          ((float)OneArcSec)
#define PLAN_STEP            ((float)degToRad(GOTO_AVOID_STEP))
#define PLAN_REFINE          ((float)degToRad(GOTO_AVOID_REFINE))
// samples spent trying to clear the region holding a whole slew before its timing is used
#define PLAN_BOX_TRIAL       1
// furthest a waypoint is moved from the blocked slew
#define PLAN_PUSH_MAX        ((float)degToRad(360.0))

void GotoPlanner::begin(PlanPoint start, PlanPoint end, bool search) {
  this->start = start;
  this->end = end;
  this->search = search;

  northern = site.location.latitude >= 0.0;
  pole = northern ? PLAN_DEG90 : -PLAN_DEG90;
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

  latitudeSine = site.locationEx.latitude.sine;
  latitudeCosine = site.locationEx.latitude.cosine;

  // instrument coordinates of the zenith for each pier side
  float a2 = transform.mountType == ALTALT ? 0.0F : site.location.latitude;
  zenith[0].a1 = 0.0F;
  zenith[0].a2 = a2;
  zenith[1].a1 = PLAN_DEG180;
  zenith[1].a2 = northern ? PLAN_DEG180 - a2 : (-PLAN_DEG180) - a2;

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
  if (counters.samples > PLAN_SAMPLES_MAX) { finish(PLAN_FAILED); return; }
  switch (phase) {
    case PP_DIRECT:
      // other limits are left to the existing behaviour on the direct path
      if (!legActive) { legBegin(start, end, true); legActive = true; }
      switch (legStep()) {
        case PL_RUNNING: break;
        case PL_PASS: finish(PLAN_DIRECT); break;
        case PL_FAIL:
          legActive = false;
          if (search) surveyBegin(legTime); else finish(PLAN_FAILED);
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

// the blocked slew is surveyed for its highest point and where it is within the tolerance of the overhead limit,
// starting where the direct test failed: walk on through the zone, find the highest point by golden section search
// around the highest sample, then bisect for where the slew enters and leaves the zone
#define PLAN_GOLDEN 0.618034F

void GotoPlanner::surveyBegin(float time) {
  legBegin(start, end, true);
  surveyTime = time;
  surveyInterval = PLAN_STEP/4.0F/legRate;
  surveyResolution = PLAN_STEP/10.0F/legRate;
  highestAltitude = -PLAN_DEG90;
  highestTime = time;
  highest = legPosition(time);
  zoneEntry = highest;
  zoneExit = end;
  zoneEntered = false;
  zoneLeft = false;
  entryLow = time;
  entryHigh = time;
  surveyStage = SS_WALK;
  phase = PP_SURVEY;
}

void GotoPlanner::surveyStep() {
  switch (surveyStage) {
    case SS_WALK: {
      bool inZone;
      surveySample(surveyTime, &inZone);
      if (!zoneEntered) {
        if (inZone) { zoneEntered = true; entryHigh = surveyTime; exitLow = surveyTime; } else entryLow = surveyTime;
      } else
      if (!inZone) {
        zoneLeft = true;
        exitHigh = surveyTime;
        surveyPeakBegin();
        return;
      } else exitLow = surveyTime;
      if (surveyTime >= legDuration) { surveyPeakBegin(); return; }
      surveyTime += surveyInterval;
      if (surveyTime > legDuration) surveyTime = legDuration;
    } break;

    case SS_PEAK:
      if (goldenNeed == 1) { goldenAltitudeC = surveySample(goldenC, NULL); goldenNeed = 2; return; }
      if (goldenNeed == 2) { goldenAltitudeD = surveySample(goldenD, NULL); goldenNeed = 0; return; }
      if (goldenHigh - goldenLow <= surveyResolution) {
        surveyStage = zoneEntered ? SS_ENTRY : SS_EXIT;
        return;
      }
      if (goldenAltitudeC > goldenAltitudeD) {
        goldenHigh = goldenD;
        goldenD = goldenC;
        goldenAltitudeD = goldenAltitudeC;
        goldenC = goldenHigh - PLAN_GOLDEN*(goldenHigh - goldenLow);
        goldenAltitudeC = surveySample(goldenC, NULL);
      } else {
        goldenLow = goldenC;
        goldenC = goldenD;
        goldenAltitudeC = goldenAltitudeD;
        goldenD = goldenLow + PLAN_GOLDEN*(goldenHigh - goldenLow);
        goldenAltitudeD = surveySample(goldenD, NULL);
      }
    break;

    case SS_ENTRY:
      if (entryHigh - entryLow <= surveyResolution) {
        zoneEntry = legPosition(entryHigh);
        surveyStage = SS_EXIT;
        return;
      } else {
        bool inZone;
        surveySample((entryLow + entryHigh)/2.0F, &inZone);
        if (inZone) entryHigh = (entryLow + entryHigh)/2.0F; else entryLow = (entryLow + entryHigh)/2.0F;
      }
    break;

    case SS_EXIT:
      if (!zoneLeft || exitHigh - exitLow <= surveyResolution) {
        zoneExit = zoneLeft ? legPosition(exitLow) : end;
        pushBegin();
        return;
      }
      bool inZone;
      surveySample((exitLow + exitHigh)/2.0F, &inZone);
      if (inZone) exitLow = (exitLow + exitHigh)/2.0F; else exitHigh = (exitLow + exitHigh)/2.0F;
    break;
  }
}

// the highest point is bracketed by the samples either side of the highest walk sample
void GotoPlanner::surveyPeakBegin() {
  goldenLow = highestTime - surveyInterval;
  if (goldenLow < 0.0F) goldenLow = 0.0F;
  goldenHigh = highestTime + surveyInterval;
  if (goldenHigh > legDuration) goldenHigh = legDuration;
  goldenC = goldenHigh - PLAN_GOLDEN*(goldenHigh - goldenLow);
  goldenD = goldenLow + PLAN_GOLDEN*(goldenHigh - goldenLow);
  goldenNeed = 1;
  surveyStage = SS_PEAK;
}

// altitude of the slew at this time and if it is within the tolerance of the overhead limit, keeping the highest point found
float GotoPlanner::surveySample(float time, bool *inZone) {
  PlanPoint p = legPosition(time);
  float c[PC_COUNT], altitude;
  clearanceSide(p, isWest(p), c, &altitude);
  counters.samples++;
  if (altitude > highestAltitude) { highestAltitude = altitude; highestTime = time; highest = p; }
  if (inZone != NULL) *inZone = c[PC_ALTITUDE_MAX] < tolerance;
  return altitude;
}

// waypoints are moved in steps away from the nearest image of the zenith, then to either side of the slew,
// a single waypoint from the highest point first then a pair from where the slew enters and leaves the zone
void GotoPlanner::pushBegin() {
  float d1 = zoneExit.a1 - zoneEntry.a1;
  float d2 = zoneExit.a2 - zoneEntry.a2;
  float length = sqrtf(d1*d1 + d2*d2);
  if (length < PLAN_ARCSEC) {
    d1 = end.a1 - start.a1;
    d2 = end.a2 - start.a2;
    length = sqrtf(d1*d1 + d2*d2);
  }
  pushDirection[1].a1 = -d2/length;
  pushDirection[1].a2 = d1/length;
  pushDirection[2].a1 = d2/length;
  pushDirection[2].a2 = -d1/length;

  PlanPoint z = nearestZenith(highest);
  d1 = highest.a1 - z.a1;
  d2 = highest.a2 - z.a2;
  length = sqrtf(d1*d1 + d2*d2);
  if (length < PLAN_ARCSEC) pushDirection[0] = pushDirection[1]; else {
    pushDirection[0].a1 = d1/length;
    pushDirection[0].a2 = d2/length;
  }

  pushIndex = 0;
  pushPair = false;
  pushDistance = 0.0F;
  candidateActive = false;
  phase = PP_PUSH;
}

void GotoPlanner::pushStep() {
  if (!candidateActive) {
    pushDistance += PLAN_STEP;
    if (pushDistance > PLAN_PUSH_MAX || pushBlocked()) {
      pushDistance = 0.0F;
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
    bisectLow = pushDistance - PLAN_STEP;
    bisectHigh = pushDistance;
    phase = PP_BISECT;
  }
}

// back toward the blocked slew while the waypoints stay clear
void GotoPlanner::bisectStep() {
  if (!candidateActive) {
    if (bisectHigh - bisectLow <= PLAN_REFINE) { finish(PLAN_WAYPOINTS); return; }
    candidateBegin((bisectLow + bisectHigh)/2.0F);
    return;
  }

  PlanLegResult result = candidateStep();
  if (result == PL_RUNNING) return;
  if (result == PL_PASS) bisectHigh = (bisectLow + bisectHigh)/2.0F; else bisectLow = (bisectLow + bisectHigh)/2.0F;
}

// a waypoint can move past the overhead limit but stops at any other limit
bool GotoPlanner::pushBlocked() {
  for (uint8_t i = 0; i < (pushPair ? 2 : 1); i++) {
    PlanPoint p = pushPoint(pushPair ? (i == 0 ? zoneEntry : zoneExit) : highest, pushDistance);
    if (!transform.meridianFlips && fabsf(p.a2) > PLAN_DEG90) return true;
    float c[PC_COUNT];
    clearance(p, c);
    for (int k = 0; k < PC_COUNT; k++) if (k != PC_ALTITUDE_MAX && c[k] < tolerance) return true;
  }
  return false;
}

PlanPoint GotoPlanner::pushPoint(PlanPoint from, float distance) {
  PlanPoint p;
  p.a1 = from.a1 + pushDirection[pushIndex].a1*distance;
  p.a2 = from.a2 + pushDirection[pushIndex].a2*distance;
  return p;
}

void GotoPlanner::candidateBegin(float distance) {
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
  float best = CLEARANCE_UNLIMITED;
  int sides = transform.meridianFlips ? 2 : 1;
  for (int side = 0; side < sides; side++) {
    for (int turn = -1; turn <= 1; turn++) {
      PlanPoint z = zenith[side];
      z.a1 += turn*PLAN_DEG360;
      float distance = fabsf(z.a1 - p.a1) + fabsf(z.a2 - p.a2);
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
  float distance[2] = { (float)fabsf(to.a1 - from.a1), (float)fabsf(to.a2 - from.a2) };
  for (int i = 0; i < 2; i++) legProfile[i].init(distance[i], model[i].accel, model[i].vLow, model[i].vHigh);
  legDuration = legProfile[0].duration() > legProfile[1].duration() ? legProfile[0].duration() : legProfile[1].duration();
  legRate1 = legProfile[0].rateMax();
  legRate2 = legProfile[1].rateMax();
  legRate = legRate1 + legRate2;
  legTime = 0.0F;
  legSamples = 0;
  legBox = false;
  boxCount = 0;
  legResult = PL_RUNNING;
  counters.legs++;

  // a limit already exceeded at the start is left to the existing limit recovery behaviour
  float cFrom[PC_COUNT], cTo[PC_COUNT];
  pointClearance(from, cFrom);
  pointClearance(to, cTo);
  float requiredMin = tolerance;
  for (int k = 0; k < PC_COUNT; k++) {
    if (cFrom[k] <= 0.0F || (overheadOnly && k != PC_ALTITUDE_MAX)) { legRequired[k] = -CLEARANCE_UNLIMITED; continue; }
    if (cTo[k] < 0.0F) legResult = PL_FAIL;
    float required = tolerance;
    if (cFrom[k] < required) required = cFrom[k];
    if (cTo[k] < required) required = cTo[k] > 0.0F ? cTo[k] : 0.0F;
    legRequired[k] = required;
    if (required < requiredMin) requiredMin = required;
  }

  legSlack = requiredMin/2.0F;
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

  float altitude;
  float step;
  float distance = spare(p, &altitude, &step);
  counters.samples++;
  if (distance < legSlack - PLAN_ROUNDING || ++legSamples > PLAN_LEG_SAMPLES_MAX) return legResult = PL_FAIL;

  // horizon slowdown changes the timing, so from here on any path inside the region to the end must be clear
  if (bandEnabled) {
    float band = altitude - bandTop;
    if (band < PLAN_BAND_SWITCH) {
      boxPush(p, legTo);
      legBox = true;
      return PL_RUNNING;
    }
    float bandStep = (float)(band/altitudeRate(p));
    if (bandStep < step) step = bandStep;
  }

  if (legTime >= legDuration) return legResult = PL_PASS;
  legTime += step;
  if (legTime > legDuration) legTime = legDuration;
  return PL_RUNNING;
}

PlanLegResult GotoPlanner::boxStep() {
  if (boxCount == 0) return legResult = PL_PASS;

  PlanBox b = box[--boxCount];
  float width1 = b.a1High - b.a1Low;
  float width2 = b.a2High - b.a2Low;
  float halfSize = (width1 + width2)/2.0F;
  PlanPoint center = {b.a1Low + width1/2.0F, b.a2Low + width2/2.0F};

  counters.samples++;
  if (++legSamples > PLAN_LEG_SAMPLES_MAX) return legResult = PL_FAIL;
  bool clear = spare(center, NULL, NULL) >= halfSize;
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
  float a1 = p.a1;
  float a2 = p.a2;
  if (west) {
    mount.pierSide = PIER_SIDE_WEST;
    a1 -= PLAN_DEG180;
    a2 = northern ? PLAN_DEG180 - a2 : (-PLAN_DEG180) - a2;
  }
  if (transform.mountType == ALTALT) { mount.aa1 = a1; mount.aa2 = a2; } else { mount.h = a1; mount.d = a2; }
  mount.a1 = a1;
  mount.a2 = a2;
  return mount;
}

// clearance from each limit as Limits::poll() tests them
void GotoPlanner::clearanceSide(PlanPoint p, bool west, float *clearance, float *altitude) {
  Coordinate mount = mountCoordinate(p, west);
  // altitude as Transform::aaToHor() and Transform::equToAlt() find it
  float sinAlt;
  if (transform.mountType == ALTALT) sinAlt = cosf(mount.aa2)*cosf(mount.aa1); else
    sinAlt = sinf(mount.d)*latitudeSine + cosf(mount.d)*latitudeCosine*cosf(mount.h);
  if (sinAlt > 1.0F) sinAlt = 1.0F;
  if (sinAlt < -1.0F) sinAlt = -1.0F;
  mount.a = asinf(sinAlt);

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
void GotoPlanner::clearance(PlanPoint p, float *clearance) {
  float other[PC_COUNT];
  bool west = isWest(p);
  float toPole = fabsf(p.a2 - pole);
  clearanceSide(p, west, clearance, NULL);
  if (toPole >= tolerance) return;
  clearanceSide(p, !west, other, NULL);
  for (int k = 0; k < PC_COUNT; k++) {
    float c = other[k] > toPole ? other[k] : toPole;
    if (c < clearance[k]) clearance[k] = c;
  }
}

void GotoPlanner::pointClearance(PlanPoint p, float *clearance) {
  float *known = NULL;
  if (p.a1 == start.a1 && p.a2 == start.a2) known = startClearance; else
  if (p.a1 == end.a1 && p.a2 == end.a2) known = endClearance;
  if (known == NULL) { this->clearance(p, clearance); return; }
  for (int k = 0; k < PC_COUNT; k++) clearance[k] = known[k];
}

// least clearance beyond that required for the leg being tested, and the time before any limit could use it up
float GotoPlanner::spare(PlanPoint p, float *altitude, float *step) {
  float margin[PC_COUNT];
  bool west = isWest(p);
  clearanceSide(p, west, margin, altitude);

  float result = CLEARANCE_UNLIMITED;
  for (int k = 0; k < PC_COUNT; k++) {
    margin[k] -= legRequired[k];
    if (margin[k] < result) result = margin[k];
  }

  // the other pier side can only lower the result when the pole is near enough
  float toPole = fabsf(p.a2 - pole);
  bool poleNear = toPole < result + legRequiredMax;
  if (poleNear) {
    float other[PC_COUNT];
    clearanceSide(p, !west, other, NULL);
    result = CLEARANCE_UNLIMITED;
    for (int k = 0; k < PC_COUNT; k++) {
      float o = (other[k] > toPole ? other[k] : toPole) - legRequired[k];
      if (o < margin[k]) margin[k] = o;
      if (margin[k] < result) result = margin[k];
    }
  }

  if (step != NULL) {
    // altitude changes no faster than the distance moved on the sky, the other limits are on one axis coordinate
    // except near the pole where the pier side changes
    float rateAltitude = altitudeRate(p);
    float rate[PC_COUNT] = { rateAltitude, rateAltitude, legRate1, legRate1, legRate1, legRate1, legRate2, legRate2 };
    bool sideLimits = false;
    *step = legDuration;
    for (int k = 0; k < PC_COUNT; k++) {
      if (legRequired[k] <= -CLEARANCE_UNLIMITED) continue;
      if (k >= PC_MERIDIAN_E) {
        sideLimits = true;
        if (poleNear) rate[k] = legRate;
      }
      if (rate[k] > 0.0F && margin[k]/rate[k] < *step) *step = margin[k]/rate[k];
    }
    if (sideLimits && !poleNear && legRate2 > 0.0F && (toPole - legRequiredMax)/legRate2 < *step) *step = (toPole - legRequiredMax)/legRate2;
  }

  return result;
}

// fastest change in altitude, from the distance moved on the sky by each axis at this axis2 coordinate
float GotoPlanner::altitudeRate(PlanPoint p) {
  return legRate2 + fabsf(cosf(p.a2))*legRate1;
}

bool GotoPlanner::isValid(PlanPoint p) {
  if (!transform.meridianFlips && fabsf(p.a2) > PLAN_DEG90) return false;
  float c[PC_COUNT];
  clearance(p, c);
  for (int k = 0; k < PC_COUNT; k++) if (c[k] < tolerance) return false;
  return true;
}

GotoPlanner gotoPlanner;

#endif
