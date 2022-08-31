// serial channel encoder
#pragma once

#include "../../../../../Common.h"

#ifdef SERIAL_ENCODER

class Encoder {
  public:
    Encoder(int16_t channel = 1);
    int32_t read();
    void write(int32_t position);

  private:
    int32_t raw();

    bool initialized = false;

    char channel[2] = "0";
    int32_t position = 0;
    int32_t offset = 0;

    unsigned long lastRead = 0;
};

#endif
