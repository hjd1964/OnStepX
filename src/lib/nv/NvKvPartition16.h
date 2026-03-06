// -----------------------------------------------------------------------------------
// NvKvPartition16.h
//
// KV partition on 16-byte blocks (v1 format).
//
// This KV layer stores named blobs in a fixed-size partition.
// The partition is a 16-byte-block address space provided by NvVolume.
// Supported partition size: up to 8192 bytes (512 x 16B blocks).
//
// Layout
// - Table entries are stored at blocks [0 .. tableBlocks-1].
// - Data blocks are stored at [dataBegin .. partBlocks-1], where dataBegin = tableBlocks.
// - tableBlocks targets KEY_TABLE_PERCENT (default 40%) and can slide upward
//   to at most partBlocks/2 as needed so dataCount fits byte addressing.
//
// Terminology
// - Block: 16 bytes.
// - Data address (addr8): 8-bit index into the data region.
//     addr8=0 -> physical block dataBegin
//     addr8=1 -> physical block dataBegin+1
//     ...
//     addr8=254 -> physical block dataBegin+254
// - addr8 value 0xFF is reserved as ADDR_END sentinel.
//
// Keying
// - Each record key is name24:
//     name24 = CRC24( KV_NAME_SALT + asciiName )
// - CRC24 uses Crc24::init/update from Crc.h.
//
// Allocation model
// - Overwrite-in-place semantics; no compaction.
// - Data block allocation is top-down within the data region.
// - Addressable data region is limited to 255 blocks (0..254), clamped by partition size.
//
// ===================================================================================================
// v1 ENTRY FORMAT (16 bytes, one entry per key)
//   a[0..2]  : name24 (LE)
//   a[3]     : header = flags3 + len5
//              flags bits:
//                FLAG_INLINE_ONLY       : body contains inline payload only
//                FLAG_INLINE_PLUS_BLOCK : body contains inline payload + one addr8
//                FLAG_TAIL              : addr-list mode; len5 = bytes used in last block
//   a[4..12] : body72 (9 bytes), interpreted by mode:
//              - INLINE_ONLY: up to 9 inline bytes
//              - INLINE_PLUS_BLOCK: 8 inline bytes + 1 addr8
//              - ADDR_LIST: up to 9 addr8 values (0xFF sentinel-terminated)
//   a[13..15]: crc24 (LE), written last
//              CRC covers entry (with crc bytes zeroed) and referenced payload blocks.
//
// Mode max payload sizes:
// - InlineOnly:       9 bytes
// - InlinePlusBlock:  8 + 16 = 24 bytes
// - AddrList:         9 * 16 = 144 bytes
// Overall max file size: 144 bytes
// -----------------------------------------------------------------------------------
#pragma once

#include <stdint.h>
#include <string.h>
#include <type_traits>

#include "NvVolume.h"
#include "../math/Crc.h"

class __FlashStringHelper;

#ifndef MAX_BLOCKS
  #define MAX_BLOCKS 2048u
#endif

#ifndef KEY_TABLE_PERCENT
  #define KEY_TABLE_PERCENT 400u // 40% table by default
#endif

class KvPartition16 {
public:
  enum class Status : uint8_t {
    Ok = 0,
    NotBound,
    BadGeom,
    IoError,
    Corrupt,
    NotFound,
    NoSpace,
    OutOfRange,
  };

  static constexpr uint8_t  BLOCK_BYTES = 16u;
  static constexpr uint8_t  ADDR_END = 0xFFu;
  static constexpr uint32_t EMPTY_NAME24 = 0x00FFFFFFu;

  static constexpr uint8_t FLAG_INLINE_ONLY       = 0x01u;
  static constexpr uint8_t FLAG_INLINE_PLUS_BLOCK = 0x02u;
  static constexpr uint8_t FLAG_TAIL              = 0x04u;

  static constexpr uint8_t INLINE_CAP            = 9u;
  static constexpr uint8_t INLINE_PLUS_HEAD_CAP  = 8u;
  static constexpr uint8_t ADDR_CAP              = 9u;
  static constexpr uint16_t MAX_PART_BLOCKS      = 512u;

  struct Geom {
    uint16_t partBlocks   = 0;
    uint16_t tableBlocks  = 0;
    uint16_t slotCount    = 0;
    uint16_t dataBegin    = 0;
    uint16_t dataCount    = 0;
    uint16_t mainPoolBase = 0;
  };

