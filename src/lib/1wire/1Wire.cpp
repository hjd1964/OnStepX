// -----------------------------------------------------------------------------------
// Dallas/Maxim 1-Wire library

#include "1Wire.h"

#if defined(DS1820_DEVICES_PRESENT) || GPIO_DEVICE == DS2413

OneWire oneWire(ONE_WIRE_PIN);

#endif