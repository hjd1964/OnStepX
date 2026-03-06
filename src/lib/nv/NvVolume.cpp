// NvVolume.cpp
#include "NvVolume.h"
#include "../math/Crc.h"

uint16_t NvVolume::crc16_sb(const uint8_t* data, uint16_t lenNoCrc) {
  return Crc16::compute(data, lenNoCrc);
}

// Reserve 0 for "unnamed" sentinel
static uint16_t normalizeTag16(uint16_t t) {
  if (t == 0x0000u) t = 0x0001u;
  return t;
}

uint16_t NvVolume::computeVolumeName16(const char* projectAscii, uint16_t storageVersion) {
  const uint16_t base = normalizeTag16(Crc16::saltedAscii(kSalt, projectAscii));
  // Compatibility token: base(project) + storageVersion (non-cryptographic, deterministic).
  return normalizeTag16((uint16_t)(base + storageVersion));
}

uint16_t NvVolume::computePartName16(const char* partitionAscii) {
  return normalizeTag16(Crc16::saltedAscii(kSalt, partitionAscii));
}

// bytes11 points at sb[3..13] (11 bytes, 88 bits). bit0 is LSB of bytes11[0].
uint32_t NvVolume::getBitsLSB(const uint8_t* bytes11, uint32_t bitIndex, uint32_t bitCount) {
  uint32_t v = 0;
  for (uint32_t i = 0; i < bitCount; ++i) {
    const uint32_t b = bitIndex + i;
    const uint32_t byte = b >> 3;
    const uint32_t bit  = b & 7u;
    const uint32_t bitVal = (bytes11[byte] >> bit) & 1u;
    v |= (bitVal << i);
  }
  return v;
}

void NvVolume::setBitsLSB(uint8_t* bytes11, uint32_t bitIndex, uint32_t bitCount, uint32_t value) {
  for (uint32_t i = 0; i < bitCount; ++i) {
    const uint32_t b = bitIndex + i;
    const uint32_t byte = b >> 3;
    const uint32_t bit  = b & 7u;
    const uint8_t mask = (uint8_t)(1u << bit);
    if ((value >> i) & 1u) bytes11[byte] |= mask;
    else                   bytes11[byte] &= (uint8_t)~mask;
  }
}

NvVolume::Status NvVolume::init(NvDevice& dev) {
  dev_ = &dev;

  uint32_t bytes = dev_->sizeBytes();
  if (bytes < kBlockSize) return status_ = Status::BadGeometry;
  if (bytes > 65536u)     return status_ = Status::BadGeometry;
  if (bytes & (kBlockSize - 1u)) return status_ = Status::BadGeometry;

  const uint32_t blocks = bytes / kBlockSize;
  if (blocks > kMaxBlocks) return status_ = Status::BadGeometry;

  blockCount_ = (uint16_t)blocks;

  mounted_ = false;
  partCount_ = 0;
  flags8_ = 0;
  flags4_ = 0;
  storedVolumeName16_ = 0;
  hasPartNames_ = false;
  for (uint8_t i = 0; i < kMaxParts; ++i) {
    parts_[i] = Part{};
    partName16_[i] = 0;
  }

  builderActive_ = false;
  builderProject_ = nullptr;
  builderStorageVersion_ = 0;
  builderCount_ = 0;
  for (uint8_t i = 0; i < kMaxParts; ++i) {
    builderPartAscii_[i] = nullptr;
    builderSizeBlocks_[i] = 0;
  }

  status_ = Status::Ok;
  return status_;
}

