// -----------------------------------------------------------------------------------
// Command processing
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../Extended.Config.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"

#include "BufferCmds.h"
#include "SerialWrapper.h"
#include "../lib/Telescope.h"

#include "ProcessCmds.h"

extern Telescope telescope;

GeneralError generalError;

// command processors
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

CommandError CommandProcessor::command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply) {
  commandError = CE_NONE;

  // Handle telescope commands
  if (telescope.command(reply, command, parameter, supressFrame, numericReply, &commandError)) return commandError;

  return CE_CMD_UNKNOWN;
}

void CommandProcessor::appendChecksum(char s[]) {
  char HEXS[3]="";
  byte cks=0; for (unsigned int cksCount0=0; cksCount0 < strlen(s); cksCount0++) {  cks+=s[cksCount0]; }
  sprintf(HEXS,"%02X",cks);
  strcat(s,HEXS);
}
