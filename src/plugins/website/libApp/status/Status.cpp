// -----------------------------------------------------------------------------------
// Mount status from OnStep

#include <Arduino.h>

#include "../../../../lib/debug/Debug.h"

#include "../../Constants.h"
#include "../../Config.h"

#include "../../locales/Locale.h"
#include "../cmd/Cmd.h"

#include "Status.h"

bool Status::update()
{
  char result[80] = "";
  if (!onStepFound) {
    mountFound = SD_UNKNOWN;
    focuserFound = SD_UNKNOWN;
    rotatorFound = SD_UNKNOWN;
    auxiliaryFound = SD_UNKNOWN;

    if (!onStep.command(":GVP#", result) || result[0] == 0 || !strstr(result, "On-Step")) {
      onStepFound = false;
      return false;
    } delay(0);
    if (!onStep.command(":GVN#", result) || result[0] == 0 ) {
      onStepFound = false;
      return false;
    } delay(0);
    strcpy(id, "OnStep");
    strcpy(ver, result);
    if (strlen(result) > 0) {
      ver_patch = result[strlen(result) - 1];
      result[strlen(result) - 1] = 0;
    }

    char *resultMinor = strchr(result, '.');
    if (resultMinor != NULL) {
      resultMinor[0] = 0;
      resultMinor++;
      ver_maj = atol(result);
      ver_min = atol(resultMinor);
    }

    if (ver_maj < 0 || ver_maj > 99 || ver_min < 0 || ver_min > 99 || ver_patch < 'a' || ver_patch > 'z') {
      ver_maj = -1;
      ver_min = -1;
      ver_patch = 0;
      onStepFound = false;
      strcpy(configName, "");
    } else {
      onStepFound = true;
      if (onStep.command(":GVC#", result)) {
        strncpy(configName, result, 40);
      }
    }
  }

  if (onStepFound) {
    mountScan();
    focuserScan();
    rotatorScan();
    auxiliaryScan();

    if (mountFound == SD_TRUE) {
      if (onStep.command(":GU#", result)) {
        delay(0);
        tracking = false;
        inGoto = false;
        if (!strstr(result, "N")) inGoto = true; else tracking = !strstr(result, "n");

        parked      = strstr(result, "P");
        if (strstr(result, "p")) parked = false;
        parking     = strstr(result, "I");
        parkFail    = strstr(result, "F");

        pecRecorded = strstr(result, "R");
        pecIgnore   = strstr(result, "/");
        pecReadyPlay= strstr(result, ",");
        pecPlaying  = strstr(result, "~");
        pecReadyRec = strstr(result, ";");
        pecRecording= strstr(result, "^");
        if (!pecRecording && !pecReadyRec && !pecPlaying && !pecReadyPlay && !pecIgnore && !pecRecorded) pecEnabled = false; else pecEnabled = true;

        syncToEncodersOnly = strstr(result, "e");
        atHome      = strstr(result, "H");
        homing      = strstr(result, "h");
        autoHome    = strstr(result, "B");
        ppsSync     = strstr(result, "S");
        pulseGuiding= strstr(result, "G");
        guiding     = strstr(result, "g");
        if (pulseGuiding) guiding = true;
        axisFault   = strstr(result, "f");

        if (strstr(result, "r")) { if (strstr(result, "s")) rateCompensation = RC_REFR_RA; else rateCompensation = RC_REFR_BOTH; } else
        if (strstr(result, "t")) { if (strstr(result, "s")) rateCompensation = RC_FULL_RA; else rateCompensation = RC_FULL_BOTH; } else rateCompensation = RC_NONE;

        waitingHome   = strstr(result, "w");
        pauseAtHome   = strstr(result, "u");
        buzzerEnabled = strstr(result, "z");

        if (strstr(result,"E")) mountType = MT_GEM; else
        if (strstr(result,"K")) mountType = MT_FORK; else
        if (strstr(result,"k")) mountType = MT_FORKALT; else
        if (strstr(result,"A")) mountType = MT_ALTAZM; else mountType = MT_UNKNOWN;

        if (mountType == MT_GEM) autoMeridianFlips = strstr(result, "a"); else autoMeridianFlips = false;

        guideRatePulse = result[strlen(result) - 3] - '0';
        if (guideRatePulse < 0) guideRatePulse = 0;
        if (guideRatePulse > 9) guideRatePulse = 9;
        guideRate = result[strlen(result) - 2] - '0';
        if (guideRate < 0) guideRate = 0;
        if (guideRate > 9) guideRate = 9;

        int e = result[strlen(result) - 1] - '0';
        if (e < ERR_NONE) lastError = ERR_UNSPECIFIED;
        if (e > ERR_NV_INIT) lastError = ERR_UNSPECIFIED;
        lastError = (Errors)(e);

        // get meridian status
        if (onStep.command(":GX94#", result) && result[0] != 0) {
          delay(0);
          meridianFlips = !strstr(result, "N");
          pierSide = strtol(&result[0], NULL, 10);

          // align status
          if (onStep.command(":A?#", result) && strlen(result) == 3) {
            if (result[0] >= '0' && result[0] <= '9') alignMaxStars = result[0] - '0';
            if (result[1] >= '0' && result[1] <= '9') alignThisStar = result[1] - '0';
            if (result[2] >= '0' && result[2] <= '9') alignLastStar = result[2] - '0';
            if (alignThisStar != 0 && alignThisStar <= alignLastStar) aligning = true; else aligning = false;
          } else {
            alignMaxStars = 0;
            alignThisStar = 0;
            alignLastStar = 0;
            aligning = false;
            onStepFound = false;
          }
          delay(0);
        } else onStepFound = false;
      } else onStepFound = false;
    } else {
      if (!onStep.command(":GVP#", result) || result[0] == 0 || !strstr(result, "On-Step")) onStepFound = false;
      delay(0);
    }
  }

  return onStepFound;
}

