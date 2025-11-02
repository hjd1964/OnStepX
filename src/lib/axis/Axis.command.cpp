//--------------------------------------------------------------------------------------------------
// telescope mount control, commands

#include "Axis.h"

#ifdef MOTOR_PRESENT

#include "../../lib/nv/Nv.h"

#include "../convert/Convert.h"

bool Axis::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  *supressFrame = false;

  if (command[0] == 'G' && command[1] == 'X') {

    // :GXA[n],[p]# Get axis/motor/driver parameter
    //              Returns: Value
    if (parameter[0] == 'A' && parameter[1] >= '1' && parameter[1] <= '9' && parameter[2] == ',') {

      if (parameter[1] - '0' != axisNumber) return false;

      // return motor/driver name
      if (parameter[3] == 'M' && parameter[4] == 0) {
        strcpy(reply, motor->name());
        *numericReply = false;
        return true;
      }

      int parameterNumber = atoi(&parameter[3]);
      if (parameterNumber < 0 || parameterNumber > getParameterCount()) { *commandError = CE_PARAM_RANGE; return true; }

      uint16_t axesToRevert = nv.readUI(NV_AXIS_SETTINGS_REVERT);

      // check that all axes are not set to revert
      if (bitRead(axesToRevert, 0) == AP_DEFAULTS) { *commandError = CE_0; return true; }

      // check that this axis is not set to revert
      if (bitRead(axesToRevert, axisNumber)) { *commandError = CE_0; return true; }

      // requesting parameter 0 returns the parameter count
      if (parameterNumber == 0) {
        sprintf(reply, "%d", (int)getParameterCount());
        *numericReply = false;
        return true;
      }

      // get parameter values
      double value = ((parameterNumber == 1) ? stepsPerMeasureValueNv : getParameter(parameterNumber)->valueNv);
      float min = getParameter(parameterNumber)->min;
      float max = getParameter(parameterNumber)->max;
      AxisParameterType type = getParameter(parameterNumber)->type;

      // convert to degrees if necessary
      if (getParameter(parameterNumber)->type == AXP_FLOAT_RAD) {
        value = radToDeg(value);
        min = radToDeg(min);
        max = radToDeg(max);
        type = AXP_FLOAT;
      }
      if (getParameter(parameterNumber)->type == AXP_FLOAT_RAD_INV) {
        value = degToRad(value);
        min = degToRad(min);
        max = degToRad(max);
        type = AXP_FLOAT;
      }

      char valueStr[24];
      sprintF(valueStr, "%1.3f", value);
      convert.stripNumericStr(valueStr);

      sprintf(reply, "%s,%ld,%ld,%d,%s", valueStr, lround(min), lround(max), (int)type, getParameter(parameterNumber)->name);

      *numericReply = false;
    }

    #ifdef SERVO_MOTOR_PRESENT
      // :GXS[n]#   Get axis servo delta (in counts) and velocity
      //            Returns: Values
      if (parameter[0] == 'S' && parameter[2] == 0) {
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
    if (parameter[0] == 'U' && parameter[2] == 0) {
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

  // :SXA[n],R#           Revert axis/motor/driver parameters to default
  // :SXA[n],[p],nnnn.n#  Set axis/motor/driver parameter value
  if (command[0] == 'S' && command[1] == 'X' && parameter[0] == 'A' && parameter[2] == ',') {
    uint16_t axesToRevert = nv.readUI(NV_AXIS_SETTINGS_REVERT);

    // check for a valid axisNumber
    int index = parameter[1] - '1';
    if (index + 1 != axisNumber) return false;

    // check if all axes are set to revert
    if (bitRead(axesToRevert, 0) == AP_DEFAULTS) { *commandError = CE_0; return true; }

    // check if this axis is set to revert
    if (bitRead(axesToRevert, axisNumber)) { *commandError = CE_0; return true; }

    // :SXA[n],R# reverts this axis to defaults
    if (parameter[3] == 'R' && parameter[4] == 0) {
      bitSet(axesToRevert, axisNumber);
      nv.update(NV_AXIS_SETTINGS_REVERT, axesToRevert);
      return true;
    }

    // :SXA[n],[p],nnn.n#  Set axis/motor/driver parameter value
    // :SXA[n],[p],nnn.n#  :SXA1,14,12.5#
    int parameterNumber = atoi(&parameter[3]);
    char* valueStr = strchr(&parameter[3], ',');
    if (valueStr == NULL) { *commandError = CE_PARAM_FORM; return true; }
    char* conv_end;
    double value = strtod(++valueStr, &conv_end);
    if (&valueStr[0] == conv_end) { *commandError = CE_PARAM_FORM; return true; }

    // convert to radians if necessary
    if (getParameter(parameterNumber)->type == AXP_FLOAT_RAD) value = degToRad(value);
    if (getParameter(parameterNumber)->type == AXP_FLOAT_RAD_INV) value = radToDeg(value);

    if (parameterIsValid(getParameter(parameterNumber), value, true)) {
      // save parameter values
      getParameter(parameterNumber)->valueNv = value;
      if (parameterNumber == 1) stepsPerMeasureValueNv = value;

      // get ready to write to NV
      AxisStoredSettings nvAxisSettings;
      nvAxisSettings.stepsPerMeasure = stepsPerMeasureValueNv;
      for (int i = 1; i <= getParameterCount(); i++) nvAxisSettings.value[i - 1] = getParameter(i)->valueNv;

      // write the settings to NV
      nv.updateBytes(NV_AXIS_SETTINGS_BASE + (axisNumber - 1)*AxisStoredSettingsSize, &nvAxisSettings, sizeof(AxisStoredSettings));

      // update immediate parameters now
      AxisParameterType type = getParameter(parameterNumber)->type;
      if (type == AXP_BOOLEAN_IMMEDIATE || type == AXP_INTEGER_IMMEDIATE || type == AXP_FLOAT_IMMEDIATE) {
        getParameter(parameterNumber)->value = value;
      }

    } else *commandError = CE_PARAM_RANGE;
  } else return false;

  return true;
}

#endif