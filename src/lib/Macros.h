// Library general purpose macros
#pragma once

// conversion math
#define degToRad(x)                 ((x)/(double)RAD_DEG_RATIO)
#define degToRadF(x)                ((x)/(double)RAD_DEG_RATIO_F)
#define radToDeg(x)                 ((x)*(double)RAD_DEG_RATIO)
#define radToDegF(x)                ((x)*(double)RAD_DEG_RATIO_F)
#define hrsToRad(x)                 ((x)/(double)RAD_HOUR_RATIO)
#define radToHrs(x)                 ((x)*(double)RAD_HOUR_RATIO)
#define arcsecToRad(x)              ((x)/(double)206264.8062470963L)
#define radToArcsec(x)              ((x)*(double)206264.8062470963L)
#define siderealToRad(x)            ((x)/(double)13750.98708313975L)
#define siderealToRadF(x)           ((x)/13750.987F)
#define radToSidereal(x)            ((x)*(double)13750.98708313975L)
#define hzToSubMicros(x)            ((x)*(double)266666.666666667L)
#define hzToSidereal(x)             ((x)/(double)SIDEREAL_RATE_HZ)
#define siderealToHz(x)             ((x)*(double)SIDEREAL_RATE_HZ)
#define fequal(x,y)                 (fabs((x)-(y))<SmallestFloat)
#define fgt(x,y)                    ((x)-(y)>SmallestFloat)
#define flt(x,y)                    ((y)-(x)>SmallestFloat)
#define NormalizeAzimuth(x)         (x<0.0L?x+360.0L:x)

// strings

// tests if strings are equal
#define strequ(str1, str2) (strlen(str1) != strlen(str2) ? 0 : strstr(str1, str2))

// safe string copy from (char*)source to (char *)dest for a destination array of size bytes 
#define sstrcpy(dest, source, size) for (int i = 0; source[i] && i < size - 1; i++) { dest[i] = source[i]; dest[i + 1] = 0; }

// convert string to lower case
#define strtolower(source) for (int i = 0; source[i]; i++) source[i] = tolower(source[i])

#define _allowed_fqdn "0123456789abcdefghijklmnopqrstuvwxyz"
// convert to lower case and strip away characters that aren't allowed in an hostname
#define strtohostname(source) for (int i = 0; source[i]; i++) { source[i] = tolower(source[i]); if (!strchr(_allowed_fqdn "-", source[i])) { if (i < (int)(strlen(source) - 1)) { memmove(&source[i], &source[i + 1], strlen(&source[i])); i--; } else source[i] = 0;; } }

// convert to lower case and strip away characters that aren't allowed in an hostname (and '-')
#define strtohostname2(source) for (int i = 0; source[i]; i++) { source[i] = tolower(source[i]); if (!strchr(_allowed_fqdn, source[i])) { if (i < (int)(strlen(source) - 1)) { memmove(&source[i], &source[i + 1], strlen(&source[i])); i--; } else source[i] = 0;; } }

// embeds a string in a macro
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// ip address helper copies from one ip[4] address to another
#define ip4toip4(ip4, ip) for (int i = 0; i < 4; i++) ip4[i] = ip[i]

// check for an invalid ip[4] address of 0:0:0:0 
#define validip4(ip4) (ip4[0]!=0?true:ip4[1]!=0?true:ip4[2]!=0?true:ip4[3]!=0?true:false)

// an empty string
#ifndef EmptyStr
  #define EmptyStr ""
#endif

// pins
#define DAC_PIN(v)                  ((v)+0x100)
#define GPIO_PIN(v)                 ((v)+0x200)

// sense
#define THLD(v)                     ((v)<<1)  // 10 bit analog threshold, bits 1 through 10
#define HYST(v)                     ((v)<<11) // 10 bit hysteresis, bits 11 through 20
#define ANLG(v)                     ((v)<<21) // 10 bit analog value, bits 21 through 30
#ifndef INPUT_PULLDOWN
  #define INPUT_PULLDOWN INPUT
#endif

// automatically use fast I/O if available
#ifndef digitalReadF
  #ifdef HAL_HAS_DIGITAL_FAST
    #define digitalReadF(pin)           ( digitalReadFast(pin) )
  #else
    #define digitalReadF(pin)           ( digitalRead(pin) )
  #endif
#endif
#ifndef digitalWriteF
  #ifdef HAL_HAS_DIGITAL_FAST
    #define digitalWriteF(pin,value)   { digitalWriteFast(pin,value); }
  #else
    #define digitalWriteF(pin,value)   { digitalWrite(pin,value); }
  #endif
#endif

// supress compiler warnings for unused parameters
#ifndef UNUSED
  #define UNUSED(x) (void)(x)
#endif
