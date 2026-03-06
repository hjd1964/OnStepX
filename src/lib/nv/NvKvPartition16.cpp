// -----------------------------------------------------------------------------------
// NvKvPartition16.cpp
#include "NvKvPartition16.h"

#if defined(ARDUINO)
  #include <Arduino.h>
#elif defined(ARDUINO_ARCH_AVR)
  #include <avr/pgmspace.h>
#endif

namespace {
static inline uint8_t readProgByte16_(const char* p) {
#if defined(ARDUINO_ARCH_AVR)
  return (uint8_t)pgm_read_byte(p);
#else
  return (uint8_t)(*p);
#endif
}
}

bool KvPartition16::initGeom_() {
  if (!vol_) return false;

  const uint16_t pb = vol_->partitionBlockSize(partIdx_);
  if (pb < 2u) return false;
  if (pb > MAX_PART_BLOCKS) return false;

  uint32_t t = (uint32_t)pb * (uint32_t)KEY_TABLE_PERCENT + 500u;
  t /= 1000u;
  if (t < 1u) t = 1u;

  const uint16_t tableMax = (uint16_t)(pb / 2u);
  if (t > tableMax) t = tableMax;

  const uint16_t minTableForAddr8 = (pb > 255u) ? (uint16_t)(pb - 255u) : 0u;
  if (t < minTableForAddr8) t = minTableForAddr8;
  if (t > tableMax) t = tableMax;

  g_.partBlocks = pb;
  g_.tableBlocks = (uint16_t)t;
  g_.slotCount = g_.tableBlocks;

  if (g_.tableBlocks == 0u || g_.tableBlocks >= g_.partBlocks) return false;

  g_.dataBegin = g_.tableBlocks;
  {
    uint16_t dc = (uint16_t)(g_.partBlocks - g_.dataBegin);
    if (dc > 255u) dc = 255u;
    g_.dataCount = dc;
  }

  g_.mainPoolBase = g_.dataBegin;
  return g_.dataCount != 0u;
}

KvPartition16::Status KvPartition16::init(NvVolume& vol, uint8_t partIdx) {
  if (!vol.isMounted()) return status_ = Status::NotBound;
  if (partIdx >= vol.partitionCount()) return status_ = Status::BadGeom;

  vol_ = &vol;
  partIdx_ = partIdx;
  allocInit_ = false;
  memset(allocBits_, 0, sizeof(allocBits_));

  if (!initGeom_()) return status_ = Status::BadGeom;

  const Status s = initAllocMap_();
  if (s != Status::Ok) {
    allocInit_ = false;
    return status_ = s;
  }

  return status_ = Status::Ok;
}

KvPartition16::Status KvPartition16::init(NvVolume& vol, uint16_t partitionName16) {
  if (partitionName16 == 0u) return status_ = Status::BadGeom;
  if (!vol.isMounted()) return status_ = Status::NotBound;

  const int8_t idx = vol.partitionFindByName16(partitionName16);
  if (idx < 0) return status_ = Status::NotBound;

  return init(vol, (uint8_t)idx);
}

KvPartition16::Status KvPartition16::init(NvVolume& vol, const char* partitionAscii) {
  if (!partitionAscii || !partitionAscii[0]) return status_ = Status::BadGeom;
  if (!vol.isMounted()) return status_ = Status::NotBound;

  uint8_t idx = 0;
  if (!vol.partitionFind(partitionAscii, idx)) return status_ = Status::NotBound;

  return init(vol, idx);
}

KvPartition16::Status KvPartition16::readSlot_(uint16_t slotIdx, uint8_t A[16]) {
  if (!vol_) return Status::NotBound;
  if (slotIdx >= g_.slotCount) return Status::OutOfRange;

  const uint16_t bo = slotToBlockOfs_(slotIdx);
  return (vol_->partitionReadBlock(partIdx_, bo, A) == NvVolume::Status::Ok) ? Status::Ok : Status::IoError;
}

