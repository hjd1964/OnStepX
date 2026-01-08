//--------------------------------------------------------------------------------------------------
// telescope focuser control local node, using the CANbus interface

#include "Focuser.h"

#ifdef FOCUSER_CAN_CLIENT_PRESENT

// --------------------------------------------------------------------------
// Heartbeat RX (local node): update last-heard timestamps for focuser 1..6.
// --------------------------------------------------------------------------
static void focHb0(uint8_t data[8], uint8_t len) { (void)data; (void)len; focuser.heartbeat(0); }
static void focHb1(uint8_t data[8], uint8_t len) { (void)data; (void)len; focuser.heartbeat(1); }
static void focHb2(uint8_t data[8], uint8_t len) { (void)data; (void)len; focuser.heartbeat(2); }
static void focHb3(uint8_t data[8], uint8_t len) { (void)data; (void)len; focuser.heartbeat(3); }
static void focHb4(uint8_t data[8], uint8_t len) { (void)data; (void)len; focuser.heartbeat(4); }
static void focHb5(uint8_t data[8], uint8_t len) { (void)data; (void)len; focuser.heartbeat(5); }

void Focuser::begin() {
  canPlus.callbackRegisterId((int)(CAN_FOCUSER_HB_ID_BASE + 0), focHb0);
  canPlus.callbackRegisterId((int)(CAN_FOCUSER_HB_ID_BASE + 1), focHb1);
  canPlus.callbackRegisterId((int)(CAN_FOCUSER_HB_ID_BASE + 2), focHb2);
  canPlus.callbackRegisterId((int)(CAN_FOCUSER_HB_ID_BASE + 3), focHb3);
  canPlus.callbackRegisterId((int)(CAN_FOCUSER_HB_ID_BASE + 4), focHb4);
  canPlus.callbackRegisterId((int)(CAN_FOCUSER_HB_ID_BASE + 5), focHb5);
}

