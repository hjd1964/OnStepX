// -----------------------------------------------------------------------------------
// Telescope control related functions
#include "Mount.h"

#include "../KeyValue.h"
#include "../Page.h"
#include "../Pages.common.h"

void processMountGet();

void handleMount()
{
  char temp[240] = "";

  state.updateMount(true);

  SERIAL_ONSTEP.setTimeout(webTimeout);
  onStep.serialRecvFlush();

  processMountGet();

  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/html", String());

  String data = FPSTR(html_head_begin);
  data.concat(FPSTR(html_main_css_begin));
  www.sendContentAndClear(data);
  data.concat(FPSTR(html_main_css_core));
  www.sendContentAndClear(data);
  data.concat(FPSTR(html_main_css_no_select));
  data.concat(FPSTR(html_main_css_control));
  www.sendContentAndClear(data);
  data.concat(FPSTR(html_main_css_buttons));
  data.concat(FPSTR(html_main_css_collapse));
  data.concat(FPSTR(html_main_css_end));
  data.concat(FPSTR(html_head_end));
  www.sendContentAndClear(data);

  // show this page
  data.concat(FPSTR(html_body_begin));
  pageHeader(PAGE_MOUNT);
  data.concat(FPSTR(html_onstep_page_begin));

  // OnStep wasn't found, show warning and info.
  if (!status.onStepFound)
  {
    data.concat(FPSTR(html_bad_comms_message));
    data.concat(FPSTR(html_page_and_body_end));
    www.sendContentAndClear(data);
    www.sendContent("");
    return;
  }

  // scripts
  sprintf_P(temp, html_script_ajax_get, "mount-ajax-get.txt");
  data.concat(temp);
  data.concat(FPSTR(html_script_ajax_shortcuts));
  data.concat(FPSTR(html_script_ajax_date_time_return));
  data.concat(F("<script>var ajaxPage='mount-ajax.txt';</script>\n"));
  www.sendContentAndClear(data);
  data.concat(FPSTR(html_script_ajax));
  www.sendContentAndClear(data);

  // page contents
  siteTile(data);
  homeParkTile(data);
  alignTile(data);
  gotoTile(data);
  libraryTile(data);
  guideTile(data);
  trackingTile(data);
  if (status.pecEnabled) pecTile(data);
  limitsTile(data);

  data.concat(F("<br class='clear' />"));
  data.concat(FPSTR(html_page_and_body_end));

  data.concat(FPSTR(html_script_collapsible));

  www.sendContentAndClear(data);
  www.sendContent("");
}

void mountAjaxGet()
{
  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/plain", String());

  processMountGet();

  www.sendContent("");
}

void mountAjax()
{
  String data = "";

  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/plain", String());

  if (status.onStepFound)
  {
    siteTileAjax(data);
    homeParkTileAjax(data);
    alignTileAjax(data);
    gotoTileAjax(data);
    libraryTileAjax(data);
    guideTileAjax(data);
    trackingTileAjax(data);
    if (status.pecEnabled) pecTileAjax(data);
    limitsTileAjax(data);
  }
  else
  {
    // Disable everything
    char keys_en[27][8] = {
      "c_goto", "bzr_off", "bzr_on", "mfa_off", "mfa_on", "mfp_off", "mfp_on",
      "sr_vf", "sr_f", "sr_n", "sr_s", "sr_vs", "trk_off", "trk_on", "alg1",
      "alg2", "alg3", "alga", "rpa", "trk_sid", "trk_sol", "trk_lun",
      "ot_on", "ot_ref", "ot_off", "ot_dul", "ot_sgl",
    };
    for (int i = 0; i < 27; i++) data.concat(keyValueBoolEnabled(keys_en[i], false));

    char keys_str[14][18] = {
      "date_ut", "time_ut", "time_lst", "site_long", "site_lat", "pier_side", "idx_a1", 
      "idx_a2", "tgt_a1", "tgt_a2", "track", "align_progress", "align_lr", "align_ud",
    };
    for (int i = 0; i < 5; i++) data.concat(keyValueString(keys_str[i], "?"));
  }

  www.sendContentAndClear(data);
  www.sendContent("");

  state.lastMountPageLoadTime = millis();
}

void processMountGet()
{
  siteTileGet();
  homeParkTileGet();
  alignTileGet();
  gotoTileGet();
  libraryTileGet();
  guideTileGet();
  trackingTileGet();
  if (status.pecEnabled) pecTileGet();
  limitsTileGet();

  state.lastMountPageLoadTime = millis();
}

