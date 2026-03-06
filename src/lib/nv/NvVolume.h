// NvVolume.h
#pragma once
#include <stdint.h>
#include <string.h>

#include "device/NvDeviceBase.h"

// Superblocks v1:
// - block size = 16 bytes
// - block 0 = SB0 (layout)
// - block 1 = SB1 (partition name tags, optional)
// - partitions start at block >= 2
class NvVolume {
public:
  enum class Status : uint8_t {
    Ok = 0,
    NotReady,
    BadGeometry,
    Unaligned,
    OutOfRange,
    ReadOnly,
    IoError,
    Unformatted,
  };

  static constexpr uint16_t kBlockSize   = 16;
  static constexpr uint16_t kMaxBlocks   = 4096; // 12-bit block indices
  static constexpr uint8_t  kMaxParts    = 7;
  static constexpr uint8_t  kSbVersionV1 = 1;

  // SB0 flags8:
  // bit1 LAST_IS_END_MARKER
  // bits2..3 reserved (0)
  // bits4..7 version (v1=1)
  static constexpr uint8_t kFlagLastIsEndMarker = 0x02;
  static constexpr uint8_t kFlagReservedMask    = 0x0C; // bits2..3 must be 0
  static constexpr uint8_t kFlagVersionShift    = 4;

  struct Part {
    uint16_t beginBlock = 0; // inclusive
    uint16_t endBlock   = 0; // exclusive
  };

  struct PartitionView16 {
    uint8_t  idx = 0xFF;
    uint16_t beginBlock = 0;
    uint16_t endBlock   = 0;
    uint16_t blockCount() const { return (uint16_t)(endBlock - beginBlock); }
    uint16_t byteCount()  const { return (uint16_t)(blockCount() << 4); }
  };

  NvVolume() = default;

  // ---------------- device binding / identity ----------------
  Status init(NvDevice& dev);

  // Name tags (hashing lives in Crc16 now)
  static uint16_t computeVolumeName16(const char* projectAscii, uint16_t storageVersion);
  static uint16_t computePartName16(const char* partitionAscii);

  // ---------------- mount / format ----------------
  // Mount validates SB0 CRC/version/layout and loads partitions.
  // SB1 (partition names) is optional; if its CRC fails, names are treated as absent.
  Status mount();
  Status mount(const char* projectAscii, uint16_t storageVersion);

  // causes the next volume mount to fail, triggering a reset to defaults normally
  Status invalidateNextMount();

  // Format SB0 + SB1.
  // Note the internal 16-bit tag is ONLY very probably unique for a unique name (65536 combinations.)
  Status formatAll(const char* projectAscii,
                   uint16_t storageVersion,
                   const uint16_t starts[kMaxParts],
                   bool lastIsEndMarker,
                   const char* partAscii[kMaxParts]);

  // Builder API (chained calls style; strings must remain valid until formatCommit()).
  Status formatBegin(const char* projectAscii, uint16_t storageVersion);
  bool   formatAddPartition(const char* partitionAscii, uint32_t sizeBytes); // sizeBytes==0 => remainder
  Status formatCommit();

  // ---------------- volume query ----------------
  bool     isMounted() const { return mounted_; }
  uint16_t blockCount() const { return blockCount_; }
  uint32_t byteCount()  const { return (uint32_t)blockCount_ << 4; }
  bool     hasPartNames() const { return hasPartNames_; }

  bool isReadable() const;
  bool isWritable() const;

  // ---------------- partition query ----------------
  uint8_t  partitionCount() const { return partCount_; }
  Part     partition(uint8_t idx) const { return (idx < partCount_) ? parts_[idx] : Part{}; }
  uint16_t partitionName16(uint8_t idx) const { return (idx < partCount_) ? partName16_[idx] : 0; }

  uint16_t partitionBlockSize(uint8_t idx) const;
  uint16_t partitionByteSize(uint8_t idx) const;

  // ---------------- partition find (requires valid SB1) ----------------
  // Find by already-computed 16-bit name tag (tag==0 reserved => "unnamed").
  int8_t partitionFindByName16(uint16_t tag) const;

