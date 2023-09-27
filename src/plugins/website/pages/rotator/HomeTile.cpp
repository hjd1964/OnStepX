// -----------------------------------------------------------------------------------
// Rotator Home tile
#include "HomeTile.h"

#include "../KeyValue.h"
#include "../Pages.common.h"

// create the related webpage tile
void rotatorHomeTile(String &data)
{
  char temp[240] = "";

  sprintf_P(temp, html_tile_beg, "22em", "13em", L_HOMING);
  data.concat(temp);
  data.concat(F("<br /><hr>"));

  data.concat(FPSTR(html_rotatorHome));

  data.concat(F("<hr>"));

  data.concat(FPSTR(html_tile_end));
  www.sendContentAndClear(data);
}

// use Ajax key/value pairs to pass related data to the web client in the background
void rotatorHomeTileAjax(String &data)
{
  UNUSED(data);
}

// pass related data back to OnStep
extern void rotatorHomeTileGet()
{
  String v;
  v = www.arg("rot");
  if (!v.equals(EmptyStr))
  {
    if (v.equals("reset")) onStep.commandBlind(":rF#"); // reset rotator at home
    if (v.equals("home"))  onStep.commandBlind(":rC#"); // move rotator to home
  }
}
