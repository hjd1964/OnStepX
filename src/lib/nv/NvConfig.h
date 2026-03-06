// NvConfig.h
#pragma once
#include <stdint.h>

// Provide driver-dependent defaults for:
//   - E2END (sizeBytes = E2END + 1)
//   - NV_I2C_ADDRESS (7-bit I2C addr)  [I2C devices only]
//   - NV_WAIT_MS (only meaningful for emulated EEPROM backends)
//

#ifndef NV_DRIVER
  #error "NV_DRIVER must be defined before including NvConfig.h"
#endif

// OFF = KvPartition16 (16B blocks / v1), ON = KvPartition32 (virtual 32B blocks / v2)
#ifndef NV_USE_KV32
  #define NV_USE_KV32 OFF
#endif

// NV_SHIM options
#define NV_SHIM_OFF     0
#define NV_SHIM_CACHED  1
#define NV_SHIM_DELAYED 2

// If your VM requires size to be multiple-of-16, enforce it (default ON).
#ifndef NVS_REQUIRE_SIZE_MULTIPLE_OF_16
  #define NVS_REQUIRE_SIZE_MULTIPLE_OF_16 0
#endif

// ---- Default I2C address (7-bit) ------------------------------------------------
// Only meaningful for I2C devices. User may override by defining NV_I2C_ADDRESS earlier.
#if (NV_DRIVER == NV_2416) || (NV_DRIVER == NV_2432) || (NV_DRIVER == NV_2464) || \
    (NV_DRIVER == NV_24128) || (NV_DRIVER == NV_24256) || (NV_DRIVER == NV_AT24C32) || \
    (NV_DRIVER == NV_MB85RC32) || (NV_DRIVER == NV_MB85RC64) || (NV_DRIVER == NV_MB85RC256)

  #ifndef NV_I2C_ADDRESS
    #if (NV_DRIVER == NV_AT24C32)
      #define NV_I2C_ADDRESS 0x57  // common ZS042 module
    #else
      #define NV_I2C_ADDRESS 0x50
    #endif
  #endif

  #ifndef NV_SHIM
    #define NV_SHIM NV_SHIM_CACHED
  #endif

  #ifndef NV_DEVICE_I2C
    #define NV_DEVICE_I2C
  #endif

#endif

// ---- Default shim selection for emulated EEPROM backends -----------------------
#if (NV_DRIVER == NV_ESP) || (NV_DRIVER == NV_M0)
  #ifndef NV_SHIM
    #define NV_SHIM NV_SHIM_DELAYED
  #endif
#endif

// ---- Default shim selection for emulated EEPROM backends -----------------------
#if NV_DRIVER == NV_EEPROM
  #ifndef NV_SHIM
    #define NV_SHIM NV_SHIM_CACHED
  #endif
#endif

// ---- Sanity checks --------------------------------------------------------------
// Your system constraint: <= 65536 bytes
#if ((E2END + 1u) > 65536u)
  #error "E2END+1 exceeds 65536 bytes"
#endif

#if NVS_REQUIRE_SIZE_MULTIPLE_OF_16
  #if (((E2END + 1u) & 15u) != 0)
    #error "NV size (E2END+1) must be a multiple of 16 bytes"
  #endif
#endif

#ifndef NV_SHIM
  #define NV_SHIM NV_SHIM_OFF
#endif

#if (NV_SHIM == NV_SHIM_DELAYED)
  #ifndef NV_WAIT_MS
    #define NV_WAIT_MS 5000u
  #endif
#endif
