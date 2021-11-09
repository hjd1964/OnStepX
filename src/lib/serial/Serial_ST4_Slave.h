// -----------------------------------------------------------------------------------
// Serial ST4 slave
#pragma once

/*
ST4 port data communication scheme:

5V power ---- Teensy3.2, Nano, etc.
Gnd ---------

HC              Signal               OnStep
RAw  ----        Data         --->   Recv. data
DEs  <---        Clock        ----   Clock
DEn  <---        Data         ----   Send data
RAe  ---- 12.5Hz Square wave  --->   100% sure SHC is present, switches DEs & DEn to OUTPUT

Data is exchanged on clock edges similar to SPI so is timing insensitive (runs with interrupts enabled.)

One data byte is exchanged (in both directions w/basic error detection and recovery.)  A value 0x00 byte 
means "no data" and is ignored on both sides.  Mega2560 hardware runs at (fastest) 10mS/byte (100 Bps) and 
all others (Teensy3.x, etc.) at 2mS/byte (500 Bps.)
*/

#include "../../Common.h"

#if defined(SERIAL_ST4_SLAVE) && SERIAL_ST4_SLAVE == ON

#include <Stream.h>

#if !defined(ST4_W_PIN) && !defined(ST4_S_PIN) && !defined(ST4_N_PIN) && !defined(ST4_E_PIN)
  #warning "ST4 interface pins aren't defined, using defaults."
  #define ST4_W_PIN 2
  #define ST4_S_PIN 3
  #define ST4_N_PIN 4
  #define ST4_E_PIN 5
#endif

void dataClock();
void shcTone();

class Sst4 : public Stream {
  public:
    void begin(long baudRate);
    void end();
    void paused(bool state);
    bool active();
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *, size_t);
    virtual int available(void);
    virtual int read(void);
    virtual int peek(void);
    virtual void flush(void);

    inline int availableForWrite(void) { return 0; }
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    using Print::write;

    volatile char xmit_buffer[256] = "";
    volatile uint8_t xmit_head     = 0;
    volatile uint8_t xmit_tail     = 0;
    volatile char recv_buffer[256] = "";
    volatile uint8_t recv_tail     = 0;
    volatile unsigned long lastMs  = 0;

  private:
    uint8_t recv_head = 0;
    long time_out = 500;
    bool isActive = false;
};

extern Sst4 SerialST4;
#ifdef SERIAL_ST4
  #undef SERIAL_ST4
#endif
#define SERIAL_ST4 SerialST4

#endif
