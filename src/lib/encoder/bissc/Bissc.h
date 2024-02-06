// BISS-C encoders
#pragma once

#include "../Encoder.h"

#ifdef HAS_BISS_C

  // designed according protocol description found in as38-H39e-b-an100.pdf

  // similar broadcom devices have range of 10MHz to 80 KHz
  #ifndef BISSC_CLOCK_RATE_KHZ
    #define BISSC_CLOCK_RATE_KHZ 4000
  #endif

  // default to single turn mode
  #ifndef BISSC_SINGLE_TURN
    #define BISSC_SINGLE_TURN ON
  #endif

  // allow for inverting the signal state to fix incorrect wiring if needed
  #ifndef HIGH_SLO
  #define HIGH_SLO HIGH
  #endif
  #ifndef LOW_SLO
  #define LOW_SLO LOW
  #endif
  #ifndef HIGH_MA
  #define HIGH_MA HIGH
  #endif
  #ifndef LOW_MA
  #define LOW_MA LOW
  #endif

  class Bissc : public Encoder {
    public:
      // get device ready for use
      void init();

      // set encoder origin
      void setOrigin(uint32_t count);

      // read encoder count
      int32_t read();

      // write encoder position
      void write(int32_t count);

    protected:
      // read encoder position with error recovery
      bool readEncLatest(uint32_t &position);

      // read encoder position
      virtual bool readEnc(uint32_t &position);

      uint32_t good = 0;
      uint32_t bad = 0;
      int16_t axis;
      uint16_t nvAddress = 0;

      int16_t maPin;
      int16_t sloPin;

      uint32_t lastValidTime = 0;
      uint32_t lastValidPosition = 0;
  };

#endif
