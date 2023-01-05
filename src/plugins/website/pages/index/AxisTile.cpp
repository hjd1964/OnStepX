// -----------------------------------------------------------------------------------
// Axis tile
#include "AxisTile.h"

#include "../KeyValue.h"
#include "../Pages.common.h"

void sendAxisParams(AxisSettings* a, int axis);

// create the related webpage tile
bool axisTile(int axis, String &data)
{
  bool success = false;
  char temp[240] = "";
  char temp1[80] = "";
  char temp2[80] = "";

  if (axis <= 1 && status.mountFound != SD_TRUE) return success;
  if (axis == 2 && status.rotatorFound != SD_TRUE) return success;
  if (axis >= 3 && !status.focuserPresent[axis - 3]) return success;

  char axisTitleStr[32] = "";
  switch (axis) {
    case 0: sprintf(axisTitleStr, "Axis%d RA/Azm", 1); break;
    case 1: sprintf(axisTitleStr, "Axis%d Dec/Alt", 2); break;
    case 2: sprintf(axisTitleStr, "Axis%d Rotator", 3); break;
    case 3: sprintf(axisTitleStr, "Axis%d Focuser1", 4); break;
    case 4: sprintf(axisTitleStr, "Axis%d Focuser2", 5); break;
    case 5: sprintf(axisTitleStr, "Axis%d Focuser3", 6); break;
    case 6: sprintf(axisTitleStr, "Axis%d Focuser4", 7); break;
    case 7: sprintf(axisTitleStr, "Axis%d Focuser5", 8); break;
    case 8: sprintf(axisTitleStr, "Axis%d Focuser6", 9); break;
  }
  sprintf_P(temp, html_tile_text_beg, "22em", "11em", axisTitleStr);
  data.concat(temp);
  data.concat("<br /><hr>");
  if (state.driverStatusStr[axis][0] == '?') strcpy(temp1, L_UNKNOWN); else strcpy(temp1, state.driverStatusStr[axis]);
  sprintf_P(temp, html_indexDriverStatus, axis, temp1);
  data.concat(temp);

  data.concat("<hr>");

  #if DRIVE_CONFIGURATION == ON
    if (status.getVersionMajor() > 3) {
      sprintf_P(temp, html_collapsable_beg, L_SETTINGS "...");
      data.concat(temp);

      AxisSettings a;

      // Axis1 RA/Azm
      if (axis == 0) {
        if (!onStep.command(":GXA1#", temp1)) strcpy(temp1, "0");
        if (decodeAxisSettings(temp1, &a)) {
          sprintf_P(temp, html_form_begin, "index.htm");
          data.concat(temp);
          if (validateAxisSettings(1, status.mountType == MT_ALTAZM, a)) {
            if (!onStep.command(":GXE7#", temp1)) strcpy(temp1, "0");

            long spwr = strtol(temp1, NULL, 10);
            sprintf_P(temp, html_configAxisSpwr, spwr, 1, 0, 129600000L);
            data.concat(temp);
            www.sendContentAndClear(data);

            dtostrf(a.stepsPerMeasure, 1, 3, temp1);
            stripNum(temp1);
            sprintf_P(temp, html_configAxisSpd, temp1, 1, 150, 122400L);
            data.concat(temp);
            www.sendContentAndClear(data);

            #if DRIVE_MAIN_AXES_REVERSE == ON
              sprintf_P(temp, html_configAxisReverse, a.reverse == ON ? 1 : 0, 1);
              data.concat(temp);
              www.sendContentAndClear(data);
            #endif

            sprintf_P(temp, html_configAxisMin, (int)a.min, 1, -360, -90, "&deg;,");
            data.concat(temp);
            www.sendContentAndClear(data);

            sprintf_P(temp, html_configAxisMax, (int)a.max, 1, 90, 360, "&deg;,");
            data.concat(temp);
            www.sendContentAndClear(data);

            sendAxisParams(&a, 1);

            data.concat(F("<br /><button type='submit'>" L_UPLOAD "</button> "));
          }
          sprintf_P(temp, html_configAxisRevert, 1);
          data.concat(temp);
          data.concat(FPSTR(html_form_end));
          www.sendContentAndClear(data);
          success = true;
        }
      }

      // Axis2 Dec/Alt
      if (axis == 1) {
        if (!onStep.command(":GXA2#", temp1)) strcpy(temp1, "0");
        if (decodeAxisSettings(temp1, &a)) {
          sprintf_P(temp, html_form_begin, "index.htm");
          data.concat(temp);
          if (validateAxisSettings(2, status.mountType == MT_ALTAZM, a)) {

            dtostrf(a.stepsPerMeasure, 1, 3, temp1);
            stripNum(temp1);
            sprintf_P(temp, html_configAxisSpd, temp1, 2, 150, 122400L);
            data.concat(temp);
            www.sendContentAndClear(data);

            #if DRIVE_MAIN_AXES_REVERSE == ON
              sprintf_P(temp, html_configAxisReverse, a.reverse == ON ? 1 : 0, 2);
              data.concat(temp);
            #endif

            sprintf_P(temp, html_configAxisMin, (int)a.min, 2, -90, 0, "&deg;,");
            data.concat(temp);
            www.sendContentAndClear(data);

            sprintf_P(temp, html_configAxisMax, (int)a.max, 2, 0, 90, "&deg;,");
            data.concat(temp);
            www.sendContentAndClear(data);

            sendAxisParams(&a, 2);

            data.concat(F("<br /><button type='submit'>" L_UPLOAD "</button> "));
          }
          sprintf_P(temp, html_configAxisRevert, 2);
          data.concat(temp);
          data.concat(FPSTR(html_form_end));
          www.sendContentAndClear(data);
          success = true;
        }
      }

      // Axis3 Rotator
      if (axis == 2) {
        if (!onStep.command(":GXA3#", temp1)) strcpy(temp1, "0");
        if (decodeAxisSettings(temp1, &a)) {
          sprintf_P(temp, html_form_begin, "index.htm");
          data.concat(temp);
          if (validateAxisSettings(3, status.mountType == MT_ALTAZM, a))
          {
            dtostrf(a.stepsPerMeasure, 1, 3, temp1);
            stripNum(temp1);
            sprintf_P(temp, html_configAxisSpd, temp1, 3, 10, 3600L);
            data.concat(temp);
            www.sendContentAndClear(data);

            sprintf_P(temp, html_configAxisReverse, a.reverse == ON ? 1 : 0, 3);
            data.concat(temp);
            www.sendContentAndClear(data);

            sprintf_P(temp, html_configAxisMin, (int)a.min, 3, -360, 0, "&deg;,");
            data.concat(temp);
            www.sendContentAndClear(data);

            sprintf_P(temp, html_configAxisMax, (int)a.max, 3, 0, 360, "&deg;,");
            data.concat(temp);
            www.sendContentAndClear(data);

            sendAxisParams(&a, 3);

            data.concat(F("<br /><button type='submit'>" L_UPLOAD "</button> "));
          }
          sprintf_P(temp, html_configAxisRevert, 3);
          data.concat(temp);
          data.concat(FPSTR(html_form_end));
          www.sendContentAndClear(data);
          success = true;
        }
      }

      // Axis4 to Axis9 Focusers
      if (axis >= 3 && axis <= 8) {
        int focuser = axis - 3;
        if (status.focuserPresent[focuser]) {
          sprintf(temp2, ":GXA%d#", focuser + 4);
          if (!onStep.command(temp2, temp1)) strcpy(temp1, "0");
          if (decodeAxisSettings(temp1, &a)) {
            sprintf_P(temp, html_form_begin, "index.htm");
            data.concat(temp);
            if (validateAxisSettings(focuser + 4, status.mountType == MT_ALTAZM, a)) {

              dtostrf(a.stepsPerMeasure, 1, 3, temp1);
              stripNum(temp1);
              sprintf_P(temp, html_configAxisSpu, temp1, focuser + 4);
              data.concat(temp);
              www.sendContentAndClear(data);

              sprintf_P(temp, html_configAxisReverse, a.reverse == ON ? 1 : 0, focuser + 4);
              data.concat(temp);
              www.sendContentAndClear(data);

              sprintf_P(temp, html_configAxisMin, (int)a.min, focuser + 4, -500, 500, "mm,");
              data.concat(temp);
              www.sendContentAndClear(data);

              sprintf_P(temp, html_configAxisMax, (int)a.max, focuser + 4, -500, 500, "mm,");
              data.concat(temp);
              www.sendContentAndClear(data);

              sendAxisParams(&a, focuser + 4);

              data.concat(F("<br /><button type='submit'>" L_UPLOAD "</button> "));
            }
            sprintf_P(temp, html_configAxisRevert, focuser + 4);
            data.concat(temp);
            data.concat(FPSTR(html_form_end));
            www.sendContentAndClear(data);
            success = true;
          }
        }
      }

      if (!success) {
        data.concat(L_ADV_SET_AXIS_NO_EDIT "<br />");
      }
      data.concat(FPSTR(html_collapsable_end));
    }
  #endif
  data.concat(FPSTR(html_tile_end));
  www.sendContentAndClear(data);

  return success;
}

