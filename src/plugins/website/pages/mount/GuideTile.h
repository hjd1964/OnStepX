// -----------------------------------------------------------------------------------
// Guide tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

extern void guideTile(String &data);
extern void guideTileAjax(String &data);
extern void guideTileGet();

const char html_guidePad[] PROGMEM =
"<button class='gb' onpointerdown=\"g('n1')\" onpointerup=\"g('n0')\" type='button'>" BUTTON_N "</button><br />"
"<button class='gb' onpointerdown=\"g('e1')\" onpointerup=\"g('e0')\" type='button'>" BUTTON_E "</button>"
"<button class='gb' onpointerdown=\"g('sy')\" title='Sync' type='button'>" BUTTON_SYNC "</button>"
"<button class='gb' onpointerdown=\"g('w1')\" onpointerup=\"g('w0')\" type='button'>" BUTTON_W "</button><br />"
"<button class='gb' onpointerdown=\"g('s1')\" onpointerup=\"g('s0')\" type='button'>" BUTTON_S "</button><br />";

const char html_guidePulseRates[] PROGMEM =
"<button id='guide_r0' class='btns_right' onpointerdown=\"g('R0')\" type='button'>0.25</button>"
"<button id='guide_r1' class='btns_mid' onpointerdown=\"g('R1')\" type='button'>0.5</button>"
"<button id='guide_r2' class='btns_left' onpointerdown=\"g('R2')\" type='button'>1x</button><br />";

const char html_guideRates[] PROGMEM =
"<button id='guide_r3' class='btns_right' onpointerdown=\"g('R3')\" type='button'>2x</button>"
"<button id='guide_r4' class='btns_mid' onpointerdown=\"g('R4')\" type='button'>4x</button>"
"<button id='guide_r5' class='btns_mid' onpointerdown=\"g('R5')\" type='button'>8x</button>"
"<button id='guide_r6' class='btns_mid' onpointerdown=\"g('R6')\" type='button'>20x</button>"
"<button id='guide_r7' class='btns_mid' onpointerdown=\"g('R7')\" type='button'>48x</button>"
"<button id='guide_r8' class='btns_mid' onpointerdown=\"g('R8')\" type='button'>VF</button>"
"<button id='guide_r9' class='btns_left' onpointerdown=\"g('R9')\" type='button'>VVF</button><br />";
