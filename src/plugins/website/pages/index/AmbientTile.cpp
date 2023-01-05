// -----------------------------------------------------------------------------------
// Ambient tile
#include "AmbientTile.h"

#include "../KeyValue.h"
#include "../Pages.common.h"

#if DISPLAY_WEATHER == ON

// create the related webpage tile
void ambientTile(String &data)
{
  char temp[240] = "";

  sprintf_P(temp, html_tile_text_beg, "22em", "11em", "Weather");
  data.concat(temp);
  data.concat("<br /><hr>");

  // Ambient conditions
  sprintf_P(temp, html_indexTPHD, L_AMBIENT_TEMPERATURE ":", 't', state.siteTemperatureStr); data.concat(temp);
  sprintf_P(temp, html_indexTPHD, L_PRESSURE ":", 'p', state.sitePressureStr); data.concat(temp);
  sprintf_P(temp, html_indexTPHD, L_HUMIDITY ":", 'h', state.siteHumidityStr); data.concat(temp);
  sprintf_P(temp, html_indexTPHD, L_DEW_POINT ":", 'd', state.siteDewPointStr); data.concat(temp);
  www.sendContentAndClear(data);

  data.concat("<hr>");

  data.concat(FPSTR(html_tile_end));
  www.sendContentAndClear(data);

}

// use Ajax key/value pairs to pass related data to the web client in the background
void ambientTileAjax(String &data)
{
  #if DISPLAY_WEATHER == ON
    data.concat("tphd_t|"); data.concat(state.siteTemperatureStr); data.concat("\n");
    data.concat("tphd_p|"); data.concat(state.sitePressureStr); data.concat("\n");
    data.concat("tphd_h|"); data.concat(state.siteHumidityStr); data.concat("\n");
    data.concat("tphd_d|"); data.concat(state.siteDewPointStr); data.concat("\n");
  #endif

  www.sendContentAndClear(data);
}

// pass related data back to OnStep
extern void ambientTileGet()
{
  String v;
}

#endif
