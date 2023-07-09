//--------------------------------------------------------------------------------------------------
// telescope mount control, commands

#include "Axis.h"

#ifdef MOTOR_PRESENT

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
          AxisStoredSettings thisAxis;
          nv.readBytes(NV_AXIS_SETTINGS_BASE + index*AxisStoredSettingsSize, &thisAxis, sizeof(AxisStoredSettings));
          if (axisNumber <= 2) {
            // convert axis1 and axis2 into degrees
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
          char ps1[40]; sprintF(ps1, "%1.3f", thisAxis.param1);
          char ps2[40]; sprintF(ps2, "%1.3f", thisAxis.param2);
          char ps3[40]; sprintF(ps3, "%1.3f", thisAxis.param3);
          char ps4[40]; sprintF(ps4, "%1.3f", thisAxis.param4);
          char ps5[40]; sprintF(ps5, "%1.3f", thisAxis.param5);
          char ps6[40]; sprintF(ps6, "%1.3f", thisAxis.param6);
          sprintf(reply,"%s,%d,%d,%d,%s,%s,%s,%s,%s,%s,%c",
            spm,
            (int)thisAxis.reverse,
            (int)round(thisAxis.limits.min),
            (int)round(thisAxis.limits.max),
            ps1, ps2, ps3, ps4, ps5, ps6,
            motor->getParameterTypeCode());
          *numericReply = false;
        } else *commandError = CE_0;
      } else *commandError = CE_0;
    } else

    #ifdef SERVO_MOTOR_PRESENT
      // :GXS[n]#   Get axis servo delta (in counts) and velocity
      //            Returns: Values
      if (parameter[0] == 'S') {
        int index = parameter[1] - '1';
        if (index > 8) { *commandError = CE_PARAM_RANGE; return true; }
        if (index + 1 != axisNumber) return false; // command wasn't processed
        if (motor->driverType != SERVO) { *commandError = CE_CMD_UNKNOWN; return true; } // not a servo

        char temp[20];
        sprintF(temp, "%0.3f", ((ServoMotor*)motor)->velocityPercent);
        sprintf(reply, "%ld,%s", ((ServoMotor*)motor)->delta, temp);
        *numericReply = false;
      } else
    #endif

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
        strcat(reply, status.overTemperatureWarning ? "PW," : ","); // > 120C
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
          AxisStoredSettings thisAxis = settings;
          if (decodeAxisSettings(&parameter[3], thisAxis)) {
            // convert axis1, 2 into radians
            if (axisNumber <= 2) {
              thisAxis.stepsPerMeasure *= RAD_DEG_RATIO;
              thisAxis.limits.min = degToRadF(thisAxis.limits.min);
              thisAxis.limits.max = degToRadF(thisAxis.limits.max);
            } else
            // convert axis > 3 min/max into microns
            if (axisNumber > 3) {
              thisAxis.limits.min = thisAxis.limits.min*1000.0F;
              thisAxis.limits.max = thisAxis.limits.max*1000.0F;
            }
            // save the settings to NV, and update axis immediately if supported
            if (validateAxisSettings(axisNumber, thisAxis)) {
              nv.updateBytes(NV_AXIS_SETTINGS_BASE + (axisNumber - 1)*AxisStoredSettingsSize, &thisAxis, sizeof(AxisStoredSettings));
              if (motor->driverType == SERVO) motor->setParameters(thisAxis.param1, thisAxis.param2, thisAxis.param3, thisAxis.param4, thisAxis.param5, thisAxis.param6);
            } else *commandError = CE_PARAM_FORM;
          } else *commandError = CE_PARAM_FORM;
        }
      } else *commandError = CE_0;
    } else *commandError = CE_0;
  } else return false;

  return true;
}

// convert axis settings string into numeric form
bool Axis::decodeAxisSettings(char *s, AxisStoredSettings &a) {
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
bool Axis::validateAxisSettings(int axisNum, AxisStoredSettings a) {
  if (!motor->validateParameters(a.param1, a.param2, a.param3, a.param4, a.param5, a.param6)) return false;

  int minLimitL, minLimitH, maxLimitL, maxLimitH;
  float stepsLimitL, stepsLimitH;

  if (unitsStr[0] == 'u') {
    minLimitL = 0;
    minLimitH = 500000;
    maxLimitL = 0;
    maxLimitH = 500000;
    stepsLimitL = 0.001;
    stepsLimitH = 1000.0;
  } else {
    minLimitL = -360;
    minLimitH = 360;
    maxLimitL = -360;
    maxLimitH = 360;
    stepsLimitL = 1.0;
    stepsLimitH = 360000.0;
  }

  if (unitsRadians) {
    a.stepsPerMeasure /= RAD_DEG_RATIO;
    a.limits.min = radToDegF(a.limits.min);
    a.limits.max = radToDegF(a.limits.max);
  }

  if (a.stepsPerMeasure < stepsLimitL || a.stepsPerMeasure > stepsLimitH) {
    DF("ERR: Axis::validateAxisSettings(), Axis"); D(axisNum); DF(" bad stepsPerMeasure="); DL(a.stepsPerMeasure);
    return false;
  }

  if (a.reverse != OFF && a.reverse != ON) {
    DF("ERR: Axis::validateAxisSettings(), Axis"); D(axisNum+1); DF(" bad reverse="); DL(a.reverse);
    return false;
  }

  if (a.limits.min < minLimitL || a.limits.min > minLimitH) {
    DF("ERR: Axis::validateAxisSettings(), Axis"); D(axisNum); DF(" bad min="); DL(a.limits.min);
    return false;
  }

  if (a.limits.max < maxLimitL || a.limits.max > maxLimitH) {
    DF("ERR: Axis::validateAxisSettings(), Axis"); D(axisNum); DF(" bad max="); DL(a.limits.max); 
    return false;
  }

  UNUSED(axisNum);

  return true;
}

#endif