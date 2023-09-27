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

void QuadratureEsp32::init() {
  if (initialized) { VF("WRN: Encoder QuadratureEsp32"); V(axis); VLF(" init(), already initialized!"); return; }

  ab = new ESP32Encoder;
  if (ab == NULL) {
    VF("ERR: Encoder QuadratureEsp32"); V(axis); VLF(" init(), didn't get instance!"); 
    return;
  }

  ab->attachFullQuad(APin, BPin);
  ab->setCount(0);

  initialized = true;
}

int32_t QuadratureEsp32::read() {
  if (!initialized) { VF("WRN: Encoder QuadratureEsp32"); V(axis); VLF(" read(), not initialized!"); return 0; }

  count = (int32_t)ab->getCount();

  return count + origin;
}

void QuadratureEsp32::write(int32_t count) {
  if (!initialized) { VF("WRN: Encoder QuadratureEsp32"); V(axis); VLF(" write(), not initialized!"); return; }

  count -= origin;

  ab->setCount(count);
}

#endif