NvVolume::Status NvVolume::mount() {
  if (!dev_) return status_ = Status::NotReady;
  if (blockCount_ < 2) return status_ = Status::BadGeometry;

  uint8_t sb0[16];
  Status rb = readBlockSt(*dev_, 0, sb0);
  if (rb != Status::Ok) return status_ = rb;

  const uint16_t storedCrc0 = le16(&sb0[14]);
  const uint16_t calcCrc0   = crc16_sb(&sb0[0], 14);
  if (storedCrc0 != calcCrc0) {
    mounted_ = false; partCount_ = 0; hasPartNames_ = false;
    return status_ = Status::Unformatted;
  }

  const uint8_t flags8 = sb0[2];
  const uint8_t ver = (uint8_t)(flags8 >> kFlagVersionShift);
  if (ver != kSbVersionV1) {
    mounted_ = false; partCount_ = 0; hasPartNames_ = false;
    return status_ = Status::Unformatted;
  }

  if (flags8 & kFlagReservedMask) {
    mounted_ = false; partCount_ = 0; hasPartNames_ = false;
    return status_ = Status::Unformatted;
  }

  storedVolumeName16_ = le16(&sb0[0]);

  Status e = enumerateFromSb0(sb0);
  if (e != Status::Ok) {
    mounted_ = false; partCount_ = 0; hasPartNames_ = false;
    return status_ = Status::Unformatted; // keep mount policy
  }

  (void)loadNamesBlock(); // optional; keep prior semantics

  mounted_ = true;
  return status_ = Status::Ok;
}

NvVolume::Status NvVolume::mount(const char* projectAscii, uint16_t storageVersion) {
  Status st = mount();                 // call existing one
  if (st != Status::Ok) return st;

  if (!projectAscii || !projectAscii[0]) {
    mounted_ = false; partCount_ = 0; hasPartNames_ = false;
    return status_ = Status::BadGeometry;
  }

  const uint16_t expected = computeVolumeName16(projectAscii, storageVersion);
  if (storedVolumeName16_ != expected) {
    mounted_ = false; partCount_ = 0; hasPartNames_ = false;
    return status_ = Status::Unformatted;   // policy: treat as "not ours / incompatible"
  }

  return status_ = Status::Ok;
}

NvVolume::Status NvVolume::enumerateFromSb0(const uint8_t sb0[16]) {
  flags8_ = sb0[2];

  const uint8_t* packed = &sb0[3];
  flags4_ = (uint8_t)getBitsLSB(packed, 0, 4);

  uint16_t p[7];
  for (uint8_t i = 0; i < 7; ++i) {
    p[i] = (uint16_t)getBitsLSB(packed, 4u + (uint32_t)i * 12u, 12u);
  }

  uint16_t starts[7];
  uint8_t N = 0;
  for (uint8_t i = 0; i < 7; ++i) {
    const uint16_t v = (uint16_t)(p[i] & 0x0FFFu);
    if (v == 0x0FFFu) break;
    starts[N++] = v;
  }

  const bool lastIsEndMarker = (flags8_ & kFlagLastIsEndMarker) != 0;
  if (lastIsEndMarker && N < 2) return Status::BadGeometry;

  for (uint8_t i = 0; i < N; ++i) {
    const uint16_t s = starts[i];
    if (s < 2) return Status::BadGeometry;
    if (s >= blockCount_) return Status::OutOfRange;
    if (i > 0 && starts[i - 1] >= s) return Status::BadGeometry;
  }

  uint8_t partCount = N;
  uint16_t limitBlock = blockCount_;
  if (lastIsEndMarker) {
    partCount = (uint8_t)(N - 1);
    limitBlock = starts[N - 1];
    if (limitBlock >= blockCount_) return Status::OutOfRange;
  }

  for (uint8_t i = 0; i < partCount; ++i) {
    const uint16_t begin = starts[i];
    uint16_t end = 0;

    if (i + 1 < partCount) end = starts[i + 1];
    else end = lastIsEndMarker ? limitBlock : blockCount_;

    if (end <= begin) return Status::BadGeometry;

    parts_[i].beginBlock = begin;
    parts_[i].endBlock   = end;
  }

  for (uint8_t i = partCount; i < kMaxParts; ++i) {
    parts_[i].beginBlock = 0;
    parts_[i].endBlock = 0;
  }

  partCount_ = partCount;
  return Status::Ok;
}

