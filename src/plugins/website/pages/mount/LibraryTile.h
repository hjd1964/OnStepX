// -----------------------------------------------------------------------------------
// Library tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

extern void libraryTile(String &data);
extern void libraryTileAjax(String &data);
extern void libraryTileGet();

const char html_libCatalogSelect[] PROGMEM =
L_CATALOG " <select name='selBox' onchange=\"busy(); s('lib_index',this.value);\">"
"<option value='0' selected>-</option>"
"<option value='1' >1</option>"
"<option value='2' >2</option>"
"<option value='3' >3</option>"
"<option value='4' >4</option>"
"<option value='5' >5</option>"
"<option value='6' >6</option>"
"<option value='7' >7</option>"
"<option value='8' >8</option>"
"<option value='9' >9</option>"
"<option value='10' >10</option>"
"<option value='11' >11</option>"
"<option value='12' >12</option>"
"<option value='13' >13</option>"
"<option value='14' >14</option>"
"<option value='15' >15</option>"
"</select>&nbsp;\n";

const char html_libShowMessage[] PROGMEM =
"<div id='lib_message' style='margin: 0 auto; width: 14em; margin-top: 0.5em; margin-bottom: 1em; background-color: #222222; color: #aaaaaa; border: 1px solid #551111; padding: 2px;'>" L_CAT_NO_CAT "</div>\n";

const char html_catPrevRec[] PROGMEM =
"&nbsp;<button id='cat_prev' type='button' onclick=\"s('lib','cat_prev');\" disabled>" "Prev" "</button>\n";

const char html_catNextRec[] PROGMEM =
"&nbsp;<button id='cat_next' type='button' onclick=\"s('lib','cat_next');\" disabled>" "Next" "</button>\n";

const char html_catSetTarget[] PROGMEM =
"&nbsp;<button id='cat_select' type='button' onclick=\"s('lib','cat_select');\" disabled>" "Set Target" "</button>\n";

const char html_catShowMessage[] PROGMEM =
"<div id='cat_message' style='margin: 0 auto; width: 14em; margin-top: 0.5em; margin-bottom: 1em; background-color: #222222; color: #aaaaaa; border: 1px solid #551111; padding: 2px;'>" L_CAT_NO_OBJECT "</div>\n";

const char html_libUploadCatalog[] PROGMEM =
"&nbsp;&nbsp;<button id='cat_upload' type='button' onclick=\"busy(); t('cat_upload',document.getElementById('cat_data').value);\" disabled>" L_UPLOAD "</button>\n";

const char html_libDownloadCatalog[] PROGMEM =
"&nbsp;<button id='cat_download' type='button' onclick=\"busy(); s('lib','cat_download');\" disabled>" L_DOWNLOAD "</button>\n";

const char html_libClearCatalog[] PROGMEM =
"&nbsp;<button id='lib_clear' type='button' onclick=\"busy(); if (confirm('" L_ARE_YOU_SURE "?')) s('lib','clear')\">" L_CAT_CLEAR_LIB "</button>\n";

const char html_libEditCatalog[] PROGMEM =
L_CAT_DATA "<br />"
"<textarea id='cat_data' style='background-color: #222222; color: #ffffff; border: 1px solid #551111;' "
"rows='10' cols='40' oninput=\"document.getElementById('cat_upload').disabled=false;\"></textarea><br /><br />\n";

const char html_libCatalogHelp[] PROGMEM =
"<br /><a href='libraryHelp.htm'>Library Help</a><br />";

// Javascript for library status
const char html_script_ajax_library[] PROGMEM =
"<script>\n"
"function t(key,v) { if (v.length==0) v='DELETE'; s(key,v.replace(/ /g, '_')); }\n"
"function busy() {\n"
  "document.getElementById('lib_message').innerHTML='Working...';"
  "document.getElementById('cat_upload').disabled=true;"
  "document.getElementById('cat_download').disabled=true;"
"}\n"
"</script>\n";
