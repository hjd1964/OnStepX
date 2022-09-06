// serial channel encoder

#include "SerialEncoder.h"

#include "../../../../tasks/OnTask.h"

#ifdef SERIAL_ENCODER
  Encoder::Encoder(int16_t channel) {
    if (channel == 1) this->channel[0] = '1'; else
    if (channel == 2) this->channel[0] = '2';
  }

  int32_t Encoder::read() {
    if (millis() - lastRead > 20) {
      position = raw();
      lastRead = millis();
    }
    return position + offset;
  }

  void Encoder::write(int32_t position) {
    offset = position - raw();
  }  

  int32_t Encoder::raw() {
    if (!initialized) {

      #if defined(SERIAL_ENCODER_RX) && defined(SERIAL_ENCODER_TX) && !defined(SERIAL_ENCODER_RXTX_SET)
        SERIAL_ENCODER.begin(SERIAL_ENCODER_BAUD_DEFAULT, SERIAL_8N1, SERIAL_ENCODER_RX, SERIAL_ENCODER_TX);
      #else
        SERIAL_ENCODER.begin(SERIAL_ENCODER_BAUD_DEFAULT);
      #endif

      delay(100);
      initialized = true;
    }

    SERIAL_ENCODER.print(channel);
    
    char c;
    char result[32] = "";
    int index = 0;
    unsigned long start = millis();
    do {
      if (SERIAL_ENCODER.available()) c = SERIAL_ENCODER.read(); else c = 'x';
      if ((c >= '0' && c <= '9') || c == '-') {
        result[index++] = c;
        result[index] = 0;
      }
    } while (c != 13 && (millis() - start) < 4 && index < 16);

    if (strlen(result) > 0) {
      return atoi(result);
    } else {
      VLF("MSG: SerialEncoder, timed out!");
      return 0;
    }
  } 
#endif
