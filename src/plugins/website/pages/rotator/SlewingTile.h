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
