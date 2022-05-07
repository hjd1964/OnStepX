// -----------------------------------------------------------------------------------
// Polling serial for Mega2560
#pragma once

#include "../../Common.h"

#if defined(SERIAL_MEGA2560) && SERIAL_MEGA2560 == ON

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

class PollingSerial : public Stream {
  public:
    PollingSerial();
    
    inline void begin() { begin(9600); }
    virtual void begin(long baud);
    virtual void end();
    inline virtual bool poll(void) { return true; }

    virtual int read(void);

    inline int available(void) { 
      uint8_t p = recv_head;
      uint8_t c = 0;
      while (recv_buffer[p] && p != recv_tail && c < 255) { c++; p++; } return c;
    }

    inline int peek(void) { if (!available()) return -1; char c = recv_buffer[recv_head]; return c; }

    inline void flush(void) { while (poll()) {}; }

    inline size_t write(uint8_t data) {
      //if (xmit_tail == xmit_index) return 0; 
      xmit_buffer[xmit_tail] = data;
      xmit_tail++; xmit_tail &= 0b111111;
      xmit_buffer[xmit_tail] = 0;
      return 1;
    }
    virtual size_t write(const uint8_t* data, size_t count) {
      for (int i = 0; i < (int)count; i++) { if (!write(data[i])) return i; }
      return count;
    }
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }

    using Print::write;

    volatile char recv_buffer[256] = "";
    volatile uint8_t recv_head = 0;
    volatile uint8_t recv_tail = 0;

  protected:
    char xmit_buffer[64] = "";
    uint8_t xmit_index = 0;
    uint8_t xmit_tail = 0;
};

#ifdef HAL_POLLING_MEGA2560_SERIAL_A
class PollingSerialA : public PollingSerial {
  public:
    void begin(long baud);
    void end();
    bool poll(void);

    int read(void);
};
extern PollingSerialA SerialA;
#endif

#ifdef HAL_POLLING_MEGA2560_SERIAL_B
class PollingSerialB : public PollingSerial {
  public:
    void begin(long baud);
    void end();
    bool poll(void);

    int read(void);
};
extern PollingSerialB SerialB;
#endif

#ifdef HAL_POLLING_MEGA2560_SERIAL_C
class PollingSerialC : public PollingSerial {
  public:
    void begin(long baud);
    void end();
    bool poll(void);

    int read(void);
};
extern PollingSerialC SerialC;
#endif

#ifdef HAL_POLLING_MEGA2560_SERIAL_D
class PollingSerialD : public PollingSerial {
  public:
    void begin(long baud);
    void end();
    bool poll(void);

    int read(void);
};

extern PollingSerialD SerialD;

#endif

#endif

#endif
