// NvDeviceEepromEmuEsp.h
#pragma once
#include "NvDeviceBase.h"

#if defined(ESP32) || defined(ESP8266)
  #include <EEPROM.h>
#endif

class NvDeviceEepromEmuEsp : public NvDevice {
public:
  bool init() override {
    #if defined(ESP32)
      const uint32_t sz = sizeBytes();
      VLF("MSG: NvDevice, using built-in EEPROM 4KB via. ESP32 core library");
      return EEPROM.begin((int)sz);
    #elif defined(ESP8266)
      const uint32_t sz = sizeBytes();
      VLF("MSG: NvDevice, using built-in EEPROM 4KB via. ESP8266 core library");
      EEPROM.begin((int)sz);
      return true;
    #else
      VLF("MSG: NvDevice, NOT FOUND!");
      return false;
    #endif
  }

  uint32_t sizeBytes() const override {
    return 4096u;
  }

  // Device write cycle endurance
  // Low < 100K writes, Mid > ~100K writes, High > ~100M writes (FRAM)
  Endurance endurance() const override { return Endurance::Low; }

  IoStatus read(uint16_t addr, void* dst, uint16_t len) override {
    #if !(defined(ESP32) || defined(ESP8266))
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
    #if !(defined(ESP32) || defined(ESP8266))
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
    #if !(defined(ESP32) || defined(ESP8266))
      return IoStatus::IoError;
    #else
      bool ok = EEPROM.commit();
      VLF("MSG: Nv, commit");
      return ok ? IoStatus::Ok : IoStatus::IoError;
    #endif
  }

  bool commitDone() const override { return true; } // synchronous in Arduino EEPROM
};