NvVolume::Status NvVolume::loadNamesBlock() {
  hasPartNames_ = false;
  for (uint8_t i = 0; i < kMaxParts; ++i) partName16_[i] = 0;

  if (!dev_ || blockCount_ < 2) return Status::Ok;

  uint8_t sb1[16];
  Status rb = readBlockSt(*dev_, 1, sb1);
  if (rb != Status::Ok) return Status::Ok; // optional block; treat as absent (including NotReady)

  const uint16_t stored = le16(&sb1[14]);
  const uint16_t calc   = crc16_sb(&sb1[0], 14);
  if (stored != calc) return Status::Ok;

  for (uint8_t i = 0; i < kMaxParts; ++i) {
    partName16_[i] = le16(&sb1[i * 2]);
  }
  hasPartNames_ = true;
  return Status::Ok;
}

NvVolume::Status NvVolume::writeNamesBlock(const uint16_t partName16[kMaxParts]) {
  if (!dev_) return Status::NotReady;
  if (!dev_->isWritable()) return Status::ReadOnly;
  if (blockCount_ < 2) return Status::BadGeometry;

  uint8_t sb1[16] = {0};
  for (uint8_t i = 0; i < kMaxParts; ++i) {
    wrle16(&sb1[i * 2], partName16[i]);
  }
  const uint16_t crc = crc16_sb(&sb1[0], 14);
  wrle16(&sb1[14], crc);

  Status wb = writeBlockSt(*dev_, 1, sb1);
  if (wb != Status::Ok) return wb;

  for (uint8_t i = 0; i < kMaxParts; ++i) partName16_[i] = partName16[i];
  hasPartNames_ = true;
  return Status::Ok;
}

NvVolume::Status NvVolume::scrubPartitionBlocksOnFormat_() {
  if (!dev_) return Status::NotReady;
  if (!dev_->isWritable()) return Status::ReadOnly;

  uint8_t ff[16];
  for (uint8_t i = 0; i < 16; ++i) ff[i] = 0xFF;

  for (uint8_t i = 0; i < partCount_; ++i) {
    const Part& p = parts_[i];
    if (p.endBlock <= p.beginBlock) continue;

    uint16_t lo = p.beginBlock;
    uint16_t hi = (uint16_t)(p.endBlock - 1u);

    if (lo >= blockCount_ || hi >= blockCount_) return Status::OutOfRange;

    // Don’t touch SB0/SB1 (physical blocks 0 and 1)
    if (hi < 2u) continue;
    if (lo < 2u) lo = 2u;

    for (uint16_t j = lo; j <= hi; ++j) {
      Status s = writeBlockSt(*dev_, j, ff);
      if (s != Status::Ok) return s;
    }
  }

  return Status::Ok;
}

NvVolume::Status NvVolume::formatAll(const char* projectAscii,
                                     uint16_t storageVersion,
                                     const uint16_t starts[kMaxParts],
                                     bool lastIsEndMarker,
                                     const char* partAscii[kMaxParts]) {
  if (!dev_) return status_ = Status::NotReady;
  if (!dev_->isWritable()) return status_ = Status::ReadOnly;
  if (blockCount_ < 3) return status_ = Status::BadGeometry;

  uint16_t prev = 0;
  uint8_t N = 0;
  for (uint8_t i = 0; i < kMaxParts; ++i) {
    const uint16_t s = (uint16_t)(starts[i] & 0x0FFFu);
    if (s == 0x0FFFu) break;
    if (s < 2)            return status_ = Status::BadGeometry;
    if (s >= blockCount_) return status_ = Status::OutOfRange;
    if (i > 0 && prev >= s) return status_ = Status::BadGeometry;
    prev = s;
    ++N;
  }
  if (lastIsEndMarker && N < 2) return status_ = Status::BadGeometry;

  uint8_t sb0[16] = {0};

  const uint16_t volName16 = computeVolumeName16(projectAscii, storageVersion);
  wrle16(&sb0[0], volName16);

  uint8_t flags8 = 0;
  if (lastIsEndMarker) flags8 |= kFlagLastIsEndMarker;
  flags8 |= (uint8_t)(kSbVersionV1 << kFlagVersionShift);
  sb0[2] = flags8;

  uint8_t* packed = &sb0[3];
  for (uint8_t i = 0; i < 11; ++i) packed[i] = 0;

  setBitsLSB(packed, 0, 4, 0);
  for (uint8_t i = 0; i < kMaxParts; ++i) {
    setBitsLSB(packed, 4u + (uint32_t)i * 12u, 12u, (uint32_t)(starts[i] & 0x0FFFu));
  }

  const uint16_t crc0 = crc16_sb(&sb0[0], 14);
  wrle16(&sb0[14], crc0);

  Status wb0 = writeBlockSt(*dev_, 0, sb0);
  if (wb0 != Status::Ok) return status_ = wb0;

  uint16_t pn[7] = {0};
  for (uint8_t i = 0; i < kMaxParts; ++i) {
    if (partAscii && partAscii[i] && partAscii[i][0]) pn[i] = computePartName16(partAscii[i]);
    else pn[i] = 0;
  }

  Status w1 = writeNamesBlock(pn);
  if (w1 != Status::Ok) return status_ = w1;

  storedVolumeName16_ = volName16;
  flags8_ = flags8;
  flags4_ = 0;

  // Post-format self-check: this is our own SB0 buffer; propagate real cause.
  Status e = enumerateFromSb0(sb0);
  if (e != Status::Ok) { mounted_ = false; partCount_ = 0; return status_ = e; }

  Status scrub = scrubPartitionBlocksOnFormat_();
  if (scrub != Status::Ok) { mounted_ = false; partCount_ = 0; return status_ = scrub; }

  mounted_ = true;
  return status_ = Status::Ok;
}

