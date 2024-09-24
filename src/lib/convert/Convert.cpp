//--------------------------------------------------------------------------------------------------
// convert to/from C strings

#include "../../Common.h"
#include "Convert.h"

void strncpyex(char *result, const char *source, size_t length) {
  strncpy(result, source, length);
  result[length - 1] = 0;
}

#if defined(ARDUINO_ARDUINO_NANO33BLE) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_MBED_RP2040) || defined(ARDUINO_ARCH_RP2040)
  void sprintF(char *result, const char *source, double f) {
    sprintf(result, source, f);
  }
#else
  void sprintF(char *result, const char *source, double f) {
    bool ok = false;
    uint8_t mas = 0, frac = 0, len = 0;
    char* a;
    do {
      a = (char*)strchr(source, '%');
      if (a == NULL) return;
      // "%f" form
      if (a[1] == 'f') { len = 2; ok = true; break; }
      // "%.2f" form
      if (a[1] == '.' && a[2] >= '0' && a[2] <= '9' && a[3] == 'f') { frac = a[2] - '0'; len = 4; ok = true; break; }
      // "%1.2f" form
      if (a[1] >= '0' && a[1] <= '9' && a[2] == '.' && a[3] >= '0' && a[3] <= '9' && a[4] == 'f') {
        mas = a[1] - '0';
        frac = a[3] - '0';
        len = 5;
        ok = true;
        break;
      }
    } while (!ok);

    char s[20];
    dtostrf(f, mas, frac, s);

    uint8_t j = 0;
    for (uint8_t i = 0; i < a - source; i++) result[j++] = source[i];
    for (uint8_t i = 0; i < strlen(s); i++) result[j++] = s[i];
    for (uint8_t i = len; i < strlen(a); i++) result[j++] = a[i];
    result[j++] = 0;
  }
#endif

bool Convert::tzToDouble(double *value, char *hm) {
  int16_t sign = 1;
  int16_t hour, minute = 0;

  if (strlen(hm) < 1 || strlen(hm) > 6) return false;

  // if there's a decimal part just convert it and return
  if (strchr(hm, '.') != NULL) { return atof2(hm, value); }

  // determine if the sign was used, skip any '+'
  if (hm[0] == '-') { sign = -1; hm++; } else if (hm[0] == '+') hm++;

  // if there's a minute part convert it and mark the end of the hours string
  char* m = strchr(hm, ':');
  if (m != NULL) {
    m[0] = 0;
    m++;
    if (strlen(m) != 2) return false;
    if (!atoi2(m, &minute, false)) return false;
    // only these exact minutes are allowed for time zones
    if (minute != 45 && minute != 30 && minute != 0) return false;
  }

  if (!atoi2(hm, &hour, false)) return false;
  *value = sign*(hour + minute/60.0);

  return true;
}

bool Convert::hmsToDouble(double *value, char *hms, PrecisionMode p) {
  char h[3], m[5];
  int16_t hour, minute, decimal = 0;
  double second = 0;

  while (*hms == ' ') hms++;

  if (strlen(hms) > 13) hms[13] = 0;
  int16_t length = strlen(hms);
  
  if (p == PM_HIGHEST || p == PM_HIGH) { if (length != 8 && length < 10) return false; } else
  if (p == PM_LOW) { if (length != 7) return false; }

  // convert the hours part
  h[0] = *hms++; h[1] = *hms++; h[2] = 0;
  if (!atoi2(h, &hour, false)) return false;

  // make sure the seperator is an allowed character, then convert the minutes part
  if (*hms++ != ':') return false;
  m[0] = *hms++; m[1] = *hms++; m[2] = 0;
  if (!atoi2(m, &minute, false)) return false;

  if (p == PM_HIGHEST || p == PM_HIGH) {
    // make sure the seperator is an allowed character, then convert the seconds part
    if (*hms++ != ':') return false;
    if (!atof2(hms, &second, false)) return false;
  } else
  if (p == PM_LOW) {
    // make sure the seperator is an allowed character, then convert the decimal minutes part
    if (*hms++ != '.') return false;
    decimal = (*hms++) - '0';
  }
  
  if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || decimal < 0 || decimal > 9 || second < 0 || second > 59.9999) return false;

  *value = hour + minute/60.0 + (double)decimal/600.0 + second/3600.0;
  return true;
}

