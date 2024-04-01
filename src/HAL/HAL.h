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
  #include "HAL_AtMega328.h"

#elif defined(__AVR_ATmega1280__)
  #define MCU_STR "Mega1280"
  #include "HAL_Mega2560.h"

#elif defined(__AVR_ATmega2560__)
  #define MCU_STR "Mega2560"
  #include "HAL_Mega2560.h"

#elif defined(_mk20dx128_h_) || defined(__MK20DX128__)
  // Teensy 3.0
  #define MCU_STR "Teensy3.0"
  #include "HAL_Teensy_3.2.h"

#elif defined(__MK20DX256__)
  // Teensy 3.2
  #define MCU_STR "Teensy3.2"
  #include "HAL_Teensy_3.2.h"

#elif defined(__MK64FX512__)
  // Teensy 3.5
  #define MCU_STR "Teensy3.5"
  #include "HAL_Teensy_3.5.h"

#elif defined(__MK66FX1M0__)
  // Teensy 3.6
  #define MCU_STR "Teensy3.6"
  #include "HAL_Teensy_3.6.h"

#elif defined(ARDUINO_TEENSY40)
  // Teensy 4.0
  #define MCU_STR "Teensy4.0"
  #include "HAL_Teensy_4.0.h"

#elif defined(ARDUINO_TEENSY41)
  // Teensy 4.1
  #define MCU_STR "Teensy4.1"
  #include "HAL_Teensy_4.1.h"

#elif defined(STM32F103xB)
  // STM32F103C8/CB: 72MHz, 128K flash, 64K RAM, ARM Cortex M3
  #define MCU_STR "STM32F103"
  #include "HAL_STM32F103.h"

#elif defined(STM32F303xC)
  // RobotDyn BlackPill STM32F303, 256K flash, ARM Cortex M4 (STM32duino board manager)
  #define MCU_STR "STM32F303"
  #include "HAL_STM32F303.h"

#elif defined(STM32F401xC)
  // Blackpill board with STM32F401CC
  #define MCU_STR "STM32F401"
  #include "HAL_STM32F4x1.h"

#elif defined(STM32F411xE)
  // Blackpill board with STM32F411CE
  #define MCU_STR "STM32F411"
  #include "HAL_STM32F4x1.h"

#elif defined(STM32F407xx)
  // BTT SKR PRO board with STM32F407
  #define MCU_STR "STM32F407"
  #include "HAL_STM32F407.h"

#elif defined(STM32F446xx)
  // FYSETC S6 board with STM32F446
  #define MCU_STR "STM32F446"
  #include "HAL_STM32F446.h"

#elif defined(STM32H743xx)
  // WeAct Studio board with STM32H743
  #define MCU_STR "STM32H743"
  #include "HAL_STM32H7xx.h"

#elif defined(STM32H750xx)
  // WeAct Studio board with STM32H750
  #define MCU_STR "STM32H750"
  #include "HAL_STM32H7xx.h"

#elif defined(ESP32)
  // ESP32
  #define MCU_STR "ESP32"
  #include "HAL_ESP32.h"

#elif defined(__SAM3X8E__)
  // Arduino Due
  #define MCU_STR "SAM3X8E (Arduino DUE)"
  #include "HAL_Due.h"  

#elif defined(ARDUINO_UNOWIFIR4)
  // Arduino UNO R4 WIFI
  #define MCU_STR "RENESAS RA4M1 (Arduino UNO R4 WIFI)"
  #include "HAL_UNO_R4_WIFI.h"  

#else
  // Generic
  #warning "Unknown Platform! If this is a new platform, it would probably do best with a new HAL designed for it."
  #define MCU_STR "Generic (Unknown)"
  #include "HAL_MISC.h"  
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

#if NV_DRIVER == NV_2416
  #define E2END 2047
#elif NV_DRIVER == NV_2432
  #define E2END 4095
#elif NV_DRIVER == NV_AT24C32
  #define E2END 4095
#elif NV_DRIVER == NV_2464
  #define E2END 8191
#elif NV_DRIVER == NV_24128
  #define E2END 16383
#elif NV_DRIVER == NV_24256
  #define E2END 32767
#elif NV_DRIVER == NV_MB85RC64
  #define E2END 8191
#elif NV_DRIVER == NV_MB85RC256
  #define E2END 32767
#endif
#if NV_DRIVER == NV_2416 || NV_DRIVER == NV_2432 || NV_DRIVER == NV_AT24C32 || NV_DRIVER == NV_2464 || NV_DRIVER == NV_24128 || NV_DRIVER == NV_24256
  #if NV_DRIVER == NV_AT24C32
    #ifndef NV_ENDURANCE
      #define NV_ENDURANCE NVE_HIGH
    #endif
    #ifndef NV_ADDRESS
      #define NV_ADDRESS 0x57
    #endif
  #else
    #ifndef NV_ADDRESS
      #define NV_ADDRESS 0x50
    #endif
  #endif
  #ifndef NV_CACHED
    #define NV_CACHED true
  #endif
  #include "../lib/nv/NV_24XX.h"
  #define HAL_NV_INIT() nv.init(E2END + 1, NV_CACHED, 0, false, &HAL_Wire, NV_ADDRESS)
#elif NV_DRIVER == NV_MB85RC64 || NV_DRIVER == NV_MB85RC256
  #ifndef NV_ENDURANCE
    #define NV_ENDURANCE NVE_VHIGH
  #endif
  #ifndef NV_ADDRESS
    #define NV_ADDRESS 0x50
  #endif
  #ifndef NV_CACHED
    #define NV_CACHED true
  #endif
  #include "../lib/nv/NV_MB85RC.h"
  #define HAL_NV_INIT() nv.init(E2END + 1, NV_CACHED, 0, false, &HAL_Wire, NV_ADDRESS)
#endif

// Non-volatile storage
#ifndef NV_ENDURANCE
  #define NV_ENDURANCE NVE_MID
#endif