KvPartition16::Status KvPartition16::writeSlot_(uint16_t slotIdx, const uint8_t A[16]) {
  if (!vol_) return Status::NotBound;
  if (slotIdx >= g_.slotCount) return Status::OutOfRange;

  const uint16_t bo = slotToBlockOfs_(slotIdx);
  return (vol_->partitionWriteBlock(partIdx_, bo, A) == NvVolume::Status::Ok) ? Status::Ok : Status::IoError;
}

KvPartition16::Status KvPartition16::readSlotName24_(uint16_t slotIdx, uint32_t& outName24) {
  if (!vol_) return Status::NotBound;
  if (slotIdx >= g_.slotCount) return Status::OutOfRange;

  const uint16_t bo = slotToBlockOfs_(slotIdx);
  uint8_t n3[3];
  if (vol_->partitionReadInBlock(partIdx_, bo, 0, n3, 3) != NvVolume::Status::Ok) return Status::IoError;
  outName24 = le24_(n3) & 0x00FFFFFFu;
  return Status::Ok;
}

KvPartition16::Status KvPartition16::readDataBlockByAddr_(uint8_t addr8, uint8_t out16[16]) const {
  if (!vol_) return Status::NotBound;
  if (!dataAddrValid_(addr8)) return Status::Corrupt;

  const uint16_t b = dataAddrToBlock_(addr8);
  return (vol_->partitionReadBlock(partIdx_, b, out16) == NvVolume::Status::Ok) ? Status::Ok : Status::IoError;
}

KvPartition16::Status KvPartition16::writeDataBlockByAddr_(uint8_t addr8, const uint8_t in16[16]) {
  if (!vol_) return Status::NotBound;
  if (!dataAddrValid_(addr8)) return Status::OutOfRange;

  const uint16_t b = dataAddrToBlock_(addr8);
  return (vol_->partitionWriteBlock(partIdx_, b, in16) == NvVolume::Status::Ok) ? Status::Ok : Status::IoError;
}

KvPartition16::Status KvPartition16::decodeEntry_(const uint8_t A[16], Decoded& out) const {
  out = Decoded{};

  out.flags3 = hdrFlags3_(A[3]);
  out.len5 = hdrLen5_(A[3]);

  const bool fInlineOnly = (out.flags3 & FLAG_INLINE_ONLY) != 0u;
  const bool fInlinePlus = (out.flags3 & FLAG_INLINE_PLUS_BLOCK) != 0u;
  const bool fTail = (out.flags3 & FLAG_TAIL) != 0u;

  if (fInlineOnly) {
    if (fInlinePlus || fTail) return Status::Corrupt;
    if (out.len5 > INLINE_CAP) return Status::Corrupt;

    out.mode = Mode::InlineOnly;
    out.inlineLen = out.len5;
    return Status::Ok;
  }

  if (fInlinePlus) {
    if (fTail) return Status::Corrupt;
    if (out.len5 <= INLINE_PLUS_HEAD_CAP || out.len5 > (uint8_t)(INLINE_PLUS_HEAD_CAP + BLOCK_BYTES)) return Status::Corrupt;

    uint8_t body[9];
    bodyRead_(A, body);

    if (!dataAddrValid_(body[8])) return Status::Corrupt;

    out.mode = Mode::InlinePlusBlock;
    out.inlineLen = INLINE_PLUS_HEAD_CAP;
    out.oneBlockAddr = body[8];
    return Status::Ok;
  }

  if (!fTail) return Status::Corrupt;
  if (out.len5 < 1u || out.len5 > BLOCK_BYTES) return Status::Corrupt;

  uint8_t body[9];
  bodyRead_(A, body);

  out.mode = Mode::AddrList;
  out.tailLen = out.len5;
  out.blockCount = 0;

  for (uint8_t i = 0; i < ADDR_CAP; i++) {
    const uint8_t a = body[i];
    if (a == ADDR_END) break;
    if (!dataAddrValid_(a)) return Status::Corrupt;

    out.blocks[out.blockCount++] = a;
  }

  if (out.blockCount == 0u) return Status::Corrupt;
  out.blocks[out.blockCount] = ADDR_END;
  return Status::Ok;
}

