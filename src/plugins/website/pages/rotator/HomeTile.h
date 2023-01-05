// -----------------------------------------------------------------------------------
// Rotator Home tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

extern void rotatorHomeTile(String &data);
extern void rotatorHomeTileAjax(String &data);
extern void rotatorHomeTileGet();

const char html_rotatorHome[] PROGMEM =
  "<button style='height: 2.1em' onpointerdown=\"s('rot','home')\" type='button'>"
  ARROW_DR HOME_CH " " L_FIND_HOME "</button>"
  "<button style='height: 2.1em' onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) s('rot','reset')\" type='button'>"
  CAUTION_CH HOME_CH " " L_RESET_HOME "</button>";
