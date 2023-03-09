// -----------------------------------------------------------------------------------
// Local Serial for sending internal commands

#include "Serial_Local.h"

#if defined(SERIAL_LOCAL_MODE) && SERIAL_LOCAL_MODE == ON

void SerialLocal::begin(long baud) {
  // init the buffers
  xmit_index = 0;
  xmit_buffer[0] = 0;
  recv_head = 0;
  recv_tail = 0;
  recv_buffer[0] = 0;
  (void)(baud);

  #ifdef ESP32
    mutex = xSemaphoreCreateMutex();
  #endif
}

void SerialLocal::end() { }

void SerialLocal::transmit(const char *data) {
  #ifdef ESP32
    xSemaphoreTake(mutex, portMAX_DELAY);
  #endif

  int data_len = strlen(data);
  for (int i = 0; i < data_len; i++) {
    recv_buffer[recv_tail] = data[i];
    recv_tail++;
  }

  #ifdef ESP32
    xSemaphoreGive(mutex);
  #endif
}

char *SerialLocal::receive() {
  #ifdef ESP32
    xSemaphoreTake(mutex, portMAX_DELAY);
  #endif

  int i = 0;
  xmit_result[i] = 0;
  while (xmit_buffer[xmit_index] != 0) {
    xmit_result[i++] = xmit_buffer[xmit_index++];
    xmit_index &= 0b1111111;
  }
  xmit_result[i] = 0; 

  #ifdef ESP32
    xSemaphoreGive(mutex);
  #endif
  return xmit_result;
}

int SerialLocal::read(void) {
  if (!available()) return -1;

  #ifdef ESP32
    xSemaphoreTake(mutex, portMAX_DELAY);
  #endif

  recv_buffer[recv_tail] = 0;
  char c = recv_buffer[recv_head];
  if (c) recv_head++;

  #ifdef ESP32
    xSemaphoreGive(mutex);
  #endif
  return c;
}

SerialLocal serialLocal;

#endif
