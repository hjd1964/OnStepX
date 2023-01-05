// -----------------------------------------------------------------------------------
// Goto tile
#include "GotoTile.h"

#include "../KeyValue.h"
#include "../Pages.common.h"

#define KV_GTO_STATUS 0
#define KV_GTO_I1 1
#define KV_GTO_I2 2
#define KV_GTO_T1 3
#define KV_GTO_T2 4
#define KV_GTO_ACTIVE 5
#define KV_GTO_BZR_OFF 6
#define KV_GTO_BZR_ON 7
#define KV_GTO_MFA_OFF 8
#define KV_GTO_MFA_ON 9
#define KV_GTO_MFP_OFF 10
#define KV_GTO_MFP_ON 11
#define KV_GTO_RATE 12
#define KV_GTO_RATE_VF 13
#define KV_GTO_RATE_F 14
#define KV_GTO_RATE_N 15
#define KV_GTO_RATE_S 16
#define KV_GTO_RATE_VS 17

const char *goto_key[18] =
{
  "gto_rate",
  "gto_rate_vf",
  "gto_rate_f",
  "gto_rate_n",
  "gto_rate_s",
  "gto_rate_vs"
};

// create the related webpage tile
void gotoTile(String &data)
{
  char temp[320] = "";

  sprintf_P(temp, html_tile_beg, "22em", "15em", "Goto");
  data.concat(temp);

  data.concat(F("<div style='float: right; text-align: right;' id='gto_status' class='c'>"));
  sprintf(temp, "%s || %c", status.inGoto ? L_SLEWING : L_INACTIVE, state.pierSideStr[0]);
  data.concat(temp);
  data.concat(F("</div><br /><hr>"));

  data.concat(FPSTR(html_mountPositionLabels));
  sprintf_P(temp, html_mountPositionAxis1, state.indexAzmStr, state.indexRaStr, state.targetRaStr);
  data.concat(temp);
  sprintf_P(temp, html_mountPositionAxis2, state.indexAltStr, state.indexDecStr, state.targetDecStr);
  data.concat(temp);

  www.sendContentAndClear(data);

  data.concat(FPSTR(html_gotoGo));
  data.concat(F("&nbsp;&nbsp;&nbsp;"));
  data.concat(FPSTR(html_gotoStop));
  data.concat(F("&nbsp;&nbsp;&nbsp;"));
  data.concat(FPSTR(html_gotoContinue));
  data.concat(F("<hr>"));
  www.sendContentAndClear(data);

  sprintf_P(temp, html_collapsable_beg, L_CONTROLS "...");
  data.concat(temp);

  // Slew speed
  sprintf_P(temp, html_slewSpeed, state.slewSpeedStr);
  data.concat(temp);
  data.concat(FPSTR(html_slewSpeedSelect));

  // Goto Buzzer
  data.concat(FPSTR(html_gotoBuzzer));
  www.sendContentAndClear(data);

  // Goto Meridian Flips
  if (status.mountType == MT_GEM || (status.getVersionMajor() >= 10 && status.mountType == MT_FORK))
  {
    data.concat(FPSTR(html_gotoMfAuto));
    data.concat(FPSTR(html_gotoMfPause));
  }

  data.concat(FPSTR(html_collapsable_end));
  data.concat(FPSTR(html_tile_end));
  www.sendContentAndClear(data);
}

// use Ajax key/value pairs to pass related data to the web client in the background
void gotoTileAjax(String &data)
{

  char pss[2] = "N";
  pss[0] = state.pierSideStr[0];

  data.concat(keyValueString("gto_status", status.inGoto ? L_SLEWING : L_INACTIVE, " || ", pss));

  data.concat(keyValueString("gto_t1", state.targetRaStr));
  data.concat(keyValueString("gto_t2", state.targetDecStr));
  data.concat(keyValueString("gto_i1", state.indexRaStr));
  data.concat(keyValueString("gto_i2", state.indexDecStr));
  data.concat(keyValueString("gto_az1", state.indexAzmStr));
  data.concat(keyValueString("gto_az2", state.indexAltStr));

  data.concat(keyValueBoolEnabled("gto_active", status.inGoto));

  data.concat(keyValueToggleBoolSelected("gto_bzr_on", "gto_bzr_off", status.buzzerEnabled));

  if (status.mountType == MT_GEM || (status.getVersionMajor() >= 10 && status.mountType == MT_FORK)) {
    data.concat(keyValueBoolEnabled("gto_mfa_on", true));
    data.concat(keyValueBoolEnabled("gto_mfa_off", true));
    data.concat(keyValueToggleBoolSelected("gto_mfa_on", "gto_mfa_off", status.autoMeridianFlips));
    data.concat(keyValueToggleBoolSelected("gto_mfp_on", "gto_mfp_off", status.pauseAtHome));
  } else {
    data.concat(keyValueBoolEnabled("gto_mfa_on", false));
    data.concat(keyValueBoolEnabled("gto_mfa_off", false));
  }

  data.concat(keyValueString("gto_rate", state.slewSpeedStr));

  if (!isnan(state.slewSpeedNominal) && !isnan(state.slewSpeedCurrent))
  {
    double rateRatio = state.slewSpeedNominal / state.slewSpeedCurrent;
    char rate_key[5][12] = {"gto_rate_vf", "gto_rate_f", "gto_rate_n", "gto_rate_s", "gto_rate_vs"};
    bool rate_en[5] = {false, false, false, false, false};

    if (rateRatio > 1.75) { rate_en[0] = true; }
    else if (rateRatio > 1.25) { rate_en[1] = true; }
    else if (rateRatio > 0.875) { rate_en[2] = true; }
    else if (rateRatio > 0.625) { rate_en[3] = true; }
    else rate_en[4] = true;
    for (int i = 0; i < 5; i++) {
      String s = keyValueBoolSelected(rate_key[i], rate_en[i]);
      data.concat(s);
    }
  }

  www.sendContentAndClear(data);
}

// pass related data back to OnStep
extern void gotoTileGet()
{
  String v;
  char temp[80];

  v = www.arg("goto");
  if (!v.equals(EmptyStr)) {
    if (v.equals("vs")) onStep.commandBool(":SX93,5#");      // very slow, 0.5 x
    if (v.equals("s"))  onStep.commandBool(":SX93,4#");      // slow,      0.75x
    if (v.equals("n"))  onStep.commandBool(":SX93,3#");      // normal,    1.0 x
    if (v.equals("f"))  onStep.commandBool(":SX93,2#");      // fast,      1.5 x
    if (v.equals("vf")) onStep.commandBool(":SX93,1#");      // very fast, 2.0 x

    if (v.equals("bzr_on"))  onStep.commandBool(":SX97,1#"); // alert buzzer on
    if (v.equals("bzr_off")) onStep.commandBool(":SX97,0#"); // alert buzzer off

    if (v.equals("af_now")) onStep.commandBool(":MN#");      // auto-flip, now
    if (v.equals("af_on")) onStep.commandBool(":SX95,1#");   // auto-flip, on
    if (v.equals("af_off")) onStep.commandBool(":SX95,0#");  // auto-flip, off

    if (v.equals("mp_on")) onStep.commandBool(":SX98,1#");   // meridian-flip, pause at home on
    if (v.equals("mp_off")) onStep.commandBool(":SX98,0#");  // meridian-flip, pause at home off
    if (v.equals("mp_cnt")) onStep.commandBool(":SX99,1#");  // meridian flip, pause->continue

    if (v.equals("go")) onStep.command(":MS#", temp);        // goto start
    if (v.equals("stop")) onStep.commandBlind(":Q#");        // goto/slew stop
  }
}