NvVolume::Status NvVolume::formatBegin(const char* projectAscii, uint16_t storageVersion) {
  if (!dev_) return status_ = Status::NotReady;
  if (!dev_->isWritable()) return status_ = Status::ReadOnly;

  builderActive_ = true;
  builderProject_ = projectAscii ? projectAscii : "";
  builderStorageVersion_ = storageVersion;
  builderCount_ = 0;
  for (uint8_t i = 0; i < kMaxParts; ++i) {
    builderPartAscii_[i] = nullptr;
    builderSizeBlocks_[i] = 0;
  }
  return status_ = Status::Ok;
}

bool NvVolume::formatAddPartition(const char* partitionAscii, uint32_t sizeBytes) {
  if (!builderActive_) return false;
  if (builderCount_ >= kMaxParts) return false;

  if (sizeBytes == 0) {
    for (uint8_t i = 0; i < builderCount_; ++i) {
      if (builderSizeBlocks_[i] == 0) return false;
    }
    builderSizeBlocks_[builderCount_] = 0;
  } else {
    if (sizeBytes & (kBlockSize - 1u)) return false;
    const uint32_t blocks = sizeBytes / kBlockSize;
    if (blocks == 0 || blocks > 0xFFFFu) return false;
    builderSizeBlocks_[builderCount_] = (uint16_t)blocks;

    for (uint8_t i = 0; i < builderCount_; ++i) {
      if (builderSizeBlocks_[i] == 0) return false;
    }
  }

  builderPartAscii_[builderCount_] = partitionAscii;
  ++builderCount_;
  return true;
}

NvVolume::Status NvVolume::formatCommit() {
  if (!builderActive_) return status_ = Status::NotReady;
  if (!dev_) return status_ = Status::NotReady;
  if (!dev_->isWritable()) return status_ = Status::ReadOnly;
  if (blockCount_ < 3) return status_ = Status::BadGeometry;

  if (builderCount_ == 0) return status_ = Status::BadGeometry;

  uint16_t starts[kMaxParts];
  for (uint8_t i = 0; i < kMaxParts; ++i) starts[i] = 0x0FFFu;

  uint16_t cur = 2;
  for (uint8_t i = 0; i < builderCount_; ++i) {
    starts[i] = cur;

    uint16_t sz = builderSizeBlocks_[i];
    if (sz == 0) {
      if (cur >= blockCount_) return status_ = Status::OutOfRange;
      cur = blockCount_;
      break;
    } else {
      const uint32_t next = (uint32_t)cur + (uint32_t)sz;
      if (next > (uint32_t)blockCount_) return status_ = Status::OutOfRange;
      cur = (uint16_t)next;
    }
  }

  const char* partAscii[kMaxParts] = {nullptr};
  for (uint8_t i = 0; i < builderCount_; ++i) partAscii[i] = builderPartAscii_[i];

  Status s = formatAll(builderProject_, builderStorageVersion_, starts, false, partAscii);

  builderActive_ = false;
  builderProject_ = nullptr;
  builderStorageVersion_ = 0;
  builderCount_ = 0;
  for (uint8_t i = 0; i < kMaxParts; ++i) {
    builderPartAscii_[i] = nullptr;
    builderSizeBlocks_[i] = 0;
  }

  return status_ = s;
}

