// -----------------------------------------------------------------------------------
// Rotator Goto tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

extern void rotatorSlewingTile(String &data);
extern void rotatorSlewingTileAjax(String &data);
extern void rotatorSlewingTileGet();

const char html_rotPosition[] PROGMEM =
  "<input id='rot_set' style='width: 6em;' value='0.0' type='number' min='-360' max='360' step='any'> &deg;&nbsp;&nbsp;"
  "<button class='bbh' onpointerdown=\"s('rs',document.getElementById('rot_set').value)\" type='button'>" L_SET_LABEL "</button></span>";

const char html_rotStop[] PROGMEM =
  "<button class='bbh' onpointerdown=\"s('rot','stop')\" type='button' style='color: White; margin-bottom: 0.5em;' title='" L_HINT_EMERGENCY_STOP "'>" L_STOP "!</button><br />";

const char html_rotateCcw[] PROGMEM =
  "<button class='bbh' style='height: 2.1em' onpointerdown=\"s('rot','ccwf')\" onpointerup=\"s('rot','stop')\" type='button'>" ARROW_LL "</button>"
  "<button class='bbh' style='width: 2em' onpointerdown=\"s('rot','ccw')\" onpointerup=\"s('rot','stop')\" type='button'>" ARROW_L "</button>";

const char html_rotateCw[] PROGMEM =
  "<button class='bbh' style='width: 2em' onpointerdown=\"s('rot','cw')\" onpointerup=\"s('rot','stop')\" type='button'>" ARROW_R "</button>"
  "<button class='bbh' style='height: 2.1em' onpointerdown=\"s('rot','cwf')\" onpointerup=\"s('rot','stop')\" type='button'>" ARROW_RR "</button><br />";

const char html_rotateSlewSpeed[] PROGMEM =
  L_MAX_SLEW_SPEED " <span id='rot_rate' class='c'>%s</span>:<br />";

const char html_rotateGotoSelect[] PROGMEM =
  "<button id='rot_rate_vf' class='btns_right' onpointerdown=\"s('rot','vf')\" type='button'>" L_VFAST "</button>"
  "<button id='rot_rate_f' class='btns_mid' onpointerdown=\"s('rot','f')\"  type='button'>" L_FAST "</button>"
  "<button id='rot_rate_n' class='btns_mid' onpointerdown=\"s('rot','n')\"  type='button'>" L_NORMAL "</button>"
  "<button id='rot_rate_s' class='btns_mid' onpointerdown=\"s('rot','s')\"  type='button'>" L_SLOW "</button>"
  "<button id='rot_rate_vs' class='btns_left' onpointerdown=\"s('rot','vs')\" type='button'>" L_VSLOW "</button>"
  "<br /><br />\n";
