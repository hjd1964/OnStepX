/*
 * Hardware Abstraction Layer (HAL) for OnStep
 * 
 * Copyright (C) 2018 Khalid Baheyeldin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#if defined(__AVR_ATmega328P__)
  #define MCU_STR "AtMega328"
  #include "atmel/Mega328.h"

#elif defined(__AVR_ATmega1280__)
  #define MCU_STR "Mega1280"
  #include "atmel/Mega2560.h"

#elif defined(__AVR_ATmega2560__)
  #define MCU_STR "Mega2560"
  #include "atmel/Mega2560.h"

#elif defined(ARDUINO_UNOWIFIR4)
  // Arduino UNO R4 WIFI
  #define MCU_STR "RENESAS RA4M1 (Arduino UNO R4 WIFI)"
  #include "esp32/ESP32UnoR4WiFi.h"

#elif defined(ARDUINO_ARCH_SAMD)
  // ARDUINO M0
  #define MCU_STR "ARDUINO M0"
  #include "HAL_ZERO.h"

#elif defined(ESP32)
  // ESP32
  #define MCU_STR "ESP32"
  #if ESP_ARDUINO_VERSION >= 0x30000
    #include "esp/ESP32Libraries3.h"
  #elif ESP_ARDUINO_VERSION >= 0x20000 + 3
    #include "esp/ESP32Libraries2.h"
  #else
    #include "esp/ESP32Libraries1.h"
  #endif

#elif defined(ESP8266)
  // ESP8266
  #define MCU_STR "ESP8266"
  #include "esp/ESP8266.h"

#elif defined(STM32F103xB)
  // STM32F103C8/CB: 72MHz, 128K flash, 64K RAM, ARM Cortex M3
  #define MCU_STR "STM32F103"
  #include "stm32/STM32F103.h"

#elif defined(STM32F303xC)
  // RobotDyn BlackPill STM32F303, 256K flash, ARM Cortex M4 (STM32duino board manager)
  #define MCU_STR "STM32F303"
  #include "stm32/STM32F303.h"

#elif defined(STM32F401xC)
  // Blackpill board with STM32F401CC
  #define MCU_STR "STM32F401"
  #include "stm32/STM32F4x1.h"

#elif defined(STM32F411xE)
  // Blackpill board with STM32F411CE
  #define MCU_STR "STM32F411"
  #include "stm32/STM32F4x1.h"

#elif defined(STM32F407xx)
  // BTT SKR PRO board with STM32F407
  #define MCU_STR "STM32F407"
  #include "stm32/STM32F407.h"

#elif defined(STM32F446xx)
  // FYSETC S6 board with STM32F446
  #define MCU_STR "STM32F446"
  #include "stm32/STM32F446.h"

#elif defined(STM32H723xx)
  #define MCU_STR "STM32H723"
  #include "stm32/STM32H7xx.h"

#elif defined(STM32H743xx)
  // WeAct Studio board with STM32H743
  #define MCU_STR "STM32H743"
  #include "stm32/STM32H7xx.h"

#elif defined(STM32H750xx)
  // WeAct Studio board with STM32H750
  #define MCU_STR "STM32H750"
  #include "stm32/STM32H7xx.h"

#elif defined(_mk20dx128_h_) || defined(__MK20DX128__)
  // Teensy 3.0
  #define MCU_STR "Teensy3.0"
  #include "teensy/Teensy3.2.h"

#elif defined(__MK20DX256__)
  // Teensy 3.2
  #define MCU_STR "Teensy3.2"
  #include "teensy/Teensy3.2.h"

#elif defined(__MK64FX512__)
  // Teensy 3.5
  #define MCU_STR "Teensy3.5"
  #include "teensy/Teensy3.5.h"

#elif defined(__MK66FX1M0__)
  // Teensy 3.6
  #define MCU_STR "Teensy3.6"
  #include "teensy/Teensy3.6.h"

#elif defined(ARDUINO_TEENSY40)
  // Teensy 4.0
  #define MCU_STR "Teensy4.0"
  #include "teensy/Teensy4.0.h"

#elif defined(ARDUINO_TEENSY41)
  // Teensy 4.1
  #define MCU_STR "Teensy4.1"
  #include "teensy/Teensy4.1.h"

#elif defined(ARDUINO_TEENSY_MICROMOD)
  // Teensy MicroMod
  #define MCU_STR "TeensyMicroMod"
  #include "teensy/Teensy4.1.h"

#elif defined(ARDUINO_ARCH_RP2040)
  // Raspberry pi pico
  #define MCU_STR "Raspberry Pi Pico"
  #include "mbed/Rpi2040.h"

#elif defined(ARDUINO_ARCH_RP2350)
  // Raspberry pi pico2
  #define MCU_STR "Raspberry Pi Pico2"
  #include "mbed/Rpi2350.h"

#else
  // Generic
  #warning "Unknown Platform! If this is a new platform, it would probably do best with a new HAL designed for it."
  #define MCU_STR "Generic (Unknown)"
  #include "default/Default.h"
#endif

// create null decoration for non-ESP processors
#ifndef IRAM_ATTR
  #define IRAM_ATTR
#endif

#ifndef ICACHE_RAM_ATTR
  #define ICACHE_RAM_ATTR
#endif

#ifndef FPSTR
  #define FPSTR
#endif

#ifdef HAL_FRACTIONAL_SEC
  #define FRACTIONAL_SEC  HAL_FRACTIONAL_SEC
#else
  #define FRACTIONAL_SEC  100.0F
#endif

#ifndef HAL_MIN_PPS_SUB_MICRO
  #define HAL_MIN_PPS_SUB_MICRO 4
#endif

#ifndef CAT_ATTR
  #define CAT_ATTR
#endif

#if defined(HAL_WIRE_CLOCK)
  #define HAL_WIRE_SET_CLOCK() HAL_WIRE.setClock(HAL_WIRE_CLOCK)
#else
  #define HAL_WIRE_SET_CLOCK()
#endif
