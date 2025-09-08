// Serial bridge encoder

#include "SerialBridge.h"

#if (AXIS1_ENCODER == SERIAL_BRIDGE || AXIS2_ENCODER == SERIAL_BRIDGE || AXIS3_ENCODER == SERIAL_BRIDGE || \
     AXIS4_ENCODER == SERIAL_BRIDGE || AXIS5_ENCODER == SERIAL_BRIDGE || AXIS6_ENCODER == SERIAL_BRIDGE || \
     AXIS7_ENCODER == SERIAL_BRIDGE || AXIS8_ENCODER == SERIAL_BRIDGE || AXIS9_ENCODER == SERIAL_BRIDGE) && defined(SERIAL_ENCODER)

bool _serial_bridge_initialized = false;

#if SERIAL_ENCODER == HardSerial
  #undef SERIAL_ENCODER
  HardwareSerial HWSerialEncoder(SERIAL_ENCODER_RX, SERIAL_ENCODER_TX);
  #define SERIAL_ENCODER HWSerialEncoder
  #define SERIAL_ENCODER_RXTX_SET
#endif

SerialBridge::SerialBridge(int16_t axis) {
  if (axis < 1 || axis > 9) return;
  
  this->axis = axis;

  this->channel[0] = '0' + axis;

  ready = true;
}

int32_t SerialBridge::read() {
  if (!ready) return 0;

  if (millis() - lastReadMillis > 10) {
    count = getCount();
    lastReadMillis = millis();
  }

  return count + index;
}

void SerialBridge::write(int32_t position) {
  if (!ready) return;

  index = position - getCount();
}

int32_t SerialBridge::getCount() {
  if (!_serial_bridge_initialized) {
    #if defined(SERIAL_ENCODER_RX) && defined(SERIAL_ENCODER_TX) && !defined(SERIAL_ENCODER_RXTX_SET)
      SERIAL_ENCODER.begin(SERIAL_ENCODER_BAUD, SERIAL_8N1, SERIAL_ENCODER_RX, SERIAL_ENCODER_TX);
    #else
      SERIAL_ENCODER.begin(SERIAL_ENCODER_BAUD);
    #endif
    delay(100);
    _serial_bridge_initialized = true;
  }

  SERIAL_ENCODER.print(channel);
  
  char result[32] = "";
  char c;
  int i = 0;
  errorDetected = false;
  unsigned long start = millis();
  do {
    if (SERIAL_ENCODER.available()) c = SERIAL_ENCODER.read(); else c = 'x';
    if ((c >= '0' && c <= '9') || c == '-') {
      result[i++] = c;
      result[i] = 0;
    }
    if (c == 'E') errorDetected = true;
  } while (c != 13 && (millis() - start) < 4 && i < 16);

  if (strlen(result) > 0) {
    return atoi(result);
  } else {
    DLF("WRN: SerialBridge getCount(), timed out!");
    error++;
    return 0;
  }
}

#endif
