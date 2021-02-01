// -----------------------------------------------------------------------------------
// SerialWrapper a single class to allow uniform access to other serial port classes

static uint8_t _wrapper_channels = 0;
#define isChannel(x) (x == thisChannel)

class SerialWrapper : public Stream
{
  public:
    SerialWrapper();
    
    void begin();
    void begin(long baud);
    
    void end();

    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *, size_t);
    virtual int available(void);
    virtual int read(void);
    virtual int peek(void);
    virtual void flush(void);

    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }

    inline bool hasChannel(uint8_t channel) { return bitRead(_wrapper_channels, channel); }
    inline bool setChannel(uint8_t channel) { bitSet(_wrapper_channels, channel); }
    inline bool clrChannel(uint8_t channel) { bitClear(_wrapper_channels, channel); }

    using Print::write;

  private:
    uint8_t thisChannel = 0;
};

SerialWrapper::SerialWrapper() {
  static uint8_t channel = 0;
#ifdef SERIAL_A
  channel++;
  if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
#endif
#ifdef SERIAL_B
  channel++;
  if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
#endif
#ifdef SERIAL_C
  channel++;
  if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
#endif
#ifdef SERIAL_D
  channel++;
  if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
#endif
#ifdef SERIAL_ST4
  channel++;
  if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
#endif
}

void SerialWrapper::begin() {
  begin(9600);
}

void SerialWrapper::begin(long baud) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(++channel)) SERIAL_A.begin(baud);
#endif
#ifdef SERIAL_B
  if (isChannel(++channel)) SERIAL_B.begin(baud);
#endif
#ifdef SERIAL_C
  if (isChannel(++channel)) SERIAL_C.begin(baud);
#endif
#ifdef SERIAL_D
  if (isChannel(++channel)) SERIAL_D.begin(baud);
#endif
#ifdef SERIAL_ST4
  if (isChannel(++channel)) SERIAL_ST4.begin(baud);
#endif
}

void SerialWrapper::end() {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(++channel)) SERIAL_A.end();
#endif
#ifdef SERIAL_B
  if (isChannel(++channel)) SERIAL_B.end();
#endif
#ifdef SERIAL_C
  if (isChannel(++channel)) SERIAL_C.end();
#endif
#ifdef SERIAL_D
  if (isChannel(++channel)) SERIAL_D.end();
#endif
#ifdef SERIAL_ST4
  if (isChannel(++channel)) SERIAL_ST4.end();
#endif
}

size_t SerialWrapper::write(uint8_t data) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(++channel)) SERIAL_A.write(data);
#endif
#ifdef SERIAL_B
  if (isChannel(++channel)) SERIAL_B.write(data);
#endif
#ifdef SERIAL_C
  if (isChannel(++channel)) SERIAL_C.write(data);
#endif
#ifdef SERIAL_D
  if (isChannel(++channel)) SERIAL_D.write(data);
#endif
#ifdef SERIAL_ST4
  if (isChannel(++channel)) SERIAL_ST4.write(data);
#endif
}

size_t SerialWrapper::write(const uint8_t *data, size_t quantity) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(++channel)) SERIAL_A.write(data, quantity);
#endif
#ifdef SERIAL_B
  if (isChannel(++channel)) SERIAL_B.write(data, quantity);
#endif
#ifdef SERIAL_C
  if (isChannel(++channel)) SERIAL_C.write(data, quantity);
#endif
#ifdef SERIAL_D
  if (isChannel(++channel)) SERIAL_D.write(data, quantity);
#endif
#ifdef SERIAL_ST4
  if (isChannel(++channel)) SERIAL_ST4.write(data, quantity);
#endif
}

int SerialWrapper::available(void) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(++channel)) SERIAL_A.available();
#endif
#ifdef SERIAL_B
  if (isChannel(++channel)) SERIAL_B.available();
#endif
#ifdef SERIAL_C
  if (isChannel(++channel)) SERIAL_C.available();
#endif
#ifdef SERIAL_D
  if (isChannel(++channel)) SERIAL_D.available();
#endif
#ifdef SERIAL_ST4
  if (isChannel(++channel)) SERIAL_ST4.available();
#endif
}

int SerialWrapper::read(void) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(++channel)) SERIAL_A.read();
#endif
#ifdef SERIAL_B
  if (isChannel(++channel)) SERIAL_B.read();
#endif
#ifdef SERIAL_C
  if (isChannel(++channel)) SERIAL_C.read();
#endif
#ifdef SERIAL_D
  if (isChannel(++channel)) SERIAL_D.read();
#endif
#ifdef SERIAL_ST4
  if (isChannel(++channel)) SERIAL_ST4.read();
#endif
}

int SerialWrapper::peek(void) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(++channel)) SERIAL_A.peek();
#endif
#ifdef SERIAL_B
  if (isChannel(++channel)) SERIAL_B.peek();
#endif
#ifdef SERIAL_C
  if (isChannel(++channel)) SERIAL_C.peek();
#endif
#ifdef SERIAL_D
  if (isChannel(++channel)) SERIAL_D.peek();
#endif
#ifdef SERIAL_ST4
  if (isChannel(++channel)) SERIAL_ST4.peek();
#endif
}

void SerialWrapper::flush(void) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (channel == 1) SERIAL_A.flush();
#endif
#ifdef SERIAL_B
  if (channel == 2) SERIAL_B.flush();
#endif
#ifdef SERIAL_C
  if (channel == 3) SERIAL_C.flush();
#endif
#ifdef SERIAL_D
  if (channel == 4) SERIAL_D.flush();
#endif
#ifdef SERIAL_ST4
  if (channel == 4) SERIAL_ST4.flush();
#endif
}
