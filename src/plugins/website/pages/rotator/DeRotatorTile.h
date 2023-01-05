// -----------------------------------------------------------------------------------
// DeRotator tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

extern void deRotatorTile(String &data);
extern void deRotatorTileAjax(String &data);
extern void deRotatorTileGet();

const char html_controlDeRotate[] PROGMEM =
"<button id='rot_on' class='btns_right' onpointerdown=\"s('rot','on')\" type='button'>" L_ON "</button>"
"<button id='rot_off' class='btns_left' onpointerdown=\"s('rot','off')\" type='button'>" L_OFF "</button><br />"
"<button id='rot_rev' onpointerdown=\"s('rot','rev')\" type='button'>" L_HINT_ROTATOR_REV "</button><br />"
"<button onpointerdown=\"s('rot','par')\" type='button'>" L_HINT_ROTATOR_PAR "</button><br />";
