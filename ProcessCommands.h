// -----------------------------------------------------------------------------------
// Command processing

#include "src/lib/BufferCmds.h"
#include "src/lib/SerialWrapper.h"

class CommandProcessor {
  public:
    CommandProcessor(long baud) {
      serialBaud = baud;
    }

    ~CommandProcessor() {
      SerialPort.end();
    }

    CommandErrors command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply) {

      // Handle observatory date/time/location commands
      if (observatory.command(reply, command, parameter, supressFrame, numericReply, &commandError)) {} else

      // Handle telescope goto, etc. commands
      if (telescope.command(reply, command, parameter, supressFrame, numericReply, &commandError)) {} else

      return CE_CMD_UNKNOWN;
    }

    void poll() {
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

  private:

    void appendChecksum(char s[]) {
      char HEXS[3]="";
      byte cks=0; for (unsigned int cksCount0=0; cksCount0 < strlen(s); cksCount0++) {  cks+=s[cksCount0]; }
      sprintf(HEXS,"%02X",cks);
      strcat(s,HEXS);
    }

    CommandErrors commandError     = CE_NONE;
    CommandErrors lastCommandError = CE_NONE;
    bool serialReady               = false;
    long serialBaud                = 9600;

    Buffer buffer;
    SerialWrapper SerialPort;
};

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
