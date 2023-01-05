// -----------------------------------------------------------------------------------
// Help with commands, etc.
#pragma once

#include "../../Common.h"

#define SERIAL_ONSTEP SERIAL_LOCAL

class OnStepCmd {
  public:
    void serialRecvFlush();

    // low level smart LX200 aware command and response (up to 80 chars) over serial (includes any '#' frame char)
    bool processCommand(const char* cmd, char* response, long timeOutMs);

    // send command to OnStep and get any response (up 80 chars, no '#' frame char)
    bool command(const char* command, char* response);

    // send command to OnStep, expects no reply
    bool commandBlind(const char* command);

    // send command to OnStep for debugging, expects a boolean reply
    bool commandEcho(const char* command);

    // send command to OnStep, expects a boolean reply
    bool commandBool(const char* command);

    // send command to OnStep, expects a string reply (no '#' frame char)
    char *commandString(const char* command);

    // turns OnStep command error number into descriptive string
    char* commandErrorToStr(int e);

  private:
};

// timeout period for the web
extern int webTimeout;

// timeout period for the command channel(s)
extern int cmdTimeout;

extern OnStepCmd onStep;