uint16_t KvPartition16::payloadLenBytes_(const Decoded& d) const {
  if (d.mode == Mode::InlineOnly) return d.inlineLen;
  if (d.mode == Mode::InlinePlusBlock) return d.len5;

  if (d.blockCount == 0u) return 0u;
  return (uint16_t)(((uint16_t)(d.blockCount - 1u) * BLOCK_BYTES) + d.tailLen);
}

KvPartition16::Status KvPartition16::computeFullCrc24_(const uint8_t A[16], uint32_t& outCrc24) const {
  if (!vol_) return Status::NotBound;

  Decoded d;
  const Status ds = decodeEntry_(A, d);
  if (ds != Status::Ok) return ds;

  uint8_t Ac[16];
  memcpy(Ac, A, 16);
  Ac[13] = 0;
  Ac[14] = 0;
  Ac[15] = 0;

  uint32_t crc = Crc24::init();
  crc = Crc24::update(crc, Ac, 16);

  if (d.mode == Mode::InlinePlusBlock) {
    uint8_t blk[16];
    const Status rs = readDataBlockByAddr_(d.oneBlockAddr, blk);
    if (rs != Status::Ok) return rs;
    crc = Crc24::update(crc, blk, 16);
  } else if (d.mode == Mode::AddrList) {
    for (uint8_t i = 0; i < d.blockCount; i++) {
      uint8_t blk[16];
      const Status rs = readDataBlockByAddr_(d.blocks[i], blk);
      if (rs != Status::Ok) return rs;
      crc = Crc24::update(crc, blk, 16);
    }
  }

  outCrc24 = crc & 0x00FFFFFFu;
  return Status::Ok;
}

KvPartition16::Status KvPartition16::validateEntryCrc_(const uint8_t A[16]) const {
  const uint32_t stored = loadCrc24LE_(A) & 0x00FFFFFFu;
  uint32_t calc = 0;
  const Status s = computeFullCrc24_(A, calc);
  if (s != Status::Ok) return s;
  return (((stored ^ calc) & 0x00FFFFFFu) == 0u) ? Status::Ok : Status::Corrupt;
}

void KvPartition16::markUsed_(uint16_t blockIdx, bool used) {
  if (blockIdx >= g_.partBlocks) return;

  const uint16_t byteIdx = (uint16_t)(blockIdx >> 3);
  const uint8_t mask = (uint8_t)(1u << (blockIdx & 7u));
  if (byteIdx >= BITMAP_BYTES) return;

  if (used) allocBits_[byteIdx] |= mask;
  else allocBits_[byteIdx] &= (uint8_t)~mask;
}

bool KvPartition16::isUsed_(uint16_t blockIdx) const {
  if (blockIdx >= g_.partBlocks) return true;

  const uint16_t byteIdx = (uint16_t)(blockIdx >> 3);
  const uint8_t mask = (uint8_t)(1u << (blockIdx & 7u));
  if (byteIdx >= BITMAP_BYTES) return true;

  return (allocBits_[byteIdx] & mask) != 0u;
}

void KvPartition16::freeEntryBlocksFromMap_(const uint8_t A[16]) {
  Decoded d;
  if (decodeEntry_(A, d) != Status::Ok) return;

  if (d.mode == Mode::InlinePlusBlock) {
    if (dataAddrValid_(d.oneBlockAddr)) markUsed_(dataAddrToBlock_(d.oneBlockAddr), false);
    return;
  }

  if (d.mode == Mode::AddrList) {
    for (uint8_t i = 0; i < d.blockCount; i++) {
      if (dataAddrValid_(d.blocks[i])) markUsed_(dataAddrToBlock_(d.blocks[i]), false);
    }
  }
}

KvPartition16::Status KvPartition16::findSlot_(uint32_t name24, uint16_t& outSlot, uint8_t A[16]) {
  if (!vol_) return Status::NotBound;
  name24 &= 0x00FFFFFFu;

  for (uint16_t i = 0; i < g_.slotCount; i++) {
    uint32_t n = 0;
    const Status ns = readSlotName24_(i, n);
    if (ns != Status::Ok) return ns;
    if (n != name24) continue;

    const Status rs = readSlot_(i, A);
    if (rs != Status::Ok) return rs;

    const Status vs = validateEntryCrc_(A);
    if (vs == Status::IoError) return vs;
    if (vs != Status::Ok) continue;

    outSlot = i;
    return Status::Ok;
  }

  return Status::NotFound;
}

