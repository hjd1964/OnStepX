// -----------------------------------------------------------------------------------
// standard external GPIO library
#pragma once

#include "../../Common.h"

// use Fast functions for digital I/O if available
#ifdef HAL_HAS_DIGITAL_FAST
  #define digitalReadF(pin)           ( digitalReadFast(pin) )
  #define digitalWriteF(pin,value)    { digitalWriteFast(pin,value); }
#else
  #define digitalReadF(pin)           ( digitalRead(pin) )
  #define digitalWriteF(pin,value)    { digitalWrite(pin,value); }
#endif

// use "Ex" functions to exclude pins that are OFF or SHARED
#if GPIO_DEVICE != OFF
  // if an external GPIO is enabled use it for pins 1000 and up otherwise use the built-in
  #define pinModeEx(pin,mode)         { if (pin >= 0) { if (pin < 1000) pinMode(pin,mode); else gpio.pinMode(pin-1000,mode); } }
  #define digitalWriteEx(pin,value)   { if (pin >= 0) { if (pin < 1000) { digitalWriteF(pin,value); } else gpio.digitalWrite(pin-1000,value); } }
  #define digitalReadEx(pin)          ( (pin >= 0)?((pin < 1000)?digitalReadF(pin):gpio.digitalRead(pin-1000)):0 )
#else
  #define pinModeEx(pin,mode)         { if (pin >= 0) { pinMode(pin,mode); } }
  #define digitalWriteEx(pin,value)   { if (pin >= 0) { digitalWriteF(pin,value); } }
  #define digitalReadEx(pin)          ( (pin >= 0)?digitalReadF(pin):0 )
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

#endif
#endif
#endif

extern Gpio gpio;
