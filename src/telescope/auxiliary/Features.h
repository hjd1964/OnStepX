// -----------------------------------------------------------------------------------
// Auxiliary Features
#pragma once

#include "../../Common.h"

#ifdef FEATURES_PRESENT

#include "../../libApp/commands/ProcessCmds.h"
#include "../../libApp/temperature/Temperature.h"
#include "dewHeater/DewHeater.h"
#include "intervalometer/Intervalometer.h"

typedef struct Device {
   const char* name;
   int16_t purpose;
   const int64_t temp;
   int16_t pin;
   int16_t value;
   const int16_t active;
   DewHeater *dewHeater;
   Intervalometer *intervalometer;
} Device;

class Features {
  public:
    void init();

    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    void poll();
 
  private:
    int16_t auxPins[8] = { AUX1_PIN, AUX2_PIN, AUX3_PIN, AUX4_PIN, AUX5_PIN, AUX6_PIN, AUX7_PIN, AUX8_PIN };
    Device device[8] = {
      { FEATURE1_NAME, FEATURE1_PURPOSE, FEATURE1_TEMP, FEATURE1_PIN, FEATURE1_VALUE_DEFAULT, FEATURE1_ON_STATE, NULL, NULL },
      { FEATURE2_NAME, FEATURE2_PURPOSE, FEATURE2_TEMP, FEATURE2_PIN, FEATURE2_VALUE_DEFAULT, FEATURE2_ON_STATE, NULL, NULL },
      { FEATURE3_NAME, FEATURE3_PURPOSE, FEATURE3_TEMP, FEATURE3_PIN, FEATURE3_VALUE_DEFAULT, FEATURE3_ON_STATE, NULL, NULL },
      { FEATURE4_NAME, FEATURE4_PURPOSE, FEATURE4_TEMP, FEATURE4_PIN, FEATURE4_VALUE_DEFAULT, FEATURE4_ON_STATE, NULL, NULL },
      { FEATURE5_NAME, FEATURE5_PURPOSE, FEATURE5_TEMP, FEATURE5_PIN, FEATURE5_VALUE_DEFAULT, FEATURE5_ON_STATE, NULL, NULL },
      { FEATURE6_NAME, FEATURE6_PURPOSE, FEATURE6_TEMP, FEATURE6_PIN, FEATURE6_VALUE_DEFAULT, FEATURE6_ON_STATE, NULL, NULL },
      { FEATURE7_NAME, FEATURE7_PURPOSE, FEATURE7_TEMP, FEATURE7_PIN, FEATURE7_VALUE_DEFAULT, FEATURE7_ON_STATE, NULL, NULL },
      { FEATURE8_NAME, FEATURE8_PURPOSE, FEATURE8_TEMP, FEATURE8_PIN, FEATURE8_VALUE_DEFAULT, FEATURE8_ON_STATE, NULL, NULL }
    };
    uint8_t momentarySwitchTime[8] = {0, 0, 0, 0, 0, 0, 0, 0};
};

extern Features features;

#endif
