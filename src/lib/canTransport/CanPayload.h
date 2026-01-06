// -----------------------------------------------------------------------------------
// CAN payload pack/unpack helper
// Header-only, cursor-based, supports read-only and read/write usage.
//
// Location: src/lib/canTransport/CanPayload.h
//
// Notes:
// - Cursor ("pos") is always measured in bytes from start of payload buffer.
// - Read-side always uses rbuf; write-side requires wbuf != nullptr.
// - All multi-byte values are encoded LITTLE-ENDIAN on wire.
// - Float uses memcpy + U32LE to guarantee endian correctness across hosts.
//
// -----------------------------------------------------------------------------------

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string.h>

class CanPayload {
public:
  // Writable payload (TX build, or mutable RX)
  CanPayload(uint8_t *payload, uint8_t payloadSize)
    : rbuf(payload), wbuf(payload), len(payloadSize), pos(0) {}

  // Read-only payload (RX)
  CanPayload(const uint8_t *payload, uint8_t payloadSize)
    : rbuf(payload), wbuf(nullptr), len(payloadSize), pos(0) {}

  inline bool isValid() const { return (rbuf != nullptr) && (len > 0); }
  inline bool isWritable() const { return wbuf != nullptr; }

  inline uint8_t size() const { return len; }
  inline uint8_t offset() const { return pos; }
  inline uint8_t remaining() const { return (pos <= len) ? (uint8_t)(len - pos) : 0; }

  inline const uint8_t *data() const { return rbuf; }
  inline uint8_t *mutableData() const { return wbuf; }

  inline void reset() { pos = 0; }
  inline bool seek(uint8_t newOffset) {
    if (newOffset > len) return false;
    pos = newOffset;
    return true;
  }

  // -------------------------
  // Raw bytes
  // -------------------------
  inline bool writeBytes(const uint8_t *src, uint8_t n) {
    if (!wbuf || !src) return false;
    if (!ensureWrite(n)) return false;
    memcpy(&wbuf[pos], src, n);
    pos = (uint8_t)(pos + n);
    return true;
  }

  inline bool readBytes(uint8_t *dst, uint8_t n) {
    if (!dst) return false;
    if (!ensureRead(n)) return false;
    memcpy(dst, &rbuf[pos], n);
    pos = (uint8_t)(pos + n);
    return true;
  }

  // -------------------------
  // Unsigned integers
  // -------------------------
  inline bool writeU8(uint8_t v) {
    if (!wbuf) return false;
    if (!ensureWrite(1)) return false;
    wbuf[pos++] = v;
    return true;
  }

  inline bool readU8(uint8_t &v) {
    if (!ensureRead(1)) return false;
    v = rbuf[pos++];
    return true;
  }

  inline bool writeU16LE(uint16_t v) {
    if (!wbuf) return false;
    if (!ensureWrite(2)) return false;
    wbuf[pos + 0] = (uint8_t)(v & 0xFFu);
    wbuf[pos + 1] = (uint8_t)((v >> 8) & 0xFFu);
    pos = (uint8_t)(pos + 2);
    return true;
  }

  inline bool readU16LE(uint16_t &v) {
    if (!ensureRead(2)) return false;
    v = (uint16_t)rbuf[pos + 0]
      | (uint16_t)((uint16_t)rbuf[pos + 1] << 8);
    pos = (uint8_t)(pos + 2);
    return true;
  }

  inline bool writeU32LE(uint32_t v) {
    if (!wbuf) return false;
    if (!ensureWrite(4)) return false;
    wbuf[pos + 0] = (uint8_t)(v & 0xFFu);
    wbuf[pos + 1] = (uint8_t)((v >> 8) & 0xFFu);
    wbuf[pos + 2] = (uint8_t)((v >> 16) & 0xFFu);
    wbuf[pos + 3] = (uint8_t)((v >> 24) & 0xFFu);
    pos = (uint8_t)(pos + 4);
    return true;
  }

