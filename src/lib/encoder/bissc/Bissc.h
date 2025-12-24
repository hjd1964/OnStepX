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

  // default sync phase
  #ifndef BISSC_SYNC_PHASE
    #define BISSC_SYNC_PHASE 20
  #endif

  // default ack phase
  #ifndef BISSC_ACK_PHASE
    #define BISSC_ACK_PHASE 20
  #endif

  // resolution adjustment use 2, 4, or 8 (example:)
  //#define BISSC_RESOLUTION_DIVISOR 4

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
      void setOrigin(int32_t counts);

      // read encoder position
      int32_t read();

      // write encoder position
      void write(int32_t position);

    protected:
      // read encoder count with 1 second error recovery
      // returns encoder count or INT32_MAX on error
      int32_t getCountWithErrorRecovery(bool immediate = false);

      // read encoder count
      bool getCount(uint32_t &count);

      // low level bitbang read encoder count
      IRAM_ATTR void getCountBitBang(uint32_t &count);

      // the custom crc routine
      virtual uint8_t crc6(uint64_t data) = 0;

      bool foundAck = false;
      bool foundStart = false;
      bool foundCds = false;

      uint8_t  nErr = 1;
      uint8_t  nWrn = 1;
      uint8_t  frameCrc = 0;

      uint32_t turns = 0;

      // this design allows for 8 to 31 bit encoders
      char encoderName[16] = {0};
      uint8_t encoderBits = 0;
      uint32_t encoderCounts = 0;
      int32_t encoderHalfCounts = 0;
      uint8_t encoderMultiTurnBits = 0;
      uint32_t encoderMultiTurnMask = 0;
      bool encoderWrnInvert = true;
      bool encoderErrInvert = true;

      uint32_t good = 0;
      uint32_t bad = 0;
      uint16_t nvAddress = 0;

      // bit delay in nanoseconds
      const uint32_t rate = 500000/BISSC_CLOCK_RATE_KHZ;
      const uint32_t tSample = (500000/BISSC_CLOCK_RATE_KHZ) >> 2;

      int16_t maPin;
      int16_t sloPin;

      uint32_t lastValidTime = 0;
      int32_t lastValidCount = 0;
  };

#endif
