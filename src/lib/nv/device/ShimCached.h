// ShimCached.h
#pragma once

#include "NvDeviceBase.h"

#include <Arduino.h>
#include <new>
#include <string.h>

// NvShimCached
//
// Cache wrapper (old intent + fast):
//
// - Two 1-bit-per-byte bitsets:
//     dirtyReadBits[i]  == 1 => cache[i] invalid; must read backend before use
//     dirtyWriteBits[i] == 1 => cache[i] differs; must write back
//
// - poll() scans a small window (20 bytes) each call
// - On dirty write:
//     * if writeback delay elapsed: prefer 16B page-base flush IF page is fully valid in cache
//       AND page has any dirty-write bytes; otherwise fall back to 1-byte write at scanIndex.
// - On dirty read: fetch 1 byte into cache
// - "Clean" is determined by a full wrap scan pass (cacheClean/cacheCleanThisPass)
// - Backends may "pretend" they support 16B page writes: we always call writeBlock16()
//   for the 16B path; base-class default emulates it with write(...,16).

#ifndef NVS_ALLOW_CACHED_ON_COMMIT_BACKENDS
  // 0 = refuse to wrap flash/EEPROM emulation layers that already cache+commit internally.
  #define NVS_ALLOW_CACHED_ON_COMMIT_BACKENDS 0
#endif

// 1-bit-per-byte bitset helpers
static inline bool bitGet_(const uint8_t* bits, uint32_t bitIndex) {
  return (bits[bitIndex >> 3] >> (bitIndex & 7)) & 1u;
}
static inline void bitSet_(uint8_t* bits, uint32_t bitIndex, bool value) {
  const uint8_t mask = (uint8_t)(1u << (bitIndex & 7));
  uint8_t& b = bits[bitIndex >> 3];
  b = value ? (uint8_t)(b | mask) : (uint8_t)(b & (uint8_t)~mask);
}

class NvShimCached : public NvDevice {
public:
  explicit NvShimCached(NvDevice& inner) : backend(inner) {}
  ~NvShimCached() override { cleanup_(); }

  bool init() override {
    cleanup_();

    if (!backend.init()) return false;

    #if !NVS_ALLOW_CACHED_ON_COMMIT_BACKENDS
      // Avoid wrapping flash/EEPROM emulation layers that already do their own caching/commit.
      if (backend.hasCommit()) return false;
    #endif

    capacityBytes = backend.sizeBytes();
    if (capacityBytes == 0) return false;

    // We assume upstream respects "aligned usable size (multiple of 16)" contract.
    if ((capacityBytes & 0x0Fu) != 0u) return false;

    // NOTE: public read/write use uint16_t address, so > 64K is not representable.
    if (capacityBytes > 65536u) return false;

    cache = new (std::nothrow) uint8_t[capacityBytes];
    if (!cache) { cleanup_(); return false; }

    bitsetBytes = (uint16_t)((capacityBytes + 7u) / 8u);

    dirtyReadBits  = new (std::nothrow) uint8_t[bitsetBytes];
    dirtyWriteBits = new (std::nothrow) uint8_t[bitsetBytes];
    if (!dirtyReadBits || !dirtyWriteBits) { cleanup_(); return false; }

    // Heuristic: scan ~1 flag-byte per 512B of device, clamped [4..32].
    uint32_t count = capacityBytes >> 9;
    if (count < 4) count = 4;
    if (count > 32) count = 32;
    flagScanCount = (uint8_t)count;

    // Semantics:
    //   dirtyReadBits[i]  == 1 -> cache[i] invalid, must fetch before use
    //   dirtyWriteBits[i] == 1 -> cache[i] differs, must write back
    memset(dirtyReadBits,  0xFF, bitsetBytes); // all bytes invalid initially
    memset(dirtyWriteBits, 0x00, bitsetBytes); // nothing dirty initially

    scanIndex = (uint32_t)-1;
    pendingWriteCount = 0;

    writebackDelayMs = 0;
    writebackReadyMs = 0;

    // cache isn't "clean" because read-cache is invalid everywhere
    cacheClean = false;
    cacheCleanThisPass = true;

    VLF("MSG: NvDevice, memory cache active");
    return true;
  }

  // ---- Capabilities / identity pass-through ----

  uint32_t sizeBytes() const override { return capacityBytes; }
  Endurance endurance() const override { return backend.endurance(); }
  bool isReadable() const override { return backend.isReadable(); }
  bool isWritable() const override { return backend.isWritable(); }

