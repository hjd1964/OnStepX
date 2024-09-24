// -----------------------------------------------------------------------------------
// Telescope auxiliary feature related functions 
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

const char html_auxOnSwitch[] PROGMEM = 
"<button id='sw%d_on' class='btns_right' style='line-height: 1.2rem' onpointerdown=\"s('x%dv1','1')\" type='button'>" L_ON "</button>";
const char html_auxOffSwitch[] PROGMEM = 
"<button id='sw%d_off' class='btns_left' style='line-height: 1.2rem' onpointerdown=\"s('x%dv1','0')\" type='button'>" L_OFF "</button>\n";

const char html_auxStartStop1[] PROGMEM = "<button onpointerdown=\"s('";
const char html_auxStartStop2[] PROGMEM = "','1')\" type='button'>" L_START "</button><button onpointerdown=\"s('";
const char html_auxStartStop3[] PROGMEM = "','0')\" type='button'>" L_STOP "</button>\n";

const char html_auxAnalog[] PROGMEM ="<input style='width: 12em; background: #111' type='range' min='0' max='255' value='";
#if UNITS == METRIC
  const char html_auxHeater[] PROGMEM ="<input style='width: 12em; background: #111' type='range' min='-50' max='200' value='";
#else
  const char html_auxHeater[] PROGMEM ="<input style='width: 12em; background: #111' type='range' min='-90' max='360' value='";
#endif
const char html_auxExposure[] PROGMEM ="<input style='width: 12em; background: #111' type='range' min='0' max='255' value='";
const char html_auxDelay[] PROGMEM ="<input style='width: 12em; background: #111' type='range' min='10' max='255' value='";
const char html_auxCount[] PROGMEM ="<input style='width: 12em; background: #111' type='range' min='1' max='255' value='";
const char html_auxAuxE[] PROGMEM = "</div>\n";

const char html_auxEnd[] PROGMEM = "<br />\n";
