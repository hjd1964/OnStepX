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
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#ifndef EmptyStr
  #define EmptyStr ""
#endif

// pins
#define DAC_PIN(v)                  ((v)+0x100)
#define GPIO_PIN(v)                 ((v)+0x200)

// sense
#define THLD(v)                     ((v)<<1)  // 10 bit analog threshold, bits 1 through 10
#define HYST(v)                     ((v)<<11) // 10 bit hysteresis, bits 11 through 20
#ifndef INPUT_PULLDOWN
  #define INPUT_PULLDOWN INPUT
#endif

// use "Ex" functions to exclude pins that are OFF or SHARED
// pins in range 0x00 to 0xFF are normal, 0x100 to 0x1FF are DAC as digital outputs, 0x200 to 0x2FF are external GPIO pins
#define CLEAN_PIN(pin) ((uint8_t)((pin) & 0xff))
#if defined(GPIO_DEVICE) && GPIO_DEVICE != OFF
  #include "gpio/Gpio.h"
  #if defined(HAL_DAC_AS_DIGITAL)
    // external GPIO and DAC as digital
    #define pinModeEx(pin,mode)       { if (pin >= 0x200) gpio.pinMode(pin-0x200,mode); else if (pin > 0x100) pinMode(pin-0x100,mode); else if (pin >= 0) pinMode(CLEAN_PIN(pin),mode); }
    #define digitalWriteEx(pin,value) { if (pin >= 0x200) gpio.digitalWrite(pin-0x200,value); else if (pin > 0x100) analogWrite(pin-0x100,value); else if (pin >= 0) digitalWriteF(CLEAN_PIN(pin),value); }
    // special case(s) allowing digitalWriteF() to work with GPIOs
    #if GPIO_DEVICE == SSR74HC595
      #define digitalWriteF(pin,value) { if (pin >= 0x200) gpio.digitalWrite(pin-0x200,value); else if (pin > 0x100) analogWrite(pin-0x100,value); else if (pin >= 0) digitalWrite(CLEAN_PIN(pin),value); }
    #endif
  #else
    // external GPIO but no DAC as digital
    #define pinModeEx(pin,mode)       { if (pin >= 0x200) gpio.pinMode(pin-0x200,mode); else if (pin >= 0) pinMode(CLEAN_PIN(pin),mode); }
    #define digitalWriteEx(pin,value) { if (pin >= 0x200) gpio.digitalWrite(pin-0x200,value); else if (pin >= 0) digitalWriteF(CLEAN_PIN(pin),value); }
    // special case(s) allowing digitalWriteF() to work with GPIOs
    #if GPIO_DEVICE == SSR74HC595
      #define digitalWriteF(pin,value) { if (pin >= 0x200) gpio.digitalWrite(pin-0x200,value); else if (pin >= 0) digitalWrite(CLEAN_PIN(pin),value); }
    #endif
  #endif
  // no support for DAC input
  #define digitalReadEx(pin)          ( (pin >= 0)?((pin < 0x100)?digitalReadF(CLEAN_PIN(pin)):gpio.digitalRead(pin-0x200)):0 )

  // external GPIO analogWrite
  #if GPIO_DEVICE == SWS
    #define analogWriteEx(pin,value)  { if (pin >= 0x200) gpio.analogWrite(pin-0x200,value); else if (pin >= 0) analogWrite(CLEAN_PIN(pin),value); }
  #endif
#else
  #if defined(HAL_DAC_AS_DIGITAL)
    // DAC but no external GPIO
    #define pinModeEx(pin,mode)       { if (pin > 0x100) pinMode(CLEAN_PIN(pin-0x100),mode); else if (pin >= 0) pinMode(CLEAN_PIN(pin),mode); }
    #define digitalWriteEx(pin,value) { if (pin > 0x100) analogWrite(CLEAN_PIN(pin-0x100),value); else if (pin >= 0) { digitalWriteF(CLEAN_PIN(pin),value); } }
  #else
    // neither DAC as digital or external GPIO
    #define pinModeEx(pin,mode)       { if (pin >= 0) pinMode(pin,mode); }
    #define digitalWriteEx(pin,value) { if (pin >= 0) { digitalWriteF(pin,value); } }
  #endif
  // no support for DAC input and no external GPIO
  #define digitalReadEx(pin)          ( (pin >= 0)?digitalReadF(pin):0 )
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

// automatically use analogWrite
#ifndef analogWriteEx
  #define analogWriteEx(pin,value)     { analogWrite(pin,value); }
#endif

// supress compiler warnings for unused parameters
#ifndef UNUSED
  #define UNUSED(x) (void)(x)
#endif
