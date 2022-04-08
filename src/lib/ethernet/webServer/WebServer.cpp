// -----------------------------------------------------------------------------------
// Web server

#include "WebServer.h"
#include "../../tasks/OnTask.h"

#if (OPERATIONAL_MODE == ETHERNET_W5100 || OPERATIONAL_MODE == ETHERNET_W5500) && \
    WEB_SERVER == ON

  // SD CARD support, simply enable and provide a webserver.on("filename.htm") to serve each file
  #ifndef SD_CARD
    #define SD_CARD OFF
  #endif

  #if SD_CARD == ON
    #include <SD.h>
  #endif

  void WebServer::begin(long port, long timeToClose, bool autoReset) {
    if (webServer == NULL) {
      #if defined(SDCARD_CS_PIN)
        static bool SDinit = false;
        if (!SDinit) {
          #if SD_CARD == ON
            #if TEENSYDUINO
              SDfound = SD.begin(BUILTIN_SDCARD);
            #else
              SDfound = SD.begin(SDCARD_CS_PIN);
            #endif
          #else
            if (SDCARD_CS_PIN != OFF) {
              pinMode(SDCARD_CS_PIN, OUTPUT);
              digitalWrite(SDCARD_CS_PIN, HIGH);
            }
          #endif
          SDinit = true;
        }
      #endif

      ethernetManager.init();

      this->port = port;
      this->timeToClose = timeToClose;
      this->autoReset = autoReset;

      webServer = new EthernetServer(port);
      webServer->begin();
      Ethernet.setRetransmissionCount(4);
      Ethernet.setRetransmissionTimeout(25);
      VF("MSG: Ethernet started web server on port "); VL(port);
    }
  }

  void WebServer::handleClient() {
    client = webServer->available();

    if (client) {
      WL("MSG: Webserver new client");

      parameter_count = 0;
      String line = "";
      int currentSection = 1;
      int handler_number = -1;
      bool isGet = false;
      bool isPost = false;
      bool isPut = false;

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
            #if SD_CARD == ON
              if (!modifiedSinceFound && line.indexOf("If-Modified-Since:") >= 0) modifiedSinceFound = true;
            #endif
            if (!isGet && !isPost && !isPut) {
              int index = line.indexOf("GET ");
              if (index >= 0) {
                isGet = true;
                line = line.substring(index + 4);
                handler_number = getHandler(&line);
                if (handler_number >= 0) processGet(&line);
                break;
              } else {
                index = line.indexOf("POST ");
                if (index >= 0) {
                  isPost = true;
                  line = line.substring(index + 5);
                  handler_number = getHandler(&line);
                } else {
                  index = line.indexOf("PUT ");
                  if (index >= 0) {
                    isPut = true;
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
            if (isPut) processPut(&line); else
            if (isPost) processPost(&line);
          }

          line = "";
        }
        Y;
      }

      // process get or post request
      bool handlerFound = false;
      if (handler_number >= 0) {
        if (handlers[handler_number] != NULL) {
          WF("MSG: Webserver running handler# "); WL(handler_number);
         // client.print(responseHeader);
          (*handlers[handler_number])();
          handlerFound = true;
        } else {
          #if SD_CARD == ON
            char temp[512];
            if (handlers_fn[handler_number].endsWith(".js")) {
              if (modifiedSinceFound) {
                WLF("MSG: Webserver sending js304Header");
                strcpy_P(temp, http_js304Header);
              } else {
                WLF("MSG: Webserver sending jsHeader");
                strcpy_P(temp, http_jsHeader);
              }
            } else
            if (handlers_fn[handler_number].endsWith(".txt")) {
              WLF("MSG: Webserver sending textHeader");
              strcpy_P(temp, http_textHeader);
            } else
            if (handlers_fn[handler_number].endsWith(".htm") || handlers_fn[handler_number].endsWith(".html")) {
              WLF("MSG: Webserver sending htmlHeader");
              strcpy_P(temp, http_defaultHeader);
            } else {
              WLF("MSG: Webserver assuming html, sending htmlHeader");
              strcpy_P(temp, http_defaultHeader);
            }
            client.print(temp);

            if (!modifiedSinceFound) {
              WLF("MSG: Webserver sending SD file");
              sdPage(handlers_fn[handler_number], &client);
            }
            handlerFound = true;
          #endif
        }
      }
      
      // handle page not found
      if (!handlerFound && notFoundHandler != NULL) {
        WLF("MSG: Webserver didn't find handler");
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

      #if SD_CARD == ON
        modifiedSinceFound = false;
      #endif
  
      WL("MSG: Webserver client disconnected");
    }
  }

  int WebServer::getHandler(String* line) {
    int url_end = line->indexOf("HTTP/");
    if (url_end <= 0) return -1;

    WF("MSG: Webserver checking handler for ");

    // isolate the content
    *line = line->substring(0, url_end);
    int url_start = 0;
    if (line->startsWith('/')) *line = line->substring(1);
    int url_end2 = line->lastIndexOf('?');
    if (url_end2 > 0) url_end = url_end2;
    String requestedHandler = line->substring(url_start, url_end);
    *line = line->substring(url_end);
    requestedHandler.trim();
    if (requestedHandler.length() == 0) requestedHandler = "/";
    WF("["); W(requestedHandler); WL("]");

    for (int i = 0; i < handler_count; i++) {
      if (requestedHandler.equals(handlers_fn[i])) {
        WF("MSG: Webserver found handler# "); WL(i);
        return i;
      }
    }
    return -1;
  }

  void WebServer::processGet(String* line) {
    WLF("MSG: Webserver checking header GET parameters");

    if (line->equals(EmptyStr)) { WLF("MSG: Webserver GET empty"); return; }

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
        values[parameter_count - 1] = thisVal.trim();
      }
      if ((int)line->length() > j1) *line = line->substring(j1); else *line = "";

      WF("MSG: Webserver found "); W(thisArg); W(" = "); WL(thisVal);
    }
  }

  void WebServer::processPut(String* line) {
    WLF("MSG: Webserver checking header PUT parameters");

    // make all tokens start with '&'
    *line = "&" + *line;

    if (line->equals("&")) { WLF("MSG: Webserver PUT empty"); return; }

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
      if (thisArg.startsWith('?')) thisArg = thisArg.substring(1);
      String thisVal = line->substring(j + 1, j1);
      if (thisArg != "") {
        if (++parameter_count > PARAMETER_COUNT_MAX) parameter_count = PARAMETER_COUNT_MAX;
        parameters[parameter_count - 1] = thisArg;
        values[parameter_count - 1] = thisVal.trim();
      }
      if ((int)line->length() > j1) *line = line->substring(j1); else *line = "";

      WF("MSG: Webserver found "); W(thisArg); W(" = "); WL(thisVal);
    }
  }

  void WebServer::processPost(String* line) {
    WLF("MSG: Webserver checking header POST parameters");

    // make all tokens start with '&'
    *line = "&" + *line;

    if (line->equals(EmptyStr)) { WLF("MSG: Webserver POST empty"); return; }

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

      WF("MSG: Webserver found "); W(thisArg); W(" = "); WL(thisVal);
    }
  }

  void WebServer::on(String fn, webFunction handler) {
    handler_count++; if (handler_count > HANDLER_COUNT_MAX) { handler_count = HANDLER_COUNT_MAX; return; }
    handlers[handler_count - 1] = handler;
    handlers_fn[handler_count - 1] = fn;
  }
  
  void WebServer::onNotFound(webFunction handler) {
    notFoundHandler = handler;
  }
  
  String WebServer::arg(String id) {
    for (int i = 0; i < parameter_count; i++) {
      if (id == parameters[i]) return values[i];
    }
    return EmptyStr;
  }
  
  String WebServer::argLowerCase(String id) {
    for (int i = 0; i < parameter_count; i++) {
      if (id == parameters[i].toLowerCase()) return values[i];
    }
    return EmptyStr;
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

  #if SD_CARD == ON
    void WebServer::on(String fn) {
      handler_count++; if (handler_count > HANDLER_COUNT_MAX) { handler_count = HANDLER_COUNT_MAX; return; }
      handlers[handler_count - 1] = NULL;
      handlers_fn[handler_count - 1] = fn;
    }
    
    void WebServer::sdPage(String fn, EthernetClient *client) {
      char temp[512] = "";
      int n;

      // open the sdcard file
      if (SDfound) {
        File dataFile = SD.open(fn.c_str(), O_READ);
        if (dataFile) {
          do {
            n = dataFile.available();
            if (n > 256) n = 256;
            if (n > 0) {
              dataFile.read(temp, n);
              client->write(temp, n);
            }
            Y;
          } while (n > 0);
          dataFile.close();
        }
      }
    }
  #endif

  WebServer www;

#endif