// by default reply[80] == "", supressFrame == false, numericReply == true, and commandError == CE_NONE
// return true if the command has been completely handled and no further command() will be called, or false if not
// for commands that are handled repeatedly commandError might contain CE_NONE or CE_1 to indicate success
// numericReply=true means boolean/numeric-style responses (e.g., CE_1/CE_0/errors) rather than a payload
bool Focuser::command(char *reply, char *command, char *parameter,
                      bool *supressFrame, bool *numericReply, CommandError *commandError) {
  if (!canPlus.ready) return false;

  // --------------------------------------------------------------------------
  // :hP# / :hR#  park/unpark ALL present focusers
  // Numeric result only. Defaults: numericReply==true, supressFrame==false.
  // --------------------------------------------------------------------------
  if (command[0] == 'h' && (command[1] == 'P' || command[1] == 'R') && parameter[0] == 0) {
    const uint8_t op = (command[1] == 'P') ? (uint8_t)FOC_OP_PARK_HP : (uint8_t)FOC_OP_UNPARK_HR;
    static uint8_t tid = 0;

    bool anySent = false;
    CommandError worst = CE_NONE;

    for (uint8_t focuserIdx = 0; focuserIdx < 6; focuserIdx++) {
      if (!heartbeatFresh(focuserIdx)) continue;
      anySent = true;

      tid = (uint8_t)((tid + 1) & 0x07);
      const uint8_t tidop = packTidOp(tid, op);
      const uint8_t ctrl  = (uint8_t)((focuserIdx + 1) & FOC_CTRL_FOCUSER_MASK);

      uint8_t req[8] = { tidop, ctrl, 0,0,0,0,0,0 };
      uint8_t rsp[8] = {0};
      uint8_t rspLen = 0;

      if (!transact(tidop, req, 2, rsp, rspLen) || rspLen < 2 || rsp[0] != tidop) {
        worst = CE_REPLY_UNKNOWN;
        continue;
      }

      bool handled = false, nr = true, sf = false;
      uint8_t ce = 0;
      unpackStatus(rsp[1], handled, nr, sf, ce);

      if (!handled) { worst = CE_CMD_UNKNOWN; continue; }
      if ((CommandError)ce != CE_NONE && (CommandError)ce != CE_1) worst = (CommandError)ce;
    }

    *commandError = (!anySent) ? CE_0 : ((worst == CE_NONE) ? CE_1 : worst);
    return true;
  }

  // --------------------------------------------------------------------------
  // F - focuser commands only
  // --------------------------------------------------------------------------
  if (command[0] != 'F') return false;

  // find the default focuser if the current one isn't active
  // if the current focuser is unassigned (-1) or disappears scan again
  if (!heartbeatFresh(active)) {
    active = -1;
    for (int focuserIdx = 0; focuserIdx < 6; focuserIdx++) {
      if (heartbeatFresh(focuserIdx)) { active = focuserIdx; break; }
    }
  }

  // --------------------------------------------------------------------------
  // :FA# / :FA[n]# active focuser return / selection
  // --------------------------------------------------------------------------
  if (command[1] == 'A') {

    // :FA# -> query selected focuser (string reply), or numeric 0 if none
    if (parameter[0] == 0) {
      if (active < 0 || active > 5) { *commandError = CE_0; return true; } // numeric 0 via defaults

      if (!heartbeatFresh(active)) { *commandError = CE_0; return true; } // numeric 0

      const uint8_t focuserNum = (uint8_t)(active + 1);

      sprintf(reply, "%u", (unsigned)focuserNum);
      *numericReply = false;
      *supressFrame = true;
      *commandError = CE_NONE;
      return true;
    }

    // :FA[n]# -> set selected focuser; numeric success/fail only
    if (parameter[1] == 0) {
      const int i = parameter[0] - '1';
      if (i < 0 || i >= 6) { *commandError = CE_PARAM_RANGE; return true; }
      active = i;
      return true;
    }

    return false;
  }

  // --------------------------------------------------------------------------
  // :Fa#       Get primary focuser
  //            Returns: 1 true if active
  // --------------------------------------------------------------------------
  if (command[1] == 'a' && parameter[0] == 0) {
    if (!heartbeatFresh(active)) return false; else return true;
  }

  if ((command[1] >= '1' && command[1] <= '6' && parameter[0] == 'a' && parameter[1] == 0)) {
    if (!heartbeatFresh(command[1] - '1')) return false; else return true;
  }

  // --------------------------------------------------------------------------
  // Generic CAN request/response path
  // --------------------------------------------------------------------------
  uint8_t opcode = 0, tidop = 0;
  uint8_t req[8] = {0};
  uint8_t reqLen = 0;

  if (!encodeRequest(opcode, tidop, req, reqLen, command, parameter)) return false;

  uint8_t rsp[8] = {0};
  uint8_t rspLen = 0;

  if (!transact(tidop, req, reqLen, rsp, rspLen) || rspLen < 2 || rsp[0] != tidop) {
    *commandError = CE_REPLY_UNKNOWN;
    return true;
  }

  bool handled = false, nr = true, sf = false;
  uint8_t ce = 0;
  unpackStatus(rsp[1], handled, nr, sf, ce);

  if (!handled) {
    *commandError = CE_CMD_UNKNOWN;
    return true;
  }

  // remote owns policy (even for CE_NONE / CE_1 / CE_0)
  *numericReply = nr;
  *supressFrame = sf;
  *commandError = (CommandError)ce;

  // if this isn't a numeric reply decode it
  if (!*numericReply) {
    if (!decodeResponse(reply, opcode, rsp, rspLen, *supressFrame, *numericReply)) {
      *commandError = CE_REPLY_UNKNOWN;
      *numericReply = true;
      reply[0] = 0;
    }
  }

  return true;
}

Focuser focuser;

#endif
