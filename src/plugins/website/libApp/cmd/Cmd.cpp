// -----------------------------------------------------------------------------------
// Help with commands, etc.

#include "Cmd.h"
#include "../../locales/Locale.h"
#include "../../../../libApp/commands/ProcessCmds.h"

int webTimeout = TIMEOUT_WEB;
int cmdTimeout = TIMEOUT_CMD;

void OnStepCmd::serialRecvFlush() {
  while (SERIAL_ONSTEP.available() > 0) SERIAL_ONSTEP.read();
}

// smart LX200 aware command and response (up to 80 chars) over serial
bool OnStepCmd::processCommand(const char* cmd, char* response, long timeOutMs) {
  SERIAL_ONSTEP.setTimeout(timeOutMs);

  // clear the read/write buffers
  serialRecvFlush();

  // send the command
  SERIAL_ONSTEP.transmit(cmd);
  delay(0);

  response[0] = 0;
  bool noResponse = false;
  bool shortResponse = false;
  if (cmd[0] == (char)6 && cmd[1] == 0) shortResponse = true;
  if (cmd[0] == ':' || cmd[0] == ';') {
    if (cmd[1] == 'G') {
      if (strchr("RDE", cmd[2])) { if (timeOutMs < 300) timeOutMs = 300; }
    } else
    if (cmd[1] == 'M') {
      if (strchr("ewnsg", cmd[2])) noResponse = true;
      if (strchr("ADNPS", cmd[2])) shortResponse = true;
    } else
    if (cmd[1] == 'Q') {
      if (strchr("#ewns", cmd[2])) noResponse = true;
    } else
    if (cmd[1] == 'A') {
      if (strchr("W123456789+",cmd[2])) { shortResponse = true; if (timeOutMs < 1000) timeOutMs = 1000; }
    } else
    if ((cmd[1] == 'F') || (cmd[1] == 'f')) {
      if (strchr("123456", cmd[2]) && cmd[3] != '#') {
        // direct focuser select command?
        if (strchr("+-QZHhF", cmd[3])) noResponse = true;
        if (strchr("1234", cmd[3])) noResponse = true;
        if (strchr("Aapc",cmd[3])) shortResponse = true;
      } else {
        // normal command
        if (strchr("+-QZHhF", cmd[2])) noResponse = true;
        if (strchr("1234", cmd[2])) noResponse = true;
        if (strchr("Aapc",cmd[2])) shortResponse = true;
      }
    } else
    if (cmd[1] == 'r') {
      if (strchr("+-PRFC<>Q1234", cmd[2])) noResponse = true;
      if (strchr("~S",cmd[2])) shortResponse = true;
    } else
    if (cmd[1] == 'R') {
      if (strchr("AEGCMS0123456789", cmd[2])) noResponse = true;
    } else
    if (cmd[1] == 'S') {
      if (strchr("CLSGtgMNOPrdhoTBX", cmd[2])) shortResponse = true;
    } else
    if (cmd[1] == 'L') {
      if (strchr("BNCDL!",cmd[2])) noResponse = true;
      if (strchr("o$W", cmd[2])) { shortResponse = true; if (timeOutMs < 1000) timeOutMs = 1000; }
    } else
    if (cmd[1] == 'B') {
      if (strchr("+-", cmd[2])) noResponse = true;
    } else
    if (cmd[1] == 'C') {
      if (strchr("S", cmd[2])) noResponse = true;
    } else
    if (cmd[1] == 'h') {
      if (strchr("FC", cmd[2])) { noResponse = true; if (timeOutMs < 1000) timeOutMs = 1000; }
      if (strchr("QPR", cmd[2])) { shortResponse = true; if (timeOutMs < 300) timeOutMs = 300; }
    } else
    if (cmd[1] == 'T') {
      if (strchr("QR+-SLK", cmd[2])) noResponse = true;
      if (strchr("edrn", cmd[2])) shortResponse = true;
    } else
    if (cmd[1] == 'U') {
      noResponse = true; 
    } else
    if (cmd[1] == 'W') {
      if (strchr("R", cmd[2])) {
        if (strchr("+-", cmd[3])) shortResponse = true; else noResponse = true; // WR+ WR- else WR
      }
      if (strchr("S", cmd[2])) shortResponse = true;  // WS
      if (strchr("0123", cmd[2])) noResponse = true;  // W0 W1 W2 W3
    } else
    if (cmd[1] == '$' && cmd[2] == 'Q' && cmd[3] == 'Z') {
      if (strchr("+-Z/!", cmd[4])) noResponse = true;
    }

    // override for checksum protocol
    if (cmd[0] == ';') { noResponse = false; shortResponse = false; }
  }

  unsigned long timeout = millis() + (unsigned long)timeOutMs;
  if (noResponse) {
    response[0] = 0;
    delay(50);
    return true;
  } else
  if (shortResponse) {
    while ((long)(timeout - millis()) > 0) {
      delay(1);
      if (SERIAL_ONSTEP.receiveAvailable() >= 1) {
        char *recv = SERIAL_ONSTEP.receive();
        strcpy(response, recv);
        break;
      }
    }

    return (response[0] != 0);
  } else {
    int i = 0;
    // get full response, '#' terminated
    while ((long)(timeout - millis()) > 0) {
      if (i++ % 10 == 0) delay(1);
      if (SERIAL_ONSTEP.receiveAvailable() >= 1) {
        char *recv = SERIAL_ONSTEP.receive();
        strcat(response, recv);
        if (response[strlen(response) - 1] == '#') break;
      }
    }

    return response[strlen(response) - 1] == '#';
  }
}

