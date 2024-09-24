// -----------------------------------------------------------------------------------
// Home/Park tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

extern void homeParkTile(String &data);
extern void homeParkTileAjax(String &data);
extern void homeParkTileGet();

// Homing
const char html_homeReturn[] PROGMEM =
"<button onpointerdown=\"g('qh')\" type='button'>" ARROW_DR HOME_CH " " L_FIND_HOME "</button>";
const char html_homeReset[] PROGMEM =
"<button onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) g('qr')\" type='button'>" CAUTION_CH HOME_CH " " L_RESET_HOME "</button>";

// Parking
const char html_unpark[] PROGMEM =
"<button id='unpark' onpointerdown=\"s('pk','u')\" type='button' disabled>" ARROW_UR " " L_UNPARK "</button>";
const char html_park[] PROGMEM =
"<button id='park' onpointerdown=\"s('pk','p')\" type='button' disabled>" ARROW_DR " " L_PARK "</button>";
const char html_setpark[] PROGMEM =
"<button id='setpark' onpointerdown=\"s('pk','s')\" type='button'>" L_SET_PARK "</button><br />";

// Home automatically
const char html_homeAuto[] PROGMEM = 
"<button id='auto_on' class='btns_right' onpointerdown=\"s('ha','1')\" type='button'>" L_ON "</button>"
"<button id='auto_off' class='btns_left' onpointerdown=\"s('ha','0')\" type='button'>" L_OFF "</button><br />\n";

// Home offset
const char html_homeOffsetAxis1[] PROGMEM =
  "<input style='width:6em;' value='%ld' type='number' name='hc1' min='-" STR(HOME_OFFSET_RANGE_AXIS1) "' max='" STR(HOME_OFFSET_RANGE_AXIS1) "'>&nbsp;" L_HOME_OFFSET_RANGE_AXIS1 "<br />\n";
const char html_homeOffsetAxis2[] PROGMEM =
  "<input style='width:6em;' value='%ld' type='number' name='hc2' min='-" STR(HOME_OFFSET_RANGE_AXIS2) "' max='" STR(HOME_OFFSET_RANGE_AXIS2) "'>&nbsp;" L_HOME_OFFSET_RANGE_AXIS2 "<br /><br />\n";

#ifdef HOME_SWITCH_DIRECTION_CONTROL
  const char html_homeReverse[] PROGMEM =
    "<button id='hrev1' onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) s('hr','1')\" type='button'>Axis1 Reverse</button>"
    "<button id='hrev2' onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) s('hr','2')\" type='button'>Axis2 Reverse</button><br />";
#endif
