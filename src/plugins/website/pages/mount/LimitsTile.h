// -----------------------------------------------------------------------------------
// Limits and Backlash tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

extern void limitsTile(String &data);
extern void limitsTileAjax(String &data);
extern void limitsTileGet();

// Misc
const char html_configFormBegin[] PROGMEM = "<div class='content'><br />\n<form method='get' action='/configuration.htm'>";
const char html_configFormEnd[] PROGMEM = "\n</form><br />\n</div><br />\n";

// Overhead and Horizon limits
const char html_configMinAlt[] PROGMEM =
  "<input value='%d' type='number' name='hl' min='-30' max='30'>&nbsp;" L_LIMITS_RANGE_HORIZON "<br />\n";

const char html_configMaxAlt[] PROGMEM =
  "<input value='%d' type='number' name='ol' min='60' max='90'>&nbsp;" L_LIMITS_RANGE_OVERHEAD "<br />\n";

const char html_configBlAxis1[] PROGMEM =
  "<input value='%d' type='number' name='b1' min='0' max='3600'>&nbsp;" L_BACKLASH_RANGE_AXIS1 "<br />\n";

const char html_configBlAxis2[] PROGMEM =
  "<input value='%d' type='number' name='b2' min='0' max='3600'>&nbsp;" L_BACKLASH_RANGE_AXIS2 "<br />\n";

const char html_configPastMerE[] PROGMEM =
  "<input value='%d' type='number' name='el' min='-270' max='270'>&nbsp;" L_LIMITS_RANGE_MERIDIAN_E "<br />\n";

const char html_configPastMerW[] PROGMEM =
  "<input value='%d' type='number' name='wl' min='-270' max='270'>&nbsp;" L_LIMITS_RANGE_MERIDIAN_W "<br /><br />\n";

