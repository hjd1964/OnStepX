//--------------------------------------------------------------------------------------------------
// remote telescope rotator control, using the CANbus interface

#include "Rotator.h"

#ifdef ROTATOR_CAN_CLIENT_PRESENT

// --------------------------------------------------------------------------
// Heartbeat RX (local node): update last-heard timestamp for rotator
// --------------------------------------------------------------------------
static void rotHb(uint8_t data[8], uint8_t len) { (void)data; (void)len; rotator.heartbeat(); }

void Rotator::begin() {
  canPlus.callbackRegisterId((int)(CAN_ROTATOR_HB_ID_BASE), rotHb);
}

// by default reply[80] == "", suppressFrame == false, numericReply == true, and commandError == CE_NONE
// return true if the command has been completely handled and no further command() will be called, or false if not
// for commands that are handled repeatedly commandError might contain CE_NONE or CE_1 to indicate success
// numericReply=true means boolean/numeric-style responses (e.g., CE_1/CE_0/errors) rather than a payload
bool Rotator::command(char *reply, char *command, char *parameter,
                      bool *suppressFrame, bool *numericReply, CommandError *commandError) {
  if (!canPlus.ready) return false;

  // r?, hP, hR, GX98 - rotator commands only
  if (!(command[0] == 'r') ||
      !(command[0] == 'h' && command[1] == 'P') ||
      !(command[0] == 'h' && command[1] == 'R') ||
      !(command[0] == 'G' && command[1] == 'X' && parameter[0] == '9' && parameter[1] == '8')) return false;

  // Presence gate
  if (!heartbeatFresh()) return false;

  // Build CAN request (also sets opCode()/tidop internally per your new design)
  if (!encodeRequest(command, parameter)) return false;

  // Transport failure => reply unknown
  bool handled = false;
  if (!transactRequest(handled, *suppressFrame, *numericReply, *commandError)) { *commandError  = CE_REPLY_UNKNOWN; return true; }

  // Remote explicitly did not recognize/handle the command
  if (!handled) {
    *commandError  = CE_CMD_UNKNOWN;
    *numericReply  = true;
    *suppressFrame = false;
    reply[0] = 0;
    return true;
  }

  // Numeric reply: nothing to decode; caller will render numeric based on commandError
  if (*numericReply) return true;

  // Payload reply: decode into reply buffer; decode failure => reply unknown
  if (!decodeResponse(reply)) {
    *commandError  = CE_REPLY_UNKNOWN;
    *numericReply  = true;
    *suppressFrame = false;
    reply[0] = 0;
  }

  return true;
}

Rotator rotator;

#endif
