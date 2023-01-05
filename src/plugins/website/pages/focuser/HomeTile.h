// -----------------------------------------------------------------------------------
// Focuser Home tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

extern void focuserHomeTile(String &data);
extern void focuserHomeTileAjax(String &data);
extern void focuserHomeTileGet();

const char html_focuserHome[] PROGMEM =
  "<button onpointerdown=\"gf('Fh')\" type='button'>"
  ARROW_DR HOME_CH " " L_FIND_HOME "</button>"
  "<button onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) gf('FH')\" type='button'>"
  CAUTION_CH HOME_CH " " L_RESET_HOME "</button>&nbsp;&nbsp;&nbsp;&nbsp;";
