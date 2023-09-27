// -----------------------------------------------------------------------------------
// PEC tile
#include "PecTile.h"

#include "../KeyValue.h"
#include "../Pages.common.h"

// create the related webpage tile
void pecTile(String &data)
{
  char temp[240] = "";

  sprintf_P(temp, html_tile_beg, "22em", "15em", L_PAGE_PEC);
  data.concat(temp);
  data.concat(F("<br /><hr>"));

  if (status.pecEnabled) {
    data.concat(FPSTR(html_pecStatus));
  }

  data.concat(F("<br /><hr>"));
  sprintf_P(temp, html_collapsable_beg, L_CONTROLS "...");
  data.concat(temp);

  if (status.pecEnabled) {
    data.concat(FPSTR(html_pecControls1));
    data.concat(FPSTR(html_pecControls2));
    data.concat(FPSTR(html_pecControls3));
    data.concat(FPSTR(html_pecControls4));
  } else {
    data.concat(L_DISABLED_MESSAGE);
  }
  www.sendContentAndClear(data);

  data.concat(FPSTR(html_collapsable_end));
  data.concat(FPSTR(html_tile_end));
  www.sendContentAndClear(data);

}

// use Ajax key/value pairs to pass related data to the web client in the background
void pecTileAjax(String &data)
{
  char temp[80] = "";

  data.concat(F("pec_sta|"));
  if (status.mountType != MT_ALTAZM && onStep.command(":$QZ?#", temp)) {
    if (temp[0] == 'I') data.concat(L_PEC_IDLE); else
    if (temp[0] == 'p') data.concat(L_PEC_WAIT_PLAY); else
    if (temp[0] == 'P') data.concat(L_PEC_PLAYING); else
    if (temp[0] == 'r') data.concat(L_PEC_WAIT_REC); else
    if (temp[0] == 'R') data.concat(L_PEC_RECORDING); else data.concat(L_PEC_UNK);
    if (status.pecRecording) data.concat(F(" (" L_PEC_EEWRITING ")"));
  } else { data.concat("?"); }
  data.concat("\n");

  www.sendContentAndClear(data);
}

// pass related data back to OnStep
extern void pecTileGet()
{
  String v;

  v = www.arg("pec");
  if (!v.equals(EmptyStr)) {
    if (v.equals("play")) onStep.commandBlind(":$QZ+#"); // play
    if (v.equals("stop")) onStep.commandBlind(":$QZ-#"); // stop
    if (v.equals("clr")) onStep.commandBlind(":$QZZ#");  // clear
    if (v.equals("rec")) onStep.commandBlind(":$QZ/#");  // record
    if (v.equals("wrt")) onStep.commandBlind(":$QZ!#");  // write to eeprom
  }
}
