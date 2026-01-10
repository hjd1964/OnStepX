// -----------------------------------------------------------------------------------
// standard external GPIO library

#include "GpioBase.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE != OFF

bool Gpio::command(char *reply, char *command, char *parameter, bool *suppressFrame, bool *numericReply, CommandError *commandError) {
  UNUSED(reply);
  UNUSED(command);
  UNUSED(parameter);
  UNUSED(suppressFrame);
  UNUSED(numericReply);
  UNUSED(commandError);
  return false;
}

#endif
