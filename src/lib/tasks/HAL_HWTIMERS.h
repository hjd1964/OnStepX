//--------------------------------------------------------------------------------------------------
// Selects hardware timer HAL according to platform

// these must be present even if the hardware timer isn't brought in
#ifndef TASKS_HWTIMER1_ENABLE
  void (*HAL_HWTIMER1_FUN)() = NULL;
  #define HAL_HWTIMER1_SET_PERIOD()
  bool HAL_HWTIMER1_INIT(uint8_t priority) { (void)(priority); return false; }
  void HAL_HWTIMER1_DONE() { }
#endif
#ifndef TASKS_HWTIMER2_ENABLE
  void (*HAL_HWTIMER2_FUN)() = NULL;
  #define HAL_HWTIMER2_SET_PERIOD()
  bool HAL_HWTIMER2_INIT(uint8_t priority) { (void)(priority); return false; }
  void HAL_HWTIMER2_DONE() { }
#endif
#ifndef TASKS_HWTIMER3_ENABLE
  void (*HAL_HWTIMER3_FUN)() = NULL;
  #define HAL_HWTIMER3_SET_PERIOD()
  bool HAL_HWTIMER3_INIT(uint8_t priority) { (void)(priority); return false;}
  void HAL_HWTIMER3_DONE() { }
#endif
#ifndef TASKS_HWTIMER4_ENABLE
  void (*HAL_HWTIMER4_FUN)() = NULL;
  #define HAL_HWTIMER4_SET_PERIOD()
  bool HAL_HWTIMER4_INIT(uint8_t priority) { (void)(priority); return false; }
  void HAL_HWTIMER4_DONE() { }
#endif

// bring in hardware timer support
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
  #include "HAL_ATMEGA328_HWTIMER.h"
#elif defined(__AVR_ATmega1280__) ||defined(__AVR_ATmega2560__)
  #include "HAL_MEGA2560_HWTIMER.h"
#elif defined(STM32H743xx) || defined(STM32H750xx) || \
      defined(STM32F446xx) || defined(STM32F411xE) || \
      defined(STM32F401xC) || defined(STM32F407xx) || \
      defined(STM32F303xC) || defined(STM32F103xB)
  #include "HAL_STM32_HWTIMER.h"
// Teensy3.0, 3.1, 3.2, 3.5, 3.6, and 4.0
#elif defined(_mk20dx128_h_) || defined(__MK20DX128__) || \
      defined(__MK20DX256__) || defined(__MK64FX512__) || \
      defined(__MK66FX1M0__) || defined(__IMXRT1052__) || \
      defined(__IMXRT1062__)  
  #include "HAL_TEENSY_HWTIMER.h"
#elif defined(ESP32)
  #if ESP_ARDUINO_VERSION >= 0x30000
    #include "HAL_ESP32_V3_HWTIMER.h"
  #else
    #include "HAL_ESP32_HWTIMER.h"
  #endif
#else
  #include "HAL_EMPTY_HWTIMER.h"
#endif
