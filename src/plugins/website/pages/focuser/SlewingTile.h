// -----------------------------------------------------------------------------------
// Focuser Slewing tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

extern void focuserSlewingTile(String &data);
extern void focuserSlewingTileAjax(String &data);
extern void focuserSlewingTileGet();

const char html_focPosition[] PROGMEM =
  "<input id='foc_set' style='width: 6em;' value='0' type='number' min='-500000' max='500000'> um&nbsp;&nbsp;"
  "<button class='bbh' onpointerdown=\"s('fs',document.getElementById('foc_set').value)\" type='button'>" L_SET_LABEL "</button></span>";

const char html_focStop[] PROGMEM =
  "<button class='bbh' onpointerdown=\"s('foc','stop')\" type='button' style='color: White; margin-bottom: 0.5em;' title='" L_HINT_EMERGENCY_STOP "'>" L_STOP "!</button><br />";

const char html_focuserIn[] PROGMEM =
  "<button class='bbh' onpointerdown=\"s('foc','inf')\" onpointerup=\"s('foc','stop');\" type='button'>" ARROW_DD "</button>"
  "<button class='bbh' style='width: 2em' onpointerdown=\"s('foc','in')\" onpointerup=\"s('foc','stop');\" type='button'>" ARROW_D "</button>";

const char html_focuserOut[] PROGMEM =
  "<button class='bbh' style='width: 2em' onpointerdown=\"s('foc','out')\" onpointerup=\"s('foc','stop');\" type='button'>" ARROW_U "</button>"
  "<button class='bbh' onpointerdown=\"s('foc','outf')\" onpointerup=\"s('foc','stop');\" type='button'>" ARROW_UU "</button>";
