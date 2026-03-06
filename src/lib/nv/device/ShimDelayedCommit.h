// ShimDelayedCommit.h
#pragma once

#include "NvDeviceBase.h"

#include <stdint.h>

// A tiny wrapper for backends that already buffer writes internally (ESP/STM32 EEPROM emu).
// It batches commit() calls with a quiet-period timer, but does NOT add a second cache.
//
// Behavior:
// - write() passes through to backend.write() and schedules a commit for (now + delayMs).
// - poll() calls backend.poll() then triggers commit when quiet expires or hard deadline expires.
// - commitDone() is unified: "no pending scheduled commit AND backend reports done".
//
class NvShimDelayedCommit : public NvDevice {
public:
  explicit NvShimDelayedCommit(NvDevice& backendRef, uint32_t delayMs) : backend(backendRef) { this->delayMs = delayMs; }

  bool init() override {
    if (!backend.init()) return false;

    // This wrapper is only meaningful for commit-capable backends.
    if (!backend.hasCommit()) return false;

    capacityBytes = backend.sizeBytes();
    if (capacityBytes == 0 || capacityBytes > 65536u) return false;

    VLF("MSG: NvDevice, delayed commit active");

    return true;
  }

  uint32_t sizeBytes() const override { return capacityBytes; }
  Endurance endurance() const override { return backend.endurance(); }
  bool isReadable() const override { return backend.isReadable(); }
  bool isWritable() const override { return backend.isWritable(); }

  // Quiet period (ms) after last write before committing.
  // delay=0 means commit on next poll.
  void setDelayMs(uint32_t ms) { delayMs = ms; }

  // Hard bound: commit no later than this many ms after the first write in a burst.
  // maxLatencyMs=0 disables (pure debounce behavior).
  // defaults to 3 hours
  void setMaxLatencyMs(uint32_t ms) { maxLatencyMs = ms; }

  IoStatus read(uint16_t addr, void* dst, uint16_t len) override {
    return backend.read(addr, dst, len);
  }

  IoStatus write(uint16_t addr, const void* src, uint16_t len) override {
    if (!isWritable()) return IoStatus::ReadOnly;

    IoStatus st = backend.write(addr, src, len);
    if (st != IoStatus::Ok) { DLF("WRN: write failed!"); return st; }

    const uint32_t now = millis();

    // If this is the first write since we were idle, start the hard deadline window.
    if (!commitScheduled && (maxLatencyMs != 0)) {
      commitHardDueMs = now + maxLatencyMs;
    }

    // Schedule commit after quiet period (debounce).
    commitScheduled = true;
    commitDueMs = now + delayMs;
    return IoStatus::Ok;
  }

  bool hasCommit() const override { return backend.hasCommit(); }

  // Explicit commit barrier: commit immediately and clear any pending schedule.
  IoStatus commit() override {
    commitScheduled = false;
    commitDueMs = 0;
    commitHardDueMs = 0;
    return backend.commit();
  }

  // Unified "done" concept: no scheduled commit pending AND backend reports done.
  bool commitDone() const override {
    if (commitScheduled) return false;
    return backend.commitDone();
  }

  // Optional: expose whether a commit is scheduled (useful for debugging).
  bool commitPending() const { return commitScheduled; }

  void poll() override {
    backend.poll();
    if (!commitScheduled) { return; }

    const uint32_t now = millis();

    const bool quietExpired = ((int32_t)(now - commitDueMs) >= 0);
    const bool hardExpired  = (maxLatencyMs != 0) && ((int32_t)(now - commitHardDueMs) >= 0);

    if (quietExpired || hardExpired) {
      (void)backend.commit();
      commitScheduled = false;
      commitDueMs = 0;
      commitHardDueMs = 0;
    }
  }

private:
  NvDevice&  backend;
  uint32_t  capacityBytes = 0;

  bool      commitScheduled = false;

  uint32_t  delayMs = 0;
  uint32_t  commitDueMs = 0;

  uint32_t  maxLatencyMs = 10800000;  // 3 hours in ms = 3*60*60*1000
  uint32_t  commitHardDueMs = 0;      // set on first write of a burst
};
