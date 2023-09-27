// -----------------------------------------------------------------------------------
// Focuser Select tile
#include "SelectTile.h"

#include "../KeyValue.h"
#include "../Pages.common.h"

// create the related webpage tile
void focuserSelectTile(String &data)
{
  char temp[240] = "";

  sprintf_P(temp, html_tile_beg, "22em", "13em", "Select");
  data.concat(temp);
  data.concat(F("<br /><hr>"));

  if (status.focuserCount >= 1 && state.focuserSelected == 0) state.focuserSelected = 1;

  // show the active focuser
  data.concat(F("<big><span id='foc_sel' class='c'>"));
  if (state.focuserSelected == 0)
  {
    data.concat(L_FOCUSER " " L_INACTIVE);
  } else
  {
    sprintf(temp, L_FOCUSER "%d " L_SELECTED, state.focuserSelected);
    data.concat(temp);
  }
  data.concat(F("</span></big><br /><br />"));

  if (status.focuserCount > 1)
  {
    if (status.focuserPresent[0]) data.concat(FPSTR(html_selectFocuser1));
    if (status.focuserPresent[1]) data.concat(FPSTR(html_selectFocuser2));
    if (status.focuserPresent[2]) data.concat(FPSTR(html_selectFocuser3));
    if (status.focuserPresent[3]) data.concat(FPSTR(html_selectFocuser4));
    if (status.focuserPresent[4]) data.concat(FPSTR(html_selectFocuser5));
    if (status.focuserPresent[5]) data.concat(FPSTR(html_selectFocuser6));
  }

  data.concat(F("<hr>"));

  data.concat(FPSTR(html_tile_end));
  www.sendContentAndClear(data);
}

// use Ajax key/value pairs to pass related data to the web client in the background
void focuserSelectTileAjax(String &data)
{
  char temp[80];

  if (state.focuserSelected == 0)
    data.concat(keyValueString("foc_sel", L_INACTIVE));
  else
  {
    sprintf(temp, L_FOCUSER "%d " L_SELECTED, state.focuserSelected);
    data.concat(keyValueString("foc_sel", temp));
  }
  data.concat(keyValueBoolSelected("foc1_sel", state.focuserSelected == 1));
  data.concat(keyValueBoolSelected("foc2_sel", state.focuserSelected == 2));
  data.concat(keyValueBoolSelected("foc3_sel", state.focuserSelected == 3));
  data.concat(keyValueBoolSelected("foc4_sel", state.focuserSelected == 4));
  data.concat(keyValueBoolSelected("foc5_sel", state.focuserSelected == 5));
  data.concat(keyValueBoolSelected("foc6_sel", state.focuserSelected == 6));
  www.sendContentAndClear(data);
}

// pass related data back to OnStep
extern void focuserSelectTileGet()
{
  String v;

  v = www.arg("dr");
  if (!v.equals(EmptyStr))
  {
    if (v.equals("F1")) { onStep.commandBool(":FA1#"); state.focuserSelected = 1; }
    if (v.equals("F2")) { onStep.commandBool(":FA2#"); state.focuserSelected = 2; }
    if (v.equals("F3")) { onStep.commandBool(":FA3#"); state.focuserSelected = 3; }
    if (v.equals("F4")) { onStep.commandBool(":FA4#"); state.focuserSelected = 4; }
    if (v.equals("F5")) { onStep.commandBool(":FA5#"); state.focuserSelected = 5; }
    if (v.equals("F6")) { onStep.commandBool(":FA6#"); state.focuserSelected = 6; }
  }
}
