// -----------------------------------------------------------------------------------
// Simple software SPI routines (CPOL=1, CPHA=1) just for TMC stepper drivers
#pragma once
#include <Arduino.h>
#include "../../Constants.h"

#include "Pins.h"

class SoftSpi {
  public:
    // create and set pin values
    SoftSpi(DriverPins Pins) : Pins{ Pins } {};

    // check pins and report status
    bool init();
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
    const DriverPins Pins = {OFF, OFF, OFF, OFF};
};
