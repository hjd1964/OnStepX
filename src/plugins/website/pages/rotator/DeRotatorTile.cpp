// -----------------------------------------------------------------------------------
// DeRotator tile
#include "DeRotatorTile.h"

#include "../KeyValue.h"
#include "../Pages.common.h"

// create the related webpage tile
void deRotatorTile(String &data)
{
  char temp[240] = "";

  sprintf_P(temp, html_tile_beg, "22em", "13em", L_DEROTATOR);
  data.concat(temp);
  data.concat("<br /><hr>");

  data.concat(FPSTR(html_controlDeRotate));
  www.sendContentAndClear(data);

  data.concat("<hr>");

  data.concat(FPSTR(html_tile_end));
  www.sendContentAndClear(data);
}

// use Ajax key/value pairs to pass related data to the web client in the background
void deRotatorTileAjax(String &data)
{
  if (status.mountType == MT_ALTAZM) {
    data.concat(keyValueToggleBoolSelected("rot_on", "rot_off", state.rotatorDerotate));
    data.concat(keyValueBoolSelected("rot_rev", state.rotatorDerotateReverse));
    www.sendContentAndClear(data);
  }
}

// pass related data back to OnStep
extern void deRotatorTileGet()
{
  String v;
  v = www.arg("rot");
  if (!v.equals(EmptyStr))
  {
    if (v.equals("off")) onStep.commandBlind(":r-#"); // disable derotator
    if (v.equals("on"))  onStep.commandBlind(":r+#"); // enable derotator
    if (v.equals("rev")) onStep.commandBlind(":rR#"); // reverse rotator
    if (v.equals("par")) onStep.commandBlind(":rP#"); // move rotator to parallactic angle
  }

}