KvPartition16::Status KvPartition16::findOrAllocSlot_(uint32_t name24, uint16_t& outSlot, uint8_t A[16], bool& outIsExisting) {
  int16_t firstEmpty = -1;
  int16_t firstCorruptMatch = -1;
  name24 &= 0x00FFFFFFu;

  for (uint16_t i = 0; i < g_.slotCount; i++) {
    uint32_t n = 0;
    const Status ns = readSlotName24_(i, n);
    if (ns != Status::Ok) return ns;

    if (n == name24) {
      const Status rs = readSlot_(i, A);
      if (rs != Status::Ok) return rs;

      const Status vs = validateEntryCrc_(A);
      if (vs == Status::IoError) return vs;

      if (vs != Status::Ok) {
        if (firstCorruptMatch < 0) firstCorruptMatch = (int16_t)i;
        continue;
      }

      outSlot = i;
      outIsExisting = true;
      return Status::Ok;
    }

    if (n == EMPTY_NAME24 && firstEmpty < 0) firstEmpty = (int16_t)i;
  }

  if (firstCorruptMatch >= 0) {
    outSlot = (uint16_t)firstCorruptMatch;
    outIsExisting = false;
    return Status::Ok;
  }

  if (firstEmpty >= 0) {
    outSlot = (uint16_t)firstEmpty;
    outIsExisting = false;
    return Status::Ok;
  }

  return Status::NoSpace;
}

KvPartition16::Status KvPartition16::initAllocMap_() {
  memset(allocBits_, 0, sizeof(allocBits_));

  for (uint16_t b = 0; b < g_.dataBegin; b++) markUsed_(b, true);
  for (uint16_t b = (uint16_t)(g_.dataBegin + g_.dataCount); b < g_.partBlocks; b++) markUsed_(b, true);

  uint8_t A[16];

  for (uint16_t i = 0; i < g_.slotCount; i++) {
    Status s = readSlot_(i, A);
    if (s != Status::Ok) { allocInit_ = false; return s; }

    const uint32_t n = le24_(&A[0]) & 0x00FFFFFFu;
    if (n == EMPTY_NAME24) continue;

    const Status vs = validateEntryCrc_(A);
    if (vs == Status::IoError) { allocInit_ = false; return Status::IoError; }
    if (vs != Status::Ok) continue;

    Decoded d;
    if (decodeEntry_(A, d) != Status::Ok) continue;

    if (d.mode == Mode::InlinePlusBlock) {
      markUsed_(dataAddrToBlock_(d.oneBlockAddr), true);
    } else if (d.mode == Mode::AddrList) {
      for (uint8_t k = 0; k < d.blockCount; k++) markUsed_(dataAddrToBlock_(d.blocks[k]), true);
    }
  }

  allocInit_ = true;
  return Status::Ok;
}

KvPartition16::Status KvPartition16::allocDataBlocks_(uint8_t count, uint8_t outAddrs[ADDR_CAP]) {
  if (count == 0u || count > ADDR_CAP) return Status::OutOfRange;
  if (!allocInit_) return Status::Corrupt;

  uint8_t got = 0;

  const uint16_t first = g_.dataBegin;
  const uint16_t last = (uint16_t)(g_.dataBegin + g_.dataCount - 1u);

  for (int32_t b = (int32_t)last; b >= (int32_t)first; --b) {
    const uint16_t bb = (uint16_t)b;
    if (isUsed_(bb)) continue;

    markUsed_(bb, true);
    outAddrs[got++] = (uint8_t)(bb - g_.dataBegin);
    if (got == count) return Status::Ok;
  }

  for (uint8_t i = 0; i < got; i++) markUsed_(dataAddrToBlock_(outAddrs[i]), false);

  return Status::NoSpace;
}

