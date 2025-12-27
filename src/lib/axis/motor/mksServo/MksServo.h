// -----------------------------------------------------------------------------------
// axis MKS SERVO42D/57D motor driver via CAN, using 0xFE absolute position updates
// EXPERIMENTAL!!!

#pragma once
#include "../../../../Common.h"

#ifdef MKS42D_MOTOR_PRESENT

#if !defined(CAN_PLUS) || CAN_PLUS == OFF
  #error "No MKS motor CAN interface!"
#endif

#include "../Motor.h"

#ifndef MKS_SLEW_DIRECT
  #define MKS_SLEW_DIRECT OFF
#endif

#ifndef MKS_FE_ACCEL_CONST
  #define MKS_FE_ACCEL_CONST 10
#endif

typedef struct MksDriverSettings {
  int16_t model;
  int8_t  status;     // ON/OFF (reserved for future status/heartbeat support)
} MksDriverSettings;

class Mks42DMotor : public Motor {
  public:
    Mks42DMotor(uint8_t axisNumber,
                int8_t reverse,
                const MksDriverSettings *Settings,
                bool useFastHardwareTimers = true);

    bool init() override;

    uint8_t getParameterCount() override { return Motor::getParameterCount() + numParameters; }

    AxisParameter* getParameter(uint8_t number) override {
      const uint8_t base = Motor::getParameterCount();
      if (number >= 1 && number <= base) return Motor::getParameter(number);

      const uint8_t i = number - base;
      if (i == 1) return &stepsPerSecondToRpm;
      if (i == 2) return &maxRpm;
      return &invalid;
    }

    // Integrated-driver style reverse control (kept for parity with other integrated CAN motors)
    void setReverse(int8_t state);

    // Base-class reverse (prevents silent bypass if someone calls Motor::setReverse(bool))
    void setReverse(bool state) override;

    void enable(bool state) override;

    void resetPositionSteps(long value) override;

    int getStepsPerStepSlewing() override { return 256; }

    float getFrequencySteps() override;
    void  setFrequencySteps(float frequency) override;

    void setSlewing(bool state) override;

    void poll() override;

    const char* name() override { return "MKS_FE"; }

    // ISR uses this; not a Motor virtual
    void move();

  private:
    static inline uint8_t crc8(uint16_t id11, const uint8_t *data, size_t n) {
      uint32_t sum = (uint32_t)(id11 & 0x7FF);
      for (size_t i = 0; i < n; i++) sum += data[i];
      return (uint8_t)(sum & 0xFF);
    }

    static inline int32_t clamp24(int32_t v) {
      if (v >  8388607L) return  8388607L;
      if (v < -8388608L) return -8388608L;
      return v;
    }

    inline float pval(const AxisParameter &p) const {
      const float v = p.value;
      return isnan(v) ? p.valueDefault : v;
    }

    // stepsPerSecondToRpm unit: RPM per (steps/second)
    inline uint16_t speedFromStepRate(float stepsPerSec) const {
      if (stepsPerSec < 0.0F) stepsPerSec = -stepsPerSec;

      const float scale = pval(stepsPerSecondToRpm);
      float sF = stepsPerSec * scale;
      if (sF < 0.0F) sF = 0.0F;

      // MKS protocol speed range is 0..3000 RPM; keep the clamp aligned to that.
      float capF = pval(maxRpm);
      if (capF < 0.0F) capF = 0.0F;
      if (capF > 3000.0F) capF = 3000.0F;

      if (sF > capF) sF = capF;
      return (uint16_t)lroundf(sF);
    }

    inline void sendWithCrc(const uint8_t *payload, size_t n); // n<=7
    inline void sendF3(bool en);
    inline void sendFE(int32_t pos, uint16_t speed);

    void stopSyntheticMotion();
    void resetToTrackingBaseline();

    int canID = 0;

    // reserved / future
    int16_t model = 0;
    int8_t  statusMode = OFF;

    uint8_t taskHandle = 0;

    int  stepSize = 1;
    float currentFrequency = 0.0F;
    unsigned long lastPeriod = 0;
    float maxFrequency = HAL_FRACTIONAL_SEC;

    volatile int absStep = 0;

    void (*callback)() = nullptr;

    bool useFastHardwareTimers = true;
    bool isSlewing = false;

    long lastTarget = LONG_MIN;

    // runtime adjustable settings
    AxisParameter stepsPerSecondToRpm = {NAN, NAN, NAN, 0.0F, 10000.0F, AXP_FLOAT,   "Steps/s to RPM"};
    AxisParameter maxRpm              = {NAN, NAN, NAN, 0.0F, 3000.0F,  AXP_INTEGER, "Max Speed RPM"};

    const int numParameters = 2;
};

#endif
