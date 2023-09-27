// -----------------------------------------------------------------------------------
// SerialWrapper a single class to allow uniform access to other serial port classes

#include "SerialWrapper.h"

#if SERIAL_A == HardSerial
  #undef SERIAL_A
  HardwareSerial HWSerialA(SERIAL_A_RX, SERIAL_A_TX);
  #define SERIAL_A HWSerialA
  #define SERIAL_A_RXTX_SET
#endif

#if SERIAL_B == HardSerial
  #undef SERIAL_B
  HardwareSerial HWSerialB(SERIAL_B_RX, SERIAL_B_TX);
  #define SERIAL_B HWSerialB
  #define SERIAL_B_RXTX_SET
#endif

#if SERIAL_C == HardSerial
  #undef SERIAL_C
  HardwareSerial HWSerialC(SERIAL_C_RX, SERIAL_C_TX);
  #define SERIAL_C HWSerialC
  #define SERIAL_C_RXTX_SET
#endif

#ifdef SERIAL_BT
  BluetoothSerial bluetoothSerial;
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
  #ifdef SERIAL_PIP1
    if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
    channel++;
  #endif
  #ifdef SERIAL_PIP2
    if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
    channel++;
  #endif
  #ifdef SERIAL_PIP3
    if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
    channel++;
  #endif
  #ifdef SERIAL_SIP
    if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
    channel++;
  #endif
  #ifdef SERIAL_LOCAL
    if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
    channel++;
  #endif
  UNUSED(channel);
}

void SerialWrapper::begin() { begin(9600); }

void SerialWrapper::begin(long baud) {
  uint8_t channel = 0;
  #ifdef SERIAL_A
    if (isChannel(channel++))
      #if defined(SERIAL_A_RX) && defined(SERIAL_A_TX) && !defined(SERIAL_A_RXTX_SET)
        SERIAL_A.begin(baud, SERIAL_8N1, SERIAL_A_RX, SERIAL_A_TX);
      #else
        SERIAL_A.begin(baud);
      #endif
  #endif
  #ifdef SERIAL_B
    if (isChannel(channel++))
      #if defined(SERIAL_B_RX) && defined(SERIAL_B_TX) && !defined(SERIAL_B_RXTX_SET)
        SERIAL_B.begin(baud, SERIAL_8N1, SERIAL_B_RX, SERIAL_B_TX);
      #else
        SERIAL_B.begin(baud);
      #endif
  #endif
  #ifdef SERIAL_C
    if (isChannel(channel++))
      #if defined(SERIAL_C_RX) && defined(SERIAL_C_TX) && !defined(SERIAL_C_RXTX_SET)
        SERIAL_C.begin(baud, SERIAL_8N1, SERIAL_C_RX, SERIAL_C_TX);
      #else
        SERIAL_C.begin(baud);
      #endif
  #endif
  #ifdef SERIAL_D
    if (isChannel(channel++))
      #if defined(SERIAL_D_RX) && defined(SERIAL_D_TX) && !defined(SERIAL_D_RXTX_SET)
        SERIAL_D.begin(baud, SERIAL_8N1, SERIAL_D_RX, SERIAL_D_TX);
      #else
        SERIAL_D.begin(baud);
      #endif
  #endif
  #ifdef SERIAL_ST4
    if (isChannel(channel++)) SERIAL_ST4.begin(baud);
  #endif
  #ifdef SERIAL_BT
    //if (isChannel(channel++)) SERIAL_BT.begin(SERIAL_BT_NAME); // started early in .ino file
  #endif
  #ifdef SERIAL_PIP1
    if (isChannel(channel++)) SERIAL_PIP1.begin(9996, 10L*1000L, true);
  #endif
  #ifdef SERIAL_PIP2
    if (isChannel(channel++)) SERIAL_PIP2.begin(9997, 10L*1000L, true);
  #endif
  #ifdef SERIAL_PIP3
    if (isChannel(channel++)) SERIAL_PIP3.begin(9998, 10L*1000L, true);
  #endif
  #ifdef SERIAL_SIP
    if (isChannel(channel++)) SERIAL_SIP.begin(9999, 10L*1000L, true);
  #endif
  #ifdef SERIAL_LOCAL
    if (isChannel(channel++)) SERIAL_LOCAL.begin(baud);
  #endif
  UNUSED(baud);
  UNUSED(channel);
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
  #ifdef SERIAL_PIP1
    if (isChannel(channel++)) SERIAL_PIP1.end();
  #endif
  #ifdef SERIAL_PIP2
    if (isChannel(channel++)) SERIAL_PIP2.end();
  #endif
  #ifdef SERIAL_PIP3
    if (isChannel(channel++)) SERIAL_PIP3.end();
  #endif
  #ifdef SERIAL_SIP
    if (isChannel(channel++)) SERIAL_SIP.end();
  #endif
  #ifdef SERIAL_LOCAL
    if (isChannel(channel++)) SERIAL_LOCAL.end();
  #endif
  UNUSED(channel);
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
  #ifdef SERIAL_PIP1
    if (isChannel(channel++)) return SERIAL_PIP1.write(data);
  #endif
  #ifdef SERIAL_PIP2
    if (isChannel(channel++)) return SERIAL_PIP2.write(data);
  #endif
  #ifdef SERIAL_PIP3
    if (isChannel(channel++)) return SERIAL_PIP3.write(data);
  #endif
  #ifdef SERIAL_SIP
    if (isChannel(channel++)) return SERIAL_SIP.write(data);
  #endif
  #ifdef SERIAL_LOCAL
    if (isChannel(channel++)) return SERIAL_LOCAL.write(data);
  #endif
  UNUSED(data);
  UNUSED(channel);
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
  #ifdef SERIAL_PIP1
    if (isChannel(channel++)) return SERIAL_PIP1.write(data, quantity);
  #endif
  #ifdef SERIAL_PIP2
    if (isChannel(channel++)) return SERIAL_PIP2.write(data, quantity);
  #endif
  #ifdef SERIAL_PIP3
    if (isChannel(channel++)) return SERIAL_PIP3.write(data, quantity);
  #endif
  #ifdef SERIAL_SIP
    if (isChannel(channel++)) return SERIAL_SIP.write(data, quantity);
  #endif
  #ifdef SERIAL_LOCAL
    if (isChannel(channel++)) return SERIAL_LOCAL.write(data, quantity);
  #endif
  UNUSED(data);
  UNUSED(quantity);
  UNUSED(channel);
  return -1;
}

