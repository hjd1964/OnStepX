// -----------------------------------------------------------------------------------
// Command processing

#include "src/lib/BufferCmds.h"
#include "src/lib/SerialWrapper.h"

enum CommandErrors {
  CE_NONE, CE_0, CE_CMD_UNKNOWN, CE_REPLY_UNKNOWN, CE_PARAM_RANGE, CE_PARAM_FORM,
  CE_ALIGN_FAIL, CE_ALIGN_NOT_ACTIVE, CE_NOT_PARKED_OR_AT_HOME, CE_PARKED,
  CE_PARK_FAILED, CE_NOT_PARKED, CE_NO_PARK_POSITION_SET, CE_GOTO_FAIL, CE_LIBRARY_FULL,
  CE_GOTO_ERR_BELOW_HORIZON, CE_GOTO_ERR_ABOVE_OVERHEAD, CE_SLEW_ERR_IN_STANDBY, 
  CE_SLEW_ERR_IN_PARK, CE_GOTO_ERR_GOTO, CE_SLEW_ERR_OUTSIDE_LIMITS, CE_SLEW_ERR_HARDWARE_FAULT,
  CE_MOUNT_IN_MOTION, CE_GOTO_ERR_UNSPECIFIED, CE_NULL};

class Command {
  public:
    CommandErrors process(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply) {
    
// G - Get Telescope Information
      if (command[0] == 'G') {
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
        if (command[1] == 'V' && parameter[1] == 0) {
          if (parameter[0] == 'D') strcpy(reply,FirmwareDate); else
          if (parameter[0] == 'M') sprintf(reply,"OnStepX %i.%i%s",FirmwareVersionMajor,FirmwareVersionMinor,FirmwareVersionPatch); else
          if (parameter[0] == 'N') sprintf(reply,"%i.%i%s",FirmwareVersionMajor,FirmwareVersionMinor,FirmwareVersionPatch); else
          if (parameter[0] == 'P') strcpy(reply,FirmwareName); else
          if (parameter[0] == 'T') strcpy(reply,FirmwareTime); else return CE_CMD_UNKNOWN;
          *numericReply = false;
        } else return CE_CMD_UNKNOWN;
      } else

// S - Telescope Set Commands
      if (command[0] == 'S') {
//  :Sd[sDD*MM]# or :Sd[sDD*MM:SS]# or :Sd[sDD*MM:SS.SSS]#
//            Set target object declination
//            Return: 0 on failure
//                    1 on success
        if (command[1] == 'd' && parameter[0] == 0)  {
          if (!transform.dmsToDouble(&target.d,parameter,true)) return CE_PARAM_RANGE;
          target.d = degToRad(target.d);
        } else

//  :Sr[HH:MM.T]# or :Sr[HH:MM:SS]# or :Sr[HH:MM:SS.SSSS]#
//            Set target object RA
//            Return: 0 on failure
//                    1 on success
        if (command[1] == 'r' && parameter[0] == 0)  {
          if (!transform.hmsToDouble(&target.r, parameter)) return CE_PARAM_RANGE;
          target.r = hrsToRad(target.r);
        } else return CE_CMD_UNKNOWN;
      } else return CE_CMD_UNKNOWN;
    }

    void poll() {
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

        commandError = process(reply, buffer.getCmd(), buffer.getParameter(), &supressFrame, &numericReply);

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
      }
    }

  private:
    void appendChecksum(char s[]) {
      char HEXS[3]="";
      byte cks=0; for (unsigned int cksCount0=0; cksCount0 < strlen(s); cksCount0++) {  cks+=s[cksCount0]; }
      sprintf(HEXS,"%02X",cks);
      strcat(s,HEXS);
    }

    Buffer buffer;
    SerialWrapper SerialPort;
    CommandErrors commandError = CE_NONE;
    CommandErrors lastCommandError = CE_NONE;

    EquCoordinate target;
};

#ifdef SERIAL_A
  Command processCommandsA;
#endif
#ifdef SERIAL_B
  Command processCommandsB;
#endif
#ifdef SERIAL_C
  Command processCommandsC;
#endif
#ifdef SERIAL_D
  Command processCommandsD;
#endif
#ifdef SERIAL_ST4
  Command processCommandsST4;
#endif

void processCommands() {
#ifdef SERIAL_A
  processCommandsA.poll();
#endif
#ifdef SERIAL_B
  processCommandsB.poll();
#endif
#ifdef SERIAL_C
  processCommandsC.poll();
#endif
#ifdef SERIAL_D
  processCommandsD.poll();
#endif
#ifdef SERIAL_ST4
  processCommandsST4.poll();
#endif
}