  // Find by ASCII name (internally hashed to 16-bit tag).
  // Note the internal 16-bit tag is ONLY very probably unique for a unique name (65536 combinations.)
  bool partitionFind(const char* partitionAscii, uint8_t& outIdx) const;
  bool partitionFind(const char* partitionAscii, PartitionView16& out) const;

  // ---------------- partition I/O ----------------
  // Aligned I/O (offset/len must be multiples of 16).
  Status partitionRead(uint8_t partIdx, uint16_t offset, void* dst, uint16_t len);
  Status partitionWrite(uint8_t partIdx, uint16_t offset, const void* src, uint16_t len);

  // Block-scoped I/O (never cross a 16B block).
  Status partitionReadBlock(uint8_t partIdx, uint16_t partBlockOfs, uint8_t out16[16]);
  Status partitionWriteBlock(uint8_t partIdx, uint16_t partBlockOfs, const uint8_t in16[16]);
  Status partitionReadInBlock(uint8_t partIdx, uint16_t partBlockOfs, uint8_t byteOfs, void* dst, uint8_t len);
  Status partitionWriteInBlock(uint8_t partIdx, uint16_t partBlockOfs, uint8_t byteOfs, const void* src, uint8_t len);

private:
  static constexpr const char* kSalt = "OnCueV1";

  static inline uint16_t le16(const uint8_t* p) { return (uint16_t)p[0] | (uint16_t)(p[1] << 8); }
  static inline void wrle16(uint8_t* p, uint16_t v) { p[0] = (uint8_t)(v & 0xFFu); p[1] = (uint8_t)(v >> 8); }

  static uint16_t crc16_sb(const uint8_t* data, uint16_t lenNoCrc);

  static uint32_t getBitsLSB(const uint8_t* bytes11, uint32_t bitIndex, uint32_t bitCount);
  static void setBitsLSB(uint8_t* bytes11, uint32_t bitIndex, uint32_t bitCount, uint32_t value);

  Status enumerateFromSb0(const uint8_t sb0[16]);
  Status loadNamesBlock();
  Status writeNamesBlock(const uint16_t partName16[kMaxParts]);

  static inline Status mapIo(NvDevice::IoStatus st) {
    switch (st) {
      case NvDevice::IoStatus::Ok:         return Status::Ok;
      case NvDevice::IoStatus::ReadOnly:   return Status::ReadOnly;
      case NvDevice::IoStatus::OutOfRange: return Status::OutOfRange;
      case NvDevice::IoStatus::NotReady:   return Status::NotReady;
      default:                            return Status::IoError;
    }
  }

  // Status-returning 16-byte block helpers (used internally for better error propagation).
  static inline Status readBlockSt(NvDevice& d, uint16_t blockIndex, uint8_t out16[16]) {
    return mapIo(d.readBlock16(blockIndex, out16));
  }

  static inline Status writeBlockSt(NvDevice& d, uint16_t blockIndex, const uint8_t in16[16]) {
    if (d.supportsWriteBlock16()) {
      return mapIo(d.writeBlock16(blockIndex, in16));
    }
    return mapIo(d.write((uint16_t)(blockIndex << 4), in16, 16));
  }

  Status scrubPartitionBlocksOnFormat_();

private:
  NvDevice* dev_ = nullptr;

  uint16_t blockCount_ = 0;

  bool mounted_ = false;
  Status status_ = Status::NotReady;

  uint16_t storedVolumeName16_ = 0;
  uint8_t  flags8_ = 0;
  uint8_t  flags4_ = 0;

  uint8_t partCount_ = 0;
  Part    parts_[kMaxParts];

  uint16_t partName16_[kMaxParts] = {0};
  bool hasPartNames_ = false;

  // builder staging
  bool builderActive_ = false;
  const char* builderProject_ = nullptr;
  uint16_t builderStorageVersion_ = 0;
  uint8_t builderCount_ = 0;
  const char* builderPartAscii_[kMaxParts] = {nullptr};
  uint16_t builderSizeBlocks_[kMaxParts] = {0}; // 0 => remainder
};
