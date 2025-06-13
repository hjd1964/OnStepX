// -----------------------------------------------------------------------------------
// I2C PCF8575 GPIO support

#include "Pcf8575.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == X8575

// check for up to two PCF8575 devices on the I2C bus
bool GpioPcf8575::init() {
  static bool initialized = false;
  if (initialized) return found;

  for (int i = 0; i < 32; i++) mode[i] = -1;

  HAL_WIRE.begin();
  HAL_WIRE_SET_CLOCK();

  // check to be sure we find all devices
  for (int i = 0; i < GPIO_PCF8575_I2C_NUM_DEVICES; i++) {
    HAL_WIRE.beginTransmission(iicAddress[i]);
    uint8_t error = HAL_WIRE.endTransmission();
    if (error) {
      // try again
      HAL_WIRE.beginTransmission(iicAddress[i]);
      uint8_t error = HAL_WIRE.endTransmission();
      if (error) {
        if (DEBUG != OFF) { DF("WRN: Gpio.init(), PCF8575 (I2C 0x"); SERIAL_DEBUG.print(iicAddress[i], HEX); DLF(") not found"); }
        found = false;
        return false;
      }
    }
  }

  found = true;
  return found;
}

// set GPIO pin (0 to 31) mode for INPUT or OUTPUT
void GpioPcf8575::pinMode(int pin, int mode) {
  if (found && pin >= 0 && pin <= GPIO_PCF8575_I2C_NUM_DEVICES*16 - 1) {
    uint8_t device = pin >> 4;
    uint8_t devicePin = pin & 0b01111;

    #ifdef INPUT_PULLDOWN
      if (mode == INPUT_PULLDOWN) mode = INPUT;
    #endif
    if (mode == INPUT_PULLUP) mode = INPUT;
    if (mode != INPUT && mode != OUTPUT) return;

    if (this->mode[pin] != mode) {
      bitWrite(state[device], devicePin, (mode == INPUT) ? 1 : 0);

      HAL_WIRE.beginTransmission(iicAddress[device]);
      HAL_WIRE.write(state[device]);
      HAL_WIRE.endTransmission();

      this->mode[pin] = mode;
    }
  }
}

// get GPIO pin (0 to 31) state
int GpioPcf8575::digitalRead(int pin) {
  if (found && pin >= 0 && pin <= GPIO_PCF8575_I2C_NUM_DEVICES*16 - 1 && mode[pin] > 0) {
    uint8_t device = pin >> 4;
    uint8_t devicePin = pin & 0b01111;

    if (mode[pin] == INPUT) {

      HAL_WIRE.requestFrom(iicAddress[device], (uint8_t)2);

      unsigned long timeout = millis() + 1000UL;
      do { if ((long)(timeout - millis()) < 0) return 0; } while (Wire.available() < 2);

      uint16_t data = HAL_WIRE.read();
      data |= HAL_WIRE.read() << 8;

      return bitRead(data, devicePin);
    } else
      return bitRead(state[device], devicePin);
  }

  return 0;
}

// set GPIO pin (0 to 31) state
void GpioPcf8575::digitalWrite(int pin, int value) {
  if (found && pin >= 0 && pin <= GPIO_PCF8575_I2C_NUM_DEVICES*16 - 1 && mode[pin] > 0) {
    if (mode[pin] == OUTPUT) {
      uint8_t device = pin >> 4;
      uint8_t devicePin = pin & 0b01111;

      bitWrite(state[device], devicePin, (value == 0) ? 0 : 1);

      HAL_WIRE.beginTransmission(iicAddress[device]);
      HAL_WIRE.write(state[device] & 0x000FF);
      HAL_WIRE.write((state[device] & 0x0FF00) >> 8);
      HAL_WIRE.endTransmission();
    }
  }
}

GpioPcf8575 gpio;

#endif
