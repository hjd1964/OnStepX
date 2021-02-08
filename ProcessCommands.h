// -----------------------------------------------------------------------------------
// Command processing
#pragma once

#include "src/lib/BufferCmds.h"
#include "src/lib/SerialWrapper.h"

class CommandProcessor {
  public:
    // start and stop the serial port for the associated command channel
    CommandProcessor(long baud);
    ~CommandProcessor();

    // check for incomming commands and send responses
    void poll();

    // pass along commands as required for processing
    CommandErrors command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply);

  private:
    void appendChecksum(char s[]);

    CommandErrors commandError     = CE_NONE;
    CommandErrors lastCommandError = CE_NONE;
    bool serialReady               = false;
    long serialBaud                = 9600;

    Buffer buffer;
    SerialWrapper SerialPort;
};

CommandProcessor::CommandProcessor(long baud) {
  serialBaud = baud;
}

CommandProcessor::~CommandProcessor() {
  SerialPort.end();
}

void CommandProcessor::poll() {
  if (!serialReady) { SerialPort.begin(serialBaud); serialReady = true; }
  if (SerialPort.available()) buffer.add(SerialPort.read()); else return;
  if (buffer.ready()) {
    char reply[50] = "";
    bool numericReply = true;
    bool supressFrame = false;

    commandError = command(reply, buffer.getCmd(), buffer.getParameter(), &supressFrame, &numericReply);

    if (numericReply) {
      if (commandError != CE_NONE) strcpy(reply,"0"); else strcpy(reply,"1");
      supressFrame = true;
    }
    if (commandError != CE_NULL) lastCommandError = commandError;
    if (strlen(reply) > 0 || buffer.checksum) {
      if (buffer.checksum) {
        appendChecksum(reply);
        strcat(reply,buffer.getSeq());
        supressFrame = false;
      }
      if (!supressFrame) strcat(reply,"#");
      SerialPort.write(reply);
    }
    buffer.flush();
  }
}

CommandErrors CommandProcessor::command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply) {
  commandError = CE_NONE;

  // Handle telescope goto, etc. commands
  if (telescope.command(reply, command, parameter, supressFrame, numericReply, &commandError)) return commandError;

  // Handle observatory date/time/location commands
  if (observatory.command(reply, command, parameter, supressFrame, numericReply, &commandError)) return commandError;

// :GVD#      Get Telescope Firmware Date
//            Returns: MTH DD YYYY#
// :GVM#      General Message
//            Returns: s# (where s is a string up to 16 chars)
// :GVN#      Get Telescope Firmware Number
//            Returns: M.mp#
// :GVP#      Get Telescope Product Name
//            Returns: s#
// :GVT#      Get Telescope Firmware Time
//            Returns: HH:MM:SS#
  if (cmdp("GV")) {
    if (parameter[0] == 'D') strcpy(reply,FirmwareDate); else
    if (parameter[0] == 'M') sprintf(reply,"OnStepX %i.%i%s",FirmwareVersionMajor,FirmwareVersionMinor,FirmwareVersionPatch); else
    if (parameter[0] == 'N') sprintf(reply,"%i.%i%s",FirmwareVersionMajor,FirmwareVersionMinor,FirmwareVersionPatch); else
    if (parameter[0] == 'P') strcpy(reply,FirmwareName); else
    if (parameter[0] == 'T') strcpy(reply,FirmwareTime); else commandError = CE_CMD_UNKNOWN;
    *numericReply = false;
    return commandError;
  }

  return CE_CMD_UNKNOWN;
}

void CommandProcessor::appendChecksum(char s[]) {
  char HEXS[3]="";
  byte cks=0; for (unsigned int cksCount0=0; cksCount0 < strlen(s); cksCount0++) {  cks+=s[cksCount0]; }
  sprintf(HEXS,"%02X",cks);
  strcat(s,HEXS);
}

// callback wrappers
#ifdef SERIAL_A
  CommandProcessor processCommandsA(SERIAL_A_BAUD_DEFAULT);
  void processCmdsA() { processCommandsA.poll(); }
#endif
#ifdef SERIAL_B
  CommandProcessor processCommandsB(SERIAL_B_BAUD_DEFAULT);
  void processCmdsB() { processCommandsB.poll(); }
#endif
#ifdef SERIAL_C
  CommandProcessor processCommandsC(SERIAL_C_BAUD_DEFAULT);
  void processCmdsC() { processCommandsC.poll(); }
#endif
#ifdef SERIAL_D
  CommandProcessor processCommandsD(SERIAL_D_BAUD_DEFAULT);
  void processCmdsD() { processCommandsD.poll(); }
#endif
#ifdef SERIAL_ST4
  CommandProcessor processCommandsST4(9600);
  void processCmdsST4() { processCommandsST4.poll(); }
#endif
