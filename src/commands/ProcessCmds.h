// -----------------------------------------------------------------------------------
// Command processing
#pragma once
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../pinmaps/Models.h"

#include "BufferCmds.h"
#include "SerialWrapper.h"

typedef enum CommandError {
  CE_NONE, CE_0, CE_CMD_UNKNOWN, CE_REPLY_UNKNOWN, CE_PARAM_RANGE, CE_PARAM_FORM,
  CE_ALIGN_FAIL, CE_ALIGN_NOT_ACTIVE, CE_NOT_PARKED_OR_AT_HOME, CE_PARKED,
  CE_PARK_FAILED, CE_NOT_PARKED, CE_NO_PARK_POSITION_SET, CE_GOTO_FAIL, CE_LIBRARY_FULL,

  // goto/guide errors
  CE_SLEW_ERR_BELOW_HORIZON, CE_SLEW_ERR_ABOVE_OVERHEAD, CE_SLEW_ERR_IN_STANDBY, 
  CE_SLEW_ERR_IN_PARK, CE_SLEW_IN_SLEW, CE_SLEW_ERR_OUTSIDE_LIMITS, CE_SLEW_ERR_HARDWARE_FAULT,
  CE_SLEW_IN_MOTION, CE_SLEW_ERR_UNSPECIFIED, 

  CE_NULL} CommandError;

class CommandProcessor {
  public:
    // start and stop the serial port for the associated command channel
    CommandProcessor(long baud, char channel);
    ~CommandProcessor();

    // check for incomming commands and send responses
    void poll();

    // pass along commands as required for processing
    CommandError command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply);

  private:
    void appendChecksum(char s[]);

    CommandError commandError      = CE_NONE;
    CommandError lastCommandError  = CE_NONE;
    bool serialReady               = false;
    long serialBaud                = 9600;
    char channel                   = '?';

    Buffer buffer;
    SerialWrapper SerialPort;
};
