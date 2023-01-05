// -----------------------------------------------------------------------------------
// Misc functions to help with commands, etc.

#include "Misc.h"
#include "../../locales/Locale.h"
#include "../status/Status.h"

void stripNum(char* s) {
  int pp=-1;
  for (unsigned int p=0; p < strlen(s); p++) if (s[p] == '.') { pp=p; break; }
  if (pp != -1) {
    int p;
    for (p=strlen(s)-1; p >= pp; p--) { if (s[p] != '0') break; s[p]=0; }
    if (s[p] == '.') s[p]=0;
  }
  while (s[0] == '0' && s[1] != '.' && strlen(s) > 1) memmove(&s[0],&s[1],strlen(s));
}

int hexToInt(String s) {
  int i0;
  int i1;
  if (s.length() != 2) return -1;
  char c0 = s.charAt(0);
  char c1 = s.charAt(1);
  if ( ((c0 >= '0' && c0 <= '9') || (c0 >= 'A' && c0 <= 'F')) && ((c1 >= '0' && c1 <= '9') || (c1 >= 'A' && c1 <= 'F')) ) {
    if (c0 >= '0' && c0 <= '9') { i0 = c0 - '0'; } else { i0 = (c0 - 'A') + 10; }
    if (c1 >= '0' && c1 <= '9') { i1 = c1 - '0'; } else { i1 = (c1 - 'A') + 10; }
    return i0*16 + i1;
  } else return -1;
}

uint8_t timeToByte(float t) {
  float v = 10;                         // default is 1 second
  if (t <= 0.0162) v=0; else            // 0.0156 (1/64 second)        (0)
  if (t <= 0.0313) v=1; else            // 0.0313 (1/32 second)        (1)
  if (t <= 0.0625) v=2; else            // 0.0625 (1/16 second)        (2)
  if (t <= 1.0) v=2.0+t*8.0; else       // 0.125 seconds to 1 seconds  (2 to 10)
  if (t <= 10.0) v=6.0+t*4.0; else      // 0.25 seconds to 10 seconds  (10 to 46)
  if (t <= 30.0) v=26.0+t*2.0; else     // 0.5 seconds to 30 seconds   (46 to 86)
  if (t <= 120.0) v=56.0+t; else        // 1 second to 120 seconds     (86 to 176)
  if (t <= 600.0) v=168.0+t/15.0; else  // 15 seconds to 300 seconds   (176 to 208)
  if (t <= 3360.0) v=198.0+t/60.0; else // 1 minute to 56 minutes      (208 to 254)
  if (t <= 3600.0) v=255;               // 1 hour                      (255)
  if (v < 0) v=0;
  if (v > 255) v=255;
  return lround(v);
}

float byteToTime(uint8_t b) {
  float v = 1.0;                        // default is 1 second
  if (b == 0) v=0.016125; else          // 0.0156 (1/64 second)        (0)
  if (b == 1) v=0.03125; else           // 0.0313 (1/32 second)        (1)
  if (b == 2) v=0.0625; else            // 0.0625 (1/16 second)        (2)
  if (b <= 10) v=(b-2.0)/8.0; else      // 0.125 seconds to 1 seconds  (2 to 10)
  if (b <= 46) v=(b-6.0)/4.0; else      // 0.25 seconds to 10 seconds  (10 to 46)
  if (b <= 86) v=(b-26.0)/2.0; else     // 0.5 seconds to 30 seconds   (46 to 86)
  if (b <= 176) v=(b-56.0); else        // 1 second to 120 seconds     (86 to 176)
  if (b <= 208) v=(b-168.0)*15.0; else  // 15 seconds to 300 seconds   (176 to 208)
  if (b <= 254) v=(b-198.0)*60.0; else  // 1 minute to 56 minutes      (208 to 254)
  if (b == 255) v=3600.0;               // 1 hour                      (255)
  return v;
}