bool Convert::hmsToDouble(double *value, char *hms) {
  if (!hmsToDouble(value, hms, PM_HIGHEST))
    if (!hmsToDouble(value, hms, PM_HIGH))
      if (!hmsToDouble(value, hms, PM_LOW)) return false;
  return true;
}

// convert string in format as follows to double (in degrees):
// sDD:MM          PM_LOW
// DDD:MM          PM_LOW
// sDD*MM          PM_LOW
// DDD*MM          PM_LOW
// sDD:MM:SS       PM_HIGH
// DDD:MM:SS       PM_HIGH
// sDD:MM:SS.SSS   PM_HIGHEST
bool Convert::dmsToDouble(double *value, char *dms, bool signPresent, PrecisionMode p) {
  char d[4], m[5];
  int16_t length;
  int16_t sign = 1, deg, minute, lowLimit = 0, highLimit = 360;
  double second = 0;
  bool secondsOff = false;

  while (*dms == ' ') dms++;
  if (strlen(dms) > 13) dms[13] = 0;
  length = strlen(dms);

  if (p == PM_HIGHEST || p == PM_HIGH) { if (length != 9 && length < 11) return false; } else
  if (p == PM_LOW) {
    if (length == 6) secondsOff = true; else
    if (length == 9) secondsOff = false; else return false;
  }

  // determine if the sign was used and accept it if so, then convert the degrees part
  if (signPresent) {
    if (*dms == '-') sign = -1; else if (*dms != '+') return false;
    dms++; d[0] = *dms++; d[1] = *dms++; d[2] = 0;
    if (!atoi2(d, &deg, false)) return false;
  } else {
    d[0] = *dms++; d[1] = *dms++; d[2] = *dms++; d[3] = 0;
    if (!atoi2(d, &deg, false)) return false;
  }

  // make sure the seperator is an allowed character, then convert the minutes part
  if (*dms != ':' && *dms != '*' && *dms != char(223)) return false; else dms++;
  m[0] = *dms++; m[1] = *dms++; m[2] = 0;
  if (!atoi2(m, &minute, false)) return false;

  if ((p == PM_HIGHEST || p == PM_HIGH) && !secondsOff) {
    // make sure the seperator is an allowed character, then convert the seconds part
    if (*dms++ != ':' && *dms++ != '\'') return false;
    if (!atof2(dms, &second, false)) return false;
  }

  if (signPresent) { lowLimit = -90; highLimit = 90; }
  if (deg < lowLimit || deg > highLimit || minute < 0 || minute > 59 || second < 0 || second > 59.999) return false;

  *value = sign*(deg + minute/60.0 + second/3600.0);
  return true;
}

// convert string in format as follows to double (in degrees):
// sDD:MM          PM_LOW
// DDD:MM          PM_LOW
// sDD*MM          PM_LOW
// DDD*MM          PM_LOW
// sDD:MM:SS       PM_HIGH
// DDD:MM:SS       PM_HIGH
// sDD:MM:SS.SSS   PM_HIGHEST
// automatically detects PrecisionMode
bool Convert::dmsToDouble(double *value, char *dms, bool signPresent) {
  if (!dmsToDouble(value, dms, signPresent, PM_HIGHEST))
    if (!dmsToDouble(value, dms, signPresent, PM_HIGH))
      if (!dmsToDouble(value, dms, signPresent, PM_LOW)) return false;
  return true;
}