  // ---- Delay control (old: commitReadyTimeMs = now + waitMs) ----

  void setWritebackDelayMs(uint32_t ms) { writebackDelayMs = ms; }

  // ---- Required primitives ----

  IoStatus read(uint16_t addr, void* dst, uint16_t len) override {
    if (!dst) return IoStatus::OutOfRange;
    if ((uint32_t)addr + (uint32_t)len > capacityBytes) return IoStatus::OutOfRange;

    uint8_t* out = (uint8_t*)dst;
    for (uint16_t i = 0; i < len; i++) {
      uint8_t v = 0;
      IoStatus st = readByteCached_((uint32_t)addr + i, v);
      if (st != IoStatus::Ok) return st;
      out[i] = v;
    }
    return IoStatus::Ok;
  }

  IoStatus write(uint16_t addr, const void* src, uint16_t len) override {
    if (!src) return IoStatus::OutOfRange;
    if ((uint32_t)addr + (uint32_t)len > capacityBytes) return IoStatus::OutOfRange;
    if (!isWritable()) return IoStatus::ReadOnly;

    const uint8_t* in = (const uint8_t*)src;
    for (uint16_t i = 0; i < len; i++) {
      // Cache staging cannot fail here (bounds already checked).
      (void)writeByteCached_((uint32_t)addr + i, in[i]);
    }

    // Set delay gate once per call (avoid per-byte millis()).
    writebackReadyMs = millis() + writebackDelayMs;
    return IoStatus::Ok;
  }

  // Commit API is irrelevant when wrapping EEPROM/FRAM-like devices; keep consistent.
  bool hasCommit() const override { return false; }
  IoStatus commit() override { return IoStatus::Ok; }

  // "Done" for this wrapper: no dirty bytes waiting to be written back.
  // (We keep an incremental counter; if you ever need "old exact recount", add a debug validator.)
  bool commitDone() const override { return pendingWriteCount == 0; }

  // ---- Background servicing ----

  void poll() override {
    backend.poll();
    if (capacityBytes == 0) return;

    // cacheClean, return.
    if (cacheClean) return;

    const bool canWriteback =
      (writebackDelayMs == 0) || ((int32_t)(millis() - writebackReadyMs) >= 0);

    bool doWrite = false;
    bool doRead  = false;

    // rapidly move ahead through clean cache
    const uint32_t flagBytes = bitsetBytes;
    uint32_t flagIndex = (((scanIndex + 1u) >= capacityBytes) ? 0u : (scanIndex + 1u)) >> 3;

    for (uint8_t i = 0; i < flagScanCount; i++) {
      if ((dirtyWriteBits[flagIndex] | dirtyReadBits[flagIndex]) != 0u) break;

      flagIndex++;
      if (flagIndex >= flagBytes) { scanIndex = capacityBytes - 1u; break; }

      scanIndex = (flagIndex << 3) - 1u;
    }

    // Scan small window each call
    for (uint8_t step = 0; step < 16; step++) {
      scanIndex++;
      if (scanIndex >= capacityBytes) {
        if (cacheCleanThisPass) cacheClean = true;
        scanIndex = 0;
        cacheCleanThisPass = true;
      }

      const bool w = bitGet_(dirtyWriteBits, scanIndex);
      const bool r = bitGet_(dirtyReadBits,  scanIndex);

      if (w) {
        // Work exists, so this pass is not clean.
        cacheCleanThisPass = false;

        // If writeback delay not elapsed, keep scanning (but never mark clean).
        if (!canWriteback) continue;

        doWrite = true;
        break;
      }

      if (r) {
        cacheCleanThisPass = false;
        doRead = true;
        break;
      }
    }

    if (doWrite) {
      flushOneWriteFast_(scanIndex);   // page-base flush preferred
      return;
    }

    if (doRead) {
      uint8_t tmp = 0;
      (void)readByteCached_(scanIndex, tmp);
      return;
    }
  }

private:
  void cleanup_() {
    delete[] cache; cache = nullptr;
    delete[] dirtyReadBits; dirtyReadBits = nullptr;
    delete[] dirtyWriteBits; dirtyWriteBits = nullptr;

    capacityBytes = 0;
    bitsetBytes = 0;
    scanIndex = 0;

    writebackDelayMs = 0;
    writebackReadyMs = 0;

    pendingWriteCount = 0;

    cacheClean = true;
    cacheCleanThisPass = true;
  }

  // ---- Core byte cache ops ----

