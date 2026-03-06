// -----------------------------------------------------------------------------------
// Nv.h
//
// Non-volatile storage “front door” for the project.
//
// What you get
// - A single global NV system (`nv()`) that owns the selected storage backend
//   (real EEPROM, FRAM, or flash/EEPROM-emulation), plus optional shims.
// - A volume manager (`NvVolume`) on top of that device, which provides named partitions.
// - A small KV store (`nv().kv()`) for persisting settings/structures by name.
//
// Typical usage pattern
//   1) nv().init();                       // initialize backend + start background service
//   2) nv().volume().mount(...);          // mount/format volume as needed (elsewhere)
//   3) nv().kv().init(nv().volume(),"KV");// bind KV store to the "KV" partition
//   4) nv().kv().getOrInit("NAME", obj);  // load settings or write defaults
//
// Selecting KV16 vs KV32
// - This header can build with either KV implementation, chosen at compile time by
//   NV_USE_KV32 in NvConfig.h.
// - KV16 (NvKvPartition16) is the compact option for small EEPROM/FRAM.
// - KV32 (NvKvPartition32) supports larger entries/files and uses a wider key.
// - Call sites should NOT care which is used: prefer `KvPartition::Status::Ok` and
//   access the store through `nv().kv()`.
//
// Background servicing
// - Background servicing is automatic using lib/OnTask, additionally
//   setGate() can temporarily suspend this servicing when the system is busy.
//
// Notes for new users
// - If NV_DRIVER is OFF, nv().init() will fail (storage disabled).
// - This layer is intentionally small and deterministic: fixed-size partition,
//   overwrite-in-place updates, and a rebuild-on-boot allocation scan.
// -----------------------------------------------------------------------------------
#pragma once

#include "../../Common.h"

#include "device/NvDeviceBase.h"

#include "NvConfig.h"
#include "NvVolume.h"

#if NV_USE_KV32 == ON
  #include "NvKvPartition32.h"
  #define KvPartition KvPartition32
#else
  #include "NvKvPartition16.h"
  #define KvPartition KvPartition16
#endif

class NvSystem {
public:
  // Initializes backend (and optional shim), then binds NvVolume to nvActive.
  bool init(uint8_t priorityLevel = 7);

  // Overwrite the entire associated device with 0xFF
  void wipe(); 

  // Gate on xBusy status
  void setGate(bool *gate) { gate_ = gate; }

  bool ok() const { return ok_; }

  // Raw backend device (selected by NV_DRIVER)
  NvDevice& device();

  // Device you should use everywhere (raw or shimmed)
  NvDevice& active();

  // Volume manager bound to active()
  NvVolume& volume();

  void poll();

  #if NV_USE_KV32 == ON
    KvPartition32& kv() { return kv_; }
  #else
    KvPartition16& kv() { return kv_; }
  #endif

private:
  bool canService_() const { return (gate_ == nullptr) ? true : !(*gate_); }

  #if NV_USE_KV32 == ON
    KvPartition32 kv_;
  #else
    KvPartition16 kv_;
  #endif

  bool ok_ = false;
  bool *gate_ = nullptr;
};

NvSystem& nv();
