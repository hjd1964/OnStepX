// -----------------------------------------------------------------------------------
// Local Serial for sending internal commands
#pragma once

#include "../../Common.h"

#if defined(SERIAL_LOCAL_MODE) && SERIAL_LOCAL_MODE == ON

class SerialLocal : public Stream {
  public:
    inline void begin() { begin(9600); }
    void begin(long baud);
    void end();

    void setTimeout(long timeMs) { UNUSED(timeMs); }

    // sends a command for processing
    void transmit(const char *data);

    // receive has the last commands response, if one exists
    char *receive();

    int read(void);

    inline int receiveAvailable() {
      #ifdef ESP32
        xSemaphoreTake(mutex, portMAX_DELAY);
      #endif

      int i = 0;
      int x = xmit_index;
      while (xmit_buffer[xmit_index] != 0) {
        i++;
        xmit_index++;
        xmit_index &= 0b1111111;
      }
      xmit_index = x;

      #ifdef ESP32
        xSemaphoreGive(mutex);
      #endif
      return i;
    }

    inline int available(void) { 
      #ifdef ESP32
        xSemaphoreTake(mutex, portMAX_DELAY);
      #endif

      uint8_t p = recv_head;
      uint8_t c = 0;
      while (recv_buffer[p] && p != recv_tail && c < 255) { c++; p++; }

      #ifdef ESP32
        xSemaphoreGive(mutex);
      #endif
      return c;
    }

    inline int peek(void) {
      if (!available()) return -1;

      #ifdef ESP32
        xSemaphoreTake(mutex, portMAX_DELAY);
      #endif

      char c = recv_buffer[recv_head];

      #ifdef ESP32
        xSemaphoreGive(mutex);
      #endif
      return c;
    }

    inline void flush(void) {
      #ifdef ESP32
        while (receiveAvailable()) { yield(); }
      #endif
    }

    inline size_t write(uint8_t data) {
      #ifdef ESP32
        xSemaphoreTake(mutex, portMAX_DELAY);
      #endif

      xmit_buffer[xmit_tail] = data;
      xmit_tail++;
      xmit_tail &= 0b1111111;
      xmit_buffer[xmit_tail] = 0;

      #ifdef ESP32
        xSemaphoreGive(mutex);
      #endif
      return 1;
    }

    size_t write(const uint8_t* data, size_t count) {
      for (int i = 0; i < (int)count; i++) { if (!write(data[i])) return i; }
      return count;
    }

    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }

    using Print::write;

  private:
    char recv_buffer[256] = "";
    uint8_t recv_head = 0;
    uint8_t recv_tail = 0;

    char xmit_buffer[128] = "";
    char xmit_result[128] = "";
    uint8_t xmit_index = 0;
    uint8_t xmit_tail = 0;
    
    #ifdef ESP32
      SemaphoreHandle_t mutex;
    #endif
};

extern SerialLocal serialLocal;

#define SERIAL_LOCAL serialLocal

#endif
