#pragma once

#include "Pages.common.h"

const char html_bad_comms_message[] PROGMEM =
"<br /><bigger><font class=\"y\">" L_DOWN_TITLE "</font></bigger><br /><br />"
L_DOWN_MESSAGE1 L_DOWN_MESSAGE2
" <li>" L_DOWN_MESSAGE3 "</li><br />"
" <li>" L_DOWN_MESSAGE4 "</li><br />"
" <li>" L_DOWN_MESSAGE5 "</li><br />"
" <li>" L_DOWN_MESSAGE6 "</li><br />"
" <li>" L_DOWN_MESSAGE7 "</li><br />"
#if DISPLAY_RESET_CONTROLS != OFF
  "<br /><form method='get' action='/configuration.htm'>"
  "<button name='advanced' type='submit' value='reset' onclick=\"return confirm('" L_ARE_YOU_SURE "?');\" >" L_RESET "</button></form>\n"
#endif
"</ul></div><br class=\"clear\" />\n";
