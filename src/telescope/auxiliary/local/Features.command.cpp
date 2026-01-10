// -----------------------------------------------------------------------------------
// Auxiliary Features

#include "Features.h"

#ifdef FEATURES_PRESENT

#include "../../../lib/convert/Convert.h"
#include "../../../libApp/weather/Weather.h"
#include "../../../telescope/Telescope.h"

void Features::strCatPower(char *reply, int index) {
  #ifdef POWER_MONITOR_PRESENT
    // get voltage V and current I for channel [n]
    // later for CAN these don't need to cover a very large range or precision so a fixed point 16bit
    // representation will do (CanPayload.h has this format but perhaps add int16_t max to represent NAN)
    // for example :GXV1# returns 12.3,0.5,PCvVT# or, if the channel doesn't exist NAN,NAN,!!!!!#
    if (index < 0 || index > 7) return;

    char s[40];
    if (powerMonitor.hasVoltage(index)) sprintF(s, ",%1.1f", powerMonitor.getVoltage(index)); else strcpy(s, ",NAN");
    strcat(reply, s);
    if (powerMonitor.hasCurrent(index)) sprintF(s, ",%1.1f", powerMonitor.getCurrent(index)); else strcpy(s, ",NAN");
    strcat(reply, s);
    if (powerMonitor.hasChannel(index)) strcat(reply, ",P"); else strcat(reply, ",!");
    if (!powerMonitor.errOverCurrent(index)) strcat(reply, "C"); else strcat(reply, "!");
    if (!powerMonitor.errUnderVoltage(index)) strcat(reply, "v"); else strcat(reply, "!");
    if (!powerMonitor.errOverVoltage(index)) strcat(reply, "V"); else strcat(reply, "!");
    if (!powerMonitor.errOverTemperature(index)) strcat(reply, "T"); else strcat(reply, "!");
  #else
    UNUSED(reply);
    UNUSED(index);
  #endif
}

