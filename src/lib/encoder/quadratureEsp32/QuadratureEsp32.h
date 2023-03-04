// A/B Quadrature encoders (fast ESP32 hardware decode)
#pragma once

#include "../Encoder.h"

#if AXIS1_ENCODER == AB_ESP32 || AXIS2_ENCODER == AB_ESP32 || AXIS3_ENCODER == AB_ESP32 || \
    AXIS4_ENCODER == AB_ESP32 || AXIS5_ENCODER == AB_ESP32 || AXIS6_ENCODER == AB_ESP32 || \
    AXIS7_ENCODER == AB_ESP32 || AXIS8_ENCODER == AB_ESP32 || AXIS9_ENCODER == AB_ESP32

#include <ESP32Encoder.h> // https://github.com/madhephaestus/ESP32Encoder/tree/master

// for example:
// QuadratureEsp32 encoder1(AXIS1_ENCODER_A_PIN, AXIS1_ENCODER_B_PIN, 1);

// Phase 1: LLHH LLHH
// Phase 2: LHHL LHHL
// ...00 01 11 10 00 01 11 10 00 01 11 10...

class QuadratureEsp32 : public Encoder {
  public:
    QuadratureEsp32(int16_t APin, int16_t BPin, int16_t axis);
    void init();

    int32_t read();
    void write(int32_t count);

    ESP32Encoder *ab;

  private:
    int16_t axis;

    int16_t APin = OFF;
    int16_t BPin = OFF;
};

#endif
