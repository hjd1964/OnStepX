// NvDevice24xxI2C.h
#pragma once

#include "NvDeviceBase.h"
#include <Wire.h>

#ifndef EEPROM_ACK_SKIP_MS
  #define EEPROM_ACK_SKIP_MS 20
#endif

#ifndef EEPROM_ACK_TIMEOUT_MS
  #define EEPROM_ACK_TIMEOUT_MS 50
#endif

template<uint32_t DeviceBytes>
class NvDevice24xxI2C : public NvDevice {
public:
  explicit NvDevice24xxI2C(TwoWire& wire, uint8_t addr7)
    : wire_(&wire), addr_(addr7) {}

  bool init() override {
    wire_->begin();
    wire_->beginTransmission(addr_);
    bool success = (wire_->endTransmission() == 0);
    VF("MSG: NvDevice, 24xx "); V(sizeBytes()/1024); VF("KB I2C EEPROM ");
    if (success) { VLF("found"); } else { VLF("NOT FOUND!"); }
    return success;
  }

  uint32_t sizeBytes() const override { return DeviceBytes; }

  // Device write cycle endurance
  // Low < 100K writes, Mid > ~100K writes, High > ~100M writes (FRAM)
  Endurance endurance() const override { return Endurance::Mid; }

  IoStatus read(uint16_t addr, void* dst, uint16_t len) override {
    if (!dst) return IoStatus::OutOfRange;
    if ((uint32_t)addr + (uint32_t)len > sizeBytes()) return IoStatus::OutOfRange;

    uint8_t* out = (uint8_t*)dst;

    if (!waitForDevice_()) return IoStatus::NotReady;

    for (uint16_t i = 0; i < len; i++) {
      const uint16_t a = (uint16_t)(addr + i);

      wire_->beginTransmission(addr_);
      wire_->write((uint8_t)(a >> 8));
      wire_->write((uint8_t)(a & 0xFF));
      if (wire_->endTransmission(false) != 0) return IoStatus::IoError;

      if (wire_->requestFrom((int)addr_, (int)1) != 1) return IoStatus::IoError;
      if (!wire_->available()) return IoStatus::IoError;
      out[i] = (uint8_t)wire_->read();
    }
    return IoStatus::Ok;
  }

  IoStatus write(uint16_t addr, const void* src, uint16_t len) override {
    if (!src) return IoStatus::OutOfRange;
    if ((uint32_t)addr + (uint32_t)len > sizeBytes()) return IoStatus::OutOfRange;

    const uint8_t* in = (const uint8_t*)src;

    for (uint16_t i = 0; i < len; i++) {
      const uint16_t a = (uint16_t)(addr + i);

      if (!waitForDevice_()) return IoStatus::NotReady;

      wire_->beginTransmission(addr_);
      wire_->write((uint8_t)(a >> 8));
      wire_->write((uint8_t)(a & 0xFF));
      wire_->write(in[i]);
      if (wire_->endTransmission() != 0) return IoStatus::IoError;

      lastWriteTimeMs_ = millis();
    }
    return IoStatus::Ok;
  }

  bool supportsReadBlock16() const override { return true; }

  IoStatus readBlock16(uint16_t blockIdx, void* dst16) override {
    if (!dst16) return IoStatus::OutOfRange;

    const uint16_t addr = (uint16_t)(blockIdx << 4);
    if ((uint32_t)addr + 16u > sizeBytes()) return IoStatus::OutOfRange;

    if (!waitForDevice_()) return IoStatus::NotReady;

    wire_->beginTransmission(addr_);
    wire_->write((uint8_t)(addr >> 8));
    wire_->write((uint8_t)(addr & 0xFF));
    if (wire_->endTransmission(false) != 0) return IoStatus::IoError;

    if (wire_->requestFrom((int)addr_, (int)16) != 16) return IoStatus::IoError;

    uint8_t* out = (uint8_t*)dst16;
    for (uint8_t i = 0; i < 16; i++) {
      if (!wire_->available()) return IoStatus::IoError;
      out[i] = (uint8_t)wire_->read();
    }
    return IoStatus::Ok;
  }

  bool supportsWriteBlock16() const override { return true; }

  IoStatus writeBlock16(uint16_t blockIdx, const void* src16) override {
    if (!src16) return IoStatus::OutOfRange;

    const uint16_t addr = (uint16_t)(blockIdx << 4);
    if ((uint32_t)addr + 16u > sizeBytes()) return IoStatus::OutOfRange;

    if (!waitForDevice_()) return IoStatus::NotReady;

    wire_->beginTransmission(addr_);
    wire_->write((uint8_t)(addr >> 8));
    wire_->write((uint8_t)(addr & 0xFF));

    const uint8_t* p = (const uint8_t*)src16;
    for (uint8_t i = 0; i < 16; i++) wire_->write(p[i]);

    if (wire_->endTransmission() != 0) return IoStatus::IoError;

    lastWriteTimeMs_ = millis();
    return IoStatus::Ok;
  }

private:
  bool waitForDevice_() {
    if (lastWriteTimeMs_ == 0) return true;

    const uint32_t now = millis();
    if ((uint32_t)(now - lastWriteTimeMs_) >= (uint32_t)EEPROM_ACK_SKIP_MS) return true;

    const uint32_t t0 = now;
    while ((uint32_t)(millis() - t0) < (uint32_t)EEPROM_ACK_TIMEOUT_MS) {
      wire_->beginTransmission(addr_);
      if (wire_->endTransmission() == 0) return true;
    }
    return false;
  }

  uint32_t lastWriteTimeMs_ = 0;
  TwoWire*  wire_ = nullptr;
  uint8_t   addr_ = 0;
};
