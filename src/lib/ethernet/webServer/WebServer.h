// -----------------------------------------------------------------------------------
// Web server
#pragma once

#include "../../../Common.h"

#include "../EthernetManager.h"

#if (OPERATIONAL_MODE == ETHERNET_W5100 || OPERATIONAL_MODE == ETHERNET_W5500) && \
    WEB_SERVER == ON

  // Turn ON to allow webserver debug messages
  #ifndef DEBUG_WEBSERVER
    #define DEBUG_WEBSERVER OFF
  #endif

  #if DEBUG_WEBSERVER == ON
    #define W(x) V(x)
    #define WF(x) VF(x)
    #define WL(x) VL(x)
    #define WLF(x) VLF(x)
  #else
    #define W(x)
    #define WF(x)
    #define WL(x)
    #define WLF(x)
  #endif

  // misc.
  #ifndef WEB_SOCKET_TIMEOUT
  #define WEB_SOCKET_TIMEOUT     250
  #endif
  #ifndef WEB_HANDLER_COUNT_MAX
  #define WEB_HANDLER_COUNT_MAX  16
  #endif
  #define PARAMETER_COUNT_MAX    8
  #define CONTENT_LENGTH_UNKNOWN -1
  #define CONTENT_LENGTH_NOT_SET -2

  const char http_defaultHeader[] PROGMEM =
  "HTTP/1.1 200 OK\r\n" "Content-Type: text/html\r\n" "Connection: close\r\n" "\r\n";

  const char http_textHeader[] PROGMEM =
  "HTTP/1.1 200 OK\r\n" "Content-Type: text/plain\r\n" "Connection: close\r\n" "\r\n";

  const char http_jsonHeader[] PROGMEM =
  "HTTP/1.1 200 OK\r\n" "Content-Type: application/json\r\n" "Connection: close\r\n" "\r\n";

  const char http_js304Header[] PROGMEM =
  "HTTP/1.1 304 OK\r\n" "Content-Type: application/javascript\r\n" "Etag: \"3457807a63ac7bdabf8999b98245d0fe\"\r\n" "Last-Modified: Mon, 13 Apr 2015 15:35:56 GMT\r\n" "Connection: close\r\n" "\r\n";

  const char http_jsHeader[] PROGMEM =
  "HTTP/1.1 200 OK\r\n" "Content-Type: application/javascript\r\n" "Etag: \"3457807a63ac7bdabf8999b98245d0fe\"\r\n" "Last-Modified: Mon, 13 Apr 2015 15:35:56 GMT\r\n" "Connection: close\r\n" "\r\n";

  // macros to help with sending webpage data
//  #define sendHtmlStart() setResponseHeader(http_defaultHeader);
//  #define sendTextStart() setResponseHeader(http_textHeader);
//  #define sendJsonStart() setResponseHeader(http_jsonHeader);
  #define sendContentAndClear(x) sendContent(x); x = "";

  typedef void (* webFunction) ();
  
  class WebServer {
    public:
      void begin(long port = 80, long timeToClose = 100, bool autoReset = false);

      void handleClient();

      void on(String fn, webFunction handler);
      #if SD_CARD == ON
        void on(String fn);
      #endif
      void onNotFound(webFunction handler);

      String arg(String id);
      String argLowerCase(String id);
  
      void setContentLength(long length);
      void setResponseHeader(const char *str);
      void sendHeader(const char* key, const char* val, bool first = false);
      void send(int code, const char* content_type = "text/html", const String& content = "");
      void sendContent(String s);
      void sendContent(const char * s);

      bool SDfound = false;

      EthernetServer *webServer;
      EthernetClient client;
    private:
      int  getHandler(String* line);
      void processGet(String* line);
      void processPut(String* line);
      void processPost(String* line);
 
      #if SD_CARD == ON
        void sdPage(String fn, EthernetClient* client);
      #endif
  
      char responseHeader[200] = "";
      #if SD_CARD == ON
        bool modifiedSinceFound = false;
      #endif
  
      webFunction notFoundHandler = NULL;
      webFunction handlers[WEB_HANDLER_COUNT_MAX];
      String handlers_fn[WEB_HANDLER_COUNT_MAX];
      int handler_count = 0;
      
      String parameters[PARAMETER_COUNT_MAX];
      String values[PARAMETER_COUNT_MAX];
      int parameter_count;
      int port = -1;
      bool autoReset = false;
      long timeToClose = 100;

      long length;
      String header;
  };

  extern WebServer www;

#endif
