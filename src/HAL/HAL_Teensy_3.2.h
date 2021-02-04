// Platform setup ------------------------------------------------------------------------------------
// Teensy3.0, 3.1, 3.2

// This platform has digitalReadFast, digitalWriteFast, etc.
#define HAL_HAS_DIGITAL_FAST

// Lower limit (fastest) step rate in uS for this platform, width of step pulse, and set HAL_FAST_PROCESSOR is needed
#if F_CPU >= 120000000
  #define HAL_MAXRATE_LOWER_LIMIT 10
  #define HAL_PULSE_WIDTH 800
#elif F_CPU >= 96000000
  #define HAL_MAXRATE_LOWER_LIMIT 12
  #define HAL_PULSE_WIDTH 900
#elif F_CPU >= 72000000
  #define HAL_MAXRATE_LOWER_LIMIT 14
  #define HAL_PULSE_WIDTH 1000
#else
  #define HAL_MAXRATE_LOWER_LIMIT 28
  #define HAL_PULSE_WIDTH 1500
#endif

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------

// SerialA is manidatory
#define SERIAL_A Serial
// SerialB is optional
#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B Serial1
#endif
// SerialC is optional, not used
// SerialD/E are optional
#if defined(USB_DUAL_SERIAL) || defined(USB_TRIPLE_SERIAL)
  #define SERIAL_D SerialUSB1
  #define SERIAL_D_BAUD_DEFAULT 9600
#endif
#if defined(USB_TRIPLE_SERIAL)
  #define SERIAL_E SerialUSB2
  #define SERIAL_E_BAUD_DEFAULT 9600
#endif

// New symbol for the default I2C port -------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ------------------------------------------------------------------------------
#ifdef NV_DEFAULT
  #include "NV/NV_EEPROM.h"
#endif

//--------------------------------------------------------------------------------------------------
// Nanoseconds delay function
unsigned int _nanosPerPass=1;
void delayNanoseconds(unsigned int n) {
  unsigned int np=(n/_nanosPerPass);
  for (unsigned int i=0; i<np; i++) { __asm__ volatile ("nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t"); }
}

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
void HAL_Initialize(void) {
  // calibrate delayNanoseconds()
  uint32_t startTime,npp;
  cli(); startTime=micros(); delayNanoseconds(65535); npp=micros(); sei(); npp=((int32_t)(npp-startTime)*1000)/63335;
  if (npp<1) npp=1; if (npp>2000) npp=2000; _nanosPerPass=npp;

  // default 10 bit analog read resolution (0 to 1023)
  analogReadResolution(10);
}

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
float HAL_MCU_Temperature(void) {
  int Tpin = 38;
  // delta of -1.715 mV/C where 25C measures 719 mV
  float v=(analogRead(Tpin)/1024.0)*3.3;
  float t=(-(v-0.719)/0.001715)+25.0;
  return t;
}
