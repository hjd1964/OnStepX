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
//    if (telescope.command(reply, command, parameter, supressFrame, numericReply, &commandError)) {} else

// the following are just here for now, will end up in above...

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
      if (command[0] == 'G' && command[1] == 'V' && parameter[1] == 0) {
        if (parameter[0] == 'D') strcpy(reply,FirmwareDate); else
        if (parameter[0] == 'M') sprintf(reply,"OnStepX %i.%i%s",FirmwareVersionMajor,FirmwareVersionMinor,FirmwareVersionPatch); else
        if (parameter[0] == 'N') sprintf(reply,"%i.%i%s",FirmwareVersionMajor,FirmwareVersionMinor,FirmwareVersionPatch); else
        if (parameter[0] == 'P') strcpy(reply,FirmwareName); else
        if (parameter[0] == 'T') strcpy(reply,FirmwareTime); else return CE_CMD_UNKNOWN;
        *numericReply = false;
      } else return CE_CMD_UNKNOWN;

//  :Sd[sDD*MM]# or :Sd[sDD*MM:SS]# or :Sd[sDD*MM:SS.SSS]#
//            Set target object declination
//            Return: 0 on failure
//                    1 on success
      if (command[0] == 'S' && command[1] == 'd' && parameter[0] == 0)  {
        if (!transform.dmsTodouble(&target.d, parameter, true)) return CE_PARAM_RANGE;
        target.d = degToRad(target.d);
      } else

//  :Sr[HH:MM.T]# or :Sr[HH:MM:SS]# or :Sr[HH:MM:SS.SSSS]#
//            Set target object RA
//            Return: 0 on failure
//                    1 on success
      if (command[0] == 'S' && command[1] == 'r' && parameter[0] == 0)  {
        if (!transform.hmsTodouble(&target.r, parameter)) return CE_PARAM_RANGE;
        target.r = hrsToRad(target.r);
      } else return CE_CMD_UNKNOWN;

      return CE_NONE;
    }

    void poll() {
      if (!serialReady) { SerialPort.begin(serialBaud); serialReady = true; }

      //  char reply[50];
      //  static char command[3];
      //  static char parameter[45];
      //  bool numericReply = true;
      //  bool supressFrame = false;

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

    EquCoordinate target;

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
