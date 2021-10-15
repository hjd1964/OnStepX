// ------------------------------------------------------------------------------------
// Serial ST4 master

#pragma once

#include "../../Common.h"

#if defined(SERIAL_ST4_MASTER) && SERIAL_ST4_MASTER == ON

#define SST4_CLOCK_OUT ST4_DEC_S_PIN
#define SST4_DATA_OUT  ST4_DEC_N_PIN
#define SST4_DATA_IN   ST4_RA_W_PIN
#define SST4_TONE      ST4_RA_E_PIN

#if SST4_CLOCK_OUT != OFF && SST4_DATA_OUT != OFF && SST4_DATA_IN != OFF && SST4_TONE != OFF

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

#include "Stream.h"

class SerialST4Master : public Stream {
  public:
    void begin();
    void begin(long baud);
    
    void end();

    // recvs one char and transmits one char to/from buffers; recvd chars < 32 are returned directly and bypass the buffer
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
    bool trans(char *data_in, uint8_t data_out);

    char xmit_buffer[256] = "";
    uint8_t xmit_head     = 0;
    uint8_t xmit_tail     = 0;
    char recv_buffer[256] = "";
    uint8_t recv_tail     = 0;
    bool frame_error      = false;
    bool send_error       = false;
    bool recv_error       = false;
    uint8_t recv_head     = 0;
    uint32_t timeout      = 80;
    unsigned long lastMicros = 0;
};

extern SerialST4Master serialST4;
#define SERIAL_ST4 serialST4

#endif

#endif
