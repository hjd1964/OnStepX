//--------------------------------------------------------------------------------------------------
// telescope mount control, commands

#include "Axis.h"

#ifdef AXIS_PRESENT

#include "Axis.h"
#include "../convert/Convert.h"

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
          char spm[40]; sprintF(spm, "%1.3f", thisAxis.stepsPerMeasure);
          char ps1[40]; sprintF(ps1, "%1.1f", thisAxis.param1);
          char ps2[40]; sprintF(ps2, "%1.1f", thisAxis.param2);
          char ps3[40]; sprintF(ps3, "%1.1f", thisAxis.param3);
          char ps4[40]; sprintF(ps4, "%1.1f", thisAxis.param4);
          char ps5[40]; sprintF(ps5, "%1.1f", thisAxis.param5);
          char ps6[40]; sprintF(ps6, "%1.1f", thisAxis.param6);
          sprintf(reply,"%s,%d,%d,%d,%s,%s,%s,%s,%s,%s,%c",
            spm,
            (int)thisAxis.reverse,
            (int)round(thisAxis.limits.min),
            (int)round(thisAxis.limits.max),
            ps1, ps2, ps3, ps4, ps5, ps6,
            motor->getParamTypeCode());
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
      if (status.active) {
        strcat(reply, status.standstill ? "ST," : ",");
        strcat(reply, status.outputA.openLoad ? "OA," : ",");
        strcat(reply, status.outputB.openLoad ? "OB," : ",");
        strcat(reply, status.outputA.shortToGround ? "GA," : ",");
        strcat(reply, status.outputB.shortToGround ? "GB," : ",");
        strcat(reply, status.overTemperature ? "OT," : ",");           // > 150C
        strcat(reply, status.overTemperaturePreWarning ? "PW," : ","); // > 120C
        strcat(reply, status.fault ? "GF" : "");
      } else { *commandError = CE_0; return true; }
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
            #ifdef STEP_DIR_MOTOR_PRESENT
              // validate settings for step/dir drivers
              if (motor->driverType == STEP_DIR) {
                if (validateAxisSettings(axisNumber, thisAxis)) {
                  nv.updateBytes(NV_AXIS_SETTINGS_BASE + (axisNumber - 1)*AxisSettingsSize, &thisAxis, sizeof(AxisSettings));
                } else *commandError = CE_PARAM_FORM;
              }
            #endif
            #ifdef SERVO_MOTOR_PRESENT
              // validate settings for servo drivers
              if (motor->driverType == SERVO) {
                if (validateAxisSettings(axisNumber, thisAxis)) {
                  nv.updateBytes(NV_AXIS_SETTINGS_BASE + (axisNumber - 1)*AxisSettingsSize, &thisAxis, sizeof(AxisSettings));
                  // make these take effect now
                  motor->setParam(thisAxis.param1, thisAxis.param2, thisAxis.param3, thisAxis.param4, thisAxis.param5, thisAxis.param6);
                } else *commandError = CE_PARAM_FORM;
              }
            #endif
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
    double f = strtod(ws, &conv_end);
    if (&s[0] != conv_end) a.stepsPerMeasure = f; else return false;
    ws = strchr(ws, ','); if (ws != NULL) {
      ws++; a.reverse = atol(ws);
      ws = strchr(ws, ','); if (ws != NULL) {
        ws++; a.limits.min = atol(ws);
        ws = strchr(ws, ','); if (ws != NULL) {
          ws++; a.limits.max = atol(ws);
          ws = strchr(ws, ','); if (ws != NULL) {
            ws++; a.param1 = atof(ws);
            ws = strchr(ws, ','); if (ws != NULL) {
              ws++; a.param2 = atof(ws);
              ws = strchr(ws, ','); if (ws != NULL) {
                ws++; a.param3 = atof(ws);
                ws = strchr(ws, ','); if (ws != NULL) {
                  ws++; a.param4 = atof(ws);
                  ws = strchr(ws, ','); if (ws != NULL) {
                    ws++; a.param5 = atof(ws);
                    ws = strchr(ws, ','); if (ws != NULL) {
                      ws++; a.param6 = atof(ws);
                      return true;
                    }
                  }
                }
              }
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
  if (!motor->validateParam(a.param1, a.param2, a.param3, a.param4, a.param5, a.param6)) return false;

  int index = axisNum - 1;
  if (index > 3) index = 3;
  int   MinLimitL[4]   = {    -360,      -90,    -360,     0};
  int   MinLimitH[4]   = {     -90,        0,       0,   500};
  int   MaxLimitL[4]   = {      90,        0,       0,     0};
  int   MaxLimitH[4]   = {     360,       90,     360,   500};
  float StepsLimitL[4] = {   150.0,    150.0,     5.0, 0.005};
  float StepsLimitH[4] = {360000.0, 360000.0, 36000.0, 100.0};

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