void Convert::doubleToHms(char *reply, double value, bool signPresent, PrecisionMode p) {
  char sign[2] = "";
  double hour, minute, second, decimal = 0;

  // setup formatting, handle adding the sign
  if (signPresent) {
    if (value < 0) { value = -value; strcpy(sign,"-"); } else strcpy(sign,"+");
  }
  
  // round to 0.00005 second or 0.5 second, depending on precision mode
  if (p == PM_HIGHEST) value += 0.0000000139; else value += 0.000139;

  hour = floor(value);
  minute = (value - hour)*60.0;
  second = (minute - floor(minute))*60.0;

  // finish off calculations for hms and form string template
  char form[]="%s%02d:%02d:%02d.%04d";
  if (p == PM_HIGHEST) { decimal = (second - floor(second))*10000.0; } else
  if (p == PM_HIGH)    { form[16] = 0; } else
  if (p == PM_LOW)     { form[16] = 0; form[11] = '.'; form[14] = '1'; second = second/6.0; } else
  if (p == PM_LOWEST)  { form[11] = 0; second = 0; }

  // set sign and return result string
  if (p == PM_HIGHEST) sprintf(reply, form, sign, (int)hour, (int)minute, (int)second, (int)decimal); else
  if (p == PM_LOWEST)  sprintf(reply, form, sign, (int)hour, (int)minute); else
                       sprintf(reply, form, sign, (int)hour, (int)minute, (int)second);
}

// convert double (in degrees) to string in format as follows:
// sDD:MM          PM_LOW
// DDD:MM          PM_LOW
// sDD*MM          PM_LOW
// DDD*MM          PM_LOW
// DDD:MM:SS       PM_HIGH
// sDD:MM:SS.SSS   PM_HIGHEST
void Convert::doubleToDms(char *reply, double value, bool fullRange, bool signPresent, PrecisionMode p) {
  char sign[2] = "";
  double deg, minute, second = 0, decimal = 0;

  // setup formatting, handle adding the sign
  if (signPresent) {
    if (value < 0) { value = -value; strcpy(sign,"-"); } else strcpy(sign,"+");
  }

  // round to 0.0005 arc-second or 0.5 arc-second, depending on precision mode
  if (p == PM_HIGHEST) value += 0.000000139; else value += 0.000139; 

  deg = floor(value);
  minute = (value - deg)*60.0;
  second = (minute - floor(minute))*60.0;

  // finish off calculations for dms and form string template
  char form[]="%s%02d*%02d:%02d.%03d";
  if (p != PM_HIGHEST) form[16] = 0; else decimal = (second - floor(second))*1000.0;
  if (p == PM_LOW)     form[11] = 0;
  if (fullRange)                   form[4]  = '3';
 
  // return result string
  if (p == PM_HIGHEST) sprintf(reply, form, sign, (int)deg, (int)minute, (int)second, (int)decimal); else
  if (p == PM_HIGH)    sprintf(reply, form, sign, (int)deg, (int)minute, (int)second); else
  if (p == PM_LOW)     sprintf(reply, form, sign, (int)deg, (int)minute);
}

bool Convert::atoi2(char *a, int16_t *i, bool sign) {
  int16_t len = strlen(a);
  if (len == 0 || len > 6) return false;
  for (int l = 0; l < len; l++) {
    if (l == 0 && (a[l] == '+' || a[l] == '-') && sign) continue;
    if (a[l] < '0' || a[l] > '9') return false; 
  }
  long l = atol(a);
  if (l < -32767 || l > 32768) return false;
  *i = l;
  return true;
}

bool Convert::atoi2(char *a, uint8_t *u, bool sign) {
  int16_t i = *u;
  bool result = atoi2(a, &i, sign);
  if (i >= 0 && i <= 255) { *u = i; return result; } else return false;
}
              
bool Convert::atof2(char *a, double *d, bool sign) {
  int16_t dc = 0;
  int16_t len = strlen(a);
  for (int l = 0; l < len; l++) {
    if (l == 0 && (a[l] == '+' || a[l] == '-') && sign) continue;
    if (a[l] == '.') { if (dc == 0) { dc++; continue; } else return false; }
    if (a[l] < '0' || a[l] > '9') return false;
  }
  *d = atof(a);
  return true;
}

Convert convert;