void Status::mountScan() {
  if (mountFound == SD_UNKNOWN) {
    char result[80] = "";
    if (!onStep.command(":GU#", result) || result[0] == 0) mountFound = SD_FALSE; else mountFound = SD_TRUE; delay(0);
  }
}

void Status::focuserScan() {
  if (focuserFound == SD_UNKNOWN) {
    focuserFound = SD_FALSE;
    focuserCount = 0;
    for (int i = 0; i < 6; i++) focuserPresent[i] = false;
    if (getVersionMajor() >= 10) {
      if (onStep.commandBool(":F1a#")) { focuserPresent[0] = true; focuserCount++; } delay(0);
      if (onStep.commandBool(":F2a#")) { focuserPresent[1] = true; focuserCount++; } delay(0);
      if (onStep.commandBool(":F3a#")) { focuserPresent[2] = true; focuserCount++; } delay(0);
      if (onStep.commandBool(":F4a#")) { focuserPresent[3] = true; focuserCount++; } delay(0);
      if (onStep.commandBool(":F5a#")) { focuserPresent[4] = true; focuserCount++; } delay(0);
      if (onStep.commandBool(":F6a#")) { focuserPresent[5] = true; focuserCount++; } delay(0);
    } else {
      if (onStep.commandBool(":FA#")) { focuserPresent[0] = true; focuserCount++; } delay(0);
      if (onStep.commandBool(":fA#")) { focuserPresent[1] = true; focuserCount++; } delay(0);
    }
    if (focuserCount > 0) focuserFound = SD_TRUE;
  }
}

void Status::rotatorScan() {
  if (rotatorFound == SD_UNKNOWN) {
    char temp[80];
    rotatorFound = SD_FALSE;
    derotatorFound = false;
    if (onStep.command(":GX98#", temp)) {
      if (temp[0] == 'R') { rotatorFound = SD_TRUE; derotatorFound = false; }
      if (temp[0] == 'D') { rotatorFound = SD_TRUE; derotatorFound = true; }
    } delay(0);
  }
}

