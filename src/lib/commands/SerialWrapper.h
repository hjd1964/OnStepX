// -----------------------------------------------------------------------------------
// SerialWrapper a single class to allow uniform access to other serial port classes
#pragma once

#include "../../Common.h"

static uint8_t _wrapper_channels = 0;

#define isChannel(x) (x == thisChannel)

class SerialWrapper : public Stream {
  public:
    SerialWrapper();
    
    void begin();
    void begin(long baud);
    
    void end();

    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *, size_t);
    virtual int available(void);
    virtual int read(void);
    virtual int peek(void);
    virtual void flush(void);

    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }

    inline bool hasChannel(uint8_t channel) { return bitRead(_wrapper_channels, channel); }
    inline void setChannel(uint8_t channel) { bitSet(_wrapper_channels, channel); }
    inline void clrChannel(uint8_t channel) { bitClear(_wrapper_channels, channel); }

    using Print::write;

  private:
    uint8_t thisChannel = 0;
};
