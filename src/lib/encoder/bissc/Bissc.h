// BISS-C encoders
#pragma once

#include "../EncoderBase.h"

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
      bool init();

      // set encoder origin
      void setOrigin(uint32_t count);

      // read encoder position
      int32_t read();

      // write encoder position
      void write(int32_t position);

    protected:
      // read encoder count with 1 second error recovery
      // returns encoder count or INT32_MAX on error
      uint32_t getCountWithErrorRecovery(bool now = false);

      // read encoder count
      virtual bool getCount(uint32_t &count);

      uint32_t good = 0;
      uint32_t bad = 0;
      uint16_t nvAddress = 0;

      int16_t maPin;
      int16_t sloPin;

      uint32_t lastValidTime = 0;
      uint32_t lastValidCount = 0;
  };

#endif
