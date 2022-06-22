// -----------------------------------------------------------------------------------
// 74HC595 GPIO support

#include "Ssr74HC595.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == SSR74HC595

#if !defined(GPIO_SSR74HC595_LATCH_PIN) || GPIO_SSR74HC595_LATCH_PIN == OFF
  #error "GPIO device SSR74HC595 GPIO_SSR74HC595_LATCH_PIN must be present and not OFF."
#endif
#if !defined(GPIO_SSR74HC595_CLOCK_PIN) || GPIO_SSR74HC595_CLOCK_PIN == OFF
  #error "GPIO device SSR74HC595 GPIO_SSR74HC595_CLOCK_PIN must be present and not OFF."
#endif
#if !defined(GPIO_SSR74HC595_DATA_PIN) || GPIO_SSR74HC595_DATA_PIN == OFF
  #error "GPIO device SSR74HC595 GPIO_SSR74HC595_DATA_PIN must be present and not OFF."
#endif
#ifndef GPIO_SSR74HC595_COUNT
  #error "GPIO device SSR74HC595 GPIO_SSR74HC595_COUNT must be present."
#endif
#if GPIO_SSR74HC595_COUNT != 8 && GPIO_SSR74HC595_COUNT != 16 && GPIO_SSR74HC595_COUNT != 24 && GPIO_SSR74HC595_COUNT != 32
  #error "GPIO device SSR74HC595 supports GPIO_SSR74HC595_COUNT of 8, 16, 24, or 32 only."
#endif

#ifdef ESP32
  // ESP32 GPIO SSR74HC595 macros (if used, code below only works for pins 0 to 31)
  #define GPIO_SSR74HC595_LATCH_LOW() { GPIO.out_w1tc = ((uint32_t)1 << GPIO_SSR74HC595_LATCH_PIN); }
  #define GPIO_SSR74HC595_LATCH_HIGH() { GPIO.out_w1ts = ((uint32_t)1 << GPIO_SSR74HC595_LATCH_PIN); }
  #define GPIO_SSR74HC595_CLOCK_LOW() { GPIO.out_w1tc = ((uint32_t)1 << GPIO_SSR74HC595_CLOCK_PIN); }
  #define GPIO_SSR74HC595_CLOCK_HIGH() { GPIO.out_w1ts = ((uint32_t)1 << GPIO_SSR74HC595_CLOCK_PIN); }
  #define GPIO_SSR74HC595_DATA_LOW() { GPIO.out_w1tc = ((uint32_t)1 << GPIO_SSR74HC595_DATA_PIN); }
  #define GPIO_SSR74HC595_DATA_HIGH() { GPIO.out_w1ts = ((uint32_t)1 << GPIO_SSR74HC595_DATA_PIN); }
#else
  // generic GPIO SSR74HC595 macros
  #define GPIO_SSR74HC595_LATCH_LOW() digitalWrite(GPIO_SSR74HC595_LATCH_PIN, LOW)
  #define GPIO_SSR74HC595_LATCH_HIGH() digitalWrite(GPIO_SSR74HC595_LATCH_PIN, HIGH)
  #define GPIO_SSR74HC595_CLOCK_LOW() digitalWrite(GPIO_SSR74HC595_CLOCK_PIN, LOW)
  #define GPIO_SSR74HC595_CLOCK_HIGH() digitalWrite(GPIO_SSR74HC595_CLOCK_PIN, HIGH)
  #define GPIO_SSR74HC595_DATA_LOW() digitalWrite(GPIO_SSR74HC595_DATA_PIN, LOW)
  #define GPIO_SSR74HC595_DATA_HIGH() digitalWrite(GPIO_SSR74HC595_DATA_PIN, HIGH)
#endif

#include "../tasks/OnTask.h"

