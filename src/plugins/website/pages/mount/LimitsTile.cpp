// -----------------------------------------------------------------------------------
// Limits and Backlash tile
#include "LimitsTile.h"

#include "../KeyValue.h"
#include "../Pages.common.h"

// create the related webpage tile
void limitsTile(String &data)
{
  char temp[240] = "";
  char reply[120] = "";

  sprintf_P(temp, html_tile_text_beg, "22em", "15em", "Backlash and Limits");
  data.concat(temp);

  data.concat(F("<br /><hr>"));

  // get backlash and limits
  if (!onStep.command(":%BR#", reply)) strcpy(reply, "0");
  int backlashAxis1 = (int)strtol(&reply[0], NULL, 10);

  if (!onStep.command(":%BD#", reply)) strcpy(reply, "0");
  int backlashAxis2 = (int)strtol(&reply[0], NULL, 10);

   if (!onStep.command(":Gh#", reply)) strcpy(reply, "0");
  int minAlt = (int)strtol(&reply[0], NULL, 10);

  if (!onStep.command(":Go#", reply)) strcpy(reply, "0");
  int maxAlt = (int)strtol(&reply[0], NULL, 10);

  if (!onStep.command(":GXE9#", reply)) strcpy(reply, "0");
  int degPastMerE = (int)strtol(&reply[0], NULL, 10);
  degPastMerE = round((degPastMerE * 15.0) / 60.0);

  if (!onStep.command(":GXEA#", reply)) strcpy(reply, "0");
  int degPastMerW = (int)strtol(&reply[0], NULL, 10);
  degPastMerW = round((degPastMerW * 15.0) / 60.0);

  // show values
  sprintf(temp, "Backlash, Axis1 <span id='limBl1' class='c'>%d</span> arc-sec<br />", backlashAxis1);
  data.concat(temp);
  sprintf(temp, "Backlash, Axis2 <span id='limBl2' class='c'>%d</span> arc-sec<br /><br />", backlashAxis2);
  data.concat(temp);
  sprintf(temp, "Limit Horizon, min altitude <span id='limAltMin' class='c'>%d</span>&deg;<br />", minAlt);
  data.concat(temp);
  sprintf(temp, "Limit Overhead, max altitude <span id='limAltMax' class='c'>%d</span>&deg;<br />", maxAlt);
  data.concat(temp);
  sprintf(temp, "Limit past Meridian, East <span id='limMerE' class='c'>%d</span>&deg;<br />", degPastMerE);
  data.concat(temp);
  sprintf(temp, "Limit past Meridian, West <span id='limMerW' class='c'>%d</span>&deg;<br />", degPastMerW);
  data.concat(temp);

  data.concat(F("<hr>"));

  sprintf_P(temp, html_collapsable_beg, L_SETTINGS "...");
  data.concat(temp);

  sprintf_P(temp, html_form_begin, "mount.htm");
  data.concat(temp);

  // Backlash
  data.concat(F("Backlash:<br />"));

  sprintf_P(temp, html_configBlAxis1, backlashAxis1);
  data.concat(temp);
  www.sendContentAndClear(data);

  sprintf_P(temp, html_configBlAxis2, backlashAxis2);
  data.concat(temp);

  data.concat(F("<br />Limits:<br />"));

  // Overhead and Horizon Limits
  sprintf_P(temp, html_configMinAlt, minAlt);
  data.concat(temp);

  sprintf_P(temp, html_configMaxAlt, maxAlt);
  data.concat(temp);

  // Meridian Limits
  if (status.mountType == MT_GEM)
  {
    sprintf_P(temp, html_configPastMerE, degPastMerE);
    data.concat(temp);

    sprintf_P(temp, html_configPastMerW, degPastMerW);
    data.concat(temp);
  }
  else
    data.concat(F("<br />\n"));

  data.concat(F("<button type='submit'>" L_UPLOAD "</button>\n"));

  data.concat(FPSTR(html_form_end));
  www.sendContentAndClear(data);

  data.concat(FPSTR(html_collapsable_end));
  data.concat(FPSTR(html_tile_end));
  www.sendContentAndClear(data);
}

// use Ajax key/value pairs to pass related data to the web client in the background
void limitsTileAjax(String &data)
{
  UNUSED(data);
}

// pass related data back to OnStep
extern void limitsTileGet()
{
  String v;
  char temp[80];

  // Overhead limit
  v = www.arg("ol");
  if (!v.equals(EmptyStr))
  {
    if (v.toInt() >= 60 && v.toInt() <= 90)
    {
      sprintf(temp, ":So%d#", (int16_t)v.toInt());
      onStep.commandBool(temp);
    }
  }

  // Horizon limit
  v = www.arg("hl");
  if (!v.equals(EmptyStr))
  {
    if (v.toInt() >= -30 && v.toInt() <= 30)
    {
      sprintf(temp, ":Sh%d#", (int16_t)v.toInt());
      onStep.commandBool(temp);
    }
  }

  // Meridian limit E
  v = www.arg("el");
  if (!v.equals(EmptyStr))
  {
    if (v.toInt() >= -270 && v.toInt() <= 270)
    {
      sprintf(temp, ":SXE9,%d#", (int16_t)round((v.toInt() * 60.0) / 15.0));
      onStep.commandBool(temp);
    }
  }

  // Meridian limit W
  v = www.arg("wl");
  if (!v.equals(EmptyStr))
  {
    if (v.toInt() >= -270 && v.toInt() <= 270)
    {
      sprintf(temp, ":SXEA,%d#", (int16_t)round((v.toInt() * 60.0) / 15.0));
      onStep.commandBool(temp);
    }
  }

  // Backlash
  v = www.arg("b1");
  if (!v.equals(EmptyStr))
  {
    if (v.toInt() >= 0 && v.toInt() <= 3600)
    {
      sprintf(temp, ":$BR%d#", (int16_t)v.toInt());
      onStep.commandBool(temp);
    }
  }

  v = www.arg("b2");
  if (!v.equals(EmptyStr))
  {
    if (v.toInt() >= 0 && v.toInt() <= 3600)
    {
      sprintf(temp, ":$BD%d#", (int16_t)v.toInt());
      onStep.commandBool(temp);
    }
  }
}
