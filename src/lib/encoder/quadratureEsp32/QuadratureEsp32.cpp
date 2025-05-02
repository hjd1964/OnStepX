// A/B Quadrature encoders (fast ESP32 hardware decode)

#include "QuadratureEsp32.h"

#if AXIS1_ENCODER == AB_ESP32 || AXIS2_ENCODER == AB_ESP32 || AXIS3_ENCODER == AB_ESP32 || \
    AXIS4_ENCODER == AB_ESP32 || AXIS5_ENCODER == AB_ESP32 || AXIS6_ENCODER == AB_ESP32 || \
    AXIS7_ENCODER == AB_ESP32 || AXIS8_ENCODER == AB_ESP32 || AXIS9_ENCODER == AB_ESP32

// for example:
// QuadratureEsp32 encoder1(AXIS1_ENCODER_A_PIN, AXIS1_ENCODER_B_PIN, 1);

QuadratureEsp32::QuadratureEsp32(int16_t APin, int16_t BPin, int16_t axis) {
  if (axis < 1 || axis > 9) return;
  this->APin = APin;
  this->BPin = BPin;
}

bool QuadratureEsp32::init() {
  if (ready) return true;

  ab = new ESP32Encoder;
  if (ab == NULL) {
    DF("ERR: Encoder QuadratureEsp32"); D(axis); DLF(" init(), didn't get instance!"); 
    return false;
  }

  ab->attachFullQuad(APin, BPin);
  ab->setCount(0);

  ready = true;
  return true;
}

int32_t QuadratureEsp32::read() {
  if (!ready) return 0;

  count = (int32_t)ab->getCount();

  return count + origin;
}

void QuadratureEsp32::write(int32_t count) {
  if (!ready) return;

  count -= origin;

  ab->setCount(count);
}

#endif