// use Ajax key/value pairs to pass related data to the web client in the background
void axisTileAjax(int axis, String &data)
{
  char temp[80], temp1[80];

  sprintf(temp, "dvr_stat%d", axis);
  if (state.driverStatusStr[axis][0] == '?') strcpy(temp1, L_UNKNOWN); else strcpy(temp1, state.driverStatusStr[axis]);
  data.concat(keyValueString(temp, temp1));

  www.sendContentAndClear(data);
}

// pass related data back to OnStep
void axisTileGet()
{
  String v;
  char temp[80] = "";

  #if DRIVE_CONFIGURATION == ON
 
    String ssr = www.arg("revert");
    if (!ssr.equals(EmptyStr)) {
      int axis = ssr.toInt();
      if (axis == 0)
      {
        strcpy(temp,":SXEM,0#");
        onStep.commandBool(temp);
      } else
      if (axis >= 1 && axis <= 9)
      {
        sprintf(temp,":SXA%d,R#",axis);
        onStep.commandBool(temp);
      }
      return;
    }

    String axisStr = "0";
    if (!www.arg("a1spm").equals(EmptyStr)) axisStr = "1"; else
    if (!www.arg("a2spm").equals(EmptyStr)) axisStr = "2"; else
    if (!www.arg("a3spm").equals(EmptyStr)) axisStr = "3"; else
    if (!www.arg("a4spm").equals(EmptyStr)) axisStr = "4"; else
    if (!www.arg("a5spm").equals(EmptyStr)) axisStr = "5"; else
    if (!www.arg("a6spm").equals(EmptyStr)) axisStr = "6"; else
    if (!www.arg("a7spm").equals(EmptyStr)) axisStr = "7"; else
    if (!www.arg("a8spm").equals(EmptyStr)) axisStr = "8"; else
    if (!www.arg("a9spm").equals(EmptyStr)) axisStr = "9";

    if (status.getVersionMajor() < 10) {
      // send axis settings to OnStep
      String s1 = www.arg("a" + axisStr + "spm");
      String s2 = www.arg("a" + axisStr + "ustp");
      String s3 = www.arg("a" + axisStr + "I");
      String s4 = www.arg("a" + axisStr + "rev");
      String s5 = www.arg("a" + axisStr + "min");
      String s6 = www.arg("a" + axisStr + "max");

      if (axisStr.toInt() > 0 && axisStr.toInt() < 6) {
        if (s2.equals(EmptyStr)) s2 = "-1";
        if (s3.equals(EmptyStr)) s3 = "-1";
        if (s4.equals(EmptyStr)) s4 = "-1";
        if (s5.equals(EmptyStr)) s5 = "-1";
        if (s6.equals(EmptyStr)) s6 = "-1";
        if (s4.equals("0")) s4 = "-1"; else if (s4.equals("1")) s4 = "-2";

        v = s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5 + "," + s6;
        sprintf(temp, ":SXA%d,%s#", (int)axisStr.toInt(), v.c_str());
        onStep.commandBool(temp);

        if (!www.arg("a1spwr").equals(EmptyStr)) {
          sprintf(temp, ":SXE7,%s#", www.arg("a1spwr").c_str());
          onStep.commandBool(temp);
        }
      }
    } else {
      // send axis settings to OnStepX
      String s1 = www.arg("a" + axisStr + "spm");
      String s2 = www.arg("a" + axisStr + "rev");
      String s3 = www.arg("a" + axisStr + "min");
      String s4 = www.arg("a" + axisStr + "max");
      String s5 = www.arg("a" + axisStr + "ustp");
      String s6, s7, s8, s9, s10;
      if (s5.equals(EmptyStr)) {
        s5 = www.arg("a" + axisStr + "p");
        s6 = www.arg("a" + axisStr + "i");
        s7 = www.arg("a" + axisStr + "d");
        s8 = www.arg("a" + axisStr + "pGoto");
        s9 = www.arg("a" + axisStr + "iGoto");
        s10 = www.arg("a" + axisStr + "dGoto");
      } else {
        s6 = www.arg("a" + axisStr + "ustpGoto");
        s7 = www.arg("a" + axisStr + "Ih");
        s8 = www.arg("a" + axisStr + "I");
        s9 = www.arg("a" + axisStr + "Is");
        s10 = "";
      }

      if (axisStr.toInt() > 0 && axisStr.toInt() < 10) {
        if (s2.equals(EmptyStr)) s2 = "-1";
        if (s2.equals("0")) s2 = "-1"; else if (s2.equals("1")) s2 = "-2";
        if (s3.equals(EmptyStr)) s3 = "-1";
        if (s4.equals(EmptyStr)) s4 = "-1";
        if (s5.equals(EmptyStr)) s5 = "-1";
        if (s6.equals(EmptyStr)) s6 = "-1";
        if (s7.equals(EmptyStr)) s7 = "-1";
        if (s8.equals(EmptyStr)) s8 = "-1";
        if (s9.equals(EmptyStr)) s9 = "-1";
        if (s10.equals(EmptyStr)) s10 = "-1";

        v = s1 + "," + s2 + "," + s3 + "," + s4 + "," + s5 + "," + s6 + "," + s7 + "," + s8 + "," + s9 + "," + s10;
        sprintf(temp, ":SXA%d,%s#", (int)axisStr.toInt(), v.c_str());
        onStep.commandBool(temp);

        if (!www.arg("a1spwr").equals(EmptyStr)) {
          sprintf(temp, ":SXE7,%s#", www.arg("a1spwr").c_str());
          onStep.commandBool(temp);
        }
      }
    }
  #endif
}

