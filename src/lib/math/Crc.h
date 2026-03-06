// -----------------------------------------------------------------------------------
// Crc, crc8/crc16/crc32 helpers with optional cooperative yielding

#pragma once
#include <stdint.h>
#include <string.h>

#include "../tasks/OnTask.h"

#ifndef CRC_YIELD_BYTES
  #define CRC_YIELD_BYTES 64
#endif

class Crc8 {
public:
  // crc8/atm (aka crc-8)
  // poly=0x07 init=0x00 refin=false refout=false xorout=0x00

  static inline uint8_t init() { return 0x00u; }

  static inline uint8_t updateByte(uint8_t crc, uint8_t data) {
    crc ^= data;
    for (uint8_t i = 0; i < 8; i++) {
      crc = (crc & 0x80u) ? (uint8_t)((crc << 1) ^ 0x07u) : (uint8_t)(crc << 1);
    }
    return crc;
  }

  static inline uint8_t update(uint8_t crc, const uint8_t* data, uint16_t len) {
    uint16_t sinceYield = 0;
    for (uint16_t i = 0; i < len; i++) {
      crc = updateByte(crc, data[i]);
    #if CRC_YIELD_BYTES > 0
      if (++sinceYield >= CRC_YIELD_BYTES) { sinceYield = 0; tasks.yield(); }
    #endif
    }
    return crc;
  }

  static inline uint8_t compute(const uint8_t* data, uint16_t len) {
    return update(init(), data, len);
  }

  // Salted ASCII helper (optional convenience)
  static inline uint8_t saltedAscii(const char* saltAscii, const char* textAscii) {
    if (!saltAscii) saltAscii = "";
    if (!textAscii) textAscii = "";
    uint8_t crc = init();
    crc = update(crc, (const uint8_t*)saltAscii, (uint16_t)strlen(saltAscii));
    crc = update(crc, (const uint8_t*)textAscii, (uint16_t)strlen(textAscii));
    return crc;
  }
};

class Crc16 {
public:
  // crc16/ccitt-false
  // poly=0x1021 init=0xffff refin=false refout=false xorout=0x0000

  static inline uint16_t init() { return 0xFFFFu; }

  static inline uint16_t updateByte(uint16_t crc, uint8_t data) {
    crc ^= (uint16_t)data << 8;
    for (uint8_t i = 0; i < 8; i++) {
      crc = (crc & 0x8000u) ? (uint16_t)((crc << 1) ^ 0x1021u) : (uint16_t)(crc << 1);
    }
    return crc;
  }

  static inline uint16_t update(uint16_t crc, const uint8_t* data, uint16_t len) {
    uint16_t sinceYield = 0;
    for (uint16_t i = 0; i < len; i++) {
      crc = updateByte(crc, data[i]);
    #if CRC_YIELD_BYTES > 0
      if (++sinceYield >= CRC_YIELD_BYTES) { sinceYield = 0; tasks.yield(); }
    #endif
    }
    return crc;
  }

  static inline uint16_t compute(const uint8_t* data, uint16_t len) {
    return update(init(), data, len);
  }

  // Salted ASCII helpers (for volume/partition IDs, tags, etc.)
  static inline uint16_t saltedAscii(const char* saltAscii, const char* textAscii) {
    if (!saltAscii) saltAscii = "";
    if (!textAscii) textAscii = "";
    uint16_t crc = init();
    crc = update(crc, (const uint8_t*)saltAscii, (uint16_t)strlen(saltAscii));
    crc = update(crc, (const uint8_t*)textAscii, (uint16_t)strlen(textAscii));
    return crc;
  }

  static inline uint16_t saltedAscii2(const char* saltAscii, const char* aAscii, const char* bAscii) {
    if (!saltAscii) saltAscii = "";
    if (!aAscii) aAscii = "";
    if (!bAscii) bAscii = "";
    uint16_t crc = init();
    crc = update(crc, (const uint8_t*)saltAscii, (uint16_t)strlen(saltAscii));
    crc = update(crc, (const uint8_t*)aAscii, (uint16_t)strlen(aAscii));
    crc = update(crc, (const uint8_t*)bAscii, (uint16_t)strlen(bAscii));
    return crc;
  }
};

