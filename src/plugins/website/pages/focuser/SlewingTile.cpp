// -----------------------------------------------------------------------------------
// Focuser Slewing tile
#include "SlewingTile.h"

#include "../KeyValue.h"
#include "../Pages.common.h"

// create the related webpage tile
void focuserSlewingTile(String &data)
{
  char temp[240] = "";

  sprintf_P(temp, html_tile_beg, "22em", "11em", L_SLEWING);
  data.concat(temp);
  data.concat(F("<div style='float: right; text-align: right;' id='foc_sta' class='c'>"));
  if (state.focuserSlewing) data.concat(L_ACTIVE); else data.concat(L_INACTIVE);
  data.concat(F("</div><br /><hr>"));

  data.concat(L_CURRENT ": <span id='focuserpos' class='c'>");
  data.concat(state.focuserPositionStr);
  data.concat("</span><br /><br />");

  data.concat(FPSTR(html_focPosition));
  data.concat(FPSTR(html_focStop));

  data.concat(FPSTR(html_focuserIn));
  data.concat(FPSTR(html_focuserOut));

  data.concat("<hr>");

  data.concat(FPSTR(html_tile_end));
  www.sendContentAndClear(data);
}

// use Ajax key/value pairs to pass related data to the web client in the background
void focuserSlewingTileAjax(String &data)
{
  data.concat(keyValueString("foc_sta", state.focuserSlewing ? L_ACTIVE : L_INACTIVE));
  data.concat(keyValueString("focuserpos", state.focuserPositionStr));
  www.sendContentAndClear(data);
}

// pass related data back to OnStep
extern void focuserSlewingTileGet()
{
  String v;

  v = www.arg("foc");
  if (!v.equals(EmptyStr))
  {
    if (v.equals("stop")) onStep.commandBlind(":FQ#");     // stop
    if (v.equals("inf"))  onStep.commandBlind(":F4#:F-#"); // rate fast, move in
    if (v.equals("in"))   onStep.commandBlind(":F2#:F-#"); // rate slow, move in
    if (v.equals("out"))  onStep.commandBlind(":F2#:F+#"); // rate slow, move out
    if (v.equals("outf")) onStep.commandBlind(":F4#:F+#"); // rate fast, move out
  }

  // set position
  v = www.arg("fs");
  if (!v.equals(EmptyStr))
  {
    int p = v.toInt();
    if (p >= -500000 || p <= 500000)
    {
      char temp[80];
      sprintf(temp, ":FS%d#", p);
      onStep.commandBool(temp);
    }
  }

}
