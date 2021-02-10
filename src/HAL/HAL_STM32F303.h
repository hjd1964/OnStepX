// Platform setup ------------------------------------------------------------------------------------

// We define a more generic symbol, in case more STM32 boards based on different lines are supported
#define __ARM_STM32__

#define HAL_FAST_PROCESSOR

#define HAL_MAXRATE_LOWER_LIMIT 16   // Lower limit (fastest) step rate in uS (in SQW mode) assumes optimization set to Fastest (-O3)
#define HAL_PULSE_WIDTH         1900 // Width of step pulse

#include <HardwareTimer.h>

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------

// SerialA is manidatory
#define SERIAL_A Serial1
// SerialB is optional
#if SERIAL_B_BAUD_DEFAULT != OFF
  HardwareSerial Serial2(PB11, PB10);
  #define SERIAL_B Serial2
#endif
#if SERIAL_C_BAUD_DEFAULT != OFF
  #error "Configuration (Config.h): SerialC isn't supported, disable this option."
#endif

// New symbol for the default I2C port ---------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ------------------------------------------------------------------------------
#undef E2END
#ifdef NV_DEFAULT
  #include "NV/NV_I2C_EEPROM_24XX_C.h" // Defaults to 0x57 and 4KB
#endif

//----------------------------------------------------------------------------------------------------
// Nanoseconds delay function
/*
unsigned int _nanosPerPass=1;
void delayNanoseconds(unsigned int n) {
  unsigned int np=(n/_nanosPerPass);
  for (unsigned int i=0; i<np; i++) { __asm__ volatile ("nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t"); }
}
*/

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#define HAL_INIT { uint32_t startTime, npp; startTime=micros(); delayNanoseconds(65535); npp=micros(); npp=((int32_t)(npp-startTime)*1000)/63335; if (npp<1) npp=1; if (npp>2000) npp=2000; _nanosPerPass=npp; analogWriteResolution(8); }

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define HAL_TEMP ( -999 )


// Allow MCU reset -----------------------------------------------------------------------------------
#define HAL_RESET NVIC_SystemReset()
