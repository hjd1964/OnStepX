// -----------------------------------------------------------------------------------
// Goto tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

extern void gotoTile(String &data);
extern void gotoTileAjax(String &data);
extern void gotoTileGet();

#define Axis1 "&alpha;"
#define Axis2 "&delta;"

const char html_mountPositionLabels[] PROGMEM =
  "<div style='display: table; text-align: left;'><div style='display: table-cell;'>"
  L_CURRENT ":<br /><br />"
  L_TARGET ":<br />"
  "</div>";

const char html_mountPositionAxis1[] PROGMEM =
  "<div style='display: table-cell;'>"
  "&nbsp;&nbsp;&nbsp;Z <span id='gto_az1' class='c'>%s</span><br />"
  "&nbsp;&nbsp;&nbsp;" Axis1 " <span id='gto_i1' class='c'>%s</span><br />"
  "&nbsp;&nbsp;&nbsp;" Axis1 " <span id='gto_t1' class='c'>%s</span><br />"
  "</div>";

const char html_mountPositionAxis2[] PROGMEM =
  "<div style='display: table-cell;'>"
  "&nbsp;&nbsp;&nbsp;A <span id='gto_az2' class='c'>%s</span><br />"
  "&nbsp;&nbsp;&nbsp;" Axis2 " <span id='gto_i2' class='c'>%s</span><br />"
  "&nbsp;&nbsp;&nbsp;" Axis2 " <span id='gto_t2' class='c'>%s</span><br />"
  "</div></div>";

const char html_mountHorizon[] PROGMEM =
  "Z <span id='gto_az1' class='c'>%s</span>, A <span id='gto_az2' class='c'>%s</span><br />";

const char html_mountPosition[] PROGMEM =
  L_CURRENT ": " Axis1 " <span id='gto_i1' class='c'>%s</span>, " Axis2 " <span id='gto_i2' class='c'>%s</span><br />";

const char html_mountTarget[] PROGMEM =
  L_TARGET ": &nbsp;" Axis1 " <span id='gto_t1' class='c'>%s</span>, " Axis2 " <span id='gto_t2' class='c'>%s</span><br />";

const char html_gotoGo[] PROGMEM =
  "<br /><button onpointerdown=\"s('goto','go')\" type='button' class='bb'>" L_SET_LABEL "</button>";

const char html_gotoStop[] PROGMEM =
  "<button onpointerdown=\"s('goto','stop')\" type='button' class='bb' style='height: 3em; width: 5em; color: White;' title='" L_HINT_EMERGENCY_STOP "'>" L_STOP "!</button>";

const char html_gotoContinue[] PROGMEM =
  "<button id='gto_active' onpointerdown=\"s('goto','mp_cnt')\" type='button' disabled class='bb' title='" L_HINT_CONTINUE_GOTO "'>" ARROW_R2 HOME_CH ARROW_R2 "</button>";

const char html_slewSpeed[] PROGMEM =
  L_MAX_SLEW_SPEED " <span id='gto_rate' class='c'>%s</span>:<br />";

const char html_slewSpeedSelect[] PROGMEM =
  "<button id='gto_rate_vf' class='btns_right' onpointerdown=\"s('goto','vf')\" type='button'>" L_VFAST "</button>"
  "<button id='gto_rate_f' class='btns_mid' onpointerdown=\"s('goto','f')\"  type='button'>" L_FAST "</button>"
  "<button id='gto_rate_n' class='btns_mid' onpointerdown=\"s('goto','n')\"  type='button'>" L_NORMAL "</button>"
  "<button id='gto_rate_s' class='btns_mid' onpointerdown=\"s('goto','s')\"  type='button'>" L_SLOW "</button>"
  "<button id='gto_rate_vs' class='btns_left' onpointerdown=\"s('goto','vs')\" type='button'>" L_VSLOW "</button>"
  "<br /><br />\n";

const char html_gotoBuzzer[] PROGMEM = 
  L_BUZZER ": <br />"
  "<button id='gto_bzr_on' class='btns_right' onpointerdown=\"s('goto','bzr_on')\"  type='button'>" L_ON "</button>"
  "<button id='gto_bzr_off' class='btns_left' onpointerdown=\"s('goto','bzr_off')\" type='button'>" L_OFF "</button>"
  "<br /><br />\n";

const char html_gotoMfAuto[] PROGMEM =
  L_MERIDIAN_FLIP_AUTO ":<br />"
  "<button id='gto_mfa_on' class='btns_right' onpointerdown=\"s('goto','af_on')\"  type='button'>" L_ON "</button>"
  "<button id='gto_mfa_off' class='btns_left' onpointerdown=\"s('goto','af_off')\" type='button'>" L_OFF "</button>&nbsp;&nbsp;"
  "<button onpointerdown=\"s('goto','af_now')\" type='button'>" L_MERIDIAN_FLIP_NOW "</button>"
  "<br /><br />\n";

const char html_gotoMfNow[] PROGMEM =
  L_ORIENTATION_AUTO ":<br />"
  "<button onpointerdown=\"s('goto','af_now')\" type='button'>" L_MERIDIAN_FLIP_NOW "</button>"
  "<br /><br />\n";

const char html_gotoMfPause[] PROGMEM =
  "%s: <br />"
  "<button id='gto_mfp_on' class='btns_right' onpointerdown=\"s('goto','mp_on')\"  type='button'>" L_ON "</button>"
  "<button id='gto_mfp_off' class='btns_left' onpointerdown=\"s('goto','mp_off')\" type='button'>" L_OFF "</button>"
  "<br />\n";

const char html_gotoPreferredPierSide[] PROGMEM =
  "%s: <br />"
  "<button id='gto_pps_east' class='btns_right' onpointerdown=\"s('goto','pps_e')\"  type='button'>%s</button>"
  "<button id='gto_pps_west' class='btns_mid' onpointerdown=\"s('goto','pps_w')\" type='button'>%s</button>"
  "<button id='gto_pps_best' class='btns_left' onpointerdown=\"s('goto','pps_b')\" type='button'>" L_BEST "</button>"
  "<br />\n";
