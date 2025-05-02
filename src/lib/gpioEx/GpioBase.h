// -----------------------------------------------------------------------------------
// standard external GPIO library
#pragma once

#include "../../Common.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE != OFF

  #include "../commands/CommandErrors.h"

  class Gpio {
    public:
      virtual bool init();
      virtual bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);
      virtual void pinMode(int pin, int mode);
      virtual int digitalRead(int pin);
      virtual void digitalWrite(int pin, int value);

      bool lateInitError = false;
    private:
  };

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

// automatically use analogWrite
#ifndef analogWriteEx
  #define analogWriteEx(pin,value)     { analogWrite(pin,value); }
#endif

// use "Ex" functions to exclude pins that are OFF or SHARED
// pins in range 0x00 to 0xFF are normal, 0x100 to 0x1FF are DAC as digital outputs, 0x200 to 0x2FF are external GPIO pins
#define CLEAN_PIN(pin) ((uint8_t)((pin) & 0xff))