KvPartition16::Status KvPartition16::get(uint32_t name24, void* dst, uint16_t dstCap, uint16_t& outLen) {
  if (!vol_) return Status::NotBound;
  name24 &= 0x00FFFFFFu;
  if (name24 == EMPTY_NAME24) return Status::OutOfRange;

  uint8_t A[16];
  uint16_t slot = 0;
  const Status fs = findSlot_(name24, slot, A);
  (void)slot;
  if (fs != Status::Ok) return fs;

  Decoded d;
  const Status ds = decodeEntry_(A, d);
  if (ds != Status::Ok) return ds;

  outLen = payloadLenBytes_(d);
  if (!dst) return Status::Ok;
  if (dstCap < outLen) return Status::OutOfRange;

  uint8_t* p = (uint8_t*)dst;

  if (d.mode == Mode::InlineOnly) {
    uint8_t body[9];
    bodyRead_(A, body);
    if (d.inlineLen) memcpy(p, body, d.inlineLen);
    return Status::Ok;
  }

  if (d.mode == Mode::InlinePlusBlock) {
    uint8_t body[9];
    bodyRead_(A, body);
    memcpy(p, body, INLINE_PLUS_HEAD_CAP);

    uint8_t blk[16];
    const Status rs = readDataBlockByAddr_(d.oneBlockAddr, blk);
    if (rs != Status::Ok) return rs;

    const uint8_t tail = (uint8_t)(d.len5 - INLINE_PLUS_HEAD_CAP);
    if (tail) memcpy(p + INLINE_PLUS_HEAD_CAP, blk, tail);
    return Status::Ok;
  }

  uint16_t written = 0;
  for (uint8_t i = 0; i < d.blockCount; i++) {
    uint8_t blk[16];
    const Status rs = readDataBlockByAddr_(d.blocks[i], blk);
    if (rs != Status::Ok) return rs;

    const bool isLast = (i == (uint8_t)(d.blockCount - 1u));
    const uint8_t n = isLast ? d.tailLen : BLOCK_BYTES;

    memcpy(p + written, blk, n);
    written = (uint16_t)(written + n);
  }

  return (written == outLen) ? Status::Ok : Status::Corrupt;
}

