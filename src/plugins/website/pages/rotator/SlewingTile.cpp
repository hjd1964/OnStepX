// -----------------------------------------------------------------------------------
// Rotator Slewing tile
#include "SlewingTile.h"

#include "../KeyValue.h"
#include "../Pages.common.h"

// create the related webpage tile
void rotatorSlewingTile(String &data)
{
  char temp[240] = "";

  sprintf_P(temp, html_tile_beg, "22em", "13em", L_SLEWING);
  data.concat(temp);
  data.concat(F("<div style='float: right; text-align: right;' id='rot_sta' class='c'>"));
  if (state.focuserSlewing) data.concat(L_ACTIVE); else data.concat(L_INACTIVE);
  data.concat(F("</div><br /><hr>"));

  data.concat(L_CURRENT ": <span id='rotatorpos' class='c'>");
  data.concat(state.rotatorPositionStr);
  data.concat(F("</span><br /><br />"));

  data.concat(FPSTR(html_rotPosition));
  data.concat(FPSTR(html_rotStop));

  data.concat(FPSTR(html_rotateCcw));
  data.concat(FPSTR(html_rotateCw));

  data.concat(F("<hr>"));

  if (status.getVersionMajor() >= 10) {
    sprintf_P(temp, html_collapsable_beg, L_CONTROLS "...");
    data.concat(temp);

    sprintf_P(temp, html_rotateSlewSpeed, state.rotateSlewSpeedStr);
    data.concat(temp);
    data.concat(FPSTR(html_rotateGotoSelect));

    data.concat(FPSTR(html_collapsable_end));
  }

  data.concat(FPSTR(html_tile_end));
  www.sendContentAndClear(data);
}

// use Ajax key/value pairs to pass related data to the web client in the background
void rotatorSlewingTileAjax(String &data)
{
  data.concat(keyValueString("rot_sta", state.rotatorSlewing ? L_ACTIVE : L_INACTIVE));
  data.concat(keyValueString("rotatorpos", state.rotatorPositionStr));
  data.concat(keyValueString("rot_rate", state.rotateSlewSpeedStr));

  String s;
  s = keyValueBoolSelected("rot_rate_vs", state.rotatorGotoRate == 1); data.concat(s);
  s = keyValueBoolSelected("rot_rate_s", state.rotatorGotoRate == 2); data.concat(s);
  s = keyValueBoolSelected("rot_rate_n", state.rotatorGotoRate == 3); data.concat(s);
  s = keyValueBoolSelected("rot_rate_f", state.rotatorGotoRate == 4); data.concat(s);
  s = keyValueBoolSelected("rot_rate_vf", state.rotatorGotoRate == 5); data.concat(s);

  www.sendContentAndClear(data);
}

// pass related data back to OnStep
extern void rotatorSlewingTileGet()
{
  String v;

  // stop movement
  v = www.arg("rot");
  if (!v.equals(EmptyStr))
  {
    if (v.equals("stop")) onStep.commandBlind(":rQ#");         // stop
    if (v.equals("ccwf")) onStep.commandBlind(":r3#:rc#:r<#"); // rate 3, move ccw
    if (v.equals("ccw"))  onStep.commandBlind(":r1#:rc#:r<#"); // rate 1, move ccw
    if (v.equals("cw"))   onStep.commandBlind(":r1#:rc#:r>#"); // rate 1, move cw
    if (v.equals("cwf"))  onStep.commandBlind(":r3#:rc#:r>#"); // rate 3, move cw

    if (v.equals("vs"))   onStep.commandBlind(":r5#");         // goto rate very slow
    if (v.equals("s"))    onStep.commandBlind(":r6#");         // goto rate slow
    if (v.equals("n"))    onStep.commandBlind(":r7#");         // goto rate normal
    if (v.equals("f"))    onStep.commandBlind(":r8#");         // goto rate fast
    if (v.equals("vf"))   onStep.commandBlind(":r9#");         // goto rate very fast
  }

  // set position
  v = www.arg("rs");
  if (!v.equals(EmptyStr))
  {
    double f = v.toFloat();
    if (f >= -360.0 || f <= 360.0)
    {
      char temp[80], temp1[40];
      convert.doubleToDms(temp1, f, true, true, PM_HIGH);
      sprintf(temp, ":rS%s#", temp1);
      onStep.commandBool(temp);
    }
  }

}