bool decodeAxisSettings(char* s, AxisSettings* a) {
  if (status.getVersionMajor() >= 10) return decodeAxisSettingsX(s, a);

  if (strcmp(s,"0") != 0) {
    char *ws=s;
    char *conv_end;
    double f=strtod(ws, &conv_end); if (&s[0] != conv_end) a->stepsPerMeasure = f; else return false;
    ws=strchr(ws,','); if (ws != NULL) {
      ws++; a->microsteps = atol(ws);
      ws=strchr(ws,','); if (ws != NULL) {
        ws++; a->currentRun = atol(ws);
        ws=strchr(ws,','); if (ws != NULL) {
          ws++; a->reverse = atol(ws);
          ws=strchr(ws,','); if (ws != NULL) {
            ws++; a->min = atol(ws);
            ws=strchr(ws,','); if (ws != NULL) {
              ws++; a->max = atol(ws);
              a->driverType = DT_STEP_DIR_LEGACY;
              a->currentGoto = OFF;
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}

bool decodeAxisSettingsX(char* s, AxisSettings* a) {
  if (strcmp(s, "0") != 0) {
    char *ws = s;
    char *conv_end;
    double f=strtod(ws, &conv_end);
    if (&s[0] != conv_end) a->stepsPerMeasure = f; else return false;
    ws=strchr(ws,','); if (ws != NULL) {
      ws++; a->reverse = atol(ws);
      ws=strchr(ws,','); if (ws != NULL) {
        ws++; a->min = atol(ws);
        ws=strchr(ws,','); if (ws != NULL) {
          ws++; a->max = atol(ws);
          ws=strchr(ws,','); if (ws != NULL) {
            ws++; a->param1 = atof(ws);
            ws=strchr(ws,','); if (ws != NULL) {
              ws++; a->param2 = atof(ws);
              ws=strchr(ws,','); if (ws != NULL) {
                ws++; a->param3 = atof(ws);
                ws=strchr(ws,','); if (ws != NULL) {
                  ws++; a->param4 = atof(ws);
                  ws=strchr(ws,','); if (ws != NULL) {
                    ws++; a->param5 = atof(ws);
                    ws=strchr(ws,','); if (ws != NULL) {
                      ws++; a->param6 = atof(ws);
                      a->driverType = DT_NONE;
                      if (strstr(ws, "P")) {
                        // Dual PID Servo
                        a->driverType = DT_SERVO;
                        a->p = a->param1;
                        a->i = a->param2;
                        a->d = a->param3;
                        a->pGoto = a->param4;
                        a->iGoto = a->param5;
                        a->dGoto = a->param6;
                      } else
                      if (strstr(ws, "S")) {
                        // Step/Dir Standard driver
                        a->driverType = DT_STEP_DIR_STANDARD;
                        a->microsteps = round(a->param1);
                        a->microstepsGoto = round(a->param2);
                      } else
                      if (strstr(ws, "T")) {
                        // Step/Dir TMC-SPI driver
                        a->driverType = DT_STEP_DIR_TMC_SPI;
                        a->microsteps = round(a->param1);
                        a->microstepsGoto = round(a->param2);
                        a->currentHold = round(a->param3);
                        a->currentRun = round(a->param4);
                        a->currentGoto = round(a->param5);
                      }
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

bool validateAxisSettings(int axisNum, bool altAz, AxisSettings a) {
  if (status.getVersionMajor() >= 10) return validateAxisSettingsX(axisNum, a);

  int   MinLimitL[5]   = {-270,-90,-360,  0,  0};
  int   MinLimitH[5]   = { -90,  0,   0,500,500};
  int   MaxLimitL[5]   = {  90,  0,   0,  0,  0};
  int   MaxLimitH[5]   = { 270, 90, 360,500,500};
  float StepsLimitL[5] = {   150.0,   150.0,    5.0, 0.005, 0.005};
  float StepsLimitH[5] = {122400.0,122400.0, 7200.0,  20.0,  20.0};
  int   IrunLimitH[5]  = { 3000, 3000, 1000, 1000, 1000};
  if (altAz) { MinLimitL[0] = -360; MinLimitH[0] = -180; MaxLimitL[0] = 180; MaxLimitH[0] = 360; }
  axisNum--;
  if (a.stepsPerMeasure < StepsLimitL[axisNum] || a.stepsPerMeasure > StepsLimitH[axisNum]) return false;
  if (a.microsteps != OFF && (a.microsteps < 1 || a.microsteps > 256)) return false;
  if (a.currentRun != OFF && (a.currentRun < 0 || a.currentRun > IrunLimitH[axisNum])) return false;
  if (a.reverse != OFF && a.reverse != ON) return false;
  if (a.min < MinLimitL[axisNum] || a.min > MinLimitH[axisNum]) return false;
  if (a.max < MaxLimitL[axisNum] || a.max > MaxLimitH[axisNum]) return false;
  return true;
}

bool validateAxisSettingsX(int axisNum, AxisSettings a) {
  long minLimitL, minLimitH, maxLimitL, maxLimitH;
  float stepsLimitL, stepsLimitH;

  if (axisNum > 3) {
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

  axisNum--;
  if (a.stepsPerMeasure < stepsLimitL || a.stepsPerMeasure > stepsLimitH) return false;
  if (a.reverse != OFF && a.reverse != ON) return false;
  if (a.min < minLimitL || a.min > minLimitH) return false;
  if (a.max < maxLimitL || a.max > maxLimitH) return false;
  if (a.driverType != DT_SERVO) {
    if (a.microsteps     != OFF && (a.microsteps     < 1 || a.microsteps     > 256)) return false;
    if (a.microstepsGoto != OFF && (a.microstepsGoto < 1 || a.microstepsGoto > 256)) return false;
    if (a.driverType == DT_STEP_DIR_TMC_SPI) {
      if (a.currentHold != OFF && (a.currentHold < 0 || a.currentHold > 3000)) return false;
      if (a.currentRun  != OFF && (a.currentRun  < 0 || a.currentRun  > 3000)) return false;
      if (a.currentGoto != OFF && (a.currentGoto < 0 || a.currentGoto > 3000)) return false;
    }
  }

  return true;
}

void localeTemperature(char* temperatureStr) {
  float t = atof(temperatureStr);
  if (DISPLAY_UNITS == IMPERIAL) {
    t = t*(9.0/5.0) + 32.0;
    dtostrf(t, 3, 1, temperatureStr);
    strcat(temperatureStr, "&deg;F");
  } else strcat(temperatureStr, "&deg;C");
  if (isnan(t)) strcpy(temperatureStr, "?");
}

void localePressure(char* pressureStr) {
  float p = atof(pressureStr);
  if (DISPLAY_UNITS == IMPERIAL) {
    p = p/33.864;
    dtostrf(p, 4, 2, pressureStr);
    strcat(pressureStr, " inHg");
  } else strcat(pressureStr, " mb");
  if (isnan(p)) strcpy(pressureStr, "?");
}

void localeHumidity(char* humidityStr) {
  float h = atof(humidityStr);
  strcat(humidityStr, "%");
  if (isnan(h)) strcpy(humidityStr, "?");
}