uint16_t NvVolume::partitionBlockSize(uint8_t idx) const {
  if (!mounted_ || idx >= partCount_) return 0;
  const Part& p = parts_[idx];
  return (uint16_t)(p.endBlock - p.beginBlock);
}

uint16_t NvVolume::partitionByteSize(uint8_t idx) const {
  const uint16_t blocks = partitionBlockSize(idx);
  return (uint16_t)(blocks << 4);
}

int8_t NvVolume::partitionFindByName16(uint16_t tag) const {
  if (!mounted_) return -1;
  if (!hasPartNames_) return -1;
  if (tag == 0) return -1;

  for (uint8_t i = 0; i < partCount_; ++i) {
    if (partName16_[i] == tag) return (int8_t)i;
  }
  return -1;
}

bool NvVolume::partitionFind(const char* partitionAscii, uint8_t& outIdx) const {
  outIdx = 0xFF;
  if (!mounted_ || !hasPartNames_ || !partitionAscii || !partitionAscii[0]) return false;

  const uint16_t tag = computePartName16(partitionAscii);
  if (tag == 0) return false;

  const int8_t idx = partitionFindByName16(tag);
  if (idx < 0) return false;

  outIdx = (uint8_t)idx;
  return true;
}

bool NvVolume::partitionFind(const char* partitionAscii, PartitionView16& out) const {
  out = PartitionView16{};
  uint8_t idx = 0xFF;
  if (!partitionFind(partitionAscii, idx)) return false;

  out.idx = idx;
  out.beginBlock = parts_[idx].beginBlock;
  out.endBlock   = parts_[idx].endBlock;
  return true;
}

bool NvVolume::isReadable() const {
  return dev_ && dev_->isReadable();
}

bool NvVolume::isWritable() const {
  return dev_ && dev_->isWritable();
}

static inline NvVolume::Status checkRange_(uint16_t partBytes, uint16_t offset, uint16_t len) {
  if (len == 0) return NvVolume::Status::Ok;
  const uint32_t end = (uint32_t)offset + (uint32_t)len;
  return (end <= (uint32_t)partBytes) ? NvVolume::Status::Ok : NvVolume::Status::OutOfRange;
}

NvVolume::Status NvVolume::partitionRead(uint8_t partIdx, uint16_t offset, void* dst, uint16_t len) {
  if (!dev_ || !mounted_) return Status::NotReady;
  if (partIdx >= partCount_) return Status::OutOfRange;
  if (len == 0) return Status::Ok;
  if (!dst) return Status::IoError;

  const Part& p = parts_[partIdx];
  const uint16_t partBytes = (uint16_t)((p.endBlock - p.beginBlock) << 4);

  const Status chk = checkRange_(partBytes, offset, len);
  if (chk != Status::Ok) return chk;

  const uint16_t absAddr = (uint16_t)((p.beginBlock << 4) + offset);
  return mapIo(dev_->read(absAddr, dst, len));
}

NvVolume::Status NvVolume::partitionWrite(uint8_t partIdx, uint16_t offset, const void* src, uint16_t len) {
  if (!dev_ || !mounted_) return Status::NotReady;
  if (partIdx >= partCount_) return Status::OutOfRange;
  if (!dev_->isWritable()) return Status::ReadOnly;
  if (len == 0) return Status::Ok;
  if (!src) return Status::IoError;

  const Part& p = parts_[partIdx];
  const uint16_t partBytes = (uint16_t)((p.endBlock - p.beginBlock) << 4);

  const Status chk = checkRange_(partBytes, offset, len);
  if (chk != Status::Ok) return chk;

  const uint16_t absAddr = (uint16_t)((p.beginBlock << 4) + offset);
  return mapIo(dev_->write(absAddr, src, len));
}

