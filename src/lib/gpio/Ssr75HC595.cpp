// -----------------------------------------------------------------------------------
// 75HC595 GPIO support

#include "Ssr75HC595.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == SSR75HC595 && \
    GPIO_SSR75HC595_LATCH_PIN != OFF && \
    GPIO_SSR75HC595_CLOCK_PIN != OFF && \
    GPIO_SSR75HC595_DATA_PIN != OFF

#include "../tasks/OnTask.h"

// designed for a 20MHz max bit rate
IRAM_ATTR void shiftOut20MHz(uint8_t dataPin, uint8_t clockPin, uint32_t val) {
  for (int i = 0; i < 8; i++)  {
    digitalWrite(dataPin, !!(val & (1 << (7 - i))));
    digitalWrite(clockPin, HIGH);
    HAL_DELAY_25NS();
    digitalWrite(clockPin, LOW);
    HAL_DELAY_25NS();
  }
}

// get device ready
bool Ssr75HC595::init() {
  static bool initialized = false;
  if (initialized) return found;

  pinMode(GPIO_SSR75HC595_LATCH_PIN, OUTPUT);
  pinMode(GPIO_SSR75HC595_CLOCK_PIN, OUTPUT);
  pinMode(GPIO_SSR75HC595_DATA_PIN, OUTPUT);

  for (int i = 0; i < GPIO_SSR75HC595_COUNT; i++) {
    mode[i] = OUTPUT;
    state[i] = false;
  }

  found = true;
  return found;
}

// no command processing
bool Ssr75HC595::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  UNUSED(reply);
  UNUSED(command);
  UNUSED(parameter);
  UNUSED(supressFrame);
  UNUSED(numericReply);
  UNUSED(commandError);
  return false;
}

// set GPIO pin mode for INPUT, INPUT_PULLUP, or OUTPUT (input does nothing always, false)
void Ssr75HC595::pinMode(int pin, int mode) {
  if (found && pin >= 0 && pin < GPIO_SSR75HC595_COUNT) {
    #ifdef INPUT_PULLDOWN
      if (mode == INPUT_PULLDOWN) mode = INPUT;
    #endif
    this->mode[pin] = mode;
  }
}

// up to four eight channel 75HC595 GPIOs are supported, this gets the last set value (output only)
int Ssr75HC595::digitalRead(int pin) {
  if (found && pin >= 0 && pin < GPIO_SSR75HC595_COUNT) {
    return state[pin]; 
  } else return 0;
}

// up to four eight channel 75HC595 GPIOs are supported, this sets each output on or off
void Ssr75HC595::digitalWrite(int pin, bool value) {
  if (found && pin >= 0 && pin < GPIO_SSR75HC595_COUNT) {
    state[pin] = value;
    if (mode[pin] == OUTPUT) {
      if (value) bitSet(register_value, pin); else bitClear(register_value, pin);
      ::digitalWrite(GPIO_SSR75HC595_LATCH_PIN, LOW);
      if (GPIO_SSR75HC595_COUNT >= 32) shiftOut20MHz(GPIO_SSR75HC595_DATA_PIN, GPIO_SSR75HC595_CLOCK_PIN, (register_value>>24) & 0xff);
      if (GPIO_SSR75HC595_COUNT >= 24) shiftOut20MHz(GPIO_SSR75HC595_DATA_PIN, GPIO_SSR75HC595_CLOCK_PIN, (register_value>>16) & 0xff);
      if (GPIO_SSR75HC595_COUNT >= 16) shiftOut20MHz(GPIO_SSR75HC595_DATA_PIN, GPIO_SSR75HC595_CLOCK_PIN, (register_value>>8) & 0xff);
      shiftOut20MHz(GPIO_SSR75HC595_DATA_PIN, GPIO_SSR75HC595_CLOCK_PIN, (register_value) & 0xff);
      ::digitalWrite(GPIO_SSR75HC595_LATCH_PIN, HIGH);
    }
  } else return;
}

Ssr75HC595 gpio;

#endif
