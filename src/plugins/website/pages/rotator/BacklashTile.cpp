// -----------------------------------------------------------------------------------
// Rotator Backlash tile
#include "BacklashTile.h"

#include "../KeyValue.h"
#include "../Pages.common.h"

// create the related webpage tile
void rotatorBacklashTile(String &data)
{
  char temp[240] = "";
  char temp1[32] = "";

  if (!onStep.command(":rb#", temp1)) strcpy(temp1, "0");
  int backlash = atoi(temp1);

  sprintf_P(temp, html_tile_text_beg, "22em", "13em", "Backlash");
  data.concat(temp);
  data.concat("<br /><hr>");

  sprintf_P(temp, html_backlashValue, backlash);
  data.concat(temp);

  data.concat("<hr>");

  sprintf_P(temp, html_collapsable_beg, L_SETTINGS "...");
  data.concat(temp);

  sprintf_P(temp, html_form_begin, "rotator.htm");
  data.concat(temp);

  sprintf_P(temp, html_configBlAxis3, backlash);
  data.concat(temp);
  data.concat(F("<button type='submit'>" L_UPLOAD "</button>\n"));

  data.concat(FPSTR(html_form_end));
  www.sendContentAndClear(data);


  data.concat(FPSTR(html_collapsable_end));
  data.concat(FPSTR(html_tile_end));
  www.sendContentAndClear(data);
}

// use Ajax key/value pairs to pass related data to the web client in the background
void rotatorBacklashTileAjax(String &data)
{
  UNUSED(data);
}

// pass related data back to OnStep
extern void rotatorBacklashTileGet()
{
  String v;
  char temp[32] = "";

  v = www.arg("b3");
  if (!v.equals(EmptyStr)) {
    if (v.toInt() >= 0 && v.toInt() <= 32767) { 
      sprintf(temp, ":rb%d#", (int16_t)v.toInt());
      onStep.commandBool(temp);
    }
  }

}
