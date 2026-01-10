//--------------------------------------------------------------------------------------------------
// remote telescope auxiliary FEATURES control, using the CANbus interface

#include "Features.h"

#ifdef FEATURES_CAN_CLIENT_PRESENT

// Heartbeat RX: update last-heard timestamp for FEATURES
static void featHb(uint8_t data[8], uint8_t len) { (void)data; (void)len; features.heartbeat(); }

void Features::begin() {
  canPlus.callbackRegisterId((int)(CAN_FEATURES_HB_ID_BASE), featHb);
}

// by default reply[80] == "", supressFrame == false, numericReply == true, and commandError == CE_NONE
// return true if the command has been completely handled and no further command() will be called, or false if not
// for commands that are handled repeatedly commandError might contain CE_NONE or CE_1 to indicate success
// numericReply=true means boolean/numeric-style responses (e.g., CE_1/CE_0/errors) rather than a payload
bool Features::command(char *reply, char *command, char *parameter,
                      bool *suppressFrame, bool *numericReply, CommandError *commandError) {
  if (!canPlus.ready) return false;

  // Only handle Aux Feature command family:
  //  :GXXn#  :GXYn#  :SXXn,*#
  if (!((command[0] == 'G' && command[1] == 'X' && (parameter[0] == 'X' || parameter[0] == 'Y')) ||
        (command[0] == 'S' && command[1] == 'X' && parameter[0] == 'X'))) return false;

  // Presence gate (act like no FEATURES provider exists)
  if (!heartbeatFresh()) return false;

  uint8_t opcode = 0, tidop = 0;
  uint8_t req[8] = {0};
  uint8_t reqLen = 0;

  if (!encodeRequest(opcode, tidop, req, reqLen, command, parameter)) return false;

  // Choose single- vs two-frame transaction based on opcode
  if (opcode == FEAT_OP_GET_INFO_Yn) {
    uint8_t rsp14[14] = {0};
    uint8_t rspLen = 0;

    if (!transact2(tidop, req, reqLen, rsp14, rspLen) || rspLen < 2 || rsp14[0] != tidop) {
      *commandError = CE_REPLY_UNKNOWN;
      return true;
    }

    bool handled = false, nr = true, sf = false;
    uint8_t ce = 0;
    unpackStatus(rsp14[1], handled, nr, sf, ce);

    if (!handled) { *commandError = CE_CMD_UNKNOWN; return true; }

    *numericReply = nr;
    *supressFrame = sf;
    *commandError = (CommandError)ce;

    if (!*numericReply) {
      if (!decodeResponse(reply, opcode, rsp14, rspLen, *supressFrame, *numericReply)) {
        *commandError = CE_REPLY_UNKNOWN;
        *numericReply = true;
        reply[0] = 0;
      }
    }

    return true;

  } else {
    uint8_t rsp[8] = {0};
    uint8_t rspLen = 0;

    if (!transact(tidop, req, reqLen, rsp, rspLen) || rspLen < 2 || rsp[0] != tidop) {
      DL("Transact failed"); 
      *commandError = CE_REPLY_UNKNOWN;
      return true;
    }

    bool handled = false, nr = true, sf = false;
    uint8_t ce = 0;
    unpackStatus(rsp[1], handled, nr, sf, ce);

    if (!handled) { *commandError = CE_CMD_UNKNOWN; return true; }

    *numericReply = nr;
    *supressFrame = sf;
    *commandError = (CommandError)ce;

    if (!*numericReply) {
      if (!decodeResponse(reply, opcode, rsp, rspLen, *supressFrame, *numericReply)) {
        *commandError = CE_REPLY_UNKNOWN;
        *numericReply = true;
        reply[0] = 0;
      }
    }

    D("Transact response>"); DL(reply);
    return true;
  }
}

Features features;

#endif
