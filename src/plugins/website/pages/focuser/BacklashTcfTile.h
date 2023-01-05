// -----------------------------------------------------------------------------------
// Focuser Backlash tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

extern void focuserBacklashTcfTile(String &data);
extern void focuserBacklashTcfTileAjax(String &data);
extern void focuserBacklashTcfTileGet();

const char html_backlashValue[] PROGMEM =
L_BACKLASH ": <span id='f_bl' class='c'>%s step(s)</span><br />";

const char html_tcfDeadbandValue[] PROGMEM =
L_DEADBAND ": <span id='f_tcf_db' class='c'>%s step(s)</span><br />";

const char html_tcfEnableValue[] PROGMEM =
L_TCF_COEF_EN ": <span id='f_tcf_en' class='c'>%s</span><br />";

const char html_tcfCoefValue[] PROGMEM =
L_TCF_COEF ": <span id='f_tcf_coef' class='c'>%s</span><br />";

const char html_backlash[] PROGMEM =
"<input value='%s' type='number' name='foc_bl' min='0' max='32767'>&nbsp;" L_BACKLASH_RANGE_AXIS3UP "<br />";

const char html_tcfDeadband[] PROGMEM =
"<input value='%s' type='number' name='foc_tcf_db' min='1' max='32767'>&nbsp;" L_DEADBAND_RANGE_AXIS4UP "<br />";

const char html_tcfEnable[] PROGMEM =
"<input value='%d' type='number' name='foc_tcf_en' min='0' max='1' step='1'>&nbsp;" L_TCF_COEF_EN_AXIS4UP "<br />";

const char html_tcfCoef[] PROGMEM =
"<input value='%s' type='number' name='foc_tcf_coef' min='-999.0' max='999.0' step='any'>&nbsp;" L_TCF_COEF_RANGE_AXIS4UP "<br />";
