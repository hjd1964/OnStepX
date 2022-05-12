// -----------------------------------------------------------------------------------
// Command processing
#pragma once

#include <Arduino.h>
#include "../../lib/commands/BufferCmds.h"
#include "../../lib/commands/SerialWrapper.h"
#include "../../lib/commands/CommandErrors.h"

class CommandProcessor {
  public:
    // start and stop the serial port for the associated command channel
    CommandProcessor(long baud, char channel);
    ~CommandProcessor();

    // check for incomming commands and send responses
    void poll();

    // pass along commands as required for processing
    CommandError command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply);

  private:
    void logErrors(char *cmd, char *param, char *reply, CommandError e);
    void appendChecksum(char *s);

    CommandError commandError      = CE_NONE;
    CommandError lastCommandError  = CE_NONE;
    bool serialReady               = false;
    long serialBaud                = 9600;
    char channel                   = '?';

    Buffer buffer;
    SerialWrapper SerialPort;
};

extern void commandChannelInit();
