// update mount state
#include <limits.h>
#include "State.h"

#include "Status.h"
#include "../cmd/Cmd.h"
#include "../../locales/Locale.h"
#include "../../../../lib/convert/Convert.h"

void State::updateMount(bool now)
{
  if (!now && millis() - lastMountPageLoadTime > 2000) return;

  char temp[80];

  // UTC Time and Date
  if (!onStep.command(":GX80#", temp)) strcpy(temp, "?");
  strncpyex(timeStr, temp, 10);
  if (strcmp(timeStr, "00:00:00") ||
     (strlen(dateStr) == 0 && !strcmp(timeStr, "23:59:59"))) {
    if (!onStep.command(":GX81#", temp)) strcpy(temp, "?");
    if (temp[0] == '0') strcpy(&temp[0], &temp[1]);
    strncpyex(dateStr, temp, 10);
  }
  delay(0);

  // LST
  if (!onStep.command(":GS#", temp)) strcpy(temp, "?");
  strncpyex(lastStr, temp, 10); delay(0);

  if (DISPLAY_HIGH_PRECISION_COORDS == ON && status.getVersionMajor() >= 10)
  {
    // Azm,Alt current
    if (!onStep.command(":GZH#", temp)) strcpy(temp, "?");
    strncpyex(indexAzmStr, temp, 14);
    formatDegreesStr(indexAzmStr); delay(0);
    if (!onStep.command(":GAH#", temp)) strcpy(temp, "?");
    strncpyex(indexAltStr, temp, 14);
    formatDegreesStr(indexAltStr); delay(0);
  } else {
    // Azm,Alt current
    if (!onStep.command(":GZ#", temp)) strcpy(temp, "?");
    strncpyex(indexAzmStr, temp, 14);
    formatDegreesStr(indexAzmStr); delay(0);
    if (!onStep.command(":GA#", temp)) strcpy(temp, "?");
    strncpyex(indexAltStr, temp, 14);
    formatDegreesStr(indexAltStr); delay(0);
  }

  #if DISPLAY_HIGH_PRECISION_COORDS == ON
    // RA,Dec current
    if (!onStep.command(":GRa#", temp)) strcpy(temp, "?");
    strncpyex(indexRaStr, temp, 14);
    formatHoursStr(indexRaStr); delay(0);
    if (!onStep.command(":GDe#", temp)) strcpy(temp, "?");
    strncpyex(indexDecStr, temp, 14);
    formatDegreesStr(indexDecStr); delay(0);

    // RA,Dec target
    if (!onStep.command(":Gra#", temp)) strcpy(temp, "?");
    strncpyex(targetRaStr, temp, 14);
    formatHoursStr(targetRaStr); delay(0);
    if (!onStep.command(":Gde#", temp)) strcpy(temp, "?");
    strncpyex(targetDecStr, temp, 14);
    formatDegreesStr(targetDecStr); delay(0);
  #else
    // RA,Dec Current
    if (!onStep.command(":GR#", temp)) strcpy(temp, "?");
    strncpyex(indexRaStr, temp, 14);
    formatHoursStr(indexRaStr); delay(0);
    if (!onStep.command(":GD#", temp)) strcpy(temp, "?");
    strncpyex(indexDecStr, temp, 14);
    formatDegreesStr(indexDecStr); delay(0);

    // RA,Dec Target
    if (!onStep.command(":Gr#", temp)) strcpy(temp, "?");
    strncpyex(targetRaStr, temp, 14);
    formatHoursStr(targetRaStr); delay(0);
    if (!onStep.command(":Gd#", temp)) strcpy(temp, "?");
    strncpyex(targetDecStr, temp, 14);
    formatDegreesStr(targetDecStr); delay(0);
  #endif

  // Latitude
  if (status.getVersionMajor() > 3) {
    if (!onStep.command(":GtH#", temp)) strcpy(temp, "?");
  } else {
    if (!onStep.command(":Gt#", temp)) strcpy(temp, "?");
  }
  strncpyex(latitudeStr, temp, 10);
  convert.dmsToDouble(&latitude, latitudeStr, true);
  formatDegreesStr(latitudeStr);
  delay(0);

  // Longitude
  if (status.getVersionMajor() > 3) {
    if (!onStep.command(":GgH#", temp)) strcpy(temp, "?");
  } else {
    if (!onStep.command(":Gg#", temp)) strcpy(temp, "?");
  }
  strncpyex(longitudeStr, temp, 11);
  formatDegreesStr(longitudeStr);
  delay(0);

  // Pier side
  if ((status.pierSide == PierSideFlipWE1) || (status.pierSide == PierSideFlipWE2) || (status.pierSide == PierSideFlipWE3)) strcpy(temp, L_MERIDIAN_FLIP_W_TO_E); else
  if ((status.pierSide == PierSideFlipEW1) || (status.pierSide == PierSideFlipEW2) || (status.pierSide == PierSideFlipEW3)) strcpy(temp, L_MERIDIAN_FLIP_E_TO_W); else
  if (status.pierSide == PierSideWest) strcpy(temp, L_WEST); else
  if (status.pierSide == PierSideEast) strcpy(temp, L_EAST); else
  if (status.pierSide == PierSideNone) strcpy(temp, L_NONE); else strcpy(temp, L_UNKNOWN);
  if (!status.onStepFound) strcpy(temp, "?");
  strncpyex(pierSideStr, temp, 10);

  // Meridian flip
  if (status.meridianFlips) {
    strcpy(temp, "On");
    if (status.autoMeridianFlips) strcat(temp, ", " L_AUTO);
  } else strcpy(temp, "Off");
  if (!status.onStepFound) strcpy(temp, "?");
  strncpyex(meridianFlipStr, temp, 10);

  // Polar align
  strcpy(alignLrStr, "?");
  strcpy(alignUdStr, "?");
  if (!isnan(latitude) && fabs(latitude) <= 89) {
    long ud = LONG_MIN;
    if (onStep.command(":GX02#", temp)) { ud = strtol(&temp[0], NULL, 10); if (latitude < 0) ud = -ud; }
    long lr = LONG_MIN;
    if (onStep.command(":GX03#", temp)) { lr = strtol(&temp[0], NULL, 10); lr = lr/cos(latitude/57.295); }

    if (ud != LONG_MIN && lr != LONG_MIN) {
      char units = '"';
      if (labs(ud) >= 300 || labs(lr) >= 300) { ud = ud/60; lr = lr/60; units = '\''; }

      char lr_s[12];
      if (lr >= 0) strcpy(lr_s, leftTri); else strcpy(lr_s, rightTri);
      char ud_s[12];
      if (ud >= 0) strcpy(ud_s, upTri); else strcpy(ud_s, downTri);

      sprintf_P(temp, "%s %ld%c", lr_s, labs(lr), units);
      strncpyex(alignLrStr, temp, 16);

      sprintf_P(temp, "%s %ld%c", ud_s, labs(ud), units);
      strncpyex(alignUdStr, temp, 16); delay(0);
    }
  }

  // Align progress
  if (status.aligning && status.alignThisStar >= 0 && status.alignLastStar >= 0) {
    sprintf(temp, L_POINT " %d of %d", status.alignThisStar, status.alignLastStar);
  } else {
    if (status.alignThisStar > status.alignLastStar) strcpy(temp, L_COMPLETE); else strcpy(temp, L_INACTIVE);
  }
  strncpyex(alignProgress, temp, 32);

  // Park
  if (status.parked) strcpy(temp, L_PARKED); else strcpy(temp, L_NOT_PARKED);
  if (status.parking) strcpy(temp, L_PARKING); else
  if (status.parkFail) strcpy(temp, L_PARK_FAILED);
  if (status.atHome) strcat(temp, " (" L_AT_HOME ")");
  if (!status.onStepFound) strcpy(temp, "?");
  strncpyex(parkStr, temp, 40); delay(0);

  // Tracking
  double r = 0;
  if (status.tracking) {
    if (onStep.command(":GT#", temp)) {
      r = atof(temp);
      sprintF(temp, "%5.3fHz", r);
    } else strcpy(temp, "?");
    delay(0);
  } else strcpy(temp, L_INACTIVE);
  if (!status.onStepFound) strcpy(temp, "?");
  trackingSidereal = fabs(r - 60.164) < 0.001;
  trackingLunar = fabs(r - 57.900) < 0.001; 
  trackingSolar = fabs(r - 60.000) < 0.001;
  trackingKing  = fabs(r - 60.136) < 0.001;

  if (status.ppsSync) strcat(temp, "~");

  if (status.rateCompensation == RC_REFR_RA) strcat(temp, " RC"); else
  if (status.rateCompensation == RC_REFR_BOTH) strcat(temp, " RCD"); else
  if (status.rateCompensation == RC_FULL_RA) strcat(temp, " FC"); else
  if (status.rateCompensation == RC_FULL_BOTH) strcat(temp, " FCD");

  strncpyex(trackStr, temp, 40);

  // Slew speed
  if (isnan(slewSpeedNominal))
  {
    if (!onStep.command(":GX93#", temp)) strcpy(temp, "?"); else { slewSpeedNominal = atof(temp); } delay(0);
  }
  if (!onStep.command(":GX92#", temp)) strcpy(temp, "?"); else { slewSpeedCurrent = atof(temp); } delay(0);
  if (!onStep.command(":GX97#", temp)) strcpy(temp, "?"); else { strcat(temp, "&deg;/s"); } delay(0);
  strncpyex(slewSpeedStr, temp, 16);
}
