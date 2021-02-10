// -----------------------------------------------------------------------------------
// SerialWrapper a single class to allow uniform access to other serial port classes
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../Extended.Config.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"

#include "SerialWrapper.h"

SerialWrapper::SerialWrapper() {
  static uint8_t channel = 0;
#ifdef SERIAL_A
  if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
  channel++;
#endif
#ifdef SERIAL_B
  if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
  channel++;
#endif
#ifdef SERIAL_C
  if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
  channel++;
#endif
#ifdef SERIAL_D
  if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
  channel++;
#endif
#ifdef SERIAL_ST4
  if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
  channel++;
#endif
}

void SerialWrapper::begin() {
  begin(9600);
}

void SerialWrapper::begin(long baud) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(channel)) SERIAL_A.begin(baud);
  channel++;
#endif
#ifdef SERIAL_B
  if (isChannel(channel)) SERIAL_B.begin(baud);
  channel++;
#endif
#ifdef SERIAL_C
  if (isChannel(channel)) SERIAL_C.begin(baud);
  channel++;
#endif
#ifdef SERIAL_D
  if (isChannel(channel)) SERIAL_D.begin(baud);
  channel++;
#endif
#ifdef SERIAL_ST4
  if (isChannel(channel)) SERIAL_ST4.begin(baud);
  channel++;
#endif
}

void SerialWrapper::end() {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(channel)) SERIAL_A.end();
  channel++;
#endif
#ifdef SERIAL_B
  if (isChannel(channel)) SERIAL_B.end();
  channel++;
#endif
#ifdef SERIAL_C
  if (isChannel(channel)) SERIAL_C.end();
  channel++;
#endif
#ifdef SERIAL_D
  if (isChannel(channel)) SERIAL_D.end();
  channel++;
#endif
#ifdef SERIAL_ST4
  if (isChannel(channel)) SERIAL_ST4.end();
  channel++;
#endif
}

size_t SerialWrapper::write(uint8_t data) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(channel)) return SERIAL_A.write(data);
  channel++;
#endif
#ifdef SERIAL_B
  if (isChannel(channel)) return SERIAL_B.write(data);
  channel++;
#endif
#ifdef SERIAL_C
  if (isChannel(channel)) return SERIAL_C.write(data);
  channel++;
#endif
#ifdef SERIAL_D
  if (isChannel(channel)) return SERIAL_D.write(data);
  channel++;
#endif
#ifdef SERIAL_ST4
  if (isChannel(channel)) return SERIAL_ST4.write(data);
  channel++;
#endif
  return -1;
}

size_t SerialWrapper::write(const uint8_t *data, size_t quantity) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(channel)) return SERIAL_A.write(data, quantity);
  channel++;
#endif
#ifdef SERIAL_B
  if (isChannel(channel)) return SERIAL_B.write(data, quantity);
  channel++;
#endif
#ifdef SERIAL_C
  if (isChannel(channel)) return SERIAL_C.write(data, quantity);
  channel++;
#endif
#ifdef SERIAL_D
  if (isChannel(channel)) return SERIAL_D.write(data, quantity);
  channel++;
#endif
#ifdef SERIAL_ST4
  if (isChannel(channel)) return SERIAL_ST4.write(data, quantity);
  channel++;
#endif
  return -1;
}

int SerialWrapper::available(void) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(channel)) return SERIAL_A.available();
  channel++;
#endif
#ifdef SERIAL_B
  if (isChannel(channel)) return SERIAL_B.available();
  channel++;
#endif
#ifdef SERIAL_C
  if (isChannel(channel)) return SERIAL_C.available();
  channel++;
#endif
#ifdef SERIAL_D
  if (isChannel(channel)) return SERIAL_D.available();
  channel++;
#endif
#ifdef SERIAL_ST4
  if (isChannel(channel)) return SERIAL_ST4.available();
  channel++;
#endif
  return 0;
}

int SerialWrapper::read(void) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(channel)) return SERIAL_A.read();
  channel++;
#endif
#ifdef SERIAL_B
  if (isChannel(channel)) return SERIAL_B.read();
  channel++;
#endif
#ifdef SERIAL_C
  if (isChannel(channel)) return SERIAL_C.read();
  channel++;
#endif
#ifdef SERIAL_D
  if (isChannel(channel)) return SERIAL_D.read();
  channel++;
#endif
#ifdef SERIAL_ST4
  if (isChannel(channel)) return SERIAL_ST4.read();
  channel++;
#endif
  return -1;
}

int SerialWrapper::peek(void) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(channel)) return SERIAL_A.peek();
  channel++;
#endif
#ifdef SERIAL_B
  if (isChannel(channel)) return SERIAL_B.peek();
  channel++;
#endif
#ifdef SERIAL_C
  if (isChannel(channel)) return SERIAL_C.peek();
  channel++;
#endif
#ifdef SERIAL_D
  if (isChannel(channel)) return SERIAL_D.peek();
  channel++;
#endif
#ifdef SERIAL_ST4
  if (isChannel(channel)) return SERIAL_ST4.peek();
  channel++;
#endif
  return -1;
}

void SerialWrapper::flush(void) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(channel)) SERIAL_A.flush();
  channel++;
#endif
#ifdef SERIAL_B
  if (isChannel(channel)) SERIAL_B.flush();
  channel++;
#endif
#ifdef SERIAL_C
  if (isChannel(channel)) SERIAL_C.flush();
  channel++;
#endif
#ifdef SERIAL_D
  if (isChannel(channel)) SERIAL_D.flush();
  channel++;
#endif
#ifdef SERIAL_ST4
  if (isChannel(channel)) SERIAL_ST4.flush();
  channel++;
#endif
}