int SerialWrapper::available() {
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
  #ifdef SERIAL_PIP1
    if (isChannel(channel++)) return SERIAL_PIP1.available();
  #endif
  #ifdef SERIAL_PIP2
    if (isChannel(channel++)) return SERIAL_PIP2.available();
  #endif
  #ifdef SERIAL_PIP3
    if (isChannel(channel++)) return SERIAL_PIP3.available();
  #endif
  #ifdef SERIAL_SIP
    if (isChannel(channel++)) return SERIAL_SIP.available();
  #endif
  #ifdef SERIAL_LOCAL
    if (isChannel(channel++)) return SERIAL_LOCAL.available();
  #endif
  UNUSED(channel);
  return 0;
}

int SerialWrapper::read() {
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
  #ifdef SERIAL_PIP1
    if (isChannel(channel++)) return SERIAL_PIP1.read();
  #endif
  #ifdef SERIAL_PIP2
    if (isChannel(channel++)) return SERIAL_PIP2.read();
  #endif
  #ifdef SERIAL_PIP3
    if (isChannel(channel++)) return SERIAL_PIP3.read();
  #endif
  #ifdef SERIAL_SIP
    if (isChannel(channel++)) return SERIAL_SIP.read();
  #endif
  #ifdef SERIAL_LOCAL
    if (isChannel(channel++)) return SERIAL_LOCAL.read();
  #endif
  UNUSED(channel);
  return -1;
}

int SerialWrapper::peek() {
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
  #ifdef SERIAL_PIP1
    if (isChannel(channel++)) return SERIAL_PIP1.peek();
  #endif
  #ifdef SERIAL_PIP2
    if (isChannel(channel++)) return SERIAL_PIP2.peek();
  #endif
  #ifdef SERIAL_PIP3
    if (isChannel(channel++)) return SERIAL_PIP3.peek();
  #endif
  #ifdef SERIAL_SIP
    if (isChannel(channel++)) return SERIAL_SIP.peek();
  #endif
  #ifdef SERIAL_LOCAL
    if (isChannel(channel++)) return SERIAL_LOCAL.peek();
  #endif
  UNUSED(channel);
  return -1;
}

void SerialWrapper::flush() {
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
  #ifdef SERIAL_PIP1
    if (isChannel(channel++)) SERIAL_PIP1.flush();
  #endif
  #ifdef SERIAL_PIP2
    if (isChannel(channel++)) SERIAL_PIP2.flush();
  #endif
  #ifdef SERIAL_PIP3
    if (isChannel(channel++)) SERIAL_PIP3.flush();
  #endif
  #ifdef SERIAL_SIP
    if (isChannel(channel++)) SERIAL_SIP.flush();
  #endif
  #ifdef SERIAL_LOCAL
    if (isChannel(channel++)) SERIAL_LOCAL.flush();
  #endif
  UNUSED(channel);
}
