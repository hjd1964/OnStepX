// ------------------------------------------------------------------------------------
// Serial ST4 master

#include "Serial_ST4_Master.h"

#if defined(SERIAL_ST4_MASTER) && SERIAL_ST4_MASTER == ON

#include "../Constants.h"
#include "../tasks/OnTask.h"

#include "Stream.h"

#define ST4_CLOCK_OUT_PIN ST4_DEC_S_PIN  // DE-
#define ST4_DATA_OUT_PIN  ST4_DEC_N_PIN  // DE+
#define ST4_DATA_IN_PIN   ST4_RA_W_PIN   // RA-
#define ST4_TONE_PIN      ST4_RA_E_PIN   // RA+

// interval in microseconds for a clock timeout
#define ST4_MAX_BIT_TIME 10000

char SerialST4Master::poll() {
  char c = 0;
  if (trans(&c)) {
    if (!recv_error) {
      if (c >= (char)32) {
        recv_buffer[recv_tail] = c;
        recv_buffer[++recv_tail] = (char)0;
        return (char)0;
      } else return c;
    } else return (char)0;
  } else return (char)0;
}

bool SerialST4Master::trans(char *data_in) {
  static bool reset = false;
  static unsigned long resetFinishTime = 0;

  static int index = 18;

  uint8_t this_bit;

  static uint8_t this_data_in;
  static uint8_t r_parity;
  static uint8_t this_data_out;
  static uint8_t s_parity;

  // force a reset of ST4 comms
  if (reset) {
    if ((long)(millis() - resetFinishTime) > 0) reset = false; else return false;
    index = 18;
  }

  switch (--index) {
    case 17: // send start bit

      if (!send_error) {
        if (xmit_buffer[xmit_head] != (char)0) {
          this_data_out = xmit_buffer[xmit_head++];
        } else this_data_out = (char)0;
      }

      s_parity = 0;
      r_parity = 0;
      frame_error = false;
      send_error = false;
      recv_error = false;
      digitalWriteF(ST4_CLOCK_OUT_PIN, LOW);
      digitalWriteF(ST4_DATA_OUT_PIN, LOW);
    break;
    case 16: // recv start bit
      digitalWriteF(ST4_CLOCK_OUT_PIN, HIGH);
      if (digitalReadF(ST4_DATA_IN_PIN) != LOW) {
        DLF("WRN: SerialST4.poll(), frame/start error");
        frame_error = true;
        resetFinishTime = millis() + 1000;
        reset = true;
        return false;
      }
    break;
    case 15: case 13: case 11: case 9: case 7: case 5: case 3: case 1: // send data bits
      this_bit = bitRead(this_data_out, index>>1);
      s_parity += this_bit;
      digitalWriteF(ST4_CLOCK_OUT_PIN, LOW);
      digitalWriteF(ST4_DATA_OUT_PIN, this_bit);
    break;
    case 14: case 12: case 10: case 8: case 6: case 4: case 2: case 0: // recv data bits
      digitalWriteF(ST4_CLOCK_OUT_PIN, HIGH);
      this_bit = digitalReadF(ST4_DATA_IN_PIN);
      r_parity += this_bit;
      bitWrite(this_data_in, index>>1, this_bit);                    
    break;
    case -1: // send parity bit
      digitalWriteF(ST4_CLOCK_OUT_PIN, LOW);
      digitalWriteF(ST4_DATA_OUT_PIN, s_parity&1);
    break;
    case -2: // recv parity bit
      digitalWriteF(ST4_CLOCK_OUT_PIN, HIGH);
      if ((r_parity&1) != digitalReadF(ST4_DATA_IN_PIN)) recv_error = true;
    break;
    case -3: // send local parity ck bit
      digitalWriteF(ST4_CLOCK_OUT_PIN, LOW);
      digitalWriteF(ST4_DATA_OUT_PIN, recv_error);
    break;
    case -4: // recv remote parity ck bit
      digitalWriteF(ST4_CLOCK_OUT_PIN, HIGH);
      if (digitalReadF(ST4_DATA_IN_PIN) == HIGH) send_error = true;
    break;
    case -5: // send stop bit
      digitalWriteF(ST4_CLOCK_OUT_PIN, LOW);
      digitalWriteF(ST4_DATA_OUT_PIN, LOW);
    break;
    case -6: // recv stop bit
      digitalWriteF(ST4_CLOCK_OUT_PIN, HIGH);
      if (digitalReadF(ST4_DATA_IN_PIN) != LOW) frame_error = true;

      if (frame_error) {
        DLF("WRN: SerialST4.poll(), frame/stop error");
        resetFinishTime = millis() + ST4_MAX_BIT_TIME/1000 + 1;
        reset = true;
        return false;
      }

//      if (this_data_in >= 32 || this_data_out >= 32) {
//        D("Got "); if (this_data_in >= 32) { D((char)this_data_in); } else D(" ");
//        D(" Sent "); if (this_data_out >= 32) { DL((char)this_data_out); } else DL(" ");
//      }

      *data_in = this_data_in;

      if (send_error) { DLF("WRN: SerialST4.poll(), send parity error"); }
      if (recv_error) { DLF("WRN: SerialST4.poll(), recv parity error"); }

      index = 18;
      return true;
    break;
  }

  return false;
}

void SerialST4Master::begin() {
  xmit_head = 0;
  xmit_tail = 0;
  xmit_buffer[0] = 0;
  recv_head = 0;
  recv_tail = 0;
  recv_buffer[0] = 0;
}

void SerialST4Master::begin(long baud) {
  begin();
  (void)(baud);
}

void SerialST4Master::end() {
  xmit_head = 0;
  xmit_tail = 0;
  xmit_buffer[0] = 0;
  recv_head = 0;
  recv_tail = 0;
  recv_buffer[0] = 0;
}

size_t SerialST4Master::write(uint8_t data) {
  unsigned long t_start = millis();
  uint8_t xh = xmit_head;
  xh--;
  while (xmit_tail == xh) {
    Y; // yield from the command channel at priority level 5 (lets poll() run)
    if ((millis() - t_start) > timeout) return 0;
  }
  xmit_buffer[xmit_tail] = data; xmit_tail++;
  xmit_buffer[xmit_tail] = 0;
  return 1;
}

size_t SerialST4Master::write(const uint8_t *data, size_t quantity) {
  // fail if trying to write more than the buffer can hold
  if ((int)quantity > 254) return 0;

  for (int i = 0; i < (int)quantity; i++) { if (!write(data[i])) return 0; }
  return 1;
}

int SerialST4Master::available(void) {
  int a=0;
  for (uint8_t b = recv_head; recv_buffer[b] != (char)0; b++) a++;
  return a;
}

int SerialST4Master::read(void) {
  char c = recv_buffer[recv_head]; if (c != 0) recv_head++;
  if (c == 0) c = -1;
  return c;
}

int SerialST4Master::peek(void) {
  int c = recv_buffer[recv_head];
  if (c == 0) c= -1;

  return c;
}

void SerialST4Master::flush(void) {
  unsigned long startMs = millis();
  int c;
  do {
    Y; // yield from the command channel at priority level 5 (lets poll() run)
    c = xmit_buffer[xmit_head];
  } while (c != 0 || (millis() - startMs < timeout));
}

SerialST4Master serialST4;

#endif
