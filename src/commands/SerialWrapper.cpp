// -----------------------------------------------------------------------------------
// SerialWrapper a single class to allow uniform access to other serial port classes

#include "../Common.h"

#ifdef ESP32
  #include "../lib/serial/Serial_IP_ESP32.h"
#endif

#ifdef MOUNT_PRESENT
  #if ST4_INTERFACE == ON && ST4_HAND_CONTROL == ON
    #include "../lib/serial/Serial_ST4_Master.h"
  #endif
  #include "../lib/serial/Serial_Local.h"
#endif

#include "SerialWrapper.h"

#ifdef HWSERIAL_1
  HardwareSerial HWSerial1(HWSERIAL_1_RX, HWSERIAL_1_TX);
#endif

#ifdef HWSERIAL_2
  HardwareSerial HWSerial2(HWSERIAL_2_RX, HWSERIAL_2_TX);
#endif

#ifdef HWSERIAL_3
  HardwareSerial HWSerial3(HWSERIAL_3_RX, HWSERIAL_3_TX);
#endif

#if SERIAL_BT_MODE == SLAVE
  #include <BluetoothSerial.h>
  BluetoothSerial bluetoothSerial;
  #define SERIAL_BT bluetoothSerial
#endif

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
  #ifdef SERIAL_BT
    if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
    channel++;
  #endif
  #ifdef SERIAL_IP
    if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
    channel++;
  #endif
  #ifdef SERIAL_LOCAL
    if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
    channel++;
  #endif
}

void SerialWrapper::begin() { begin(9600); }

void SerialWrapper::begin(long baud) {
  uint8_t channel = 0;
  #ifdef SERIAL_A
    if (isChannel(channel++)) SERIAL_A.begin(baud);
  #endif
  #ifdef SERIAL_B
    if (isChannel(channel++)) SERIAL_B.begin(baud);
  #endif
  #ifdef SERIAL_C
    if (isChannel(channel++)) SERIAL_C.begin(baud);
  #endif
  #ifdef SERIAL_D
    if (isChannel(channel++)) SERIAL_D.begin(baud);
  #endif
  #ifdef SERIAL_ST4
    if (isChannel(channel++)) SERIAL_ST4.begin(baud);
  #endif
  #ifdef SERIAL_BT
    if (isChannel(channel++)) { delay(1000); SERIAL_BT.begin(SERIAL_BT_NAME); delay(1000); }
  #endif
  #ifdef SERIAL_IP
    if (isChannel(channel++)) SERIAL_IP.begin(9999);
  #endif
  #ifdef SERIAL_LOCAL
    if (isChannel(channel++)) SERIAL_LOCAL.begin(9999);
  #endif
}

void SerialWrapper::end() {
  uint8_t channel = 0;
  #ifdef SERIAL_A
    if (isChannel(channel++)) SERIAL_A.end();
  #endif
  #ifdef SERIAL_B
    if (isChannel(channel++)) SERIAL_B.end();
  #endif
  #ifdef SERIAL_C
    if (isChannel(channel++)) SERIAL_C.end();
  #endif
  #ifdef SERIAL_D
    if (isChannel(channel++)) SERIAL_D.end();
  #endif
  #ifdef SERIAL_ST4
    if (isChannel(channel++)) SERIAL_ST4.end();
  #endif
  #ifdef SERIAL_BT
    if (isChannel(channel++)) SERIAL_BT.end();
  #endif
  #ifdef SERIAL_IP
    if (isChannel(channel++)) SERIAL_IP.end();
  #endif
  #ifdef SERIAL_LOCAL
    if (isChannel(channel++)) SERIAL_LOCAL.end();
  #endif
}

size_t SerialWrapper::write(uint8_t data) {
  uint8_t channel = 0;
  #ifdef SERIAL_A
    if (isChannel(channel++)) return SERIAL_A.write(data);
  #endif
  #ifdef SERIAL_B
    if (isChannel(channel++)) return SERIAL_B.write(data);
  #endif
  #ifdef SERIAL_C
    if (isChannel(channel++)) return SERIAL_C.write(data);
  #endif
  #ifdef SERIAL_D
    if (isChannel(channel++)) return SERIAL_D.write(data);
  #endif
  #ifdef SERIAL_ST4
    if (isChannel(channel++)) return SERIAL_ST4.write(data);
  #endif
  #ifdef SERIAL_BT
    if (isChannel(channel++)) return SERIAL_BT.write(data);
  #endif
  #ifdef SERIAL_IP
    if (isChannel(channel++)) return SERIAL_IP.write(data);
  #endif
  #ifdef SERIAL_LOCAL
    if (isChannel(channel++)) return SERIAL_LOCAL.write(data);
  #endif
  return -1;
}

