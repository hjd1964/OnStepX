// -----------------------------------------------------------------------------------
// Tracking tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

extern void trackingTile(String &data);
extern void trackingTileAjax(String &data);
extern void trackingTileGet();

const char html_trackingEnable[] PROGMEM = 
"<button id='trk_on' class='btns_right' onpointerdown=\"s('dr','T1')\" type='button'>" L_ON "</button>"
"<button id='trk_off' class='btns_left' onpointerdown=\"s('dr','T0')\" type='button'>" L_OFF "</button>";

const char html_trackingPreset[] PROGMEM =
"<br /><button id='trk_sid' class='bbh' style='width: 2em' onpointerdown=\"g('Ts')\" title='" L_HINT_RATE_SIDEREAL "' type='button'>" SIDEREAL_CH "</button>"
"<button id='trk_lun' class='bbh' style='width: 2em' onpointerdown=\"g('Tl')\" title='" L_HINT_RATE_LUNAR "' type='button'>" LUNAR_CH "</button>"
"<button id='trk_sol' class='bbh' style='width: 2em' onpointerdown=\"g('Th')\" title='" L_HINT_RATE_SOLAR "' type='button'>" SOLAR_CH "</button>"
"<button id='trk_king' class='bbh' style='width: 2em' onpointerdown=\"g('Tk')\" title='" L_HINT_RATE_KING "' type='button'>" KING_CH "</button>";

const char html_trackingCompensationType[] PROGMEM =
L_TRK_COMP ": <br />"
"<button id='ot_on'  class='btns_right' onpointerdown=\"s('rr','otk')\"  type='button'>" L_TRK_FULL "</button>"
"<button id='ot_ref' class='btns_mid'   onpointerdown=\"s('rr','on')\"   type='button'>" L_TRK_REFR "</button>"
"<button id='ot_off' class='btns_left'  onpointerdown=\"s('rr','off')\"  type='button'>" L_OFF "</button><br />\n";

const char html_trackingCompensationAxes[] PROGMEM =
"<button id='ot_dul' class='btns_right' onpointerdown=\"s('rr','don')\"  type='button'>" L_TRK_DUAL "</button>"
"<button id='ot_sgl' class='btns_left'  onpointerdown=\"s('rr','doff')\" type='button'>" L_TRK_SINGLE "</button>"
"<br /><br />\n";

const char html_baseFrequency[] PROGMEM =
L_FREQUENCY_ADJUST ": <br />"
"<button onpointerdown=\"s('tk','f')\" type='button'>+ (0.02Hz " L_TRK_FASTER ")</button>"
"<button onpointerdown=\"s('tk','-')\" type='button'>- (0.02Hz " L_TRK_SLOWER ")</button>"
"<button onpointerdown=\"s('tk','r')\" type='button'>" L_TRK_RESET "</button>"
"<br />\n";
