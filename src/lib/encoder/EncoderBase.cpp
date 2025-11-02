// general purpose encoder class

#include "EncoderBase.h"

#if AXIS1_ENCODER != OFF || AXIS2_ENCODER != OFF || AXIS3_ENCODER != OFF || \
    AXIS4_ENCODER != OFF || AXIS5_ENCODER != OFF || AXIS6_ENCODER != OFF || \
    AXIS7_ENCODER != OFF || AXIS8_ENCODER != OFF || AXIS9_ENCODER != OFF

#include "Arduino.h"
#include "../tasks/OnTask.h"

Encoder *encoder[9];

void encoderCallback1() { encoder[0]->poll(); }
void encoderCallback2() { encoder[1]->poll(); }
void encoderCallback3() { encoder[2]->poll(); }
void encoderCallback4() { encoder[3]->poll(); }
void encoderCallback5() { encoder[4]->poll(); }
void encoderCallback6() { encoder[5]->poll(); }
void encoderCallback7() { encoder[6]->poll(); }
void encoderCallback8() { encoder[7]->poll(); }
void encoderCallback9() { encoder[8]->poll(); }

// get device ready for use
bool Encoder::init() {
  if (ready) return true;

  encoder[axis - 1] = this;

  uint8_t taskHandle = 0;
  VF("MSG: Encoder"); V(axis); VF(", start status monitor task... ");

  switch (axis) {
    case 1:
      taskHandle = tasks.add(250, 0, true, 6, encoderCallback1, "encSta1");
    break;
    case 2:
      taskHandle = tasks.add(250, 0, true, 6, encoderCallback2, "encSta2");
    break;
    case 3:
      taskHandle = tasks.add(250, 0, true, 6, encoderCallback3, "encSta3");
    break;
    case 4:
      taskHandle = tasks.add(250, 0, true, 6, encoderCallback4, "encSta4");
    break;
    case 5:
      taskHandle = tasks.add(250, 0, true, 6, encoderCallback5, "encSta5");
    break;
    case 6:
      taskHandle = tasks.add(250, 0, true, 6, encoderCallback6, "encSta6");
    break;
    case 7:
      taskHandle = tasks.add(250, 0, true, 6, encoderCallback7, "encSta7");
    break;
    case 8:
      taskHandle = tasks.add(250, 0, true, 6, encoderCallback8, "encSta8");
    break;
    case 9:
      taskHandle = tasks.add(250, 0, true, 6, encoderCallback9, "encSta9");
    break;
  }

  if (taskHandle) { VL("success"); } else { VLF("FAILED!"); return false; }

  return true;
}

// set encoder origin
void Encoder::setOrigin(uint32_t count) {
  origin = count;
}

// update encoder status
void Encoder::poll() {
  #if ENCODER_FILTER > 0
    noInterrupts();
    msNow += 250;
    interrupts();
  #endif

  // calculate the velocity in counts per second
  if (tick++ % ENCODER_VELOCITY_WINDOW == 0) {
    unsigned long now = millis();

    if (count != lastCount) {
      int32_t counts = count - lastCount;
      long period = (long)(now - lastVelocityCheckTimeMs);
      velocity = counts/(period/1000.0F);
      lastCount = count;
    } else velocity = 0;

    lastVelocityCheckTimeMs = now;
  }

  // run once every 5 seconds
  if (tick++ % 20 == 0) {

    totalErrorCount += error;
    uint32_t errors = error;
    #ifdef ENCODER_WARN_AS_ERROR
      errors += warn;
    #endif
    error = 0;

    totalWarningCount += warn;
    warn = 0;

    // look at how often errors and warnings occur on a given axis
    // VF("MSG: Encoder"); V(axis); VF(" status, errors="); V(totalErrorCount); VF(" warnings="); VL(totalWarningCount);

    if (errors > UINT16_MAX) errors = UINT16_MAX;
    errorCount[errorCountIndex++ % 12] = errors;

    // sum the errors and set the error flag as required
    errors = 0;
    for (int i = 0; i < 12; i++) { errors += errorCount[i]; }

    errorState = errors > ENCODER_ERROR_COUNT_THRESHOLD;

    if (errorState != lastErrorState) {
      if (errorState) {
        DF("WRN: Encoder"); D(axis); DF(" errors, exceeded threshold/minute at "); DL(errors);
      } else {
        DF("MSG: Encoder"); D(axis); DF(" errors, within threshold/minute at "); DL(errors);
      }
      lastErrorState = errorState;
    }
  }
}

#endif
