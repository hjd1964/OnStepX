// -----------------------------------------------------------------------------------
// Focuser Select tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

extern void focuserSelectTile(String &data);
extern void focuserSelectTileAjax(String &data);
extern void focuserSelectTileGet();

const char html_selectFocuser1[] PROGMEM =
"<button id='foc1_sel' class='bbh' onpointerdown=\"gf('F1')\" title='" L_HINT_FOC_SEL " 1' type='button'>1</button>";
const char html_selectFocuser2[] PROGMEM =
"<button id='foc2_sel' class='bbh' onpointerdown=\"gf('F2')\" title='" L_HINT_FOC_SEL " 2' type='button'>2</button>";
const char html_selectFocuser3[] PROGMEM =
"<button id='foc3_sel' class='bbh' onpointerdown=\"gf('F3')\" title='" L_HINT_FOC_SEL " 3' type='button'>3</button>";
const char html_selectFocuser4[] PROGMEM =
"<button id='foc4_sel' class='bbh' onpointerdown=\"gf('F4')\" title='" L_HINT_FOC_SEL " 4' type='button'>4</button>";
const char html_selectFocuser5[] PROGMEM =
"<button id='foc5_sel' class='bbh' onpointerdown=\"gf('F5')\" title='" L_HINT_FOC_SEL " 5' type='button'>5</button>";
const char html_selectFocuser6[] PROGMEM =
"<button id='foc6_sel' class='bbh' onpointerdown=\"gf('F6')\" title='" L_HINT_FOC_SEL " 6' type='button'>6</button>";
