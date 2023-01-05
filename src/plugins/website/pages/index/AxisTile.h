// -----------------------------------------------------------------------------------
// Ambient tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

extern bool axisTile(int axis, String &data);
extern void axisTileAjax(int axis, String &data);
extern void axisTileGet();

const char html_indexDriverStatus[] PROGMEM = L_DRIVER " " L_STATUS ": <span id='dvr_stat%d' class='c'>%s</span><br />";

// Axis3
const char html_configBlAxis3[] PROGMEM =
"<input value='%d' type='number' name='b3' min='0' max='32767'>&nbsp;" L_BACKLASH_RANGE_AXIS3UP "<br /><br />";

// Axis4 to 9
const char html_configFocuser[] PROGMEM =
"<button type='button' class='collapsible'>Axis%d " L_FOCUSER " %d</button>";
const char html_configBacklash[] PROGMEM =
"<input value='%d' type='number' name='b%d' min='0' max='32767'>&nbsp;" L_BACKLASH_RANGE_AXIS3UP "<br /><br />";
const char html_configTcfEnable[] PROGMEM =
"<input value='%d' type='number' name='en%d' min='0' max='1' step='1'>&nbsp;" L_TCF_COEF_EN_AXIS4UP "<br />";
const char html_configDeadband[] PROGMEM =
"<input value='%d' type='number' name='d%d' min='1' max='32767'>&nbsp;" L_DEADBAND_RANGE_AXIS4UP "<br />";
const char html_configTcfCoef[] PROGMEM =
"<input value='%s' type='number' name='tc%d' min='-999.0' max='999.0' step='any'>&nbsp;" L_TCF_COEF_RANGE_AXIS4UP "<br /><br />";

const char html_configAdvanced[] PROGMEM =
"<hr>" L_ADV_SET_TITLE " (<i>" L_ADV_SET_HEADER_MSG "</i>)<br /><br />";

const char html_configMountType[] PROGMEM =
" <input style='width: 7em;' value='%d' type='number' name='mountt' min='1' max='3' step='1'>&nbsp; " L_ADV_MOUNT_TYPE "<br /><br />\n";

#if DRIVE_CONFIGURATION == ON
  const char html_configAxisSpwr[] PROGMEM =
  " <input style='width: 7em;' value='%ld' type='number' name='a%dspwr' min='%d' max='%ld' step='1'>&nbsp; " L_ADV_SET_SPWR "<br />\n";
  const char html_configAxisSpd[] PROGMEM =
  " <input style='width: 7em;' value='%s' type='number' name='a%dspm' min='%d' max='%ld' step='any'>&nbsp; " L_ADV_SET_SPD "<br />\n";
  const char html_configAxisSpu[] PROGMEM =
  " <input style='width: 7em;' value='%s' type='number' name='a%dspm' min='0.01' max='10' step='any'>&nbsp; " L_ADV_SET_SPM "<br />\n";
  const char html_configAxisMicroSteps[] PROGMEM =
  " <input style='width: 7em;' value='%d' type='number' name='a%dustp' min='1' max='256' step='1'>&nbsp; " L_ADV_SET_TRAK_us "<br />\n";
  const char html_configAxisMicroStepsGoto[] PROGMEM =
  " <input style='width: 7em;' value='%d' type='number' name='a%dustpGoto' min='-1' max='256' step='1'>&nbsp; " L_ADV_SET_GOTO_us "<br />\n";
  const char html_configAxisCurrentHold[] PROGMEM =
  " <input style='width: 7em;' value='%d' type='number' name='a%dIh' min='-1' max='%d' step='1'>&nbsp; " L_ADV_SET_HOLD_ma "<br />\n";
  const char html_configAxisCurrentTrak[] PROGMEM =
  " <input style='width: 7em;' value='%d' type='number' name='a%dI' min='-1' max='%d' step='1'>&nbsp; " L_ADV_SET_TRAK_ma "<br />\n";
  const char html_configAxisCurrentSlew[] PROGMEM =
  " <input style='width: 7em;' value='%d' type='number' name='a%dIs' min='-1' max='%d' step='1'>&nbsp; " L_ADV_SET_SLEW_ma "<br />\n";
  const char html_configAxisP[] PROGMEM =
  " <input style='width: 7em;' value='%s' type='number' name='a%dp' min='%d' max='%ld' step='any'>&nbsp; " L_ADV_SET_P "<br />\n";
  const char html_configAxisI[] PROGMEM =
  " <input style='width: 7em;' value='%s' type='number' name='a%di' min='%d' max='%ld' step='any'>&nbsp; " L_ADV_SET_I "<br />\n";
  const char html_configAxisD[] PROGMEM =
  " <input style='width: 7em;' value='%s' type='number' name='a%dd' min='%d' max='%ld' step='any'>&nbsp; " L_ADV_SET_D "<br />\n";
  const char html_configAxisGotoP[] PROGMEM =
  " <input style='width: 7em;' value='%s' type='number' name='a%dpGoto' min='%d' max='%ld' step='any'>&nbsp; " L_ADV_SET_P_GOTO "<br />\n";
  const char html_configAxisGotoI[] PROGMEM =
  " <input style='width: 7em;' value='%s' type='number' name='a%diGoto' min='%d' max='%ld' step='any'>&nbsp; " L_ADV_SET_I_GOTO "<br />\n";
  const char html_configAxisGotoD[] PROGMEM =
  " <input style='width: 7em;' value='%s' type='number' name='a%ddGoto' min='%d' max='%ld' step='any'>&nbsp; " L_ADV_SET_D_GOTO "<br />\n";
  const char html_configAxisReverse[] PROGMEM =
  " <input style='width: 7em;' value='%d' type='number' name='a%drev' min='0' max='1' step='1'>&nbsp; " L_ADV_SET_REV " " L_ADV_BOOL "<br />\n";
  const char html_configAxisMin[] PROGMEM =
  " <input style='width: 7em;' value='%d' type='number' name='a%dmin' min='%d' max='%d' step='1'>&nbsp;%s&nbsp; " L_ADV_SET_MIN "<br />\n";
  const char html_configAxisMax[] PROGMEM =
  " <input style='width: 7em;' value='%d' type='number' name='a%dmax' min='%d' max='%d' step='1'>&nbsp;%s&nbsp; " L_ADV_SET_MAX "<br /><br />\n";
  const char html_configAxisRevert[] PROGMEM =
  "<button name='revert' value='%d' type='submit'>" L_REVERT "</button>";
#endif