bool Status::auxiliaryScan() {
  bool valid;
  char cmd[40], out[40], present[40];

  if (auxiliaryFound == SD_UNKNOWN) {
    // check which feature #'s are present
    if (!onStep.command(":GXY0#", present) || present[0] == 0 || strlen(present) != 8) valid = false; else valid = true; delay(0);

    // try to get the AF presense twice before giving up
    if (!valid) {
      if (!onStep.command(":GXY0#", present) || present[0] == 0 || strlen(present) != 8) valid = false; else valid = true; delay(0);
      if (!valid) { for (uint8_t j = 0; j < 8; j++) feature[j].purpose = 0; auxiliaryFound = SD_FALSE; return false; }
    }

    // get feature status
    for (uint8_t i = 0; i < 8; i++) {
      char *purpose_str = NULL;

      if (present[i] == '0') continue;

      sprintf(cmd, ":GXY%d#", i+1);
      if (!onStep.command(cmd, out) || out[0] == 0) valid = false; delay(0);
      if (!valid) { for (uint8_t j = 0; j < 8; j++) feature[j].purpose = 0; auxiliaryFound = SD_FALSE; return false; }

      if (strlen(out) > 1) {
        purpose_str = strstr(out,",");
        if (purpose_str) {
          purpose_str[0] = 0;
          purpose_str++;
        } else valid = false;
        char *name_str = out; if (!name_str) valid = false;
        if (!valid) { for (uint8_t j = 0; j < 8; j++) feature[j].purpose = 0; auxiliaryFound = SD_FALSE; return false; }

        if (strlen(name_str) > 10) name_str[11] = 0;
        strcpy(feature[i].name, name_str);
        if (purpose_str) feature[i].purpose = atoi(purpose_str);

        VF("MSG: Auxiliary Feature, found "); V(name_str);
        switch (feature[i].purpose) {
          case 1: VL("_SWITCH"); break;
          case 2: VL("_ANALOG_OUTPUT"); break;
          case 3: VL("_DEW_HEATER"); break;
          case 4: VL("_INTERVALOMETER"); break;
          default: VL("_UNKNOWN!"); break;
        }

        auxiliaryFound = SD_TRUE;
      }
    }
  }
  return true;
}

bool Status::getLastErrorMessage(char message[]) {
  strcpy(message,"");
  if (lastError == ERR_NONE) strcpy(message, L_GE_NONE); else
  if (lastError == ERR_MOTOR_FAULT) strcpy(message, L_GE_MOTOR_FAULT); else
  if (lastError == ERR_ALT_MIN) strcpy(message, L_GE_ALT_MIN); else
  if (lastError == ERR_LIMIT_SENSE) strcpy(message, L_GE_LIMIT_SENSE); else
  if (lastError == ERR_DEC) strcpy(message, L_GE_DEC); else
  if (lastError == ERR_AZM) strcpy(message, L_GE_AZM); else
  if (lastError == ERR_UNDER_POLE) strcpy(message, L_GE_UNDER_POLE); else
  if (lastError == ERR_MERIDIAN) strcpy(message, L_GE_MERIDIAN); else
  if (lastError == ERR_SYNC) strcpy(message, L_GE_SYNC); else
  if (lastError == ERR_PARK) strcpy(message, L_GE_PARK); else
  if (lastError == ERR_GOTO_SYNC) strcpy(message, L_GE_GOTO_SYNC); else
  if (lastError == ERR_UNSPECIFIED) strcpy(message, L_GE_UNSPECIFIED); else
  if (lastError == ERR_ALT_MAX) strcpy(message, L_GE_ALT_MAX); else
  if (lastError == ERR_WEATHER_INIT) strcpy(message, L_GE_WEATHER_INIT); else
  if (lastError == ERR_SITE_INIT) strcpy(message, L_GE_SITE_INIT); else
  if (lastError == ERR_NV_INIT) strcpy(message, L_GE_NV_INIT); else
  sprintf(message, L_GE_OTHER " %d", (int)lastError);
  return message[0];
}

Status status;
