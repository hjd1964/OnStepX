// -----------------------------------------------------------------------------------
// Command processing
#pragma once

class Buffer {
  public:
    bool checksum = false;

    void init(int mountType);

    bool add(char c);
    char* getCmd();
    char* getParameter();
    char* getSeq();
    bool ready();
    bool flush();

  private:
    int mountType = 0;
    char channel;
    char (*reader)();
    char (*writer)(char c);

    const static int bufferSize = 80;
    char cmd[4] = "";
    char pb[bufferSize] = "";
    char cb[bufferSize] = "";
    int  cbp = 0;
    char seq = 0;
};
