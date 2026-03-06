// NvDeviceMb85rcI2c.h
#pragma once

#include "NvDeviceBase.h"
#include <Wire.h>

template<uint32_t DeviceBytes>
class NvDeviceMb85rcI2C : public NvDevice {
public:
  explicit NvDeviceMb85rcI2C(TwoWire& wire, uint8_t addr7)
    : wire_(&wire), addr_(addr7) {}

  bool init() override {
    wire_->begin();
    wire_->beginTransmission(addr_);
    bool success = (wire_->endTransmission() == 0);
    VF("MSG: NvDevice, MB85RC "); V(sizeBytes()/1024); VF("KB I2C FRAM ");
    if (success) { VLF("found"); } else { VLF("NOT FOUND!"); }
    return success;
  }

  uint32_t sizeBytes() const override { return DeviceBytes; }

  // Device write cycle endurance
  // Low < 100K writes, Mid > ~100K writes, High > ~100M writes (FRAM)
  Endurance endurance() const override { return Endurance::High; }

  IoStatus read(uint16_t addr, void* dst, uint16_t len) override {
    if (!dst) return IoStatus::OutOfRange;
    if ((uint32_t)addr + (uint32_t)len > sizeBytes()) return IoStatus::OutOfRange;

    uint8_t* out = (uint8_t*)dst;

    // 24xx-like simple loop (byte-at-a-time). You can later optimize to chunked streaming reads if desired.
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
    if (!isWritable()) return IoStatus::ReadOnly;

    const uint8_t* in = (const uint8_t*)src;

    // 24xx-like simple loop (byte-at-a-time)
    for (uint16_t i = 0; i < len; i++) {
      const uint16_t a = (uint16_t)(addr + i);

      wire_->beginTransmission(addr_);
      wire_->write((uint8_t)(a >> 8));
      wire_->write((uint8_t)(a & 0xFF));
      wire_->write(in[i]);
      if (wire_->endTransmission() != 0) return IoStatus::IoError;
    }

    return IoStatus::Ok;
  }

  bool supportsReadBlock16() const override { return true; }

  // Accelerator: 16-byte block read in one I2C transaction
  IoStatus readBlock16(uint16_t blockIdx, void* dst16) override {
    if (!dst16) return IoStatus::OutOfRange;
    const uint16_t addr = (uint16_t)(blockIdx << 4);
    if ((uint32_t)addr + 16u > sizeBytes()) return IoStatus::OutOfRange;

    // Set address pointer, keep bus (repeated start)
    wire_->beginTransmission(addr_);
    wire_->write((uint8_t)(addr >> 8));
    wire_->write((uint8_t)(addr & 0xFF));
    if (wire_->endTransmission(false) != 0) return IoStatus::IoError;

    // Read 16 bytes
    if (wire_->requestFrom((int)addr_, (int)16) != 16) return IoStatus::IoError;

    uint8_t* out = (uint8_t*)dst16;
    for (uint8_t i = 0; i < 16; i++) {
      if (!wire_->available()) return IoStatus::IoError;
      out[i] = (uint8_t)wire_->read();
    }

    return IoStatus::Ok;
  }

  bool supportsWriteBlock16() const override { return true; }

  // Accelerator: 16-byte block write in one I2C transaction
  IoStatus writeBlock16(uint16_t blockIdx, const void* src16) override {
    if (!src16) return IoStatus::OutOfRange;
    const uint16_t addr = (uint16_t)(blockIdx << 4);
    if ((uint32_t)addr + 16u > sizeBytes()) return IoStatus::OutOfRange;
    if (!isWritable()) return IoStatus::ReadOnly;

    wire_->beginTransmission(addr_);
    wire_->write((uint8_t)(addr >> 8));
    wire_->write((uint8_t)(addr & 0xFF));

    const uint8_t* p = (const uint8_t*)src16;
    for (uint8_t i = 0; i < 16; i++) wire_->write(p[i]);
    if (wire_->endTransmission() != 0) return IoStatus::IoError;

    return IoStatus::Ok;
  }

  // FRAM has no commit barrier.
  bool hasCommit() const override { return false; }
  IoStatus commit() override { return IoStatus::Ok; }
  bool commitDone() const override { return true; }

private:

  TwoWire*  wire_ = nullptr;
  uint8_t   addr_ = 0;
};