KvPartition16::Status KvPartition16::put(uint32_t name24, const void* src, uint16_t len) {
  if (!vol_) return Status::NotBound;
  if (!src && len) return Status::OutOfRange;
  if (!allocInit_) return Status::Corrupt;

  name24 &= 0x00FFFFFFu;
  if (name24 == EMPTY_NAME24) return Status::OutOfRange;

  if (len > (uint16_t)(ADDR_CAP * BLOCK_BYTES)) return Status::OutOfRange;

  uint8_t Aold[16];
  uint16_t slot = 0;
  bool existing = false;

  Status s = findOrAllocSlot_(name24, slot, Aold, existing);
  if (s != Status::Ok) return s;

  if (!existing) memset(Aold, 0xFF, sizeof(Aold));

  bool freedOld = false;
  {
    const Status vs = validateEntryCrc_(Aold);
    if (vs == Status::Ok) {
      freeEntryBlocksFromMap_(Aold);
      freedOld = true;
    } else if (vs == Status::IoError) {
      return Status::IoError;
    }
  }

  uint8_t A[16];
  memset(A, 0xFF, sizeof(A));
  wrle24_(&A[0], name24);

  const uint8_t* p = (const uint8_t*)src;

  if (len <= INLINE_CAP) {
    A[3] = packHeader_(FLAG_INLINE_ONLY, (uint8_t)len);

    uint8_t body[9];
    memset(body, 0xFF, sizeof(body));
    if (len) memcpy(body, p, len);
    bodyWrite_(A, body);

    uint32_t crc24 = 0;
    Status cs = computeFullCrc24_(A, crc24);
    if (cs != Status::Ok) { invalidateAlloc_(); return cs; }
    storeCrc24LE_(A, crc24);

    Status ws = writeSlot_(slot, A);
    if (ws == Status::IoError) invalidateAlloc_();
    return ws;
  }

  if (len <= (uint16_t)(INLINE_PLUS_HEAD_CAP + BLOCK_BYTES)) {
    uint8_t addrs[ADDR_CAP] = {0};
    s = allocDataBlocks_(1u, addrs);
    if (s != Status::Ok) {
      if (freedOld) invalidateAlloc_();
      return s;
    }

    uint8_t blk[16];
    memset(blk, 0xFF, sizeof(blk));
    const uint8_t tail = (uint8_t)(len - INLINE_PLUS_HEAD_CAP);
    if (tail) memcpy(blk, p + INLINE_PLUS_HEAD_CAP, tail);

    Status wsb = writeDataBlockByAddr_(addrs[0], blk);
    if (wsb != Status::Ok) {
      markUsed_(dataAddrToBlock_(addrs[0]), false);
      invalidateAlloc_();
      return wsb;
    }

    A[3] = packHeader_(FLAG_INLINE_PLUS_BLOCK, (uint8_t)len);

    uint8_t body[9];
    memset(body, 0xFF, sizeof(body));
    memcpy(body, p, INLINE_PLUS_HEAD_CAP);
    body[8] = addrs[0];
    bodyWrite_(A, body);

    uint32_t crc24 = 0;
    Status cs = computeFullCrc24_(A, crc24);
    if (cs != Status::Ok) { invalidateAlloc_(); return cs; }
    storeCrc24LE_(A, crc24);

    Status ws = writeSlot_(slot, A);
    if (ws == Status::IoError) invalidateAlloc_();
    return ws;
  }

  const uint8_t blocksNeeded = (uint8_t)((len + (BLOCK_BYTES - 1u)) / BLOCK_BYTES);
  if (blocksNeeded == 0u || blocksNeeded > ADDR_CAP) return Status::OutOfRange;

  uint8_t addrs[ADDR_CAP] = {0};
  s = allocDataBlocks_(blocksNeeded, addrs);
  if (s != Status::Ok) {
    if (freedOld) invalidateAlloc_();
    return s;
  }

  uint8_t tmp[16];
  for (uint8_t i = 0; i < blocksNeeded; i++) {
    memset(tmp, 0xFF, sizeof(tmp));

    const uint16_t ofs = (uint16_t)i * BLOCK_BYTES;
    const uint16_t avail = (len > ofs) ? (uint16_t)(len - ofs) : 0u;
    const uint16_t chunk = (avail >= BLOCK_BYTES) ? BLOCK_BYTES : avail;
    if (chunk) memcpy(tmp, p + ofs, chunk);

    Status ws = writeDataBlockByAddr_(addrs[i], tmp);
    if (ws != Status::Ok) {
      for (uint8_t k = 0; k < blocksNeeded; k++) markUsed_(dataAddrToBlock_(addrs[k]), false);
      invalidateAlloc_();
      return ws;
    }
  }

  const uint8_t tailLen = (uint8_t)((len & (BLOCK_BYTES - 1u)) ? (len & (BLOCK_BYTES - 1u)) : BLOCK_BYTES);
  A[3] = packHeader_(FLAG_TAIL, tailLen);

  uint8_t body[9];
  memset(body, ADDR_END, sizeof(body));
  for (uint8_t i = 0; i < blocksNeeded; i++) body[i] = addrs[i];
  bodyWrite_(A, body);

  uint32_t crc24 = 0;
  Status cs = computeFullCrc24_(A, crc24);
  if (cs != Status::Ok) { invalidateAlloc_(); return cs; }
  storeCrc24LE_(A, crc24);

  Status ws = writeSlot_(slot, A);
  if (ws == Status::IoError) invalidateAlloc_();
  return ws;
}

KvPartition16::Status KvPartition16::del(uint32_t name24) {
  if (!vol_) return Status::NotBound;
  if (!allocInit_) return Status::Corrupt;

  name24 &= 0x00FFFFFFu;
  if (name24 == EMPTY_NAME24) return Status::OutOfRange;

  uint8_t A[16];
  uint16_t slot = 0;
  const Status fs = findSlot_(name24, slot, A);
  if (fs != Status::Ok) return fs;

  if (validateEntryCrc_(A) == Status::Ok) freeEntryBlocksFromMap_(A);

  memset(A, 0xFF, sizeof(A));

  Status ws = writeSlot_(slot, A);
  if (ws == Status::IoError) invalidateAlloc_();
  return ws;
}

