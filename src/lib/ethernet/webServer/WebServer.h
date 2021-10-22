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

  // macros to help with sending webpage data
  #define sendHtmlStart()
  #define sendHtml(x) www.sendContent(x);
  #define sendHtmlDone(x) www.sendContent("");
  // misc.
  #define WEB_SOCKET_TIMEOUT    10000
  #define HANDLER_COUNT_MAX     16
  #define PARAMETER_COUNT_MAX   8

  const char http_js304Header[] PROGMEM =
  "HTTP/1.1 304 OK\r\n" "Content-Type: application/javascript\r\n" "Etag: \"3457807a63ac7bdabf8999b98245d0fe\"\r\n" "Last-Modified: Mon, 13 Apr 2015 15:35:56 GMT\r\n" "Connection: close\r\n" "\r\n";

  const char http_jsHeader[] PROGMEM =
  "HTTP/1.1 200 OK\r\n" "Content-Type: application/javascript\r\n" "Etag: \"3457807a63ac7bdabf8999b98245d0fe\"\r\n" "Last-Modified: Mon, 13 Apr 2015 15:35:56 GMT\r\n" "Connection: close\r\n" "\r\n";

  typedef void (* webFunction) ();
  
  class WebServer {
    public:
      void begin();

      void handleClient();
      void setResponseHeader(const char *str);
      void on(String fn, webFunction handler);
      #if SD_CARD == ON
        void on(String fn);
      #endif
      void onNotFound(webFunction handler);
      String arg(String id);
  
      void sendContent(String s);
      void sendContent(const char * s);

      bool SDfound = false;

      EthernetClient client;
    private:
      int  getHandler(String* line);
      void processGet(String* line);
      void processPost(String* line);
 
      #if SD_CARD == ON
        void sdPage(String fn, EthernetClient* client);
      #endif
  
      char responseHeader[200] = "";
      #if SD_CARD == ON
        bool modifiedSinceFound = false;
      #endif
  
      webFunction notFoundHandler = NULL;
      webFunction handlers[HANDLER_COUNT_MAX];
      String handlers_fn[HANDLER_COUNT_MAX];
      int handler_count = 0;
      
      String parameters[PARAMETER_COUNT_MAX];
      String values[PARAMETER_COUNT_MAX];
      int parameter_count;
  };

  extern WebServer www;
#endif
