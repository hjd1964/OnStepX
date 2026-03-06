// NvDeviceEepromArduino.h
#pragma once
#include "NvDeviceBase.h"
#include <EEPROM.h>

#if !defined(NV_DRIVER) || (NV_DRIVER != NV_EEPROM)
  #error "NvDeviceEepromArduino.h included but NV_DRIVER != NV_EEPROM"
#endif

#ifndef E2END
  #error "E2END must be defined for NV_EEPROM (set it in HAL or NvsConfig.h)"
#endif

class NvDeviceEepromArduino : public NvDevice {
public:
  bool init() override {
    // Nothing to init for classic EEPROM
    VLF("MSG: NvDevice, using built-in EEPROM via Arduino library");
    return true;
  }

  // reports usable aligned size (multiple of 16)
  uint32_t sizeBytes() const override {
    const uint32_t n = (uint32_t)(E2END + 1u);
    return n & ~15u;
  }

  // Device write cycle endurance
  // Low < 100K writes, Mid > ~100K writes, High > ~100M writes (FRAM)
  Endurance endurance() const override { return Endurance::Mid; }

  IoStatus read(uint16_t addr, void* dst, uint16_t len) override {
    if (!dst) return IoStatus::OutOfRange;
    if ((uint32_t)addr + (uint32_t)len > sizeBytes()) return IoStatus::OutOfRange;

    uint8_t* p = (uint8_t*)dst;
    for (uint16_t i = 0; i < len; i++) {
      p[i] = EEPROM.read((int)(addr + i));
    }
    return IoStatus::Ok;
  }

  IoStatus write(uint16_t addr, const void* src, uint16_t len) override {
    if (!src) return IoStatus::OutOfRange;
    if ((uint32_t)addr + (uint32_t)len > sizeBytes()) return IoStatus::OutOfRange;
    if (!isWritable()) return IoStatus::ReadOnly;

    const uint8_t* p = (const uint8_t*)src;
    for (uint16_t i = 0; i < len; i++) {
      const int a = (int)(addr + i);
      const uint8_t v = p[i];

      // AVR internal EEPROM: update() is supported and avoids needless wear.
      #if defined(ARDUINO_ARCH_AVR)
        EEPROM.update(a, v);
      #elif defined(HAL_EEPROM_HAS_UPDATE)
        EEPROM.update(a, v);
      #else
        EEPROM.write(a, v);
      #endif
    }
    return IoStatus::Ok;
  }

  bool hasCommit() const override { return false; }
  IoStatus commit() override { return IoStatus::Ok; }
  bool commitDone() const override { return true; }
};