/*class Crc24 {
public:
  // CRC-24/OPENPGP
  // poly=0x864CFB init=0xB704CE refin=false refout=false xorout=0x000000

  static inline uint32_t init() { return 0x00B704CEu; } // 24-bit

  static inline uint32_t updateByte(uint32_t crc, uint8_t data) {
    crc ^= (uint32_t)data << 16; // align byte to top of 24-bit register
    for (uint8_t i = 0; i < 8; i++) {
      crc = (crc & 0x800000u) ? ((crc << 1) ^ 0x864CFBu) : (crc << 1);
      crc &= 0xFFFFFFu;
    }
    return crc;
  }

  static inline uint32_t update(uint32_t crc, const uint8_t* data, uint16_t len) {
    uint16_t sinceYield = 0;
    for (uint16_t i = 0; i < len; i++) {
      crc = updateByte(crc, data[i]);
    #if CRC_YIELD_BYTES > 0
      if (++sinceYield >= CRC_YIELD_BYTES) { sinceYield = 0; tasks.yield(); }
    #endif
    }
    return crc & 0xFFFFFFu;
  }

  static inline uint32_t compute(const uint8_t* data, uint16_t len) {
    return update(init(), data, len) & 0xFFFFFFu;
  }
};
*/

class Crc24 {
public:
  static inline uint32_t init() { return 0x00B704CEu; }

  static inline uint32_t updateByte(uint32_t crc, uint8_t data) {
    const uint8_t idx = (uint8_t)(((crc >> 16) ^ (uint32_t)data) & 0xFFu);
    crc = ((crc << 8) & 0xFFFFFFu) ^ table_[idx];
    return crc & 0xFFFFFFu;
  }

  static inline uint32_t update(uint32_t crc, const uint8_t* data, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) crc = updateByte(crc, data[i]);
    return crc & 0xFFFFFFu;
  }

  static inline uint32_t compute(const uint8_t* data, uint16_t len) {
    return update(init(), data, len);
  }

private:
  static const uint32_t table_[256];
};

class Crc32 {
public:
  // crc32 (ieee 802.3), bitwise
  // poly=0xedb88320 init=0xffffffff xorout=~

  static inline uint32_t init() { return 0xFFFFFFFFu; }

  static inline uint32_t updateByte(uint32_t crc, uint8_t data) {
    crc ^= data;
    for (uint8_t b = 0; b < 8; b++) {
      uint32_t mask = -(int32_t)(crc & 1u);
      crc = (crc >> 1) ^ (0xEDB88320u & mask);
    }
    return crc;
  }

  static inline uint32_t update(uint32_t crc, const uint8_t* data, uint16_t len) {
    uint16_t sinceYield = 0;
    for (uint16_t i = 0; i < len; i++) {
      crc = updateByte(crc, data[i]);
    #if CRC_YIELD_BYTES > 0
      if (++sinceYield >= CRC_YIELD_BYTES) { sinceYield = 0; tasks.yield(); }
    #endif
    }
    return crc;
  }

  static inline uint32_t final(uint32_t crc) { return ~crc; }

  static inline uint32_t compute(const uint8_t* data, uint16_t len) {
    return final(update(init(), data, len));
  }

  // Salted ASCII helper (optional convenience)
  static inline uint32_t saltedAscii(const char* saltAscii, const char* textAscii) {
    if (!saltAscii) saltAscii = "";
    if (!textAscii) textAscii = "";
    uint32_t crc = init();
    crc = update(crc, (const uint8_t*)saltAscii, (uint16_t)strlen(saltAscii));
    crc = update(crc, (const uint8_t*)textAscii, (uint16_t)strlen(textAscii));
    return final(crc);
  }
};
