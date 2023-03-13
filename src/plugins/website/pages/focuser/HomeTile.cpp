// -----------------------------------------------------------------------------------
// Focuser Home tile
#include "HomeTile.h"

#include "../KeyValue.h"
#include "../Pages.common.h"

// create the related webpage tile
void focuserHomeTile(String &data)
{
  char temp[240] = "";

  sprintf_P(temp, html_tile_beg, "22em", "13em", L_HOMING);
  data.concat(temp);
  data.concat("<br /><hr>");

  data.concat(FPSTR(html_focuserHome));

  data.concat("<hr>");

  data.concat(FPSTR(html_tile_end));
  www.sendContentAndClear(data);
}

// use Ajax key/value pairs to pass related data to the web client in the background
void focuserHomeTileAjax(String &data)
{
  UNUSED(data);
}

// pass related data back to OnStep
extern void focuserHomeTileGet()
{
  String v;
  v = www.arg("dr");
  if (!v.equals(EmptyStr))
  {
    if (v.equals("FH")) onStep.commandBlind(":FH#"); // reset focuser at home (half travel)
    if (v.equals("Fh")) onStep.commandBlind(":Fh#"); // move focuser to home (half travel)
  }

}
