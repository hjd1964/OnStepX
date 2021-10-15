//--------------------------------------------------------------------------------------------------
// telescope mount control, commands

#include "Axis.h"

#ifdef AXIS_PRESENT

#include "Axis.h"

bool Axis::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  *supressFrame = false;

  if (command[0] == 'G' && command[1] == 'X' && parameter[2] == 0) {
    // :GXA[n]#   Get axis/driver configuration
    //            Returns: Value
    if (parameter[0] == 'A') {
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
          if (axisNumber <= 2) {
            // convert axis1, 2, and 3 into degrees
            thisAxis.stepsPerMeasure /= RAD_DEG_RATIO;
            thisAxis.limits.min = radToDegF(thisAxis.limits.min);
            thisAxis.limits.max = radToDegF(thisAxis.limits.max);
          } else
          if (axisNumber > 3) {
            // convert axis > 3 min/max into mm
            thisAxis.limits.min = thisAxis.limits.min/1000.0F;
            thisAxis.limits.max = thisAxis.limits.max/1000.0F;
          }
          sprintf(reply,"%ld.%03ld,%d,%d,%d,%d,%d",
            (long)thisAxis.stepsPerMeasure,
            (long)(thisAxis.stepsPerMeasure*1000)%1000,
            (int)thisAxis.subdivisions,
            (int)thisAxis.current,
            (int)thisAxis.reverse,
            (int)round(thisAxis.limits.min),
            (int)round(thisAxis.limits.max));
          *numericReply = false;
        } else *commandError = CE_0;
      } else *commandError = CE_0;
    } else

    // :GXU[n]#   Get stepper driver statUs for axis [n]
    //            Returns: Value
    if (parameter[0] == 'U') {
      int index = parameter[1] - '1';
      if (index > 8) { *commandError = CE_PARAM_RANGE; return true; }
      if (index + 1 != axisNumber) return false; // command wasn't processed
      DriverStatus status = getStatus();
      strcat(reply, status.standstill ? "ST," : ",");
      strcat(reply, status.outputA.openLoad ? "OA," : ",");
      strcat(reply, status.outputB.openLoad ? "OB," : ",");
      strcat(reply, status.outputA.shortToGround ? "GA," : ",");
      strcat(reply, status.outputB.shortToGround ? "GB," : ",");
      strcat(reply, status.overTemperature ? "OT," : ",");           // > 150C
      strcat(reply, status.overTemperaturePreWarning ? "PW," : ","); // > 120C
      strcat(reply, status.fault ? "GF" : "");
      *numericReply = false;
    } else return false;
  } else

  // :SXA[n]#   Set axis/driver configuration
  if (command[0] == 'S' && command[1] == 'X' && parameter[0] == 'A' && parameter[2] == ',') {
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
          bitSet(axesToRevert, axisNumber);
          nv.update(NV_AXIS_SETTINGS_REVERT, axesToRevert);
        } else {
          // :SXA[n],[sssss...]#
          AxisSettings thisAxis = settings;
          if (decodeAxisSettings(&parameter[3], thisAxis)) {
            if (axisNumber <= 2) {
              // convert axis1, 2 into radians
              thisAxis.stepsPerMeasure *= RAD_DEG_RATIO;
              thisAxis.limits.min = degToRadF(thisAxis.limits.min);
              thisAxis.limits.max = degToRadF(thisAxis.limits.max);
            } else
            if (axisNumber > 3) {
              // convert axis > 3 min/max into um
              thisAxis.limits.min = thisAxis.limits.min*1000.0F;
              thisAxis.limits.max = thisAxis.limits.max*1000.0F;
            }
            // validate settings for step/dir drivers
            if (motor->driverType == STEP_DIR) {
              if (validateAxisSettings(axisNumber, thisAxis)) {
                #ifdef SD_DRIVER_PRESENT
                  int subdivGoto = ((StepDirMotor*)motor)->driver->getSubdivisionsGoto();
                  if (axisNumber <= 2 && thisAxis.subdivisions < subdivGoto) thisAxis.subdivisions = subdivGoto;
                  if (((StepDirMotor*)motor)->driver->subdivisionsToCode(thisAxis.subdivisions) != OFF) {
                    nv.updateBytes(NV_AXIS_SETTINGS_BASE + (axisNumber - 1)*AxisSettingsSize, &thisAxis, sizeof(AxisSettings));
                    *numericReply = false;
                  } else *commandError = CE_PARAM_RANGE;
                #endif
              } else *commandError = CE_PARAM_FORM;
            }
          } else *commandError = CE_PARAM_FORM;
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
      ws++; a.subdivisions = atol(ws);
      ws = strchr(ws, ','); if (ws != NULL) {
        ws++; a.current = atol(ws);
        ws = strchr(ws, ','); if (ws != NULL) {
          ws++; a.reverse = atol(ws);
          ws = strchr(ws, ','); if (ws != NULL) {
            ws++; a.limits.min = atol(ws);
            ws = strchr(ws, ','); if (ws != NULL) {
              ws++; a.limits.max = atol(ws);
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
bool Axis::validateAxisSettings(int axisNum, AxisSettings a) {
  int index = axisNum - 1;
  if (index > 3) index = 3;
  int   MinLimitL[4]   = {    -360,      -90,    -360,     0};
  int   MinLimitH[4]   = {     -90,        0,       0,   500};
  int   MaxLimitL[4]   = {      90,        0,       0,     0};
  int   MaxLimitH[4]   = {     360,       90,     360,   500};
  float StepsLimitL[4] = {   150.0,    150.0,     5.0, 0.005};
  float StepsLimitH[4] = {360000.0, 360000.0, 36000.0, 100.0};
  int   IrunLimitH[4]  = {    3000,     3000,    1000,  1000};

  if (axisNum <= 2) {
    // convert axis1 & 2 into degrees
    a.stepsPerMeasure /= RAD_DEG_RATIO;
    a.limits.min = radToDegF(a.limits.min);
    a.limits.max = radToDegF(a.limits.max);
  } else
  if (axisNum > 3) {
    // convert axis > 3 min/max into mm
    a.limits.min = a.limits.min/1000.0F;
    a.limits.max = a.limits.max/1000.0F;
  }

  if (a.stepsPerMeasure < StepsLimitL[index] || a.stepsPerMeasure > StepsLimitH[index]) {
    DF("ERR, Axis::validateAxisSettings(): Axis"); D(axisNum); DF(" bad stepsPerMeasure="); DL(a.stepsPerMeasure);
    return false;
  }
  if (a.subdivisions != OFF && (a.subdivisions < 1 || a.subdivisions > 256)) {
    DF("ERR, Axis::validateAxisSettings(): Axis"); D(axisNum); DF(" bad subdivisions="); DL(a.subdivisions);
    return false;
  }
  if (a.current != OFF && (a.current < 0 || a.current > IrunLimitH[index])) {
    DF("ERR, Axis::validateAxisSettings(): Axis"); D(axisNum); DF(" bad current="); DL(a.current);
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

#endif