uint32_t KvPartition16::computeKey(const char* nameAscii) {
  uint32_t crc = Crc24::init();
  crc = Crc24::update(crc, (const uint8_t*)KV_NAME_SALT, (uint16_t)strlen(KV_NAME_SALT));
  crc = Crc24::update(crc, (const uint8_t*)(nameAscii ? nameAscii : ""), (uint16_t)strlen(nameAscii ? nameAscii : ""));
  return (crc & 0x00FFFFFFu);
}

uint32_t KvPartition16::computeKey(const __FlashStringHelper* nameAsciiF) {
#if defined(ARDUINO)
  if (!nameAsciiF) nameAsciiF = F("");

  uint32_t crc = Crc24::init();
  crc = Crc24::update(crc, (const uint8_t*)KV_NAME_SALT, (uint16_t)strlen(KV_NAME_SALT));

  const char* p = (const char*)nameAsciiF;
  for (;;) {
    const uint8_t c = readProgByte16_(p++);
    if (c == 0) break;
    crc = Crc24::updateByte(crc, c);
  }

  return (crc & 0x00FFFFFFu);
#else
  (void)nameAsciiF;
  return computeKey("");
#endif
}

KvPartition16::Status KvPartition16::scrub(uint16_t* outDeleted) {
  if (outDeleted) *outDeleted = 0;

  if (!vol_ || !vol_->isMounted()) return Status::NotBound;
  if (!initGeom_()) return Status::BadGeom;

  uint8_t A[16];
  uint16_t deleted = 0;

  for (uint16_t i = 0; i < g_.slotCount; i++) {
    const Status rs = readSlot_(i, A);
    if (rs != Status::Ok) return rs;

    const uint32_t n = le24_(&A[0]) & 0x00FFFFFFu;
    if (n == EMPTY_NAME24) continue;

    const Status vs = validateEntryCrc_(A);
    if (vs == Status::Ok) continue;
    if (vs == Status::IoError) return Status::IoError;
    if (vs != Status::Corrupt) return vs;

    memset(A, 0xFF, sizeof(A));
    const Status ws = writeSlot_(i, A);
    if (ws != Status::Ok) {
      if (ws == Status::IoError) invalidateAlloc_();
      return ws;
    }

    deleted++;
  }

  const Status ms = initAllocMap_();
  if (ms != Status::Ok) {
    if (ms == Status::IoError) invalidateAlloc_();
    return ms;
  }

  if (outDeleted) *outDeleted = deleted;
  return Status::Ok;
}

KvPartition16::Status KvPartition16::stats(Stats& out) {
  memset(&out, 0, sizeof(out));
  if (!vol_ || !vol_->isMounted()) return Status::NotBound;
  if (!initGeom_()) return Status::BadGeom;
  if (!allocInit_) return Status::Corrupt;

  out.dataBlocksTotal = g_.dataCount;
  out.slotsTotal = g_.slotCount;

  uint16_t usedData = 0;
  for (uint16_t a = 0; a < g_.dataCount; a++) {
    const uint16_t b = (uint16_t)(g_.dataBegin + a);
    const uint16_t byteIdx = (uint16_t)(b >> 3);
    const uint8_t mask = (uint8_t)(1u << (b & 7u));
    if (byteIdx >= BITMAP_BYTES) break;
    if (allocBits_[byteIdx] & mask) usedData++;
  }
  out.dataBlocksFree = (usedData <= g_.dataCount) ? (uint16_t)(g_.dataCount - usedData) : 0u;

  uint16_t empty = 0;
  for (uint16_t i = 0; i < g_.slotCount; i++) {
    uint32_t n = 0;
    const Status ns = readSlotName24_(i, n);
    if (ns != Status::Ok) return ns;
    if (n == EMPTY_NAME24) empty++;
  }
  out.slotsFree = empty;

  return Status::Ok;
}
