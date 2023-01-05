// -----------------------------------------------------------------------------------
// Rotator Backlash tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

extern void rotatorBacklashTile(String &data);
extern void rotatorBacklashTileAjax(String &data);
extern void rotatorBacklashTileGet();

const char html_backlashValue[] PROGMEM =
L_BACKLASH ": <span class='c'>%d step(s)</span><br />";

const char html_configBlAxis3[] PROGMEM =
"<input value='%d' type='number' name='b3' min='0' max='32767'>&nbsp;" L_BACKLASH_RANGE_AXIS3UP "<br />";
