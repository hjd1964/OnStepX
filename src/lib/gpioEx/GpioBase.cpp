// -----------------------------------------------------------------------------------
// standard external GPIO library

#include "GpioBase.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE != OFF

bool Gpio::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  UNUSED(reply);
  UNUSED(command);
  UNUSED(parameter);
  UNUSED(supressFrame);
  UNUSED(numericReply);
  UNUSED(commandError);
  return false;
}

#endif