void sendAxisParams(AxisSettings* a, int axis) {
  char temp[300], temp1[40];
  String data = "";

  if (a->driverType == DT_SERVO) {
    data.concat(L_ADV_SET_IMMEDIATE);
    data.concat("<br/><br/>");
    www.sendContentAndClear(data);

    dtostrf(a->p, 1, 3, temp1);
    stripNum(temp1);
    sprintf_P(temp, html_configAxisP, temp1, axis, 0, 99999999L);
    data.concat(temp);
    www.sendContentAndClear(data);

    dtostrf(a->i, 1, 3, temp1);
    stripNum(temp1);
    sprintf_P(temp, html_configAxisI, temp1, axis, 0, 99999999L);
    data.concat(temp);
    www.sendContentAndClear(data);

    dtostrf(a->d, 1, 3, temp1);
    stripNum(temp1);
    sprintf_P(temp, html_configAxisD, temp1, axis, 0, 99999999L);
    data.concat(temp);
    www.sendContentAndClear(data);

    dtostrf(a->pGoto, 1, 3, temp1);
    stripNum(temp1);
    sprintf_P(temp, html_configAxisGotoP, temp1, axis, 0, 99999999L);
    data.concat(temp);
    www.sendContentAndClear(data);

    dtostrf(a->iGoto, 1, 3, temp1);
    stripNum(temp1);
    sprintf_P(temp, html_configAxisGotoI, temp1, axis, 0, 99999999L);
    data.concat(temp);
    www.sendContentAndClear(data);

    dtostrf(a->dGoto, 1, 3, temp1);
    stripNum(temp1);
    sprintf_P(temp, html_configAxisGotoD, temp1, axis, 0, 99999999L);
    data.concat(temp);
    www.sendContentAndClear(data);
  } else

  if (a->driverType == DT_STEP_DIR_STANDARD) {
    data.concat(L_ADV_SET_SPECIAL);
    data.concat("<br/><br/>");
    www.sendContentAndClear(data);

    #if DRIVE_MAIN_AXES_MICROSTEPS == ON
      sprintf_P(temp, html_configAxisMicroSteps, (int)a->microsteps, axis);
      data.concat(temp);
      www.sendContentAndClear(data);

      sprintf_P(temp, html_configAxisMicroStepsGoto, (int)a->microstepsGoto, axis);
      data.concat(temp);
      www.sendContentAndClear(data);
    #endif
  } else

  if (a->driverType == DT_STEP_DIR_TMC_SPI) {
    data.concat(L_ADV_SET_SPECIAL);
    data.concat("<br/><br/>");
    www.sendContentAndClear(data);

    #if DRIVE_MAIN_AXES_MICROSTEPS == ON
      sprintf_P(temp, html_configAxisMicroSteps, (int)a->microsteps, axis);
      data.concat(temp);
      www.sendContentAndClear(data);

      sprintf_P(temp, html_configAxisMicroStepsGoto, (int)a->microstepsGoto, axis);
      data.concat(temp);
      www.sendContentAndClear(data);
    #endif
    #if DRIVE_MAIN_AXES_CURRENT == ON
      sprintf_P(temp, html_configAxisCurrentHold, (int)a->currentHold, axis, 3000);
      data.concat(temp);
      www.sendContentAndClear(data);

      sprintf_P(temp, html_configAxisCurrentTrak, (int)a->currentRun, axis, 3000);
      data.concat(temp);
      www.sendContentAndClear(data);

      sprintf_P(temp, html_configAxisCurrentSlew, (int)a->currentGoto, axis, 3000);
      data.concat(temp);
      www.sendContentAndClear(data);
    #endif
  } else

  if (a->driverType == DT_STEP_DIR_LEGACY) {
    #if DRIVE_MAIN_AXES_MICROSTEPS == ON
      if (a->microsteps != OFF) {
        sprintf_P(temp, html_configAxisMicroSteps, (int)a->microsteps, axis);
        data.concat(temp);
        www.sendContentAndClear(data);
      }
    #endif
    #if DRIVE_MAIN_AXES_CURRENT == ON
      if (a->currentRun != OFF) {
        sprintf_P(temp, html_configAxisCurrentTrak, (int)a->currentRun, axis, 3000);
        data.concat(temp);
        www.sendContentAndClear(data);
      }
      if (a->currentGoto != OFF) {
        sprintf_P(temp, html_configAxisCurrentSlew, (int)a->currentGoto, axis, 3000);
        data.concat(temp);
        www.sendContentAndClear(data);
      }
    #endif
  }
}