  IoStatus readByteCached_(uint32_t address, uint8_t& out) {
    if (address >= capacityBytes) { out = 0; return IoStatus::OutOfRange; }

    if (bitGet_(dirtyReadBits, address)) {
      uint8_t v = 0;
      IoStatus st = backend.read((uint16_t)address, &v, 1);
      if (st != IoStatus::Ok) { out = 0; return st; }

      cache[address] = v;
      bitSet_(dirtyReadBits, address, false);
    }

    out = cache[address];
    return IoStatus::Ok;
  }

  // Cache staging write; always succeeds if address is in range.
  // If the backend is temporarily NotReady and we can't read the old value,
  // we still stage the write (dirty) and rely on poll() to flush later.
  IoStatus writeByteCached_(uint32_t address, uint8_t value) {
    if (address >= capacityBytes) return IoStatus::OutOfRange;

    // Any write makes cache "not clean".
    cacheClean = false;
    cacheCleanThisPass = false;

    // Read old value (through cache) to avoid marking dirty if same.
    uint8_t oldValue = 0;
    IoStatus stOld = readByteCached_(address, oldValue);
    if (stOld == IoStatus::Ok && oldValue == value) return IoStatus::Ok;

    // If stOld != Ok, proceed anyway: stage new value and mark dirty.
    cache[address] = value;

    // Mark write dirty (only count 0->1 transitions)
    if (!bitGet_(dirtyWriteBits, address)) {
      bitSet_(dirtyWriteBits, address, true);
      pendingWriteCount++;
    }

    // Ensure subsequent reads never overwrite our cached value.
    bitSet_(dirtyReadBits, address, false);

    return IoStatus::Ok;
  }

  // ---- Old intent + fast writeback ----
  //
  // When a dirty byte is found at idx, prefer flushing the *page base* (idx&~0x0F)
  // via writeBlock16 if:
  //   - the full page is valid in cache (dirtyRead == 0 for all 16 bytes)
  //   - and the page contains any dirtyWrite bytes
  // Otherwise fall back to a 1-byte write at idx.

  void flushOneWriteFast_(uint32_t idx) {
    if (!backend.isWritable()) return;
    if (idx >= capacityBytes) return;

    const uint32_t base = idx & ~0x0Fu;

    if (base + 16u <= capacityBytes) {
      // base is 16B aligned => also 8-bit aligned in bitset
      const uint32_t bi = base >> 3; // base / 8

      // Page is fully valid if both corresponding bitset bytes are 0.
      const bool pageValid = (dirtyReadBits[bi] == 0u) && (dirtyReadBits[bi + 1] == 0u);

      if (pageValid) {
        // Only flush page if any byte in it is dirty-write.
        const bool anyDirtyW = (dirtyWriteBits[bi] != 0u) || (dirtyWriteBits[bi + 1] != 0u);

        if (anyDirtyW) {
          const uint16_t blockIdx = (uint16_t)(base >> 4);
          IoStatus st = backend.writeBlock16(blockIdx, &cache[base]);
          if (st != IoStatus::Ok) return; // NotReady/IoError => leave bits set, try later

          // Clear dirty-write bits for the page; adjust pending count accurately.
          for (uint8_t k = 0; k < 16; k++) {
            const uint32_t a = base + k;
            if (bitGet_(dirtyWriteBits, a)) {
              bitSet_(dirtyWriteBits, a, false);
              if (pendingWriteCount) pendingWriteCount--;
            }
          }
          return;
        }
      }
    }

    // Fallback: 1-byte write at idx (old behavior fallback)
    if (bitGet_(dirtyReadBits, idx)) return; // cache byte isn't valid; don't write garbage

    IoStatus st = backend.write((uint16_t)idx, &cache[idx], 1);
    if (st != IoStatus::Ok) return;

    if (bitGet_(dirtyWriteBits, idx)) {
      bitSet_(dirtyWriteBits, idx, false);
      if (pendingWriteCount) pendingWriteCount--;
    }
  }

private:
  // ---- Member state ----
  NvDevice& backend;

  uint32_t capacityBytes = 0;
  uint8_t* cache = nullptr;

  uint8_t* dirtyReadBits  = nullptr;
  uint8_t* dirtyWriteBits = nullptr;
  uint16_t bitsetBytes = 0;

  uint8_t flagScanCount = 4;
  uint32_t scanIndex = 0;

  uint32_t writebackDelayMs = 0;
  uint32_t writebackReadyMs = 0;

  uint32_t pendingWriteCount = 0;

  bool cacheClean = true;
  bool cacheCleanThisPass = true;
};
