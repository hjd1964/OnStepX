// -----------------------------------------------------------------------------------
// axis servo DC motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../Common.h"

#if defined(SERVO_PE_PRESENT) || defined(SERVO_EE_PRESENT) || defined(SERVO_TMC2130_DC_PRESENT) || defined(SERVO_TMC5160_DC_PRESENT)

#ifndef ANALOG_WRITE_RANGE
  #define ANALOG_WRITE_RANGE 255
#endif
#ifndef SERVO_ANALOG_WRITE_RANGE
  #define SERVO_ANALOG_WRITE_RANGE ANALOG_WRITE_RANGE
#endif

#include "../ServoDriver.h"

// allow velocity in encoder CPS to scale analogWrite()
// pwm values so the resulting slew rate is more accurate
#ifndef AXIS1_ANALOG_WRITE_MIN
#define AXIS1_ANALOG_WRITE_MIN 0.0F // in percent
#endif
#ifndef AXIS2_ANALOG_WRITE_MIN
#define AXIS2_ANALOG_WRITE_MIN 0.0F
#endif
#ifndef AXIS3_ANALOG_WRITE_MIN
#define AXIS3_ANALOG_WRITE_MIN 0.0F
#endif
#ifndef AXIS4_ANALOG_WRITE_MIN
#define AXIS4_ANALOG_WRITE_MIN 0.0F
#endif
#ifndef AXIS5_ANALOG_WRITE_MIN
#define AXIS5_ANALOG_WRITE_MIN 0.0F
#endif
#ifndef AXIS6_ANALOG_WRITE_MIN
#define AXIS6_ANALOG_WRITE_MIN 0.0F
#endif
#ifndef AXIS7_ANALOG_WRITE_MIN
#define AXIS7_ANALOG_WRITE_MIN 0.0F
#endif
#ifndef AXIS8_ANALOG_WRITE_MIN
#define AXIS8_ANALOG_WRITE_MIN 0.0F
#endif
#ifndef AXIS9_ANALOG_WRITE_MIN
#define AXIS9_ANALOG_WRITE_MIN 0.0F
#endif

#ifndef AXIS1_ANALOG_WRITE_MAX
#define AXIS1_ANALOG_WRITE_MAX 100.0F // in percent
#endif
#ifndef AXIS2_ANALOG_WRITE_MAX
#define AXIS2_ANALOG_WRITE_MAX 100.0F
#endif
#ifndef AXIS3_ANALOG_WRITE_MAX
#define AXIS3_ANALOG_WRITE_MAX 100.0F
#endif
#ifndef AXIS4_ANALOG_WRITE_MAX
#define AXIS4_ANALOG_WRITE_MAX 100.0F
#endif
#ifndef AXIS5_ANALOG_WRITE_MAX
#define AXIS5_ANALOG_WRITE_MAX 100.0F
#endif
#ifndef AXIS6_ANALOG_WRITE_MAX
#define AXIS6_ANALOG_WRITE_MAX 100.0F
#endif
#ifndef AXIS7_ANALOG_WRITE_MAX
#define AXIS7_ANALOG_WRITE_MAX 100.0F
#endif
#ifndef AXIS8_ANALOG_WRITE_MAX
#define AXIS8_ANALOG_WRITE_MAX 100.0F
#endif
#ifndef AXIS9_ANALOG_WRITE_MAX
#define AXIS9_ANALOG_WRITE_MAX 100.0F
#endif

class ServoDcDriver : public ServoDriver {
  public:
    // constructor
    ServoDcDriver(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings);

    // returns the number of axis parameters
    uint8_t getParameterCount() { return numParameters; }

    // returns the specified axis parameter
    AxisParameter* getParameter(uint8_t number) { if (number > numParameters) return &invalid; else return parameter[number]; }

    // set motor velocity
    // \param velocity as needed to reach the target position, in encoder counts per second
    // \returns velocity in effect, in encoder counts per second
    float setMotorVelocity(float velocity);

  protected:
    // convert from encoder counts per second to analogWriteRange units to roughly match velocity
    long toAnalogRange(float velocity) {
      long sign = 1;
      if (velocity < 0.0F) {
        velocity = -velocity;
        sign = -1;
      }
  
      long power = 0;
      if (velocity != 0.0F) {
        power = lround(((float)velocity/velocityMax)*(analogWriteRange - 1));
        long pwmMin = lround(pwmMinimum.value/100.0F*(analogWriteRange - 1));
        long pwmMax = lround(pwmMaximum.value/100.0F*(analogWriteRange - 1));

        power = map(power, 0, analogWriteRange - 1, pwmMin, pwmMax);
      }

      return power*sign;
    }

    // motor control update
    virtual void pwmUpdate(long power) { }

    // regulate velocity changes
    float velocityRamp = 0;

    long analogWriteRange = SERVO_ANALOG_WRITE_RANGE;

    // runtime adjustable settings
    AxisParameter pwmMinimum = {NAN, NAN, NAN, 0.0, 25.0, AXP_FLOAT_IMMEDIATE, "Min power, in %"};
    AxisParameter pwmMaximum = {NAN, NAN, NAN, 25.0, 100.0, AXP_FLOAT_IMMEDIATE, "Max power, in %"};

    const int numParameters = 2;
    AxisParameter* parameter[3] = {&invalid, &pwmMinimum, &pwmMaximum};
};

#endif
