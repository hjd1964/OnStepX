// -----------------------------------------------------------------------------------
// Dallas/Maxim 1-Wire library
#pragma once

#include "../../Common.h"

#if defined(DS1820_DEVICES_PRESENT) || (defined(GPIO_DEVICE) && GPIO_DEVICE == DS2413)

#include <OneWire.h>    // my OneWire library https://github.com/hjd1964/OneWire

extern OneWire oneWire;

#endif
