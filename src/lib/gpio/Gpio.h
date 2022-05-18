// -----------------------------------------------------------------------------------
// standard external GPIO library
#pragma once

#include "../../Common.h"

#if defined(GPIO_DEVICE)

#if GPIO_DEVICE == DS2413
  #include "Ds2413.h"
#endif

#if GPIO_DEVICE == MCP23008
  #include "Mcp23008.h"
#endif

#if GPIO_DEVICE == MCP23017
  #include "Mcp23017.h"
#endif

#if GPIO_DEVICE == X9555
  #include "Tca9555.h"
#endif

#if GPIO_DEVICE == X8575
  #include "Pcf8575.h"
#endif

#if GPIO_DEVICE == SWS
  #include "Sws.h"
#endif

#if GPIO_DEVICE == SSR74HC595
  #include "Ssr74HC595.h"
#endif

#if GPIO_DEVICE == OFF
  #include "../commands/CommandErrors.h"
  class Gpio {
    public:
      bool init();
      bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);
      void pinMode(int pin, int mode);
      int digitalRead(int pin);
      void digitalWrite(int pin, int value);
    private:
  };
  extern Gpio gpio;
#endif

#endif