  struct Stats {
    uint16_t dataBlocksTotal;
    uint16_t dataBlocksFree;
    uint16_t slotsTotal;
    uint16_t slotsFree;
  };

  KvPartition16() = default;

  Status init(NvVolume& vol, uint8_t partIdx);
  Status init(NvVolume& vol, uint16_t partitionName16);
  Status init(NvVolume& vol, const char* partitionAscii);

  const Geom& geom()   const { return g_; }
  Status      status() const { return status_; }

  Status get(uint32_t name24, void* dst, uint16_t dstCap, uint16_t& outLen);
  Status put(uint32_t name24, const void* src, uint16_t len);
  Status del(uint32_t name24);

  static uint32_t computeKey(const char* nameAscii);
  static uint32_t computeKey(const __FlashStringHelper* nameAsciiF);

  Status get(const char* nameAscii, void* dst, uint16_t dstCap, uint16_t& outLen) { return get(name24_(nameAscii), dst, dstCap, outLen); }
  Status put(const char* nameAscii, const void* src, uint16_t len) { return put(name24_(nameAscii), src, len); }

  Status put(const char* nameAscii, const char* zstr) {
    if (!zstr) return put(nameAscii, (const void*)"", 1);
    const uint16_t n = (uint16_t)strlen(zstr) + 1u;
    return put(nameAscii, (const void*)zstr, n);
  }

  Status del(const char* nameAscii) { return del(name24_(nameAscii)); }

  Status scrub(uint16_t* outDeleted);

  template <typename T>
  Status put(const char* nameAscii, const T& v) { return put(nameAscii, (const void*)&v, (uint16_t)sizeof(T)); }

  template <typename T>
  Status put(uint32_t name24, const T& v) { return put(name24, (const void*)&v, (uint16_t)sizeof(T)); }

  template <typename T>
  Status get(const char* nameAscii, T& out) {
    T tmp = out;
    uint16_t outLen = 0;

    Status s = get(nameAscii, (void*)&tmp, (uint16_t)sizeof(T), outLen);
    if (s != Status::Ok) return s;
    if (outLen != sizeof(T)) return Status::Corrupt;

    out = tmp;
    return Status::Ok;
  }

  template <typename T>
  Status get(uint32_t name24, T& out) {
    T tmp = out;
    uint16_t outLen = 0;

    Status s = get(name24, (void*)&tmp, (uint16_t)sizeof(T), outLen);
    if (s != Status::Ok) return s;
    if (outLen != sizeof(T)) return Status::Corrupt;

    out = tmp;
    return Status::Ok;
  }

  bool isValid() const { return (status_ == Status::Ok) && allocInit_; }

  template <typename T>
  bool getOrInit(const char* nameAscii, T& inOutDefaults) {
    if (!nameAscii) return false;
    if (!isValid()) return false;

    const Status gs = get(nameAscii, inOutDefaults);
    if (gs == Status::Ok) return true;
    if (gs == Status::IoError) return false;

    if (put(nameAscii, inOutDefaults) != Status::Ok) { initErrorFlag_ = true; }
    return true;
  }

  template <typename T>
  bool getOrInit(uint32_t name24, T& inOutDefaults) {
    if ((name24 & 0x00FFFFFFu) == EMPTY_NAME24) return false;
    if (!isValid()) return false;

    const Status gs = get(name24, inOutDefaults);
    if (gs == Status::Ok) return true;
    if (gs == Status::IoError) return false;

    if (put(name24, inOutDefaults) != Status::Ok) { initErrorFlag_ = true; }
    return true;
  }

  void resetInitErrorFlag() { initErrorFlag_ = false; }
  bool getInitErrorFlag()   { return initErrorFlag_; }

  Status stats(Stats& out);

private:
  enum class Mode : uint8_t {
    InlineOnly = 0,      // max 9 bytes
    InlinePlusBlock = 1, // max 24 bytes
    AddrList = 2,        // max 9 * 16 = 144 bytes (sentinel terminated)
  };

  struct Decoded {
    Mode mode = Mode::InlineOnly;
    uint8_t flags3 = 0;
    uint8_t len5 = 0;

