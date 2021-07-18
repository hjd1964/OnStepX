// -----------------------------------------------------------------------------------
// Simple software SPI routines (CPOL=1, CPHA=1) just for TMC stepper drivers
#pragma once

#include <Arduino.h>

class SoftSpi {
  public:
    // check pins and report status
    bool init(int16_t mosi, int16_t sck, int16_t cs, int16_t miso);
    // setup pins and activate CS
    void begin();
    // cycle CS to reset conversation
    void pause();
    // set CS high to disable conversation
    void end();
    // send/receive an 8 bit value
    uint8_t transfer(uint8_t data_out);
    // send/receive an 32 bit value
    uint32_t transfer32(uint32_t data_out);

  private:
    int16_t miso, mosi, sck, cs;
};
