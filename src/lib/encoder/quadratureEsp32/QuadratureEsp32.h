// A/B Quadrature encoders (fast ESP32 hardware decode)
#pragma once

#include "../EncoderBase.h"

#if AXIS1_ENCODER == AB_ESP32 || AXIS2_ENCODER == AB_ESP32 || AXIS3_ENCODER == AB_ESP32 || \
    AXIS4_ENCODER == AB_ESP32 || AXIS5_ENCODER == AB_ESP32 || AXIS6_ENCODER == AB_ESP32 || \
    AXIS7_ENCODER == AB_ESP32 || AXIS8_ENCODER == AB_ESP32 || AXIS9_ENCODER == AB_ESP32

#include <ESP32Encoder.h> // https://github.com/madhephaestus/ESP32Encoder/tree/master

// for example:
// QuadratureEsp32 encoder1(1, AXIS1_ENCODER_A_PIN, AXIS1_ENCODER_B_PIN);

class QuadratureEsp32 : public Encoder {
  public:
    QuadratureEsp32(int16_t axis, int16_t APin, int16_t BPin);
    bool init();

    int32_t read();
    void write(int32_t position);

    ESP32Encoder *ab = nullptr;

  private:
    int16_t APin = OFF;
    int16_t BPin = OFF;
};

#endif
