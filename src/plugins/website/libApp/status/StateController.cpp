// update controller state
#include "State.h"

#include "Status.h"
#include "../cmd/Cmd.h"
#include "../misc/Misc.h"
#include "../../locales/Locale.h"
#if OPERATIONAL_MODE == WIFI && DISPLAY_WIFI_SIGNAL_STRENGTH == ON
  #include "../../../../lib/wifi/WifiManager.h"
#endif

void State::updateController(bool now)
{
  if (!now && millis() - lastControllerPageLoadTime > 2000) return;

  char temp[80], temp1[80];

  // Ambient conditions
  #if DISPLAY_WEATHER == ON
    if (!onStep.command(":GX9A#", temp)) strcpy(temp, "?"); else localeTemperature(temp);
    strncpy(siteTemperatureStr, temp, 16); siteTemperatureStr[15] = 0; delay(0);
    if (!onStep.command(":GX9B#", temp)) strcpy(temp, "?"); else localePressure(temp);
    strncpy(sitePressureStr, temp, 16); sitePressureStr[15] = 0; delay(0);
    if (!onStep.command(":GX9C#", temp)) strcpy(temp, "?"); else localeHumidity(temp);
    strncpy(siteHumidityStr, temp, 16); siteHumidityStr[15] = 0; delay(0);
    if (!onStep.command(":GX9E#", temp)) strcpy(temp, "?"); else localeTemperature(temp);
    strncpy(siteDewPointStr, temp, 16); siteDewPointStr[15] = 0; delay(0);
  #endif

  // Driver status
  int numAxes = 2;
  if (status.getVersionMajor() >= 10) numAxes = 9;
  for (int axis = 0; axis < numAxes; axis++) {
    if (driver[axis].valid) {
      strcpy(temp1, "");
      if (driver[axis].communicationFailure) strcat(temp1, L_COMMS_FAILURE ", ");
      if (!driver[axis].communicationFailure) {
        if (driver[axis].standstill) strcat(temp1, L_STANDSTILL ", "); else {
          if (driver[axis].outputA.openLoad || driver[axis].outputB.openLoad) {
            strcat(temp1, L_OPEN_LOAD " ");
            if (driver[axis].outputA.openLoad) strcat(temp1,"A");
            if (driver[axis].outputB.openLoad) strcat(temp1,"B");
            strcat(temp1,", ");
          }
        }
        if (driver[axis].outputA.shortToGround || driver[axis].outputB.shortToGround) {
          strcat(temp1, L_SHORT_GND " ");
          if (driver[axis].outputA.shortToGround) strcat(temp1,"A");
          if (driver[axis].outputB.shortToGround) strcat(temp1,"B");
          strcat(temp1,", ");
        }
        if (driver[axis].overTemperature) strcat(temp1, L_SHUTDOWN_OVER " 150C, ");
        if (driver[axis].overTemperaturePreWarning) strcat(temp1, L_PRE_WARNING " &gt;120C, ");
      }
      if (strlen(temp1) > 2) temp1[strlen(temp1) - 2] = 0;

      if (driver[axis].fault) strcat(temp1, " (" L_DRIVER_FAULT ")");
      if (strlen(temp1) == 0) strcpy(temp1, "Ok");

      char *thisDriverStatusStr = driverStatusStr[axis];
      strncpy(thisDriverStatusStr, temp1, 40); thisDriverStatusStr[39] = 0; delay(0);
    }
  }

  // MCU Temperature
  #if DISPLAY_INTERNAL_TEMPERATURE == ON
    if (!onStep.command(":GX9F#", temp)) strcpy(temp, "?"); else localeTemperature(temp);
    strncpy(controllerTemperatureStr, temp, 16); controllerTemperatureStr[15] = 0; delay(0);
  #endif

  // General Error
  strcpy(temp, "");
  if (status.lastError != ERR_NONE) strcat(temp, "<font class=\"y\">"); 
  status.getLastErrorMessage(temp1);
  if (!status.onStepFound) strcat(temp, "?"); else strcat(temp, temp1);
  if (status.lastError != ERR_NONE) strcat(temp, "</font>"); 
  strncpy(lastErrorStr, temp, 80); lastErrorStr[79] = 0; delay(0);

  // Loop time
  if (status.getVersionMajor() < 10) {
    if (!onStep.command(":GXFA#", temp)) strcpy(temp, "?%");
    strncpy(workLoadStr, temp, 20); workLoadStr[19] = 0; delay(0);
  }

  // wifi signal strength
  #if OPERATIONAL_MODE == WIFI && DISPLAY_WIFI_SIGNAL_STRENGTH == ON
    long signal_strength_dbm = WiFi.RSSI();
    long signal_strength_qty = 2*(signal_strength_dbm + 100);
    if (signal_strength_qty > 100) signal_strength_qty = 100; 
    else if (signal_strength_qty < 0) signal_strength_qty = 0;
    sprintf(temp, "%lddBm (%ld%%)", signal_strength_dbm, signal_strength_qty);
    strncpy(signalStrengthStr, temp, 20); signalStrengthStr[19] = 0; delay(0);
  #endif

  // update the axis status
  axisStatusUpdate();
}

// attempt to get the driver status for all 9 axes, mark the ones that return false so we don't attempt to process again
void State::axisStatusUpdate() {
  static int driverStatusFailedAttempts[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  int maxAxes = 2;
  if (status.getVersionMajor() >= 10) maxAxes = 9;
  for (int axis = 0; axis < maxAxes; axis++) {
    if (driverStatusFailedAttempts[axis] < 3) {
      char cmd[40];
      char reply[40];
      sprintf(cmd, ":GXU%d#", axis + 1);
      if (onStep.command(cmd, reply) && reply[0] != '0') {
        driverStatusFailedAttempts[axis] = 0;
        driver[axis].valid = true;
        driver[axis].communicationFailure = strstr(reply, "ST,OA,OB,GA,GB,OT,PW");
        driver[axis].standstill = strstr(reply, "ST");
        driver[axis].outputA.openLoad = strstr(reply, "OA");
        driver[axis].outputB.openLoad = strstr(reply, "OB");
        driver[axis].outputA.shortToGround = strstr(reply, "GA");
        driver[axis].outputB.shortToGround = strstr(reply, "GB");
        driver[axis].overTemperature = strstr(reply, "OT");
        driver[axis].overTemperaturePreWarning = strstr(reply, "PW");
        driver[axis].fault = strstr(reply, "GF");
      } else {
        driverStatusFailedAttempts[axis]++;
        driver[axis].valid = false;
      }
      delay(0);
    }
  }
}