// designed for a 20MHz max bit rate
IRAM_ATTR void shiftOut20MHz(uint32_t val) {
  if ((val & 0b10000000) == 0) { GPIO_SSR74HC595_DATA_LOW(); } else { GPIO_SSR74HC595_DATA_HIGH(); }
  GPIO_SSR74HC595_CLOCK_HIGH();
  GPIO_SSR74HC595_CLOCK_LOW();
  if ((val & 0b01000000) == 0) { GPIO_SSR74HC595_DATA_LOW(); } else { GPIO_SSR74HC595_DATA_HIGH(); }
  GPIO_SSR74HC595_CLOCK_HIGH();
  GPIO_SSR74HC595_CLOCK_LOW();
  if ((val & 0b00100000) == 0) { GPIO_SSR74HC595_DATA_LOW(); } else { GPIO_SSR74HC595_DATA_HIGH(); }
  GPIO_SSR74HC595_CLOCK_HIGH();
  GPIO_SSR74HC595_CLOCK_LOW();
  if ((val & 0b00010000) == 0) { GPIO_SSR74HC595_DATA_LOW(); } else { GPIO_SSR74HC595_DATA_HIGH(); }
  GPIO_SSR74HC595_CLOCK_HIGH();
  GPIO_SSR74HC595_CLOCK_LOW();
  if ((val & 0b00001000) == 0) { GPIO_SSR74HC595_DATA_LOW(); } else { GPIO_SSR74HC595_DATA_HIGH(); }
  GPIO_SSR74HC595_CLOCK_HIGH();
  GPIO_SSR74HC595_CLOCK_LOW();
  if ((val & 0b00000100) == 0) { GPIO_SSR74HC595_DATA_LOW(); } else { GPIO_SSR74HC595_DATA_HIGH(); }
  GPIO_SSR74HC595_CLOCK_HIGH();
  GPIO_SSR74HC595_CLOCK_LOW();
  if ((val & 0b00000010) == 0) { GPIO_SSR74HC595_DATA_LOW(); } else { GPIO_SSR74HC595_DATA_HIGH(); }
  GPIO_SSR74HC595_CLOCK_HIGH();
  GPIO_SSR74HC595_CLOCK_LOW();
  if ((val & 0b00000001) == 0) { GPIO_SSR74HC595_DATA_LOW(); } else { GPIO_SSR74HC595_DATA_HIGH(); }
  GPIO_SSR74HC595_CLOCK_HIGH();
  GPIO_SSR74HC595_CLOCK_LOW();
}

// get device ready
bool Ssr74HC595::init() {
  static bool initialized = false;
  if (initialized) return found;

  ::pinMode(GPIO_SSR74HC595_LATCH_PIN, OUTPUT);
  ::pinMode(GPIO_SSR74HC595_CLOCK_PIN, OUTPUT);
  ::pinMode(GPIO_SSR74HC595_DATA_PIN, OUTPUT);

  for (int i = 0; i < GPIO_SSR74HC595_COUNT; i++) {
    mode[i] = OUTPUT;
    state[i] = false;
  }

  VLF("MSG: GPIO, SSR74HC595 Initialized");

  found = true;
  return found;
}

// no command processing
bool Ssr74HC595::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  UNUSED(reply);
  UNUSED(command);
  UNUSED(parameter);
  UNUSED(supressFrame);
  UNUSED(numericReply);
  UNUSED(commandError);
  return false;
}

// set GPIO pin mode for INPUT, INPUT_PULLUP, or OUTPUT (input does nothing always, false)
void Ssr74HC595::pinMode(int pin, int mode) {
  if (found && pin >= 0 && pin < GPIO_SSR74HC595_COUNT) {
    #ifdef INPUT_PULLDOWN
      if (mode == INPUT_PULLDOWN) mode = INPUT;
    #endif
    this->mode[pin] = mode;
  }
}

// up to four eight channel 74HC595 GPIOs are supported, this gets the last set value (output only)
int Ssr74HC595::digitalRead(int pin) {
  if (found && pin >= 0 && pin < GPIO_SSR74HC595_COUNT) {
    return state[pin]; 
  } else return 0;
}

// up to four eight channel 74HC595 GPIOs are supported, this sets each output on or off
void Ssr74HC595::digitalWrite(int pin, bool value) {
  if (found && pin >= 0 && pin < GPIO_SSR74HC595_COUNT) {
    cli();
    state[pin] = value;
    if (mode[pin] == OUTPUT) {
      if (value) bitSet(register_value, pin); else bitClear(register_value, pin);
      GPIO_SSR74HC595_LATCH_LOW();
      if (GPIO_SSR74HC595_COUNT >= 32) shiftOut20MHz((register_value>>24) & 0xff);
      if (GPIO_SSR74HC595_COUNT >= 24) shiftOut20MHz((register_value>>16) & 0xff);
      if (GPIO_SSR74HC595_COUNT >= 16) shiftOut20MHz((register_value>>8) & 0xff);
      shiftOut20MHz((register_value) & 0xff);
      GPIO_SSR74HC595_LATCH_HIGH();
    }
    sei();
  } else return;
}

Ssr74HC595 gpio;

#endif
