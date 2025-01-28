// ------------------------------------------------------------------------------------
// Serial ST4 master
#pragma once

/*
ST4 port data communication scheme:

5V power ---- ESP32-S, Teensy4.0, Teensy3.2, etc.
Gnd ---------

HC              Signal               OnStep
RAw  ----        Data         --->   Recv. data
DEs  <---        Clock        ----   Clock
DEn  <---        Data         ----   Send data
RAe  ---- 12.5Hz Square wave  --->   100% sure SHC is present, switches DEs & DEn to OUTPUT

Data is exchanged on clock edges similar to SPI so is timing insensitive (runs with interrupts enabled.)

One data byte is exchanged (in both directions w/basic error detection and recovery.)  A value 0x00 byte 
means "no data" and is ignored on both sides.
*/

#include "../../Common.h"

#if defined(SERIAL_ST4_MASTER) && SERIAL_ST4_MASTER == ON

#include "Stream.h"

class SerialST4Master : public Stream {
  public:
    void begin();
    void begin(long baud);
    
    void end();

    // call periodically to move data to/from buffers across the SerialST4 interface
    // recvd chars between 0 and 32 are returned directly and bypass the buffer
    // 24 calls moves one byte of data in both directions so 100us/call results in a data rate of about 4200 bps (350 cps) 
    char poll();

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
    using Print::write;

  private:
    bool trans(char *data_in);

    char xmit_buffer[256] = "";
    uint8_t xmit_head = 0;
    uint8_t xmit_tail = 0;
    char recv_buffer[256] = "";
    uint8_t recv_tail = 0;
    bool frame_error = false;
    bool send_error = false;
    bool recv_error = false;
    uint8_t recv_head = 0;
    uint32_t timeout = 80;
};

extern SerialST4Master serialST4;
#define SERIAL_ST4 serialST4

#endif
