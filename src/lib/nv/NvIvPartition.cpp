// -------------------------------------------------------------------------------------------------
// NvIvPartition.cpp
#include "NvIvPartition.h"

bool IvPartition::init(NvVolume& vol, uint8_t partIdx) {
  vol_ = &vol;
  partIdx_ = partIdx;
  ready_ = false;
  partBytes_ = 0;

  if (!vol_->isMounted()) return false;
  if (partIdx_ >= vol_->partitionCount()) return false;

  partBytes_ = vol_->partitionByteSize(partIdx_);
  if (partBytes_ == 0) return false;

  // partitionByteSize is expected to be a multiple of block size; if not, refuse.
  if ((partBytes_ % kBlockSize) != 0) return false;

  ready_ = true;
  return true;
}

// bind by ASCII partition name (requires SB1 present/valid)
bool IvPartition::init(NvVolume& vol, const char* partitionAscii) {
  if (!partitionAscii || !partitionAscii[0]) return false;
  if (!vol.isMounted()) return false;

  uint8_t idx = 0xFF;
  if (!vol.partitionFind(partitionAscii, idx)) return false;
  return init(vol, idx);
}

// bind by name16 tag (requires SB1 present/valid)
bool IvPartition::init(NvVolume& vol, uint16_t partitionName16) {
  if (partitionName16 == 0) return false; // 0 reserved/unnamed
  if (!vol.isMounted()) return false;

  int8_t idx = vol.partitionFindByName16(partitionName16);
  if (idx < 0) return false;

  return init(vol, (uint8_t)idx);
}

IvPartition::NvVolStatus IvPartition::readBlock(uint16_t partBlock, uint8_t out16[kBlockSize]) {
  if (!isReady()) return NvVolStatus::NotReady;
  if (!out16) return NvVolStatus::IoError;
  if (partBlock >= sizeBlocks()) return NvVolStatus::OutOfRange;
  return vol_->partitionReadBlock(partIdx_, partBlock, out16);
}

IvPartition::NvVolStatus IvPartition::writeBlock(uint16_t partBlock, const uint8_t in16[kBlockSize]) {
  if (!isReady()) return NvVolStatus::NotReady;
  if (!vol_->isWritable()) return NvVolStatus::ReadOnly;
  if (!in16) return NvVolStatus::IoError;
  if (partBlock >= sizeBlocks()) return NvVolStatus::OutOfRange;
  return vol_->partitionWriteBlock(partIdx_, partBlock, in16);
}

IvPartition::NvVolStatus IvPartition::readBytes(uint16_t offset, void* dst, uint16_t len) {
  if (!isReady()) return NvVolStatus::NotReady;
  if (len == 0) return NvVolStatus::Ok;
  if (!dst) return NvVolStatus::IoError;
  if ((uint32_t)offset + (uint32_t)len > (uint32_t)partBytes_) return NvVolStatus::OutOfRange;

  // Pass-through: NvVolume/backend owns any alignment/RMW/device constraints.
  return vol_->partitionRead(partIdx_, offset, dst, len);
}

IvPartition::NvVolStatus IvPartition::writeBytes(uint16_t offset, const void* src, uint16_t len) {
  if (!isReady()) return NvVolStatus::NotReady;
  if (!vol_->isWritable()) return NvVolStatus::ReadOnly;
  if (len == 0) return NvVolStatus::Ok;
  if (!src) return NvVolStatus::IoError;
  if ((uint32_t)offset + (uint32_t)len > (uint32_t)partBytes_) return NvVolStatus::OutOfRange;

  // Pass-through: NvVolume/backend owns any alignment/RMW/device constraints.
  return vol_->partitionWrite(partIdx_, offset, src, len);
}

IvPartition::NvVolStatus IvPartition::clearBytes(uint16_t offset, uint16_t len) {
  if (!isReady()) return NvVolStatus::NotReady;
  if (!vol_->isWritable()) return NvVolStatus::ReadOnly;
  if (len == 0) return NvVolStatus::Ok;
  if ((uint32_t)offset + (uint32_t)len > (uint32_t)partBytes_) return NvVolStatus::OutOfRange;

  // Chunked zeros; NvVolume/backend handles any required alignment/merging.
  uint8_t zchunk[32];
  memset(zchunk, 0, sizeof(zchunk));

  uint16_t remaining = len;
  uint16_t off = offset;
  while (remaining) {
    uint16_t take = (remaining > (uint16_t)sizeof(zchunk)) ? (uint16_t)sizeof(zchunk) : remaining;
    NvVolStatus st = vol_->partitionWrite(partIdx_, off, zchunk, take);
    if (st != NvVolStatus::Ok) return st;
    off = (uint16_t)(off + take);
    remaining = (uint16_t)(remaining - take);
  }
  return NvVolStatus::Ok;
}
