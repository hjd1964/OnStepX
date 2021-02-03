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
  if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
  channel++;
#endif
#ifdef SERIAL_B
  if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
  channel++;
#endif
#ifdef SERIAL_C
  if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
  channel++;
#endif
#ifdef SERIAL_D
  if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
  channel++;
#endif
#ifdef SERIAL_ST4
  if (!hasChannel(channel)) { thisChannel = channel; setChannel(channel); return; }
  channel++;
#endif
}

void SerialWrapper::begin() {
  begin(9600);
}

void SerialWrapper::begin(long baud) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(channel)) SERIAL_A.begin(baud);
  channel++;
#endif
#ifdef SERIAL_B
  if (isChannel(channel)) SERIAL_B.begin(baud);
  channel++;
#endif
#ifdef SERIAL_C
  if (isChannel(channel)) SERIAL_C.begin(baud);
  channel++;
#endif
#ifdef SERIAL_D
  if (isChannel(channel)) SERIAL_D.begin(baud);
  channel++;
#endif
#ifdef SERIAL_ST4
  if (isChannel(channel)) SERIAL_ST4.begin(baud);
  channel++;
#endif
}

void SerialWrapper::end() {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(channel)) SERIAL_A.end();
  channel++;
#endif
#ifdef SERIAL_B
  if (isChannel(channel)) SERIAL_B.end();
  channel++;
#endif
#ifdef SERIAL_C
  if (isChannel(channel)) SERIAL_C.end();
  channel++;
#endif
#ifdef SERIAL_D
  if (isChannel(channel)) SERIAL_D.end();
  channel++;
#endif
#ifdef SERIAL_ST4
  if (isChannel(channel)) SERIAL_ST4.end();
  channel++;
#endif
}

size_t SerialWrapper::write(uint8_t data) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(channel)) return SERIAL_A.write(data);
  channel++;
#endif
#ifdef SERIAL_B
  if (isChannel(channel)) return SERIAL_B.write(data);
  channel++;
#endif
#ifdef SERIAL_C
  if (isChannel(channel)) return SERIAL_C.write(data);
  channel++;
#endif
#ifdef SERIAL_D
  if (isChannel(channel)) return SERIAL_D.write(data);
  channel++;
#endif
#ifdef SERIAL_ST4
  if (isChannel(channel)) return SERIAL_ST4.write(data);
  channel++;
#endif
}

size_t SerialWrapper::write(const uint8_t *data, size_t quantity) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(channel)) return SERIAL_A.write(data, quantity);
  channel++;
#endif
#ifdef SERIAL_B
  if (isChannel(channel)) return SERIAL_B.write(data, quantity);
  channel++;
#endif
#ifdef SERIAL_C
  if (isChannel(channel)) return SERIAL_C.write(data, quantity);
  channel++;
#endif
#ifdef SERIAL_D
  if (isChannel(channel)) return SERIAL_D.write(data, quantity);
  channel++;
#endif
#ifdef SERIAL_ST4
  if (isChannel(channel)) return SERIAL_ST4.write(data, quantity);
  channel++;
#endif
}

int SerialWrapper::available(void) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(channel)) return SERIAL_A.available();
  channel++;
#endif
#ifdef SERIAL_B
  if (isChannel(channel)) return SERIAL_B.available();
  channel++;
#endif
#ifdef SERIAL_C
  if (isChannel(channel)) return SERIAL_C.available();
  channel++;
#endif
#ifdef SERIAL_D
  if (isChannel(channel)) return SERIAL_D.available();
  channel++;
#endif
#ifdef SERIAL_ST4
  if (isChannel(channel)) return SERIAL_ST4.available();
  channel++;
#endif
}

int SerialWrapper::read(void) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(channel)) return SERIAL_A.read();
  channel++;
#endif
#ifdef SERIAL_B
  if (isChannel(channel)) return SERIAL_B.read();
  channel++;
#endif
#ifdef SERIAL_C
  if (isChannel(channel)) return SERIAL_C.read();
  channel++;
#endif
#ifdef SERIAL_D
  if (isChannel(channel)) return SERIAL_D.read();
  channel++;
#endif
#ifdef SERIAL_ST4
  if (isChannel(channel)) return SERIAL_ST4.read();
  channel++;
#endif
}

int SerialWrapper::peek(void) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(channel)) return SERIAL_A.peek();
  channel++;
#endif
#ifdef SERIAL_B
  if (isChannel(channel)) return SERIAL_B.peek();
  channel++;
#endif
#ifdef SERIAL_C
  if (isChannel(channel)) return SERIAL_C.peek();
  channel++;
#endif
#ifdef SERIAL_D
  if (isChannel(channel)) return SERIAL_D.peek();
  channel++;
#endif
#ifdef SERIAL_ST4
  if (isChannel(channel)) return SERIAL_ST4.peek();
  channel++;
#endif
}

void SerialWrapper::flush(void) {
  uint8_t channel = 0;
#ifdef SERIAL_A
  if (isChannel(channel)) SERIAL_A.flush();
  channel++;
#endif
#ifdef SERIAL_B
  if (isChannel(channel)) SERIAL_B.flush();
  channel++;
#endif
#ifdef SERIAL_C
  if (isChannel(channel)) SERIAL_C.flush();
  channel++;
#endif
#ifdef SERIAL_D
  if (isChannel(channel)) SERIAL_D.flush();
  channel++;
#endif
#ifdef SERIAL_ST4
  if (isChannel(channel)) SERIAL_ST4.flush();
  channel++;
#endif
}
