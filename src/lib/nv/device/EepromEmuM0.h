// NvDeviceEepromEmuM0.h
#pragma once

#include "NvDeviceBase.h"

// Arduino SAMD (M0/M0 Pro/Zero) EEPROM emulation.
// Depending on core, the header may be <EEPROM.h> or <FlashAsEEPROM.h>.
#if defined(ARDUINO_ARCH_SAMD)
  #include <EEPROM.h>
#endif

#ifndef E2END
  #error "E2END must be defined for NvDeviceEepromEmuM0"
#endif

class NvDeviceEepromEmuM0 : public NvDevice {
public:
  bool init() override {
    #if !defined(ARDUINO_ARCH_SAMD)
      VLF("MSG: NvDevice, NOT FOUND!");
      return false;
    #else
      const uint32_t sz = sizeBytes();
      // Match your old guard: M0 emu is expected to be exactly 1024 bytes.
      if (sz != 1024u) return false;

      // Many SAMD EEPROM emu implementations don't require begin(size).
      // If your core provides EEPROM.begin(size), you can add it here.
      VLF("MSG: NvDevice, using built-in 1KB EEPROM via. M0 core library");
      return true;
    #endif
  }

  uint32_t sizeBytes() const override {
    return (uint32_t)(E2END + 1u);
  }

  // Device write cycle endurance
  // Low < 100K writes, Mid > ~100K writes, High > ~100M writes (FRAM)
  Endurance endurance() const override { return Endurance::Low; }

  IoStatus read(uint16_t addr, void* dst, uint16_t len) override {
    #if !defined(ARDUINO_ARCH_SAMD)
      (void)addr; (void)dst; (void)len;
      return IoStatus::IoError;
    #else
      if (!dst) return IoStatus::OutOfRange;
      if ((uint32_t)addr + (uint32_t)len > sizeBytes()) return IoStatus::OutOfRange;

      uint8_t* p = (uint8_t*)dst;
      for (uint16_t i = 0; i < len; i++) {
        p[i] = EEPROM.read((int)(addr + i));
      }
      return IoStatus::Ok;
    #endif
  }

  IoStatus write(uint16_t addr, const void* src, uint16_t len) override {
    #if !defined(ARDUINO_ARCH_SAMD)
      (void)addr; (void)src; (void)len;
      return IoStatus::IoError;
    #else
      if (!src) return IoStatus::OutOfRange;
      if ((uint32_t)addr + (uint32_t)len > sizeBytes()) return IoStatus::OutOfRange;
      if (!isWritable()) return IoStatus::ReadOnly;

      const uint8_t* p = (const uint8_t*)src;
      for (uint16_t i = 0; i < len; i++) {
        EEPROM.write((int)(addr + i), p[i]);
      }
      return IoStatus::Ok;
    #endif
  }

  bool hasCommit() const override { return true; }

  IoStatus commit() override {
    #if !defined(ARDUINO_ARCH_SAMD)
      return IoStatus::IoError;
    #else
      // Most SAMD EEPROM emulation layers expose EEPROM.commit().
      EEPROM.commit();
      return IoStatus::Ok;
    #endif
  }

  bool commitDone() const override { return true; } // commit is synchronous
};
