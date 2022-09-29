// Broadcom AS37-H39B-B BISS-C encoders
#pragma once

#include "../Encoder.h"

#if AXIS1_ENCODER == AS37_H39B_B || AXIS2_ENCODER == AS37_H39B_B || AXIS3_ENCODER == AS37_H39B_B || \
    AXIS4_ENCODER == AS37_H39B_B || AXIS5_ENCODER == AS37_H39B_B || AXIS6_ENCODER == AS37_H39B_B || \
    AXIS7_ENCODER == AS37_H39B_B || AXIS8_ENCODER == AS37_H39B_B || AXIS9_ENCODER == AS37_H39B_B

  // designed according protocol description found in as38-H39e-b-an100.pdf

  // similar broadcom devices have range of 10MHz to 80 KHz
  // the fastest supported rate for this library is 500 KHz
  // rounding makes rates of about 500, 250, 166.66, 125, 100, 83.33 KHz
  #ifndef AS37_CLOCK_RATE_KHZ
  #define AS37_CLOCK_RATE_KHZ 250
  #endif

  class As37h39bb : public Encoder {
    public:
      As37h39bb(int16_t maPin, int16_t sloPin, int16_t axis);
      int32_t read();
      void write(int32_t count);

    private:
      // read encoder position with (5 seconds) error recovery
      bool readEnc5(uint32_t &position);

      // read encoder position
      bool readEnc(uint32_t &position);

      int16_t axis;

      int16_t clkPin;
      int16_t sloPin;

      uint32_t lastValidTime = 0;
      uint32_t lastValidPosition = 0;
  };

#endif