// by default reply[80] == "", suppressFrame == false, numericReply == true, and commandError == CE_NONE
// return true if the command has been completely handled and no further command() will be called, or false if not
// for commands that are handled repeatedly commandError might contain CE_NONE or CE_1 to indicate success
// numericReply=true means boolean/numeric-style responses (e.g., CE_1/CE_0/errors) rather than a payload
bool Features::command(char *reply, char *command, char *parameter, bool *suppressFrame, bool *numericReply, CommandError *commandError) {

  // get auXiliary feature
  if (command[0] == 'G' && command[1] == 'X' && parameter[2] == 0) {
    // :GXX[n]#
    if (parameter[0] == 'X') { 
      int i = parameter[1] - '1';
      if (i < 0 || i > 7)  { *commandError = CE_PARAM_FORM; return true; }

      char s[255];
      if (device[i].purpose == SWITCH || device[i].purpose == MOMENTARY_SWITCH || device[i].purpose == COVER_SWITCH) {
        sprintf(s, "%d", device[i].value);
        strcat(reply, s);
        strCatPower(reply, i);
      } else

      if (device[i].purpose == ANALOG_OUTPUT) {
        sprintf(s, "%d", device[i].value);
        strcat(reply, s);
        strCatPower(reply, i);
      } else

      if (device[i].purpose == DEW_HEATER) {
        sprintf(s, "%d", (int)device[i].dewHeater->isEnabled());
        strcat(reply, s);
        strcat(reply, ",");

        sprintF(s, "%3.1f", device[i].dewHeater->getZero());
        strcat(reply, s);
        strcat(reply, ",");

        sprintF(s, "%3.1f", device[i].dewHeater->getSpan());
        strcat(reply, s);
        strcat(reply, ",");

        sprintF(s, "%3.1f", temperature.getChannel(i + 1) - weather.getDewPoint());
        strcat(reply, s);

        strCatPower(reply, i);
      } else

      if (device[i].purpose == INTERVALOMETER) {
        sprintf(s, "%d", (int)device[i].intervalometer->getCurrentCount());
        strcat(reply, s);
        strcat(reply, ",");

        float v;
        int d;
        v = device[i].intervalometer->getExposure();
        if (v < 1.0) d = 3; else if (v < 10.0) d = 2; else if (v < 30.0) d = 1; else d = 0;
        dtostrf(v, 0, d, s);
        strcat(reply, s);
        strcat(reply, ",");

        v = device[i].intervalometer->getDelay();
        if (v < 10.0) d = 2; else if (v < 30.0) d = 1; else d = 0;
        dtostrf(v, 0, d, s);
        strcat(reply, s);
        strcat(reply, ",");

        sprintf(s, "%d", (int)device[i].intervalometer->getCount());
        strcat(reply, s);
      } else { *commandError = CE_CMD_UNKNOWN; return true; }
      *numericReply = false;
    } else

    // :GXY[n]#
    // where [n] = 1..8 to get auXiliary feature name and purpose
    // :GXY0#
    // return active auXiliary features
    if (parameter[0] == 'Y') {
      int i = parameter[1] - '1';

      if (i == -1) {
        for (int j = 0; j < 8; j++) {
          if (device[j].purpose == OFF) reply[j] = '0'; else reply[j] = '1';
          reply[j + 1] = 0;
        }
        *numericReply = false;
        return true;
      }

      if (i < 0 || i > 7)  { *commandError = CE_PARAM_FORM; return true; }
      if (device[i].purpose == OFF) { *commandError = CE_0; return true; }

      char s[255];
      strcpy(s, device[i].name);
      if (strlen(s) > 10) s[10] = 0;
      strcpy(reply, s);
      strcat(reply, ",");

      int p = device[i].purpose;
      if (p == MOMENTARY_SWITCH || p == COVER_SWITCH) p = SWITCH;
      sprintf(s, "%d", p);
      strcat(reply, s);

      *numericReply = false;
    } else return false;

  } else

  // set auXiliary feature
  if (command[0] == 'S' && command[1] == 'X' && parameter[2] == ',') {
    // :SXX[n],V[Z][S][v]#
    // for example :SXX1,V1#  :SXX1,Z0.5#
    if (parameter[0] == 'X') { 
      int i = parameter[1] - '1';
      if (i < 0 || i > 7)  { *commandError = CE_PARAM_FORM; return true; }
      if (device[i].purpose == OFF) { *commandError = CE_CMD_UNKNOWN; return true; }

      char* conv_end;
      float f = strtof(&parameter[4], &conv_end);
      if (&parameter[4] == conv_end) { *commandError = CE_PARAM_FORM; return true; }
      long v = lroundf(f);

      if (parameter[3] == 'V' && v >= 0 && v <= 255) device[i].value = v;

      if (device[i].purpose == SWITCH || device[i].purpose == MOMENTARY_SWITCH || device[i].purpose == COVER_SWITCH) {
        if (parameter[3] == 'V') {
          if (v >= 0 && v <= 1) { // value 0..1 for enabled or not
            #ifdef COVER_SWITCH_SERVO_PRESENT
            if (device[i].purpose == COVER_SWITCH) {
              if (v == 0) cover[i].target = COVER_SWITCH_SERVO_OPEN_DEG; else
              if (v == 1) cover[i].target = COVER_SWITCH_SERVO_CLOSED_DEG;
            } else
            #endif

            digitalWriteEx(device[i].pin, v == device[i].active);
            if (device[i].purpose == MOMENTARY_SWITCH && device[i].value) momentarySwitchTime[i] = 50;

          } else *commandError = CE_PARAM_RANGE;
        } else *commandError = CE_PARAM_FORM;
      } else

      if (device[i].purpose == ANALOG_OUTPUT) {
        if (parameter[3] == 'V') { // value 0..255 for 0..100% power
          if (v >= 0 && v <= 255) {
            analogWriteEx(device[i].pin, analog8BitToAnalogRange(v));
          } else *commandError = CE_PARAM_RANGE;
        } else *commandError = CE_PARAM_FORM;
      } else

      if (device[i].purpose == DEW_HEATER) {
        if (parameter[3] == 'V') { // value 0..1 for enabled or not
          if (v >= 0 && v <= 1) device[i].dewHeater->enable(v); else *commandError = CE_PARAM_RANGE;
        } else

        if (parameter[3] == 'Z') { // zero
          if (f >= -5.0F && f <= 20.0F) device[i].dewHeater->setZero(f); else *commandError = CE_PARAM_RANGE;
        } else

        if (parameter[3] == 'S') { // span
          if (f >= -5.0F && f <= 20.0F) device[i].dewHeater->setSpan(f); else *commandError = CE_PARAM_RANGE;
        } else *commandError = CE_PARAM_FORM;
      } else

      if (device[i].purpose == INTERVALOMETER) {
        if (parameter[3] == 'V') { // value 0..1 for enabled or not
          if (v >= 0 && v <= 1) device[i].intervalometer->enable(v); else *commandError = CE_PARAM_RANGE;
        } else

        if (parameter[3] == 'E') { // exposure length
          if (f >= 0.0F && f <= 3600.0F) device[i].intervalometer->setExposure(f); else *commandError = CE_PARAM_RANGE;
        } else

        if (parameter[3] == 'D') { // delay
          if (f >= 1.0F && f <= 3600.0F) device[i].intervalometer->setDelay(f); else *commandError = CE_PARAM_RANGE;
        } else

        if (parameter[3] == 'C') { // count
          if (f >= 0.0F && f <= 255.0F) device[i].intervalometer->setCount(f); else *commandError = CE_PARAM_RANGE;
        } else *commandError = CE_PARAM_FORM;
      }
    } else return false;
  } else return false;

  return true;
}

#endif