bool OnStepCmd::command(const char* command, char* response) {
  bool success = processCommand(command, response, webTimeout);
  int l = strlen(response) - 1;
  if (l >= 0 && response[l] == '#') response[l] = 0;
  return success;
}

bool OnStepCmd::commandBlind(const char* command) {
  char response[80] = "";
  return processCommand(command, response, webTimeout);
}

bool OnStepCmd::commandEcho(const char* command) {
  char response[80] = "";
  char c[40] = "";
  sprintf(c, ":EC%s#", command);
  return processCommand(c, response, webTimeout);
}

bool OnStepCmd::commandBool(const char* command) {
  char response[80] = "";
  bool success = processCommand(command, response, webTimeout);
  int l = strlen(response) - 1; if (l >= 0 && response[l] == '#') response[l] = 0;
  if (!success) return false;
  if (response[1] != 0) return false;
  if (response[0] == '0') return false; else return true;
}

char* OnStepCmd::commandString(const char* command) {
  static char response[80] = "";
  bool success = processCommand(command, response, webTimeout);
  int l = strlen(response) - 1;
  if (l >= 0 && response[l] == '#') response[l] = 0;
  if (!success) strcpy(response,"?");
  return response;
}

char* OnStepCmd::commandErrorToStr(int e) {
  static char reply[40];
  strcpy(reply, "Error, ");
  
  switch (e) {
    case CE_NONE: strcpy(reply, L_CE_NONE); break;
    case CE_0: strcpy(reply, L_CE_0); break;
    case CE_CMD_UNKNOWN: strcat(reply, L_CE_CMD_UNKNOWN); break;
    case CE_REPLY_UNKNOWN: strcat(reply, L_CE_REPLY_UNKNOWN); break;
    case CE_PARAM_RANGE: strcat(reply, L_CE_PARAM_RANGE); break;
    case CE_PARAM_FORM: strcat(reply, L_CE_PARAM_FORM); break;
    case CE_ALIGN_FAIL: strcat(reply, L_CE_ALIGN_FAIL); break;
    case CE_ALIGN_NOT_ACTIVE: strcat(reply, L_CE_ALIGN_NOT_ACTIVE); break;
    case CE_NOT_PARKED_OR_AT_HOME: strcat(reply, L_CE_NOT_PARKED_OR_AT_HOME); break;
    case CE_PARKED: strcat(reply, L_CE_PARKED); break;
    case CE_PARK_FAILED: strcat(reply, L_CE_PARK_FAILED); break;
    case CE_NOT_PARKED: strcat(reply, L_CE_NOT_PARKED); break;
    case CE_NO_PARK_POSITION_SET: strcat(reply, L_CE_NO_PARK_POSITION_SET); break;
    case CE_GOTO_FAIL: strcat(reply, L_CE_GOTO_FAIL); break;
    case CE_LIBRARY_FULL: strcat(reply, L_CE_LIBRARY_FULL); break;
    case CE_SLEW_ERR_BELOW_HORIZON: strcat(reply, L_CE_GOTO_ERR_BELOW_HORIZON); break;
    case CE_SLEW_ERR_ABOVE_OVERHEAD: strcat(reply, L_CE_GOTO_ERR_ABOVE_OVERHEAD); break;
    case CE_SLEW_ERR_IN_STANDBY: strcat(reply, L_CE_SLEW_ERR_IN_STANDBY); break;
    case CE_SLEW_ERR_IN_PARK: strcat(reply, L_CE_SLEW_ERR_IN_PARK); break;
    case CE_SLEW_IN_SLEW: strcat(reply, L_CE_GOTO_ERR_GOTO); break;
    case CE_SLEW_ERR_OUTSIDE_LIMITS: strcat(reply, L_CE_GOTO_ERR_OUTSIDE_LIMITS); break;
    case CE_SLEW_ERR_HARDWARE_FAULT: strcat(reply, L_CE_SLEW_ERR_HARDWARE_FAULT); break;
    case CE_SLEW_IN_MOTION: strcat(reply, L_CE_MOUNT_IN_MOTION); break;
    case CE_SLEW_ERR_UNSPECIFIED: strcat(reply, L_CE_GOTO_ERR_UNSPECIFIED); break;
    case CE_NULL: strcpy(reply,""); break;
    default: strcat(reply,L_CE_UNK);
  }

  return reply;
}

OnStepCmd onStep;
