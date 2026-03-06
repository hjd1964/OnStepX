// Nv.cpp
#include "Nv.h"

#include <Wire.h>

#include "../tasks/OnTask.h"

// -------------------- Backend type selection (per driver) --------------------

#if (NV_DRIVER == NV_EEPROM)

  #include "device/EepromArduino.h"
  using NvBackend = NvDeviceEepromArduino;

#elif (NV_DRIVER == NV_ESP)

  #include "device/EepromEmuEsp.h"
  using NvBackend = NvDeviceEepromEmuEsp;

#elif (NV_DRIVER == NV_M0)

  #include "device/EepromEmuM0.h"
  using NvBackend = NvDeviceEepromEmuM0;

#elif (NV_DRIVER == NV_2416)

  #include "device/24xxI2C.h"
  using NvBackend = NvDevice24xxI2C<2048u>;

#elif (NV_DRIVER == NV_2432)

  #include "device/24xxI2C.h"
  using NvBackend = NvDevice24xxI2C<4096u>;

#elif (NV_DRIVER == NV_2464)

  #include "device/24xxI2C.h"
  using NvBackend = NvDevice24xxI2C<8192u>;

#elif (NV_DRIVER == NV_24128)

  #include "device/24xxI2C.h"
  using NvBackend = NvDevice24xxI2C<16384u>;

#elif (NV_DRIVER == NV_24256)

  #include "device/24xxI2C.h"
  using NvBackend = NvDevice24xxI2C<32768u>;

#elif (NV_DRIVER == NV_AT24C32)

  #include "device/24xxI2C.h"
  using NvBackend = NvDevice24xxI2C<4096u>;

#elif (NV_DRIVER == NV_MB85RC32)

  #include "device/Mb85rcI2C.h"
  using NvBackend = NvDeviceMb85rcI2C<4096u>;

#elif (NV_DRIVER == NV_MB85RC64)

  #include "device/Mb85rcI2C.h"
  using NvBackend = NvDeviceMb85rcI2C<8192u>;

#elif (NV_DRIVER == NV_MB85RC256)

  #include "device/Mb85rcI2C.h"
  using NvBackend = NvDeviceMb85rcI2C<32768u>;

#elif (NV_DRIVER == OFF)

  #include "device/Null.h"
  using NvBackend = NvDeviceNull;

#else
  #error "Unsupported NV_DRIVER"
#endif


// -------------------- Construct backend instance --------------------
// I2C-backed drivers require ctor(TwoWire&, addr). Others default construct.
// HAL_WIRE and NV_I2C_ADDRESS are expected to be defined in NvsConfig.h for I2C drivers.

#ifdef NV_DEVICE_I2C

  static NvBackend nvDevice(HAL_WIRE, NV_I2C_ADDRESS);

#else

  static NvBackend nvDevice;

#endif

// -------------------- Optional shim + active pointer --------------------

#if (NV_SHIM == NV_SHIM_CACHED)
  #include "device/ShimCached.h"
  static NvShimCached nvShimCached(nvDevice);
  static NvDevice* nvActive = &nvShimCached;
  constexpr uint32_t NV_SVC_MS = 20;

#elif (NV_SHIM == NV_SHIM_DELAYED)
  #include "device/ShimDelayedCommit.h"
  static NvShimDelayedCommit nvShimDelayedCommit(nvDevice, NV_WAIT_MS);
  static NvDevice* nvActive = &nvShimDelayedCommit;
  constexpr uint32_t NV_SVC_MS = 100;

#else
  static NvDevice* nvActive = &nvDevice;
  constexpr uint32_t NV_SVC_MS = 100;
#endif


// -------------------- Volume instance --------------------
// Bound to nvActive in NvSystem::init(). Partitions/format/mount happen higher up.

static NvVolume nvVolume;

// -------------------- NvSystem singleton --------------------

static NvSystem nvSystem;

NvSystem& nv() { return nvSystem; }


// -------------------- NvSystem methods --------------------

// task callback
void nvServices() {
  nv().poll();
}

bool NvSystem::init(uint8_t priorityLevel) {
  if (ok_) return true;

  #if (NV_DRIVER == OFF)
    // If OFF means “NV unavailable”, exit cleanly.
    return false;
  #endif

  // Init active layer (shim init may call down into device init)
  if (!nvActive->init()) return false;

  // Bind volume to active device/shim (does not mount/format)
  const NvVolume::Status st = nvVolume.init(*nvActive);
  if (st != NvVolume::Status::Ok) return false;

  uint32_t pollingRate = NV_SVC_MS;

  // High endurance means FRAM and we double the polling rate since there is no write wait to worry about
  if (device().endurance() == NvDevice::Endurance::High) { pollingRate /= 2U; }

  if (priorityLevel > 7) priorityLevel = 7;

  // start system service task
  VF("MSG: Nv, start service task (rate "); V(pollingRate); VF("ms priority "); V((int)priorityLevel); VF(")... ");
  // add task for system services, runs at pollingRate intervals
  if (tasks.add(pollingRate, 0, true, priorityLevel, nvServices, "SvcNv")) { VLF("success"); } else { VLF("FAILED!"); return false; }

  ok_ = true;
  return true;
}

void NvSystem::wipe() {
  static const uint8_t pattern[16] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };
  int16_t sizeBlocks = nv().device().sizeBytes()/16;

  VF("MSG: Setup, NV (EEPROM/FRAM/Flash/etc.) wiping device .");

  for (int i = 0; i < sizeBlocks; i++) {
    nv().device().writeBlock16(i, pattern);
    if (i % 128 == 0) { VF("."); }
  }

  nv().device().commit();
  VLF(" done.");
  VLF("");
  VLF("MSG: Setup, !!!!!!! REMEMBER TO DISABLE THIS FEATURE IN Config.h !!!!!!!");
  VLF("");
}

void NvSystem::poll() {
  if (!ok_) return;
  if (!canService_()) return;
  nvActive->poll();
}

NvDevice& NvSystem::device() { return nvDevice; }
NvDevice& NvSystem::active() { return *nvActive; }

NvVolume& NvSystem::volume() { return nvVolume; }
