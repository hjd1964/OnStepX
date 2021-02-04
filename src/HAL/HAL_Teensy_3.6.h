// Platform setup ------------------------------------------------------------------------------------
// Teensy3.6

// This platform has digitalReadFast, digitalWriteFast, etc.
#define HAL_HAS_DIGITAL_FAST

// Lower limit (fastest) step rate in uS for this platform, width of step pulse, and set HAL_FAST_PROCESSOR is needed
#if F_CPU >= 240000000
  #define HAL_MAXRATE_LOWER_LIMIT 2
  #define HAL_PULSE_WIDTH 260
#elif F_CPU >= 180000000
  #define HAL_MAXRATE_LOWER_LIMIT 2.6
  #define HAL_PULSE_WIDTH 400
#else
  #define HAL_MAXRATE_LOWER_LIMIT 4.8
  #define HAL_PULSE_WIDTH 500
#endif
#define HAL_FAST_PROCESSOR

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------

// SerialA is manidatory
#define SERIAL_A Serial
// SerialB is optional
#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B Serial1
#endif
// SerialC is optional
#if SERIAL_C_BAUD_DEFAULT != OFF
  #define SERIAL_C Serial4
#endif
// SerialD/E are optional
#if defined(USB_DUAL_SERIAL) || defined(USB_TRIPLE_SERIAL)
  #define SERIAL_D SerialUSB1
#endif
#if defined(USB_TRIPLE_SERIAL)
  #define SERIAL_E SerialUSB2
#endif

// New symbol for the default I2C port -------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire1
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
  uint32_t startTime, npp;
  cli(); startTime = micros(); delayNanoseconds(65535); npp=micros(); sei(); npp = ((int32_t)(npp - startTime)*1000)/63335;
  if (npp < 1) npp = 1; if (npp > 2000) npp = 2000; _nanosPerPass = npp;

  // default 10 bit analog read resolution (0 to 1023)
  analogReadResolution(10);
}

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
float HAL_MCU_Temperature(void) {
  int Tpin=70;
  // delta of -1.715 mV/C where 25C measures 719 mV
  float v=(analogRead(Tpin)/1024.0)*3.3;
  float t=(-(v-0.719)/0.001715)+25.0;
  return t;
}

//--------------------------------------------------------------------------------------------------
// for using the DAC as a digital output on Teensy3.6 A21=66 A22=67
#define digitalWrite(x,y) { if (x==66 || x==67) { if ((y)==LOW) analogWrite(x,0); else analogWrite(x,255); } else digitalWrite(x,y); }