NvVolume::Status NvVolume::partitionReadBlock(uint8_t partIdx, uint16_t partBlockOfs, uint8_t out16[16]) {
  if (!dev_ || !mounted_) return Status::NotReady;
  if (partIdx >= partCount_) return Status::OutOfRange;

  const Part& p = parts_[partIdx];
  const uint16_t blocks = (uint16_t)(p.endBlock - p.beginBlock);
  if (partBlockOfs >= blocks) return Status::OutOfRange;

  const uint16_t absBlock = (uint16_t)(p.beginBlock + partBlockOfs);
  return readBlockSt(*dev_, absBlock, out16);
}

NvVolume::Status NvVolume::partitionWriteBlock(uint8_t partIdx, uint16_t partBlockOfs, const uint8_t in16[16]) {
  if (!dev_ || !mounted_) return Status::NotReady;
  if (partIdx >= partCount_) return Status::OutOfRange;
  if (!dev_->isWritable()) return Status::ReadOnly;

  const Part& p = parts_[partIdx];
  const uint16_t blocks = (uint16_t)(p.endBlock - p.beginBlock);
  if (partBlockOfs >= blocks) return Status::OutOfRange;

  const uint16_t absBlock = (uint16_t)(p.beginBlock + partBlockOfs);
  return writeBlockSt(*dev_, absBlock, in16);
}

NvVolume::Status NvVolume::partitionReadInBlock(uint8_t partIdx, uint16_t partBlockOfs,
                                               uint8_t byteOfs, void* dst, uint8_t len) {
  if (!dev_ || !mounted_) return Status::NotReady;
  if (partIdx >= partCount_) return Status::OutOfRange;
  if (!dst || len == 0) return Status::OutOfRange;
  if (byteOfs >= 16 || (uint16_t)byteOfs + (uint16_t)len > 16) return Status::Unaligned;

  const Part& p = parts_[partIdx];
  const uint16_t blocks = (uint16_t)(p.endBlock - p.beginBlock);
  if (partBlockOfs >= blocks) return Status::OutOfRange;

  const uint16_t absBlock = (uint16_t)(p.beginBlock + partBlockOfs);
  const uint16_t absAddr  = (uint16_t)((absBlock << 4) + byteOfs);

  return mapIo(dev_->read(absAddr, dst, (uint16_t)len));
}

NvVolume::Status NvVolume::partitionWriteInBlock(uint8_t partIdx, uint16_t partBlockOfs,
                                                uint8_t byteOfs, const void* src, uint8_t len) {
  if (!dev_ || !mounted_) return Status::NotReady;
  if (partIdx >= partCount_) return Status::OutOfRange;
  if (!dev_->isWritable()) return Status::ReadOnly;
  if (!src || len == 0) return Status::OutOfRange;
  if (byteOfs >= 16 || (uint16_t)byteOfs + (uint16_t)len > 16) return Status::Unaligned;

  const Part& p = parts_[partIdx];
  const uint16_t blocks = (uint16_t)(p.endBlock - p.beginBlock);
  if (partBlockOfs >= blocks) return Status::OutOfRange;

  const uint16_t absBlock = (uint16_t)(p.beginBlock + partBlockOfs);
  const uint16_t absAddr  = (uint16_t)((absBlock << 4) + byteOfs);

  return mapIo(dev_->write(absAddr, src, (uint16_t)len));
}

NvVolume::Status NvVolume::invalidateNextMount() {
  if (!dev_) return status_ = Status::NotReady;
  if (!dev_->isWritable()) return status_ = Status::ReadOnly;
  if (blockCount_ < 1) return status_ = Status::BadGeometry;

  uint8_t sb0[16];
  Status rb = readBlockSt(*dev_, 0, sb0);
  if (rb != Status::Ok) return status_ = rb;

  // Poison ONLY the stored CRC (bytes 14..15). No need to touch protected payload.
  sb0[14] ^= 0x01u;

  Status wb = writeBlockSt(*dev_, 0, sb0);
  if (wb != Status::Ok) return status_ = wb;

  return status_ = Status::Ok;
}
