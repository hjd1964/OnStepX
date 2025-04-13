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
  ready = true;
  
  this->axis = axis;
  axis--;
  this->channel[0] = '1' + axis;
}

int32_t SerialBridge::read() {
  if (!ready) return 0;

  if (millis() - lastReadMillis > 10) {
    count = raw();
    lastReadMillis = millis();
  }

  return count + offset;
}

void SerialBridge::write(int32_t count) {
  if (!ready) return;

  offset = count - raw();
}

int32_t SerialBridge::raw() {
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
  
  char c;
  char result[32] = "";
  int index = 0;
  unsigned long start = millis();
  errorDetected = false;
  do {
    if (SERIAL_ENCODER.available()) c = SERIAL_ENCODER.read(); else c = 'x';
    if ((c >= '0' && c <= '9') || c == '-') {
      result[index++] = c;
      result[index] = 0;
    }
    if (c == 'E') errorDetected = true;
  } while (c != 13 && (millis() - start) < 4 && index < 16);

  if (strlen(result) > 0) {
    return atoi(result) + origin;
  } else {
    DLF("WRN: SerialBridge raw(), timed out!");
    error++;
    return 0  + origin;
  }
} 

#endif
