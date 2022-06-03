// -----------------------------------------------------------------------------------
// Web server

#include "WebServer.h"
#include "../../tasks/OnTask.h"

#if (OPERATIONAL_MODE == ETHERNET_W5100 || OPERATIONAL_MODE == ETHERNET_W5500) && WEB_SERVER == ON

  void WebServer::begin(long port, long timeToClose, bool autoReset) {
    if (webServer == NULL) {
      ethernetManager.init();

      this->port = port;
      this->timeToClose = timeToClose;
      this->autoReset = autoReset;

      webServer = new EthernetServer(port);
      webServer->begin();
      Ethernet.setRetransmissionCount(8);
      Ethernet.setRetransmissionTimeout(25);
      VF("MSG: Ethernet, started web server on port "); VL(port);
    }
  }

  void WebServer::handleClient() {
    client = webServer->available();

    if (client) {
      WLF("--------------------------------------------------");
      WF("MSG: Webserver, new client socket "); WL(client.getSocketNumber());

      parameter_count = 0;
      String line = "";
      int currentSection = 1;
      int handler_number = -1;
      lastMethod = HTTP_UNKNOWN;

      unsigned long to = millis() + WEB_SOCKET_TIMEOUT;
      while (client.connected() && (long)(millis() - to) < 0 && currentSection <= 2) {
        if (client.available()) {
          // read in a char
          char c = client.read(); if (c == '\r') continue;

          // build up ea. line
          if (line.length() <= 1024) line += c;

          // loop until an entire line is present
          if (c != '\n' && client.available()) continue;

          // look for end of sections
          if (line.equals("\n")) { line = ""; currentSection++; continue; }

          // scan the header
          if (currentSection == 1) {
            if (!modifiedSinceFound && line.indexOf("If-Modified-Since:") >= 0) modifiedSinceFound = true;
            if (lastMethod == HTTP_UNKNOWN) {
              int index = line.indexOf("GET ");
              if (index >= 0) {
                lastMethod = HTTP_GET;
                line = line.substring(index + 4);
                handler_number = getHandler(&line);
                if (handler_number >= 0) processGet(&line);
                break;
              } else {
                index = line.indexOf("POST ");
                if (index >= 0) {
                  lastMethod = HTTP_POST;
                  line = line.substring(index + 5);
                  handler_number = getHandler(&line);
                } else {
                  index = line.indexOf("PUT ");
                  if (index >= 0) {
                    lastMethod = HTTP_PUT;
                    line = line.substring(index + 4);
                    handler_number = getHandler(&line);
                    if (handler_number >= 0) processPut(&line);
                  }
                }
              }
            }
          } else

          // scan the request
          if (currentSection == 2 && handler_number >= 0) {
            if (lastMethod == HTTP_PUT) processPut(&line); else
            if (lastMethod == HTTP_POST) processPost(&line);
          }

          line = "";
        }
        Y;
      }

      // process get or post request
      bool handlerFound = false;
      if (handler_number >= 0) {
        if (handlers[handler_number] != NULL) {
          WF("MSG: Webserver, running handler# "); WL(handler_number);
          (*handlers[handler_number])();
          handlerFound = true;
        }
      }

      // handle page not found
      if (!handlerFound && notFoundHandler != NULL) {
        WLF("MSG: Webserver, didn't find handler");
        (*notFoundHandler)();
      }

      // port timeout to close
      tasks.yield(timeToClose);

      // make sure everything is sent
      client.flush();

      // close the connection:
      client.stop();

      // reset the webserver if requested
      if (autoReset) webServer->begin();

      modifiedSinceFound = false;
  
      WL("MSG: Webserver, client disconnected");
    }
  }

  int WebServer::getHandler(String* line) {
    int url_end = line->indexOf("HTTP/");
    if (url_end <= 0) return -1;

    WF("MSG: Webserver, checking handler for ");

    // isolate the content
    *line = line->substring(0, url_end);
    int url_start = 0;
    int url_end2 = line->lastIndexOf('?');
    if (url_end2 > 0) url_end = url_end2;
    requestedHandler = line->substring(url_start, url_end);
    *line = line->substring(url_end);
    requestedHandler.trim();
    if (requestedHandler.length() == 0) requestedHandler = "/";
    WF("["); W(requestedHandler); WL("]");

    for (int i = 0; i < handler_count; i++) {
      if (requestedHandler.equals(handlers_fn[i])) {
        WF("MSG: Webserver, found handler# "); WL(i);
        return i;
      }
    }
    return -1;
  }

  void WebServer::processGet(String* line) {
    WLF("MSG: Webserver, checking header GET parameters");

    if (line->equals(EmptyStr)) { WLF("MSG: Webserver, GET empty"); return; }

    // isolate any parameters, get their values
    // look for form "?a=1&" or "&a=1"
    while ((*line)[0] == '?' || (*line)[0] == '&') {
      *line = line->substring(1);
      int j  = line->indexOf('=');
      if (j == -1) j = line->length();
      if (j == -1) break; // invalid formatting
      int j1 = line->indexOf('&');
      if (j1 == -1) j1 = line->length() + 1;
      String thisArg = line->substring(0, j);
      String thisVal = line->substring(j + 1, j1);
      if (thisArg != "") {
        if (++parameter_count > PARAMETER_COUNT_MAX) parameter_count = PARAMETER_COUNT_MAX;
        parameters[parameter_count - 1] = thisArg;
        thisVal.trim();
        values[parameter_count - 1] = thisVal;
      }
      if ((int)line->length() > j1) *line = line->substring(j1); else *line = "";

      WF("MSG: Webserver, found "); W(thisArg); W(" = "); WL(thisVal);
    }
  }

  void WebServer::processPut(String* line) {
    WLF("MSG: Webserver, checking header PUT parameters");

    // make all tokens start with '&'
    *line = "&" + *line;

    if (line->equals("&")) { WLF("MSG: Webserver, PUT empty"); return; }

    // isolate any parameters, get their values
    // look for form "&a=1"
    while ((*line)[0] == '?' || (*line)[0] == '&') {
      *line = line->substring(1);
      int j  = line->indexOf('=');
      if (j == -1) j = line->length();
      if (j == -1) break; // invalid formatting
      int j1 = line->indexOf('&');
      if (j1 == -1) j1 = line->length() + 1;
      String thisArg = line->substring(0, j);
      if (thisArg.startsWith("?")) thisArg = thisArg.substring(1);
      String thisVal = line->substring(j + 1, j1);
      if (thisArg != "") {
        if (++parameter_count > PARAMETER_COUNT_MAX) parameter_count = PARAMETER_COUNT_MAX;
        parameters[parameter_count - 1] = thisArg;
        thisVal.trim();
        values[parameter_count - 1] = thisVal;
      }
      if ((int)line->length() > j1) *line = line->substring(j1); else *line = "";

      WF("MSG: Webserver found "); W(thisArg); W(" = "); WL(thisVal);
    }
  }

  void WebServer::processPost(String* line) {
    WLF("MSG: Webserver, checking header POST parameters");

    // make all tokens start with '&'
    *line = "&" + *line;

    if (line->equals(EmptyStr)) { WLF("MSG: Webserver, POST empty"); return; }

    // isolate any parameters, get their values
    // look for form "&a=1"
    while ((*line)[0] == '&') {
      *line = line->substring(1);
      int j  = line->indexOf('=');
      if (j == -1) j = line->length();
      if (j == -1) break; // invalid formatting
      int j1 = line->indexOf('&');
      if (j1 == -1) j1 = line->length() + 1;
      String thisArg = line->substring(0, j);
      String thisVal = line->substring(j + 1, j1);
      if (thisArg != "") {
        if (++parameter_count > PARAMETER_COUNT_MAX) parameter_count = PARAMETER_COUNT_MAX;
        parameters[parameter_count - 1] = thisArg;
        values[parameter_count - 1] = thisVal;
      }
      if ((int)line->length() > j1) *line = line->substring(j1); else *line = "";

      WF("MSG: Webserver, found "); W(thisArg); W(" = "); WL(thisVal);
    }
  }

  void WebServer::on(String fn, webFunction handler) {
    handler_count++; if (handler_count > WEB_HANDLER_COUNT_MAX) { handler_count = WEB_HANDLER_COUNT_MAX; return; }
    handlers[handler_count - 1] = handler;
    handlers_fn[handler_count - 1] = fn;
  }
  
  void WebServer::onNotFound(webFunction handler) {
    notFoundHandler = handler;
  }
  
  // get argument value by identifier
  String WebServer::arg(String id) {
    for (int i = 0; i < parameter_count; i++) {
      if (id == parameters[i]) return values[i];
    }
    return EmptyStr;
  }

  // get argument value by index
  String WebServer::arg(int i) {
    if (i >= 0 && i < parameter_count) {
      return values[i];
    } else return "";
  }
  
  // get argument identifier by index
  String WebServer::argName(int i) {
    if (i >= 0 && i < parameter_count) {
      return parameters[i];
    } else return EmptyStr;
  }
  
  // get arguments count
  int WebServer::args() {
    return parameter_count;
  }
  
  // check if argument exists
  bool WebServer::hasArg(String id) {
    for (int i = 0; i < parameter_count; i++) {
      if (id == parameters[i]) return true;
    }
    return false;
  }
  
  void WebServer::setContentLength(long length) {
    this->length = length;
  }

  void WebServer::setResponseHeader(const char* str) {
    if (!str) return;
    strcpy_P(responseHeader, str);
  }
  
  void WebServer::sendHeader(const char* key, const char* val, bool first) {
    String line = key;
    line.concat(": ");
    line.concat(val);
    line.concat("\r\n");
    if (first) header = line + header; else header.concat(line);
  }

  void WebServer::send(int code, const char* content_type, const String& content) {
    header = "HTTP/1.1 " + String(code) + " OK\r\n" + header;
    header += "Content-Type: "; header += content_type; header += "\r\n";
    if (length == CONTENT_LENGTH_UNKNOWN) {
      header += "Connection: close\r\n";
    } else {
      if (length == CONTENT_LENGTH_NOT_SET) length = content.length();
      header += "content-length: " + String(length) + "\r\n";
    }
    header += "\r\n";

    client.print(header);

    if (length) client.print(content);

    length = CONTENT_LENGTH_NOT_SET;
    header = "";
  }

  void WebServer::sendContent(String s) {
    client.print(s);
  }

  void WebServer::sendContent(const char * s) {
    client.print(s);
  }

  WebServer www;

#endif
