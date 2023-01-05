// -----------------------------------------------------------------------------------
// Ambient tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

#if DISPLAY_WEATHER == ON

extern void ambientTile(String &data);
extern void ambientTileAjax(String &data);
extern void ambientTileGet();

const char html_indexTPHD[] PROGMEM = "%s <span id='tphd_%c' class='c'>%s</span><br />";

#endif
