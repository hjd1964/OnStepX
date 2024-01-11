#pragma once

#include "Pages.common.h"

#ifndef COLOR_BODY_BACKGROUND
#define COLOR_BODY_BACKGROUND "#26262A"
#endif

#ifndef COLOR_FOREGROUND
#define COLOR_FOREGROUND "#999999"
#endif
#ifndef COLOR_LIGHT_FOREGROUND
#define COLOR_LIGHT_FOREGROUND "#AAAAAA"
#endif
#ifndef COLOR_ALERT
#define COLOR_ALERT "#FFFF00"
#endif
#ifndef COLOR_BACKGROUND
#define COLOR_BACKGROUND "#111111"
#endif
#ifndef COLOR_LIGHT_BACKGROUND
#define COLOR_LIGHT_BACKGROUND "#222222"
#endif
#ifndef COLOR_BORDER
#define COLOR_BORDER "#551111"
#endif

#ifndef COLOR_LINK
#define COLOR_LINK "#A07070"
#endif
#ifndef COLOR_LINK_BACKGROUND
#define COLOR_LINK_BACKGROUND "#332222"
#endif
#ifndef COLOR_LINK_SELECTED_BACKGROUND
#define COLOR_LINK_SELECTED_BACKGROUND "#552222"
#endif
#ifndef COLOR_LINK_BORDER
#define COLOR_LINK_BORDER "red"
#endif

#ifndef COLOR_CONTROL
#define COLOR_CONTROL "#B02020"
#endif
#ifndef COLOR_CONTROL_BACKGROUND
#define COLOR_CONTROL_BACKGROUND "#181818"
#endif
#ifndef COLOR_CONTROL_DISABLED
#define COLOR_CONTROL_DISABLED "#000"
#endif
#ifndef COLOR_CONTROL_DISABLED_BACKGROUND
#define COLOR_CONTROL_DISABLED_BACKGROUND "#733"
#endif

#ifndef COLOR_COLLAPSIBLE
#define COLOR_COLLAPSIBLE "#999"
#endif
#ifndef COLOR_COLLAPSIBLE_BACKGROUND
#define COLOR_COLLAPSIBLE_BACKGROUND "#500808"
#endif
#ifndef COLOR_COLLAPSIBLE_ACTIVE_BACKGROUND
#define COLOR_COLLAPSIBLE_ACTIVE_BACKGROUND "#661111"
#endif
#ifndef COLOR_COLLAPSIBLE_CONTENT_BACKGROUND
#define COLOR_COLLAPSIBLE_CONTENT_BACKGROUND "#301212"
#endif

#ifndef COLOR_SERVO_BACKGROUND_1
#define COLOR_SERVO_BACKGROUND_1 "#222222"
#endif
#ifndef COLOR_SERVO_BACKGROUND_2
#define COLOR_SERVO_BACKGROUND_2 "#552222"
#endif
#ifndef COLOR_SERVO_BACKGROUND_3
#define COLOR_SERVO_BACKGROUND_3 "#AA2222"
#endif
#ifndef COLOR_SERVO_PEN_1
#define COLOR_SERVO_PEN_1 "#303030"
#endif
#ifndef COLOR_SERVO_PEN_2
#define COLOR_SERVO_PEN_2 "#444444"
#endif
#ifndef COLOR_SERVO_PEN_3
#define COLOR_SERVO_PEN_3 "#666666"
#endif
#ifndef COLOR_SERVO_PEN_4
#define COLOR_SERVO_PEN_4 "#CCCCCC"
#endif

const char html_head_begin[] PROGMEM = "<!DOCTYPE HTML>\n"
                                  "<html>\n"
                                  "<head>\n"
                                  "<meta charset='utf-8'/>\n"
                                  "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">\n";
const char html_headerPec[] PROGMEM = "<meta http-equiv=\"refresh\" content=\"5; URL=/pec.htm\">\n";
const char html_headerIdx[] PROGMEM = "<meta http-equiv=\"refresh\" content=\"5; URL=/index.htm\">\n";
const char html_head_end[] PROGMEM = "</head>\n";
const char html_body_begin[] PROGMEM = "<body bgcolor='" COLOR_BODY_BACKGROUND "'>\n";
const char html_page_and_body_end[] PROGMEM = "</div></body></html>\n";

const char html_main_css_begin[] PROGMEM = 
"<STYLE>";