    uint8_t inlineLen = 0;
    uint8_t tailLen = 0;

    uint8_t oneBlockAddr = ADDR_END;
    uint8_t blocks[ADDR_CAP + 1u] = {ADDR_END};
    uint8_t blockCount = 0;
  };

  static constexpr uint16_t BITMAP_BYTES = (MAX_BLOCKS + 7u) / 8u;
  static constexpr const char* KV_NAME_SALT = "OnCueKVv1";

  static inline uint8_t packHeader_(uint8_t flags3, uint8_t len5) { return (uint8_t)((flags3 & 0x07u) | ((len5 & 0x1Fu) << 3)); }
  static inline uint8_t hdrFlags3_(uint8_t header) { return (uint8_t)(header & 0x07u); }
  static inline uint8_t hdrLen5_(uint8_t header)   { return (uint8_t)(header >> 3); }

  static inline uint32_t le24_(const uint8_t* p) {
    return ((uint32_t)p[0]) | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16);
  }
  static inline void wrle24_(uint8_t* p, uint32_t v24) {
    p[0] = (uint8_t)(v24 & 0xFFu);
    p[1] = (uint8_t)((v24 >> 8) & 0xFFu);
    p[2] = (uint8_t)((v24 >> 16) & 0xFFu);
  }

  static inline uint32_t loadCrc24LE_(const uint8_t A[16]) {
    return ((uint32_t)A[13]) | ((uint32_t)A[14] << 8) | ((uint32_t)A[15] << 16);
  }
  static inline void storeCrc24LE_(uint8_t A[16], uint32_t crc24) {
    A[13] = (uint8_t)(crc24 & 0xFFu);
    A[14] = (uint8_t)((crc24 >> 8) & 0xFFu);
    A[15] = (uint8_t)((crc24 >> 16) & 0xFFu);
  }

  static inline uint32_t name24_(const char* asciiName) { return computeKey(asciiName ? asciiName : ""); }

  uint16_t slotToBlockOfs_(uint16_t slotIdx) const { return slotIdx; }
  uint16_t dataAddrToBlock_(uint8_t addr8) const { return (uint16_t)(g_.dataBegin + (uint16_t)addr8); }
  bool dataAddrValid_(uint8_t addr8) const { return addr8 != ADDR_END && (uint16_t)addr8 < g_.dataCount; }

  Status readSlot_(uint16_t slotIdx, uint8_t A[16]);
  Status writeSlot_(uint16_t slotIdx, const uint8_t A[16]);
  Status readSlotName24_(uint16_t slotIdx, uint32_t& outName24);

  Status readDataBlockByAddr_(uint8_t addr8, uint8_t out16[16]) const;
  Status writeDataBlockByAddr_(uint8_t addr8, const uint8_t in16[16]);

  void bodyRead_(const uint8_t A[16], uint8_t out[9]) const { memcpy(out, &A[4], 9); }
  void bodyWrite_(uint8_t A[16], const uint8_t in[9]) const { memcpy(&A[4], in, 9); }

  Status decodeEntry_(const uint8_t A[16], Decoded& out) const;
  uint16_t payloadLenBytes_(const Decoded& d) const;

  Status computeFullCrc24_(const uint8_t A[16], uint32_t& outCrc24) const;
  Status validateEntryCrc_(const uint8_t A[16]) const;

  Status findSlot_(uint32_t name24, uint16_t& outSlot, uint8_t A[16]);
  Status findOrAllocSlot_(uint32_t name24, uint16_t& outSlot, uint8_t A[16], bool& outIsExisting);

  void markUsed_(uint16_t blockIdx, bool used);
  bool isUsed_(uint16_t blockIdx) const;

  Status initAllocMap_();
  Status allocDataBlocks_(uint8_t count, uint8_t outAddrs[ADDR_CAP]);
  void invalidateAlloc_() { allocInit_ = false; }

  void freeEntryBlocksFromMap_(const uint8_t A[16]);

  bool initGeom_();

private:
  NvVolume* vol_ = nullptr;
  uint8_t   partIdx_ = 0xFF;
  Geom      g_;
  Status    status_ = Status::NotBound;

  uint8_t allocBits_[BITMAP_BYTES] = {0};
  bool    allocInit_ = false;

  bool initErrorFlag_ = false;
};
