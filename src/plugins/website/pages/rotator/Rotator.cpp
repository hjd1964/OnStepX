// -----------------------------------------------------------------------------------
// Rotator
#include "Rotator.h"

#include "../Page.h"
#include "../KeyValue.h"
#include "../Pages.common.h"

void processRotatorGet();

void handleRotator() {
  char temp[240] = "";

  state.updateRotator(true);

  SERIAL_ONSTEP.setTimeout(webTimeout);
  onStep.serialRecvFlush();

  processRotatorGet();

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
  pageHeader(PAGE_ROTATOR);
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
  sprintf_P(temp, html_script_ajax_get, "rotator-ajax-get.txt");
  data.concat(temp);
  data.concat(FPSTR(html_script_ajax_shortcuts));
  data.concat("<script>var ajaxPage='rotator-ajax.txt';</script>\n");
  www.sendContentAndClear(data);
  data.concat(FPSTR(html_script_ajax));
  www.sendContentAndClear(data);

  // page contents
  rotatorHomeTile(data);
  rotatorSlewingTile(data);
  if (status.derotatorFound) deRotatorTile(data);
  rotatorBacklashTile(data);

  data.concat("<br class='clear' />");
  data.concat(FPSTR(html_page_and_body_end));

  data.concat(FPSTR(html_script_collapsible));

  www.sendContentAndClear(data);
  www.sendContent("");
}

void rotatorAjaxGet()
{
  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/plain", String());

  processRotatorGet();

  www.sendContent("");
}

void rotatorAjax()
{
  String data = "";

  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/plain", String());

  rotatorHomeTileAjax(data);
  rotatorSlewingTileAjax(data);
  if (status.derotatorFound) deRotatorTileAjax(data);
  rotatorBacklashTileAjax(data);

  www.sendContentAndClear(data);
  www.sendContent("");

  state.lastRotatorPageLoadTime = millis();
}

void processRotatorGet()
{
  rotatorHomeTileGet();
  rotatorSlewingTileGet();
  if (status.derotatorFound) deRotatorTileGet();
  rotatorBacklashTileGet();

  state.lastRotatorPageLoadTime = millis();
}