size_t SerialWrapper::write(const uint8_t *data, size_t quantity) {
  uint8_t channel = 0;
  #ifdef SERIAL_A
    if (isChannel(channel++)) return SERIAL_A.write(data, quantity);
  #endif
  #ifdef SERIAL_B
    if (isChannel(channel++)) return SERIAL_B.write(data, quantity);
  #endif
  #ifdef SERIAL_C
    if (isChannel(channel++)) return SERIAL_C.write(data, quantity);
  #endif
  #ifdef SERIAL_D
    if (isChannel(channel++)) return SERIAL_D.write(data, quantity);
  #endif
  #ifdef SERIAL_ST4
    if (isChannel(channel++)) return SERIAL_ST4.write(data, quantity);
  #endif
  #ifdef SERIAL_BT
    if (isChannel(channel++)) return SERIAL_BT.write(data, quantity);
  #endif
  #ifdef SERIAL_IP
    if (isChannel(channel++)) return SERIAL_IP.write(data, quantity);
  #endif
  #ifdef SERIAL_LOCAL
    if (isChannel(channel++)) return SERIAL_LOCAL.write(data, quantity);
  #endif
  return -1;
}

int SerialWrapper::available(void) {
  uint8_t channel = 0;
  #ifdef SERIAL_A
    if (isChannel(channel++)) return SERIAL_A.available();
  #endif
  #ifdef SERIAL_B
    if (isChannel(channel++)) return SERIAL_B.available();
  #endif
  #ifdef SERIAL_C
    if (isChannel(channel++)) return SERIAL_C.available();
  #endif
  #ifdef SERIAL_D
    if (isChannel(channel++)) return SERIAL_D.available();
  #endif
  #ifdef SERIAL_ST4
    if (isChannel(channel++)) return SERIAL_ST4.available();
  #endif
  #ifdef SERIAL_BT
    if (isChannel(channel++)) return SERIAL_BT.available();
  #endif
  #ifdef SERIAL_IP
    if (isChannel(channel++)) return SERIAL_IP.available();
  #endif
  #ifdef SERIAL_LOCAL
    if (isChannel(channel++)) return SERIAL_LOCAL.available();
  #endif
  return 0;
}

int SerialWrapper::read(void) {
  uint8_t channel = 0;
  #ifdef SERIAL_A
    if (isChannel(channel++)) return SERIAL_A.read();
  #endif
  #ifdef SERIAL_B
    if (isChannel(channel++)) return SERIAL_B.read();
  #endif
  #ifdef SERIAL_C
    if (isChannel(channel++)) return SERIAL_C.read();
  #endif
  #ifdef SERIAL_D
    if (isChannel(channel++)) return SERIAL_D.read();
  #endif
  #ifdef SERIAL_ST4
    if (isChannel(channel++)) return SERIAL_ST4.read();
  #endif
  #ifdef SERIAL_BT
    if (isChannel(channel++)) return SERIAL_BT.read();
  #endif
  #ifdef SERIAL_IP
    if (isChannel(channel++)) return SERIAL_IP.read();
  #endif
  #ifdef SERIAL_LOCAL
    if (isChannel(channel++)) return SERIAL_LOCAL.read();
  #endif
  return -1;
}

int SerialWrapper::peek(void) {
  uint8_t channel = 0;
  #ifdef SERIAL_A
    if (isChannel(channel++)) return SERIAL_A.peek();
  #endif
  #ifdef SERIAL_B
    if (isChannel(channel++)) return SERIAL_B.peek();
  #endif
  #ifdef SERIAL_C
    if (isChannel(channel++)) return SERIAL_C.peek();
  #endif
  #ifdef SERIAL_D
    if (isChannel(channel++)) return SERIAL_D.peek();
  #endif
  #ifdef SERIAL_ST4
    if (isChannel(channel++)) return SERIAL_ST4.peek();
  #endif
  #ifdef SERIAL_BT
    if (isChannel(channel++)) return SERIAL_BT.peek();
  #endif
  #ifdef SERIAL_IP
    if (isChannel(channel++)) return SERIAL_IP.peek();
  #endif
  #ifdef SERIAL_LOCAL
    if (isChannel(channel++)) return SERIAL_LOCAL.peek();
  #endif
  return -1;
}

void SerialWrapper::flush(void) {
  uint8_t channel = 0;
  #ifdef SERIAL_A
    if (isChannel(channel++)) SERIAL_A.flush();
  #endif
  #ifdef SERIAL_B
    if (isChannel(channel++)) SERIAL_B.flush();
  #endif
  #ifdef SERIAL_C
    if (isChannel(channel++)) SERIAL_C.flush();
  #endif
  #ifdef SERIAL_D
    if (isChannel(channel++)) SERIAL_D.flush();
  #endif
  #ifdef SERIAL_ST4
    if (isChannel(channel++)) SERIAL_ST4.flush();
  #endif
  #ifdef SERIAL_BT
    if (isChannel(channel++)) SERIAL_BT.flush();
  #endif
  #ifdef SERIAL_IP
    if (isChannel(channel++)) SERIAL_IP.flush();
  #endif
  #ifdef SERIAL_LOCAL
    if (isChannel(channel++)) SERIAL_LOCAL.flush();
  #endif
}
