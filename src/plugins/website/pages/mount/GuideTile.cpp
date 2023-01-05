// -----------------------------------------------------------------------------------
// Guide tile
#include "GuideTile.h"

#include "../KeyValue.h"
#include "../Pages.common.h"

// create the related webpage tile
void guideTile(String &data)
{
  char temp[240] = "";
  char temp1[120] = "";

  sprintf(temp1, L_GUIDE " <span id='guide_rate' class='c'>%s</span>", GuideRatesStr[status.guideRate]);
  sprintf_P(temp, html_tile_beg, "22em", "15em", temp1);
  data.concat(temp);
  data.concat(F("<div style='float: right; text-align: right;' id='guide_sta' class='c'>"));
  if (status.guiding) data.concat(L_SLEWING); else data.concat(L_INACTIVE);
  data.concat(F("</div><br />"));

  data.concat(FPSTR(html_guidePad));
  www.sendContentAndClear(data);

  sprintf_P(temp, html_collapsable_beg, L_CONTROLS "...");
  data.concat(temp);

  data.concat(FPSTR("Guide rate, in x sidereal:<br />"));

  sprintf_P(temp, html_form_begin, "mount.htm");
  data.concat(temp);

  data.concat(FPSTR(html_guidePulseRates));
  www.sendContentAndClear(data);

  data.concat(FPSTR(html_guideRates));
  www.sendContentAndClear(data);

  data.concat(FPSTR(html_form_end));

  data.concat(FPSTR(html_collapsable_end));
  data.concat(FPSTR(html_tile_end));
  www.sendContentAndClear(data);
}

// use Ajax key/value pairs to pass related data to the web client in the background
void guideTileAjax(String &data)
{
  data.concat(keyValueString("guide_sta", status.guiding ? L_SLEWING : L_INACTIVE));

  data.concat(keyValueString("guide_rate", GuideRatesStr[status.guideRate]));

  data.concat(keyValueBoolSelected("guide_r0", status.guideRatePulse == 0 || status.guideRate == 0));
  data.concat(keyValueBoolSelected("guide_r1", status.guideRatePulse == 1 || status.guideRate == 1));
  data.concat(keyValueBoolSelected("guide_r2", status.guideRatePulse == 2 || status.guideRate == 2));
  data.concat(keyValueBoolSelected("guide_r3", status.guideRate == 3));
  data.concat(keyValueBoolSelected("guide_r4", status.guideRate == 4));
  data.concat(keyValueBoolSelected("guide_r5", status.guideRate == 5));
  data.concat(keyValueBoolSelected("guide_r6", status.guideRate == 6));
  data.concat(keyValueBoolSelected("guide_r7", status.guideRate == 7));
  data.concat(keyValueBoolSelected("guide_r8", status.guideRate == 8));
  data.concat(keyValueBoolSelected("guide_r9", status.guideRate == 9));

  www.sendContentAndClear(data);
}

// pass related data back to OnStep
extern void guideTileGet()
{
  String v;
  v = www.arg("dr");
  if (!v.equals(EmptyStr))
  {
    // GUIDE control direction
    if (v.equals("n1")) onStep.commandBlind(":Mn#"); // move n... s,e,w
    if (v.equals("s1")) onStep.commandBlind(":Ms#");
    if (v.equals("e1")) onStep.commandBlind(":Me#");
    if (v.equals("w1")) onStep.commandBlind(":Mw#");

    if (v.equals("n0")) onStep.commandBlind(":Qn#"); // quit n... s,e,w
    if (v.equals("s0")) onStep.commandBlind(":Qs#");
    if (v.equals("e0")) onStep.commandBlind(":Qe#");
    if (v.equals("w0")) onStep.commandBlind(":Qw#");

    if (v.equals("sy")) onStep.commandBool(":CS#"); // sync

    // GUIDE control rate
    if (v.equals("R0")) onStep.commandBlind(":R0#"); // guide rate 0..9
    if (v.equals("R1")) onStep.commandBlind(":R1#");
    if (v.equals("R2")) onStep.commandBlind(":R2#");
    if (v.equals("R3")) onStep.commandBlind(":R3#");
    if (v.equals("R4")) onStep.commandBlind(":R4#");
    if (v.equals("R5")) onStep.commandBlind(":R5#");
    if (v.equals("R6")) onStep.commandBlind(":R6#");
    if (v.equals("R7")) onStep.commandBlind(":R7#");
    if (v.equals("R8")) onStep.commandBlind(":R8#");
    if (v.equals("R9")) onStep.commandBlind(":R9#");
  }
}
