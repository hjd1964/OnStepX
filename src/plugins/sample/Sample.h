// Sample plugin
#pragma once

#include "../../lib/commands/CommandErrors.h"

class Sample {
public:
  // the initialization method must be present and named: void init();
  void init();

  // the command processing method is optional
  bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

  void loop();

private:

};

extern Sample sample;
