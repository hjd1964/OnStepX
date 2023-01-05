// -----------------------------------------------------------------------------------
// PEC tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

extern void pecTile(String &data);
extern void pecTileAjax(String &data);
extern void pecTileGet();

const char html_pecStatus[] PROGMEM = 
"<div style='float: left'>" L_PEC_STATUS ":</div><br/>"
"<div id='pec_sta'>?</div>";

const char html_pecControls1[] PROGMEM =
"<button onpointerdown=\"s('pec','play')\" type='submit'>" L_PEC_PLAY "</button>";

const char html_pecControls2[] PROGMEM =
"<button onpointerdown=\"s('pec','stop')\" type='submit'>" L_STOP "</button><br /><br />"
"<button onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) s('pec','clr')\" type='submit'>" L_PEC_CLEAR "</button>"
"<button onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) s('pec','rec')\" type='submit'>" L_PEC_REC "</button><br />";

const char html_pecControls3[] PROGMEM =
L_PEC_CLEAR_MESSAGE "<br />";

const char html_pecControls4[] PROGMEM =
"<br /><button onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) s('pec','wrt')\" type='submit'>" L_PEC_EEWRITE "</button><br />" L_PEC_EEWRITE_MESSAGE "<br />";
