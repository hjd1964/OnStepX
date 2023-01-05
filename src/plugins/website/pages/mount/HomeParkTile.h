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