  inline bool readU32LE(uint32_t &v) {
    if (!ensureRead(4)) return false;
    v = (uint32_t)rbuf[pos + 0]
      | (uint32_t)((uint32_t)rbuf[pos + 1] << 8)
      | (uint32_t)((uint32_t)rbuf[pos + 2] << 16)
      | (uint32_t)((uint32_t)rbuf[pos + 3] << 24);
    pos = (uint8_t)(pos + 4);
    return true;
  }

  // -------------------------
  // Signed integers
  // -------------------------
  inline bool writeI16LE(int16_t v) { return writeU16LE((uint16_t)v); }

  inline bool readI16LE(int16_t &v) {
    uint16_t u = 0;
    if (!readU16LE(u)) return false;
    v = (int16_t)u;
    return true;
  }

  inline bool writeI32LE(int32_t v) { return writeU32LE((uint32_t)v); }

  inline bool readI32LE(int32_t &v) {
    uint32_t u = 0;
    if (!readU32LE(u)) return false;
    v = (int32_t)u;
    return true;
  }

  // -------------------------
  // Floats (IEEE-754, little-endian on wire)
  // Uses memcpy (safe w.r.t. strict aliasing) + U32LE to enforce LE on wire.
  // -------------------------
  inline bool writeF32LE(float v) {
    if (!wbuf) return false;
    uint32_t u = 0;
    static_assert(sizeof(float) == sizeof(uint32_t), "float must be 32-bit");
    memcpy(&u, &v, sizeof(u));
    return writeU32LE(u);
  }

  inline bool readF32LE(float &v) {
    uint32_t u = 0;
    if (!readU32LE(u)) return false;
    static_assert(sizeof(float) == sizeof(uint32_t), "float must be 32-bit");
    memcpy(&v, &u, sizeof(v));
    return true;
  }

  // -------------------------
  // Convenience: float fixed-point i16
  // -------------------------
  inline bool writeFixedI16LE(float value, float scale) {
    if (scale == 0.0f) return false;
    const float scaled = value * scale;
    const float adj = (scaled >= 0.0f) ? 0.5f : -0.5f;
    const int32_t v = (int32_t)(scaled + adj);
    if (v < -32768 || v > 32767) return false;
    return writeI16LE((int16_t)v);
  }

  inline bool readFixedI16LE(float &value, float scale) {
    if (scale == 0.0f) return false;
    int16_t v = 0;
    if (!readI16LE(v)) return false;
    value = ((float)v) / scale;
    return true;
  }

  // -------------------------
  // Fixed-point helpers (int16 payload, double)
  // scale = 100 means value*100 stored as int16
  // Returns false if overflow would occur.
  // -------------------------
  inline bool writeFixedI16LE(double value, int32_t scale) {
    if (scale == 0) return false;

    const double scaled = value * (double)scale;
    // round-to-nearest, half away from zero
    const double adj = (scaled >= 0.0) ? 0.5 : -0.5;
    const int32_t v = (int32_t)(scaled + adj);

    if (v < -32768 || v > 32767) return false;
    return writeI16LE((int16_t)v);
  }

  inline bool readFixedI16LE(double &value, int32_t scale) {
    if (scale == 0) return false;
    int16_t v = 0;
    if (!readI16LE(v)) return false;
    value = ((double)v) / (double)scale;
    return true;
  }

private:
  const uint8_t *rbuf;
  uint8_t *wbuf;
  uint8_t len;
  uint8_t pos;

  inline bool ensureRead(uint8_t n) const {
    if (!rbuf) return false;
    if (pos > len) return false;
    return (uint8_t)(len - pos) >= n;
  }

  inline bool ensureWrite(uint8_t n) const {
    if (!wbuf) return false;
    if (pos > len) return false;
    return (uint8_t)(len - pos) >= n;
  }
};
