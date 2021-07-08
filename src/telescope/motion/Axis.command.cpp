//--------------------------------------------------------------------------------------------------
// telescope mount control, commands

#include "../../Common.h"

#include "../mount/coordinates/Transform.h"
#include "../mount/site/Site.h"
#include "../../commands/ProcessCmds.h"

#include "Axis.h"

int8_t driverModels[9] = {AXIS1_DRIVER_MODEL, AXIS2_DRIVER_MODEL, AXIS3_DRIVER_MODEL, AXIS4_DRIVER_MODEL, AXIS5_DRIVER_MODEL, AXIS6_DRIVER_MODEL, AXIS7_DRIVER_MODEL, AXIS8_DRIVER_MODEL, AXIS9_DRIVER_MODEL};
int8_t driverMicrostepsGoto[2] = {AXIS1_DRIVER_MICROSTEPS_GOTO, AXIS2_DRIVER_MICROSTEPS_GOTO};

bool Axis::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  *supressFrame = false;

  // :GXA[n]#   Get axis/driver configuration
  //            Returns: Value
  if (cmdGX("GXA")) {
    int index = parameter[1] - '1';
    if (index > 8) { *commandError = CE_PARAM_RANGE; return true; }
    if (index + 1 != axisNumber) return false; // command wasn't processed
    uint16_t axesToRevert = nv.readUI(NV_AXIS_SETTINGS_REVERT);

    // check that all axes are not set to revert
    if (axesToRevert & 1) {
      // check that this axis is not set to revert
      if (!(axesToRevert & (1 << axisNumber))) {
        AxisSettings thisAxis;
        nv.readBytes(NV_AXIS_SETTINGS_BASE + index*AxisSettingsSize, &thisAxis, sizeof(AxisSettings));
        if (axisNumber <= 3) {
          // convert axis1, 2, and 3 into degrees
          thisAxis.stepsPerMeasure /= RAD_DEG_RATIO;
          thisAxis.limits.min = radToDeg(thisAxis.limits.min);
          thisAxis.limits.max = radToDeg(thisAxis.limits.max);
        } else {
          // convert axis > 3 min/max into mm
          thisAxis.limits.min = thisAxis.limits.min/1000.0F;
          thisAxis.limits.max = thisAxis.limits.max/1000.0F;
        }
        sprintf(reply,"%ld.%03ld,%d,%d,%d,%d,%d",
          (long)thisAxis.stepsPerMeasure,
          (long)(thisAxis.stepsPerMeasure*1000)%1000,
          (int)thisAxis.microsteps,
          (int)thisAxis.currentRun,
          (int)thisAxis.reverse,
          (int)round(thisAxis.limits.min),
          (int)round(thisAxis.limits.max));
        *numericReply = false;
      } else *commandError = CE_0;
    } else *commandError = CE_0;
  } else

  // :GXUn#     Get stepper driver statUs (all axes)
  //            Returns: Value
  if (cmdGX("GXU")) {
    int index = parameter[1] - '1';
    if (index > 8) { *commandError = CE_PARAM_RANGE; return true; }
    if (index + 1 != axisNumber) return false; // command wasn't processed
    DriverStatus status = driver.getStatus();
    strcat(reply, status.standstill ? "ST," : ",");
    strcat(reply, status.outputA.openLoad ? "OA," : ",");
    strcat(reply, status.outputB.openLoad ? "OB," : ",");
    strcat(reply, status.outputA.shortToGround ? "GA," : ",");
    strcat(reply, status.outputB.shortToGround ? "GB," : ",");
    strcat(reply, status.overTemperature ? "OT," : ",");           // > 150C
    strcat(reply, status.overTemperaturePreWarning ? "PW," : ","); // > 120C
    strcat(reply, status.fault ? "GF" : "");
    *numericReply = false;
  } else

  // :SXA[n]#   Set axis/driver configuration
  if (cmdSX("SXA")) {
    uint16_t axesToRevert = nv.readUI(NV_AXIS_SETTINGS_REVERT);

    // check for a valid axisNumber
    int index = parameter[1] - '1';
    if (index + 1 != axisNumber) return false;
    // check that all axes are not set to revert
    if (axesToRevert & 1) {
      // check that this axis is not set to revert
      if (!(axesToRevert & (1 << axisNumber))) {
        if (parameter[3] == 'R' && parameter[4] == 0) {
          // :SXA[n],R# reverts this axis to defaults
          axesToRevert |= 1 << axisNumber;
          bitSet(axesToRevert, axisNumber);
          nv.update(NV_AXIS_SETTINGS_REVERT, axesToRevert);
        } else {
          // :SXA[n],[sssss...]#
          AxisSettings thisAxis;
          if (decodeAxisSettings(&parameter[3], thisAxis)) {
            if (axisNumber <= 3) {
              // convert axis1, 2, and 3 into radians
              thisAxis.stepsPerMeasure *= RAD_DEG_RATIO;
              thisAxis.limits.min = degToRad(thisAxis.limits.min);
              thisAxis.limits.max = degToRad(thisAxis.limits.max);
            } else {
              // convert axis > 3 min/max into um
              thisAxis.limits.min = thisAxis.limits.min*1000.0F;
              thisAxis.limits.max = thisAxis.limits.max*1000.0F;
            }
            if (validateAxisSettings(axisNumber, MOUNT_TYPE == ALTAZM, thisAxis)) {
              if (axisNumber <= 2 && thisAxis.microsteps < driverMicrostepsGoto[index]) thisAxis.microsteps = driverMicrostepsGoto[index];
              if (driver.microstepsToCode(driverModels[index], thisAxis.microsteps) != OFF) {
                nv.updateBytes(NV_AXIS_SETTINGS_BASE + (axisNumber - 1)*AxisSettingsSize, &thisAxis, sizeof(AxisSettings));
                *numericReply = false;
              } else *commandError = CE_PARAM_RANGE;
            }
          }
        }
      } else *commandError = CE_0;
    } else *commandError = CE_0;
  } else return false;

  return true;
}

