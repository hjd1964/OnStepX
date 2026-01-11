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

// by default reply[80] == "", suppressFrame == false, numericReply == true, and commandError == CE_NONE
// return true if the command has been completely handled and no further command() will be called, or false if not
// for commands that are handled repeatedly commandError might contain CE_NONE or CE_1 to indicate success
// numericReply=true means boolean/numeric-style responses (e.g., CE_1/CE_0/errors) rather than a payload
bool Focuser::command(char *reply, char *command, char *parameter,
                      bool *suppressFrame, bool *numericReply, CommandError *commandError) {
  if (!canPlus.ready) return false;

  // find the default focuser if the current one isn't active
  // if the current focuser is unassigned (-1) or disappears scan again
  if (!heartbeatFresh(active)) {
    active = -1;
    for (int focuserIdx = 0; focuserIdx < 6; focuserIdx++) {
      if (heartbeatFresh(focuserIdx)) { active = focuserIdx; break; }
    }
  }

  // --------------------------------------------------------------------------
  // :hP# / :hR#  park/unpark ALL present focusers
  // Numeric result only. Defaults: numericReply==true, suppressFrame==false.
  // --------------------------------------------------------------------------
  if (command[0] == 'h' && (command[1] == 'P' || command[1] == 'R') && parameter[0] == 0) {
    const uint8_t op = (command[1] == 'P') ? (uint8_t)FOC_OP_PARK_HP : (uint8_t)FOC_OP_UNPARK_HR;

    bool anySent = false;
    CommandError worst = CE_NONE; 

    for (uint8_t focuserIdx = 0; focuserIdx < 6; focuserIdx++) {
      uint8_t ctrl = (uint8_t)(focuserIdx & FOC_CTRL_FOCUSER_MASK);

      if (!heartbeatFresh(focuserIdx)) continue;
      anySent = true;

      if (beginNewRequest(op)) {
        writeU8(ctrl);
        bool handled = false;
        if (transactRequest(handled, *suppressFrame, *numericReply, *commandError)) {
          if (!handled) worst = CE_CMD_UNKNOWN;
        } else *commandError = CE_REPLY_UNKNOWN;
      }

      if (*commandError != CE_NONE && *commandError != CE_1) worst = *commandError;
    }

    *commandError = (!anySent) ? CE_0 : ((worst == CE_NONE) ? CE_1 : worst);
    return true;
  }

  // --------------------------------------------------------------------------
  // F - focuser commands only
  // --------------------------------------------------------------------------
  if (command[0] != 'F') return false;

  // --------------------------------------------------------------------------
  // active focuser return / selection
  // --------------------------------------------------------------------------

  if (command[1] == 'A') {

    // :FA#    Focuser Active?
    //            Return: 0 on failure (no focusers)
    //                    1 to 6 on success
    if (parameter[0] == 0) {
      if (active < 0) { *commandError = CE_0; return true; }

      const uint8_t focuserNum = (uint8_t)(active + 1);
      sprintf(reply, "%u", (unsigned)focuserNum);
      *numericReply = false;
      *suppressFrame = true;
      *commandError = CE_NONE;
      return true;
    } else

    // :FA[n]#    Select focuser where [n] = 1 to 6
    //            Return: 0 on failure
    //                    1 on success
    if (parameter[1] == 0) {
      const int i = parameter[0] - '1';
      if (i < 0 || i > 5 || !heartbeatFresh(i)) { *commandError = CE_PARAM_RANGE; return true; }
      active = i;
      return true;
    } else return false;
  }

  // --------------------------------------------------------------------------
  // :Fa#       Focuser presence detection
  //            Returns: 1 true if present
  // --------------------------------------------------------------------------
  if (command[1] == 'a' && parameter[0] == 0) {
    const int i = parameter[0] - '1';
    if (i < 0 || i > 5 || !heartbeatFresh(i)) { *commandError = CE_PARAM_RANGE; return true; }
    return true;
  }

  // :F[n]a#    Focuser presence detection
  if ((command[1] >= '1' && command[1] <= '6' && parameter[0] == 'a' && parameter[1] == 0)) {
    sprintf(reply, "%u", heartbeatFresh((int)(command[1] - '1')) ? 1u : 0u);
    *numericReply = false;
    *suppressFrame = true;
    *commandError = CE_NONE;
  } else

  // --------------------------------------------------------------------------
  // Generic CAN request/response path
  // --------------------------------------------------------------------------

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

Focuser focuser;

#endif
