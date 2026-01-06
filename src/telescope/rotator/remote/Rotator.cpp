//--------------------------------------------------------------------------------------------------
// remote telescope rotator control, using the CANbus interface

#include "Rotator.h"

#ifdef ROTATOR_CAN_CLIENT_PRESENT

// by default reply[80] == "", supressFrame == false, numericReply == true, and commandError == CE_NONE
// return true if the command has been completely handled and no further command() will be called, or false if not
// for commands that are handled repeatedly commandError might contain CE_NONE or CE_1 to indicate success
// note the default numericReply == true overides supressFrame so it's false (0 or 1 is returned)
bool Rotator::command(char *reply, char *command, char *parameter,
                      bool *supressFrame, bool *numericReply, CommandError *commandError) {
  if (!canPlus.ready) return false;

  uint8_t opcode = 0, tidop = 0;
  uint8_t req[8] = {0};
  uint8_t reqLen = 0;

  // --------------------------------------------------------------------------
  // r?, hP, hR, GX98 - rotator commands only
  // --------------------------------------------------------------------------
  if (!(command[0] == 'r') &&
      !(command[0] == 'h' && command[1] == 'P') &&
      !(command[0] == 'h' && command[1] == 'R') &&
      !(command[0] == 'G' && command[1] == 'X' && parameter[0] == '9' && parameter[1] == '8')) return false;

  if (!encodeRequest(opcode, tidop, req, reqLen, command, parameter)) return false;

  uint8_t rsp[8] = {0};
  uint8_t rspLen = 0;

  // Combined validation: transact + minimum payload + strong correlation
  if (!transact(tidop, req, reqLen, rsp, rspLen) || rspLen < 2 || rsp[0] != tidop) {
    *commandError = CE_REPLY_UNKNOWN;
    return true; // numeric 0 via defaults
  }

  bool handled = false, nr = true, sf = false;
  uint8_t ce = 0;
  unpackStatus(rsp[1], handled, nr, sf, ce);

  if (!handled) {
    *commandError = CE_CMD_UNKNOWN;
    return true; // numeric 0 via defaults
  }

  // remote owns policy (even for CE_NONE / CE_1 / CE_0)
  *numericReply = nr;
  *supressFrame = sf;
  *commandError = (CommandError)ce;

  // if this isn't a numeric reply decode it
  if (!*numericReply) {
    if (!decodeResponse(reply, opcode, rsp, rspLen, *supressFrame, *numericReply)) {
      // force failure numeric code 0 if decode fails
      *commandError = CE_REPLY_UNKNOWN;
      *numericReply = true;
      reply[0] = 0;
    }
  }

  return true;
}

Rotator rotator;

#endif
