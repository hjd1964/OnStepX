// -----------------------------------------------------------------------------------
// Focuser
#include "Focuser.h"

#include "../KeyValue.h"
#include "../Page.h"
#include "../Pages.common.h"

void processFocuserGet();

void handleFocuser() {
  char temp[240] = "";

  state.updateFocuser(true);

  SERIAL_ONSTEP.setTimeout(webTimeout);
  onStep.serialRecvFlush();

  processFocuserGet();

  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/html", String());

  // send a standard http response header
  String data = FPSTR(html_head_begin);
  data.concat(FPSTR(html_main_css_begin));
  www.sendContentAndClear(data);
  data.concat(FPSTR(html_main_css_core));
  data.concat(FPSTR(html_main_css_control));
  www.sendContentAndClear(data);
  data.concat(FPSTR(html_main_css_buttons));
  data.concat(FPSTR(html_main_css_collapse));
  data.concat(FPSTR(html_main_css_end));
  data.concat(FPSTR(html_head_end));
  www.sendContentAndClear(data);

  // show this page
  data.concat(FPSTR(html_body_begin));
  www.sendContentAndClear(data);
  pageHeader(PAGE_FOCUSER);
  data.concat(FPSTR(html_onstep_page_begin));

  // OnStep wasn't found, show warning and info.
  if (!status.onStepFound) {
    data.concat(FPSTR(html_bad_comms_message));
    data.concat(FPSTR(html_page_and_body_end));
    www.sendContentAndClear(data);
    www.sendContent("");
    return;
  }

  // scripts
  sprintf_P(temp, html_script_ajax_get, "focuser-ajax-get.txt");
  data.concat(temp);
  data.concat(FPSTR(html_script_ajax_shortcuts));
  data.concat(F("<script>var ajaxPage='focuser-ajax.txt';</script>\n"));
  www.sendContentAndClear(data);
  data.concat(FPSTR(html_script_ajax));
  www.sendContentAndClear(data);

  // page contents
  focuserSelectTile(data);
  focuserHomeTile(data);
  focuserSlewingTile(data);
  focuserBacklashTcfTile(data);

  data.concat(F("<br class='clear' />"));
  data.concat(FPSTR(html_page_and_body_end));

  data.concat(FPSTR(html_script_collapsible));

  www.sendContentAndClear(data);
  www.sendContent("");
}

void focuserAjaxGet()
{
  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/plain", String());

  processFocuserGet();

  www.sendContent("");
}

void focuserAjax()
{
  String data = "";

  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/plain", String());

  focuserSelectTileAjax(data);
  focuserHomeTileAjax(data);
  focuserSlewingTileAjax(data);
  focuserBacklashTcfTileAjax(data);

  www.sendContentAndClear(data);
  www.sendContent("");

  state.lastFocuserPageLoadTime = millis();
}

void processFocuserGet()
{
  focuserSelectTileGet();
  focuserHomeTileGet();
  focuserSlewingTileGet();
  focuserBacklashTcfTileGet();

  state.lastFocuserPageLoadTime = millis();
}
