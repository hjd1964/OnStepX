// -------------------------------------------------------------------------------------------------
// NvIvPartition.h
//
// Index/Value partition
//
// Provides a simple "flat byte array" API over a volume partition, plus typed helpers.
//
// - Addressing: partition-local offsets [0 .. partitionByteSize-1].
// - Alignment/RMW: This layer does NOT perform block-based RMW.
//   Any underlying device constraints (e.g., 16-byte aligned/multiple-of-16 I/O) must be
//   satisfied/handled by NvVolume and/or the selected backend.
// - Endianness: typed helpers store the host representation as raw bytes.
//   (If you need a stable cross-platform format, add explicit LE/BE conversions.)
//
// This layer does NOT add its own CRC. If you want integrity, store CRC fields in your payload
// or use the KV partition for "file-like" items with CRC.
//
// -------------------------------------------------------------------------------------------------
#pragma once

#include <stdint.h>
#include <string.h>

#include "NvVolume.h"

class IvPartition {
public:
  using NvVolStatus = NvVolume::Status;
  static constexpr uint16_t kBlockSize = NvVolume::kBlockSize; // 16

  IvPartition() = default;

  // Bind to a mounted volume + partition index.
  bool init(NvVolume& vol, uint8_t partIdx);

  // Bind by ASCII partition name (requires valid SB1 names block).
  bool init(NvVolume& vol, const char* partitionAscii);

  // Bind by partition name16 tag (requires valid SB1 names block).
  bool init(NvVolume& vol, uint16_t partitionName16);

  bool isReady() const { return (vol_ != nullptr) && ready_; }
  uint8_t partIndex() const { return partIdx_; }

  uint16_t sizeBytes() const { return partBytes_; }
  uint16_t sizeBlocks() const { return (uint16_t)(partBytes_ / kBlockSize); }

  // Explicit block ops (partition-local block index).
  // These are useful for callers that already operate on block granularity.
  NvVolStatus readBlock(uint16_t partBlock, uint8_t out16[kBlockSize]);
  NvVolStatus writeBlock(uint16_t partBlock, const uint8_t in16[kBlockSize]);

  // Raw byte access (pass-through; NvVolume/backend handles any alignment/device constraints).
  NvVolStatus readBytes(uint16_t offset, void* dst, uint16_t len);
  NvVolStatus writeBytes(uint16_t offset, const void* src, uint16_t len);

  // Zero-fill a range (implemented via small chunked writes).
  NvVolStatus clearBytes(uint16_t offset, uint16_t len);

  // Typed helpers
  template <typename T>
  NvVolStatus read(uint16_t offset, T& outVal) {
    return readBytes(offset, &outVal, (uint16_t)sizeof(T));
  }

  template <typename T>
  NvVolStatus write(uint16_t offset, const T& val) {
    return writeBytes(offset, &val, (uint16_t)sizeof(T));
  }

  template <typename T>
  NvVolStatus readArray(uint16_t offset, T* outVals, uint16_t count) {
    return readBytes(offset, outVals, (uint16_t)(sizeof(T) * count));
  }

  template <typename T>
  NvVolStatus writeArray(uint16_t offset, const T* vals, uint16_t count) {
    return writeBytes(offset, vals, (uint16_t)(sizeof(T) * count));
  }

private:
  NvVolume* vol_ = nullptr;
  uint8_t partIdx_ = 0;
  bool ready_ = false;

  uint16_t partBytes_ = 0;
};
