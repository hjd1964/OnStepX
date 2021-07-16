// -----------------------------------------------------------------------------------
// standard external GPIO library
#pragma once

#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.common.h"
#include "../../Config.defaults.h"
#include "../../HAL/HAL.h"

// use Fast functions for digital I/O if available
#ifdef HAL_HAS_DIGITAL_FAST
  #define digitalReadF(pin)           ( digitalReadFast(pin) )
  #define digitalWriteF(pin,value)    { digitalWriteFast(pin,value); }
#else
  #define digitalReadF(pin)           ( digitalRead(pin) )
  #define digitalWriteF(pin,value)    { digitalWrite(pin,value); }
#endif

// use "Ex" functions to exclude pins that are OFF or SHARED
// pins in range 0x00 to 0xFF are normal, 0x100 to 0x1FF are DAC as digital outputs, 0x200 to 0x2FF are external GPIO pins
#if GPIO_DEVICE != OFF
  #if defined(DAC_AS_DIGITAL)
    // external GPIO and DAC as digital
    #define pinModeEx(pin,mode)         { if (pin >= 0x200) gpio.pinMode(pin-0x200,mode); else if (pin > 0x100) pinMode(pin-0x100,mode); else if (pin >= 0) pinMode(pin,mode); }
    #define digitalWriteEx(pin,value)   { if (pin >= 0x200) gpio.digitalWrite(pin-0x200,value); else if (pin > 0x100) analogWrite(pin-0x100,value); else if (pin >= 0) digitalWriteF(pin,value); }
  #else
    // external GPIO but no DAC as digital
    #define pinModeEx(pin,mode)         { if (pin >= 0x200) gpio.pinMode(pin-0x200,mode); else if (pin >= 0) pinMode(pin,mode); }
    #define digitalWriteEx(pin,value)   { if (pin >= 0x200) gpio.digitalWrite(pin-0x200,value); else if (pin >= 0) digitalWriteF(pin,value); }
  #endif
  // no support for DAC input
  #define digitalReadEx(pin)            ( (pin >= 0)?((pin < 0x100)?digitalReadF(pin):gpio.digitalRead(pin-1000)):0 )
#else
  #if defined(DAC_AS_DIGITAL)
    // DAC but no external GPIO
    #define pinModeEx(pin,mode)         { if (pin > 0x100) pinMode(pin-0x100,mode); else if (pin >= 0) pinMode(pin,mode); }
    #define digitalWriteEx(pin,value)   { if (pin > 0x100) analogWrite(pin-0x100,value); else if (pin >= 0) { digitalWriteF(pin,value); } }
  #else
    // neither DAC as digital or external GPIO
    #define pinModeEx(pin,mode)         { if (pin >= 0) pinMode(pin,mode); }
    #define digitalWriteEx(pin,value)   { if (pin >= 0) { digitalWriteF(pin,value); } }
  #endif
  // no support for DAC input and no external GPIO
  #define digitalReadEx(pin)            ( (pin >= 0)?digitalReadF(pin):0 )
#endif

#include "Ds2413.h"
#if GPIO_DEVICE != DS2413

#include "Mcp23008.h"
#if GPIO_DEVICE != MCP23008

#include "Mcp23017.h"
#if GPIO_DEVICE != MCP23017

class Gpio {
  public:
    bool init();

    void pinMode(int pin, int mode);

    int digitalRead(int pin);

    void digitalWrite(int pin, int value);

  private:
};

extern Gpio gpio;

#endif
#endif
#endif
