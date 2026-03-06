// -------------------------------------------------------------------------------------------------
// NvDeviceNull.h
//
// Null NV backend (no persistence).
//
// - Reads return 0xFF bytes (erased-state semantics).
// - Writes succeed but are discarded.
// - Useful for bring-up, unit tests, or builds without NV hardware.
//
// Contract:
// - sizeBytes() reports a usable aligned size (multiple of 16).
// - read/write are flat device-relative addressing with 16-bit addr/len.
//
// -------------------------------------------------------------------------------------------------
#pragma once

#include <stdint.h>
#include <string.h>

#include "NvDeviceBase.h"

class NvDeviceNull final : public NvDevice {
public:
  // Ensure the reported size is a multiple of 16 (minimum 16).
  explicit NvDeviceNull(uint32_t sizeBytes = kDefaultSizeBytes)
    : sizeBytes_(align16_(sizeBytes)) {}

  void setSizeBytes(uint32_t sizeBytes) { sizeBytes_ = align16_(sizeBytes); }

  // ---- NvDevice ----
  bool init() override { return true; }

  uint32_t sizeBytes() const override { return sizeBytes_; }

  IoStatus read(uint16_t addr, void* dst, uint16_t len) override {
    if (len == 0) return IoStatus::Ok;
    if (!dst) return IoStatus::IoError;

    const uint32_t end = (uint32_t)addr + (uint32_t)len;
    if (end > sizeBytes_) return IoStatus::OutOfRange;

    memset(dst, 0xFF, (size_t)len);
    return IoStatus::Ok;
  }

  IoStatus write(uint16_t addr, const void* src, uint16_t len) override {
    if (len == 0) return IoStatus::Ok;
    if (!src) return IoStatus::IoError;

    const uint32_t end = (uint32_t)addr + (uint32_t)len;
    if (end > sizeBytes_) return IoStatus::OutOfRange;

    // Discard writes.
    return IoStatus::Ok;
  }

  // Advertise block16 accelerators as supported. They will call through
  // the base default implementations (which call read/write).
  bool supportsWriteBlock16() const override { return true; }
  bool supportsReadBlock16()  const override { return true; }

private:
  static constexpr uint32_t kDefaultSizeBytes = 64u * 1024u;

  static uint32_t align16_(uint32_t n) {
    if (n < 16u) n = 16u;
    return (n + 15u) & ~15u;
  }

  uint32_t sizeBytes_ = kDefaultSizeBytes;
};
