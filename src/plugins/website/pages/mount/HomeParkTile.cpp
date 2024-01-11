// -----------------------------------------------------------------------------------
// Home/Park tile
#include "HomeParkTile.h"

#include "../KeyValue.h"
#include "../Pages.common.h"

const char *getHomeParkStateStr();

// create the related webpage tile
void homeParkTile(String &data)
{
  char temp[240] = "";
  char reply[120] = "";

  sprintf_P(temp, html_tile_beg, "22em", "15em", L_HOME_PARK_TITLE);
  data.concat(temp);
  data.concat(F("<div style='float: right; text-align: right;' id='hp_sta' class='c'>"));
  data.concat(getHomeParkStateStr());
  data.concat(F("</div><br /><hr>"));

  data.concat(FPSTR(html_homeReturn));
  data.concat(FPSTR(html_homeReset));
  data.concat(F("<br />"));
  data.concat(FPSTR(html_unpark));
  data.concat(FPSTR(html_park));
  data.concat(F("<hr>"));

  sprintf_P(temp, html_collapsable_beg, L_CONTROLS "...");
  data.concat(temp);

  data.concat(F(L_SET_PARK_CURRENT_COORDS ":<br />"));

  data.concat(FPSTR(html_setpark));
  www.sendContentAndClear(data);

  // home options when home sense is available
  if (status.getVersionMajor()*1000 + status.getVersionMinor() >= 1020) {
    data.concat(F("<br />" L_HOME_AUTO "<br />"));
    data.concat(html_homeAuto);
    www.sendContentAndClear(data);

    if (onStep.command(":h?#", reply)) {
      status.hasHomeSense = false;
      long homeAutomatic = false;
      long homeOffsetAxis1 = 0;
      long homeOffsetAxis2 = 0;
      sscanf(reply, "%ld,%ld,%ld", &status.hasHomeSense, &homeOffsetAxis1, &homeOffsetAxis2);

      sprintf_P(temp, html_form_begin, "mount.htm");
      data.concat(temp);

      if (status.hasHomeSense) {
        data.concat(F("<br />" L_HOME_OFFSET "<br />"));
        sprintf_P(temp, html_homeOffsetAxis1, homeOffsetAxis1);
        data.concat(temp);
        sprintf_P(temp, html_homeOffsetAxis2, homeOffsetAxis2);
        data.concat(temp);
        data.concat(F("<button type='submit'>" L_UPLOAD "</button><br />\n"));
        data.concat(FPSTR(html_form_end));
        www.sendContentAndClear(data);
      }
    }
  }

  data.concat(FPSTR(html_collapsable_end));
  data.concat(FPSTR(html_tile_end));
  www.sendContentAndClear(data);

}

// use Ajax key/value pairs to pass related data to the web client in the background
void homeParkTileAjax(String &data)
{
  data.concat(keyValueString("hp_sta", getHomeParkStateStr()));

  if (status.atHome || status.parked) {
    data.concat(keyValueBoolEnabled("park", false));
    data.concat(keyValueBoolEnabled("unpark", true));
  } else {
    data.concat(keyValueBoolEnabled("park", !(status.parkFail || status.parking)));
    data.concat(keyValueBoolEnabled("unpark", false));
  }

  if (status.hasHomeSense) {
    data.concat(keyValueToggleBoolSelected("auto_on", "auto_off", status.autoHome));
  }
    
  www.sendContentAndClear(data);
}

// pass related data back to OnStep
void homeParkTileGet()
{
  String v;
  char temp[80] = "";

  v = www.arg("pk");
  if (!v.equals(EmptyStr))
  {
    if (v.equals("p")) onStep.commandBool(":hP#"); // park
    if (v.equals("u")) onStep.commandBool(":hR#"); // un-park
    if (v.equals("s")) onStep.commandBool(":hQ#"); // set-park
  }

  v = www.arg("qb");
  if (!v.equals(EmptyStr))
  {
    if (v.equals("hf")) onStep.commandBlind(":hF#"); // home, reset
    if (v.equals("hc")) onStep.commandBlind(":hC#"); // home, find
  }

  v = www.arg("dr");
  if (!v.equals(EmptyStr))
  {
    if (v.equals("qr")) onStep.commandBlind(":hF#"); // home, reset
    if (v.equals("qh")) onStep.commandBlind(":hC#"); // home, find
    if (v.equals("pk")) onStep.commandBool(":hP#"); // park
    if (v.equals("pu")) onStep.commandBool(":hR#"); // un-park
  }

  // home options when home sense is available
  if (status.hasHomeSense) {
    v = www.arg("ha");
    if (!v.equals(EmptyStr))
    {
      if (v.equals("0")) onStep.commandBlind(":hA0#"); // turn auto home off
      if (v.equals("1")) onStep.commandBlind(":hA1#"); // turn auto home on
    }

    v = www.arg("hc1");
    if (!v.equals(EmptyStr))
    {
      if (v.toInt() >= -162000 && v.toInt() <= 162000)
      {
        sprintf(temp, ":hC1,%ld#", v.toInt());
        onStep.commandBlind(temp);
      }
    }

    v = www.arg("hc2");
    if (!v.equals(EmptyStr))
    {
      if (v.toInt() >= -162000 && v.toInt() <= 162000)
      {
        sprintf(temp, ":hC2,%ld#", v.toInt());
        onStep.commandBlind(temp);
      }
    }
  }

}

const char *getHomeParkStateStr()
{
  static char hsta[32];
  if (status.parking) strncpyex(hsta, L_PARKING, 32); else
  if (status.homing) strncpyex(hsta, L_HOMING, 32); else
  if (status.parkFail) strncpyex(hsta, L_PARK_FAILED, 32); else
  if (status.parked) strncpyex(hsta, L_PARKED, 32); else
  if (status.atHome) strncpyex(hsta, L_AT_HOME, 32); else strncpyex(hsta, L_INACTIVE, 32);
  return hsta;
}
