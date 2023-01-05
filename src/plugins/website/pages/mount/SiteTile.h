// -----------------------------------------------------------------------------------
// Date/Time/Location tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

extern void siteTile(String &data);
extern void siteTileAjax(String &data);
extern void siteTileGet();

const char html_browserTime[] PROGMEM =
  "<span id=\"datetime\"></span> " L_TIME_BROWSER_UT
  "<script>update_date_time();</script>"
  "<br />\n";

const char html_date[] PROGMEM = "<span id='date_ut' class='c'>%s</span>";
const char html_time[] PROGMEM = "&nbsp;<span id='time_ut' class='c'>%s</span>&nbsp;" L_UT;
const char html_sidereal[] PROGMEM = "&nbsp;(<span id='time_lst' class='c'>%s</span>&nbsp; " L_LST ")<br />";
const char html_site[] PROGMEM =
L_LAT " <span id='site_lat' class='c'>%s</span><br />"
L_LONG " <span id='site_long' class='c'>%s</span><br /><br />";
const char html_setDateTime[] PROGMEM =
  "<form style='display: inline' method='get' action='/mount.htm'>\n"
  "<div style='text-align: center'>"
  "<small><button name='qb' class='bb' value='st' onpointerdown='SetDateTime();' type='submit'>" CLOCK_CH " " L_HINT_CLOCK "</button></small>"
  "</div>\n"
  "<input id='dm' type='hidden' name='dm'><input id='dd' type='hidden' name='dd'><input id='dy' type='hidden' name='dy'>"
  "<input id='th' type='hidden' name='th'><input id='tm' type='hidden' name='tm'><input id='ts' type='hidden' name='ts'>\n"
  "</form>\n";

const char html_longMsg[] PROGMEM = L_LOCATION_LONG ":<br />\n";
const char html_latMsg[] PROGMEM = "<br />" L_LOCATION_LAT ":<br />\n";
const char html_offsetMsg[] PROGMEM = "<br />" L_LOCATION_RANGE_UTC_OFFSET ":<br />";
const char html_offsetMin[] PROGMEM =
"&nbsp;<select name='u2'><option value='0' %s>00</option><option value='30' %s>30</option><option value='45' %s>45</option></select>&nbsp;m\n";
const char html_offsetFooterMsg[] PROGMEM = "<br />" L_LOCATION_MESSAGE_UTC_OFFSET "<br />";
