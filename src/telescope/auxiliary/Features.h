// -----------------------------------------------------------------------------------
// Auxiliary Features
#pragma once

#include "../../Common.h"

#ifdef FEATURES_PRESENT

#include "../../commands/ProcessCmds.h"
#include "../../lib/temperature/Temperature.h"
#include "dewHeater/DewHeater.h"
#include "intervalometer/Intervalometer.h"

typedef struct Device {
   const char* name;
   const int16_t purpose;
   const int64_t temp;
   int16_t pin;
   int16_t value;
   const int16_t active;
   DewHeater *dewHeater;
   Intervalometer *intervalometer;
} Device;

class Features {
  public:
    // initialize the auxiliary feature devices
    void init(bool validKey);

    // scan the auxiliary feature devices for service
    void poll();

    // process auxiliary feature commands
    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // :GXXn#
    void getCommand(char *parameter, char *reply, bool &boolReply);
 
    // :GXYn#
    void getInfoCommand(char *parameter, char *reply, bool &boolReply);

    // :SXX[n],V[Z][S][v]#
    // for example :SXX1,V1#  :SXX1,Z0.5#
    void setCommand(char *parameter);

    // work-around for Mega2560 64 bit conditional comparison bug ((feature[i].pin & DS_MASK) == DS2413)
    bool isDS2413(int64_t v);
 
  private:
    int16_t auxPins[8] = { AUX1_PIN, AUX2_PIN, AUX3_PIN, AUX4_PIN, AUX5_PIN, AUX6_PIN, AUX7_PIN, AUX8_PIN };
    Device device[8] = {
      { FEATURE1_NAME, FEATURE1_PURPOSE, FEATURE1_TEMP, FEATURE1_PIN, FEATURE1_DEFAULT_VALUE, FEATURE1_ACTIVE_STATE, NULL, NULL },
      { FEATURE2_NAME, FEATURE2_PURPOSE, FEATURE2_TEMP, FEATURE2_PIN, FEATURE2_DEFAULT_VALUE, FEATURE2_ACTIVE_STATE, NULL, NULL },
      { FEATURE3_NAME, FEATURE3_PURPOSE, FEATURE3_TEMP, FEATURE3_PIN, FEATURE3_DEFAULT_VALUE, FEATURE3_ACTIVE_STATE, NULL, NULL },
      { FEATURE4_NAME, FEATURE4_PURPOSE, FEATURE4_TEMP, FEATURE4_PIN, FEATURE4_DEFAULT_VALUE, FEATURE4_ACTIVE_STATE, NULL, NULL },
      { FEATURE5_NAME, FEATURE5_PURPOSE, FEATURE5_TEMP, FEATURE5_PIN, FEATURE5_DEFAULT_VALUE, FEATURE5_ACTIVE_STATE, NULL, NULL },
      { FEATURE6_NAME, FEATURE6_PURPOSE, FEATURE6_TEMP, FEATURE6_PIN, FEATURE6_DEFAULT_VALUE, FEATURE6_ACTIVE_STATE, NULL, NULL },
      { FEATURE7_NAME, FEATURE7_PURPOSE, FEATURE7_TEMP, FEATURE7_PIN, FEATURE7_DEFAULT_VALUE, FEATURE7_ACTIVE_STATE, NULL, NULL },
      { FEATURE8_NAME, FEATURE8_PURPOSE, FEATURE8_TEMP, FEATURE8_PIN, FEATURE8_DEFAULT_VALUE, FEATURE8_ACTIVE_STATE, NULL, NULL }
    };
};

#endif
