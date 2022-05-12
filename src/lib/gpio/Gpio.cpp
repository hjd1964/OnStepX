// -----------------------------------------------------------------------------------
// standard external GPIO library

#include "Gpio.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == OFF

bool Gpio::init() {
  return true;
}

bool Gpio::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  UNUSED(reply);
  UNUSED(command);
  UNUSED(parameter);
  UNUSED(supressFrame);
  UNUSED(numericReply);
  UNUSED(commandError);
  return false;
}

void Gpio::pinMode(int pin, int mode) {
  pin = pin;
  mode = mode;
}

int Gpio::digitalRead(int pin) {
  pin = pin;
  return 0;
}

void Gpio::digitalWrite(int pin, int value) {
  pin = pin;
  value = value;
}

Gpio gpio;

#endif
