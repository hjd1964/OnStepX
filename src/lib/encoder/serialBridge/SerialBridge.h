// Serial bridge encoder
#pragma once

#include "../Encoder.h"

#if (AXIS1_ENCODER == SERIAL_BRIDGE || AXIS2_ENCODER == SERIAL_BRIDGE || AXIS3_ENCODER == SERIAL_BRIDGE || \
     AXIS4_ENCODER == SERIAL_BRIDGE || AXIS5_ENCODER == SERIAL_BRIDGE || AXIS6_ENCODER == SERIAL_BRIDGE || \
     AXIS7_ENCODER == SERIAL_BRIDGE || AXIS8_ENCODER == SERIAL_BRIDGE || AXIS9_ENCODER == SERIAL_BRIDGE) && defined(SERIAL_ENCODER)

#ifndef SERIAL_ENCODER_BAUD
  #define SERIAL_ENCODER_BAUD 460800
#endif

class SerialBridge : public Encoder {
  public:
    SerialBridge(int16_t axis);
    int32_t read();
    void write(int32_t count);

  private:
    int32_t raw();

    int32_t offset = 0;

    char channel[2] = "0";
    unsigned long lastReadMillis = 0;
};

#endif
