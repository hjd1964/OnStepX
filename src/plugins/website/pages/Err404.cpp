// -----------------------------------------------------------------------------------
// Error not found, 404

#include "Pages.common.h"

void handleNotFound() {
  String message = "<body>";

  message.concat("SmartWebServer, 404 File Not Found<br /><br />\n");
  message.concat("URI: " + www.uri() + "<br />\n");

  message.concat("Method: ");
  if (www.method() == HTTP_GET) message.concat("GET<br />\n"); else
  if (www.method() == HTTP_PUT) message.concat("PUT<br />\n"); else
  if (www.method() == HTTP_POST) message.concat("POST<br />\n"); else message.concat("Unknown<br />\n");

  message.concat("\nArguments: " + String(www.args()) + "<br />\n");
  for (uint8_t i = 0; i < www.args(); i++){
    message.concat(" " + www.argName(i) + ": " + www.arg(i) + "<br />\n");
  }

  message.concat("</body>");

  www.send(404, "text/html", message);
}