const char html_main_css_core[] PROGMEM =
".clear { clear: both; } .a { background-color: " COLOR_BACKGROUND "; } "
".t { padding: 10px 10px 20px 10px; border: 5px solid " COLOR_BORDER "; margin: 25px 25px 0px 25px;"
"color: " COLOR_FOREGROUND "; background-color: " COLOR_BACKGROUND "; min-width: 30em; } "
"input { text-align:center; padding: 2px; margin: 3px; font-weight: bold; width:5em; background-color: " COLOR_CONTROL "}"
".b { padding: 10px; border-left: 5px solid " COLOR_BORDER "; border-right: 5px solid " COLOR_BORDER "; border-bottom: 5px solid " COLOR_BORDER "; margin: 0px 25px 25px 25px; color: " COLOR_FOREGROUND ";"
"background-color: " COLOR_BACKGROUND "; min-width: 30em; } select { width:4em; font-weight: bold; background-color: " COLOR_CONTROL "; padding: 2px 2px; }"
".c { color: " COLOR_CONTROL "; font-weight: bold; }"
"h1 { text-align: right; } a:hover, a:active { background-color: " COLOR_LINK_BORDER "; } .y { color: " COLOR_ALERT "; font-weight: bold; }"
"a:link, a:visited { background-color: " COLOR_LINK_BACKGROUND "; color: " COLOR_LINK "; border:1px solid " COLOR_LINK_BORDER "; padding: 5px 10px; margin: none; text-align: center; text-decoration: none; display: inline-block; }"
"button:disabled { background-color: " COLOR_CONTROL_DISABLED_BACKGROUND "; color: " COLOR_CONTROL_DISABLED "; } button { background-color: " COLOR_CONTROL "; font-weight: bold; border-radius: 5px; margin: 2px; padding: 4px 8px; }";

const char html_main_css_no_select[] PROGMEM =
"body { -webkit-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; }";

const char html_main_css_buttons[] PROGMEM = 
".btns_left { margin-left: -1px; border-top-left-radius: 0px; border-bottom-left-radius: 0px; }"
".btns_mid { margin-left: -1px; margin-right: -1px; border-top-left-radius: 0px; border-bottom-left-radius: 0px; border-top-right-radius: 0px; border-bottom-right-radius: 0px; }"
".btns_right { margin-right: -1px; border-top-right-radius: 0px; border-bottom-right-radius: 0px; }";

const char html_main_css_control[] PROGMEM = 
".b1 { float: left; border: 2px solid " COLOR_BORDER "; background-color: " COLOR_CONTROL_BACKGROUND "; text-align: center; margin: 5px; padding: 15px; padding-top: 3px; }"
".gb {  font-weight: bold; font-size: 150%; font-family: 'Times New Roman', Times, serif; width: 60px; height: 50px; padding: 0px; }"
".bb {  font-weight: bold; font-size: 105%; } .bbh {  font-weight: bold; font-size: 100%; height: 2.1em; }";

const char html_main_css_collapse[] PROGMEM =
".collapsible { background-color: " COLOR_COLLAPSIBLE_BACKGROUND "; color: " COLOR_COLLAPSIBLE "; cursor: pointer; padding: 7px; width: 80%; border: none; text-align: left; outline: none; font-size: 14px; }"
".active, .collapsible:hover { background-color: " COLOR_COLLAPSIBLE_ACTIVE_BACKGROUND "; } .content { padding: 2px 18px; display: none; overflow: hidden; background-color: " COLOR_COLLAPSIBLE_CONTENT_BACKGROUND "; }";

const char html_main_css_end[] PROGMEM = 
"</STYLE>";

// tile: begin takes title and width
const char html_tile_beg[] PROGMEM = "<div class='b1' style='width: %s; min-height: %s;'><div style='float: left;'>%s:</div>\n";
// tile: begin for text takes title and width
const char html_tile_text_beg[] PROGMEM = "<div class='b1' style='text-align: left; width: %s; min-height: %s;'><div style='float: left;'>%s:</div>\n";
// tile: adds status to first line
const char html_tile_status[] PROGMEM = "<div style='float: right; text-align: right;' id='%s' class='c'>%s</div><br />\n";
// tile: end
const char html_tile_end[] PROGMEM = "</div>\n";

// collapsable: begin takes title
const char html_collapsable_beg[] PROGMEM = 
"<div style='text-align: left;'> <button type='button' class='collapsible' style='vertical-align: bottom'>%s</button>"
"<div class='content' style='text-align: left;'><br />\n";
// collapsable: end
const char html_collapsable_end[] PROGMEM = "<br /></div></div>\n";

// form: begin takes webpage for get
const char html_form_begin[] PROGMEM = "<form method='get' action='/%s'>\n";
// form: end
const char html_form_end[] PROGMEM = "</form>\n";

// input: medium width numeric takes name (for get,) value, min, max, unit
const char html_ninput_wide[] PROGMEM =
"<input style='width: 4em;' type='number' name='%s' value='%s' min='%s' max='%s'>&nbsp;%s";
// input: narrow width numeric takes name (for get,) value, min, max, unit
const char html_ninput[] PROGMEM =
"<input style='width: 3.5em;' type='number' name='%s' value='%s' min='%s' max='%s'>&nbsp;%s";
