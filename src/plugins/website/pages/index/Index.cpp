// -----------------------------------------------------------------------------------
// The home page, status information
#include "Index.h"

#include "../Page.h"
#include "../Pages.common.h"

void processIndexGet();

void handleRoot()
{
  char temp[240] = "";

  state.updateController(true);

  SERIAL_ONSTEP.setTimeout(webTimeout);
  onStep.serialRecvFlush();

  processIndexGet();

  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/html", String());

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
  pageHeader(PAGE_CONTROLLER);
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
  data.concat("<script>var ajaxPage='index.txt';</script>\n");
  www.sendContentAndClear(data);
  data.concat(FPSTR(html_script_ajax));
  www.sendContentAndClear(data);
  sprintf_P(temp, html_script_ajax_get, "index-ajax-get.txt");
  data.concat(temp);
  www.sendContentAndClear(data);

  // page contents
  #if DISPLAY_WEATHER == ON
    ambientTile(data);
  #endif

  statusTile(data);

  int numShown = 0;
  int numAxes = 5;
  if (status.getVersionMajor() >= 10) numAxes = 9;
  for (int axis = 0; axis < numAxes; axis++)
  {
    if (axisTile(axis, data)) numShown++;
  }

  data.concat("<br class=\"clear\" />\n");

  #if DISPLAY_SERVO_MONITOR == ON
    servoTile(data);
    data.concat("<br class=\"clear\" />\n");
  #endif

  #if DRIVE_CONFIGURATION == ON
    if (numShown == 0) data.concat("<br />" L_ADV_SET_NO_EDIT "<br />");

    sprintf_P(temp, html_form_begin, "index.htm");
    data.concat(temp);

    data.concat(F("<br /><button name='advanced' type='submit' "));
    if (numShown == 0) data.concat("value='enable'>" L_ADV_ENABLE "</button>"); else data.concat("value='disable'>" L_ADV_DISABLE "</button>");

    data.concat(FPSTR(html_form_end));
    www.sendContentAndClear(data);

    if (numShown > 0) {
      data.concat(FPSTR(html_configAxesNotes));
      if (status.getVersionMajor() < 10) data.concat(FPSTR(html_configAxesNotesOnStep));
    } else
      data.concat("<br />");
    www.sendContentAndClear(data);
  #endif

  data.concat(FPSTR(html_script_collapsible));

  data.concat(FPSTR(html_page_and_body_end));

  www.sendContentAndClear(data);
  www.sendContent("");
}

void indexAjaxGet()
{
  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/plain", String());

  processIndexGet();

  www.sendContent("");
}

void indexAjax() {
  String data = "";

  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/plain", String());

  if (status.onStepFound)
  {
    #if DISPLAY_WEATHER == ON
      ambientTileAjax(data);
    #endif
    statusTileAjax(data);
    int numAxes = 2;
    if (status.getVersionMajor() >= 10) numAxes = 9;
    for (int axis = 0; axis < numAxes; axis++) axisTileAjax(axis, data);
    #if DISPLAY_SERVO_MONITOR == ON
      servoTileAjax(data);
    #endif
  }

  www.sendContentAndClear(data);
  www.sendContent("");

  state.lastControllerPageLoadTime = millis();
}

void processIndexGet()
{
  String ssa = www.arg("advanced");
  if (ssa.equals("enable"))
  {
    onStep.commandBool(":SXAC,0#");
  } else
  if (ssa.equals("disable"))
  {
    onStep.commandBool(":SXAC,1#");
  }

  #if DISPLAY_WEATHER == ON
    ambientTileGet();
  #endif
  statusTileGet();
  axisTileGet();
  #if DISPLAY_SERVO_MONITOR == ON
    servoTileGet();
  #endif

  state.lastControllerPageLoadTime = millis();
}
