// -----------------------------------------------------------------------------------
// Intervalometer control
#pragma once

#include "../../../common.h"

class Intervalometer {
  public:
    void init(int index, bool validKey);

    void poll();

    float getExposure();
    void setExposure(float t);

    float getDelay();
    void setDelay(float t);

    float getCurrentCount();
    float getCount();
    void setCount(float c);

    bool isEnabled();
    void enable(bool state);

    bool isOn();

  private:
    uint8_t timeToByte(float t);
    float byteToTime(uint8_t b);

    enum Pressed { P_STANDBY, P_EXP_START, P_EXP_START_PHASE2, P_EXP_DONE, P_EXP_DONE_PHASE2, P_WAIT };

    Pressed pressed = P_STANDBY;
    bool enabled = false;

    float expTime = 0;
    float expDelay = 0;
    int expCount = 0;
    int thisCount = 0;

    unsigned long expDone = 0;
    unsigned long waitDone = 0;

    int index = 0;
};
