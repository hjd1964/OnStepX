// -----------------------------------------------------------------------------------
// I2C PCF8574 GPIO support

#include "Pcf8574.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == X8574

// check for up to four PCF8574 devices on the I2C bus
bool GpioPcf8574::init() {
  static bool initialized = false;
  if (initialized) return found;

  for (int i = 0; i < 32; i++) mode[i] = -1;

  HAL_WIRE.begin();
  HAL_WIRE_SET_CLOCK();

  // check to be sure we find all devices
  for (int i = 0; i < GPIO_PCF8574_I2C_NUM_DEVICES; i++) {
    HAL_WIRE.beginTransmission(iicAddress[i]);
    uint8_t error = HAL_WIRE.endTransmission();
    if (error) {
      // try again
      HAL_WIRE.beginTransmission(iicAddress[i]);
      uint8_t error = HAL_WIRE.endTransmission();
      if (error) {
        if (DEBUG != OFF) { DF("WRN: Gpio.init(), PCF8574 (I2C 0x"); SERIAL_DEBUG.print(iicAddress[i], HEX); DLF(") not found"); }
        found = false;
        return false;
      }
    }
  }

  found = true;
  return found;
}

// set GPIO pin (0 to 31) mode for INPUT or OUTPUT
void GpioPcf8574::pinMode(int pin, int mode) {
  if (found && pin >= 0 && pin <= GPIO_PCF8574_I2C_NUM_DEVICES*8 - 1) {
    uint8_t device = pin >> 3;
    uint8_t devicePin = pin & 0b0111;

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
int GpioPcf8574::digitalRead(int pin) {
  if (found && pin >= 0 && pin <= GPIO_PCF8574_I2C_NUM_DEVICES*8 - 1 && mode[pin] > 0) {
    uint8_t device = pin >> 3;
    uint8_t devicePin = pin & 0b0111;

    if (mode[pin] == INPUT) {

      HAL_WIRE.requestFrom(iicAddress[device], (uint8_t)1);

      unsigned long timeout = millis() + 1000UL;
      do { if ((long)(timeout - millis()) < 0) return 0; } while (Wire.available() < 1);

      uint8_t data = HAL_WIRE.read();
      return bitRead(data, devicePin);
    } else
      return bitRead(state[device], devicePin);
  }

  return 0;
}

// set GPIO pin (0 to 31) state
void GpioPcf8574::digitalWrite(int pin, int value) {
  if (found && pin >= 0 && pin <= GPIO_PCF8574_I2C_NUM_DEVICES*8 - 1 && mode[pin] > 0) {
    if (mode[pin] == OUTPUT) {
      uint8_t device = pin >> 3;
      uint8_t devicePin = pin & 0b0111;

      bitWrite(state[device], devicePin, (value == 0) ? 0 : 1);

      HAL_WIRE.beginTransmission(iicAddress[device]);
      HAL_WIRE.write(state[device]);
      HAL_WIRE.endTransmission();
    }
  }
}

GpioPcf8574 gpio;

#endif