// convert axis settings string into numeric form
bool Axis::decodeAxisSettings(char *s, AxisSettings &a) {
  if (strcmp(s, "0") != 0) {
    char *ws = s;
    char *conv_end;
    double f = strtod(ws, &conv_end); if (&s[0] != conv_end) a.stepsPerMeasure = f; else return false;
    ws = strchr(ws, ','); if (ws != NULL) {
      ws++; a.microsteps = strtol(ws, NULL, 10);
      ws = strchr(ws, ','); if (ws != NULL) {
        ws++; a.currentRun = strtol(ws, NULL, 10);
        ws = strchr(ws, ','); if (ws != NULL) {
          ws++; a.reverse = strtol(ws, NULL, 10);
          ws = strchr(ws, ','); if (ws != NULL) {
            ws++; a.limits.min = strtol(ws, NULL, 10);
            ws = strchr(ws, ','); if (ws != NULL) {
              ws++; a.limits.max = strtol(ws, NULL, 10);
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}

// convert axis settings string into numeric form
bool Axis::validateAxisSettings(int axisNum, bool altAz, AxisSettings a) {
  int index = axisNum - 1;
  if (index > 4) index = 4;
  int   MinLimitL[4]   = {-270,-90,-360,  0};
  int   MinLimitH[4]   = { -90,  0,   0,500};
  int   MaxLimitL[4]   = {  90,  0,   0,  0};
  int   MaxLimitH[4]   = { 270, 90, 360,500};
  float StepsLimitL[4] = {   150.0,   150.0,    5.0, 0.005};
  float StepsLimitH[4] = {122400.0,122400.0, 7200.0,  20.0};
  int   IrunLimitH[4]  = { 3000, 3000, 1000, 1000};
  if (altAz) {
    MinLimitL[0] = -360; MinLimitH[0] = -180; MaxLimitL[0] = 180; MaxLimitH[0] = 360;
  }

  if (axisNum <= 3) {
    // convert axis1, 2, and 3 into degrees
    a.stepsPerMeasure /= RAD_DEG_RATIO;
    a.limits.min = radToDeg(a.limits.min);
    a.limits.max = radToDeg(a.limits.max);
  } else {
    // convert axis > 3 into min/max into mm
    a.limits.min = a.limits.min/1000.0F;
    a.limits.max = a.limits.max/1000.0F;
  }

  if (a.stepsPerMeasure < StepsLimitL[index] || a.stepsPerMeasure > StepsLimitH[index]) {
    DF("ERR, Axis::validateAxisSettings(): Axis"); D(axisNum); DF(" bad stepsPerMeasure="); DL(a.stepsPerMeasure);
    return false;
  }
  if (a.microsteps != OFF && (a.microsteps < 1 || a.microsteps > 256)) {
    DF("ERR, Axis::validateAxisSettings(): Axis"); D(axisNum); DF(" bad microsteps="); DL(a.microsteps);
    return false;
  }
  if (a.currentRun != OFF && (a.currentRun < 0 || a.currentRun > IrunLimitH[index])) {
    DF("ERR, Axis::validateAxisSettings(): Axis"); D(axisNum); DF(" bad IRUN="); DL(a.currentRun);
    return false;
  }
  if (a.reverse != OFF && a.reverse != ON) {
    DF("ERR, Axis::validateAxisSettings(): Axis"); D(axisNum+1); DF(" bad reverse="); DL(a.reverse);
    return false;
  }
  if (a.limits.min < MinLimitL[index] || a.limits.min > MinLimitH[index]) {
    DF("ERR, Axis::validateAxisSettings(): Axis"); D(axisNum); DF(" bad min="); DL(a.limits.min);
    return false;
  }
  if (a.limits.max < MaxLimitL[index] || a.limits.max > MaxLimitH[index]) {
    DF("ERR, Axis::validateAxisSettings(): Axis"); D(axisNum); DF(" bad max="); DL(a.limits.max); 
    return false;
  }
  return true;
}
