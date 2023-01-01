// ------------------------------------------------------------------------------------
// Serial ST4 master

#include "Serial_ST4_Master.h"

#if defined(SERIAL_ST4_MASTER) && SERIAL_ST4_MASTER == ON

#if SST4_CLOCK_OUT != OFF && SST4_DATA_OUT != OFF && SST4_DATA_IN != OFF && SST4_TONE != OFF

#include "../Constants.h"
#include "../tasks/OnTask.h"

#include "Stream.h"

// SerialSt4 needs a minimum of 1500us between bytes transferred
#define LOOP_TIME 1600

// compensate for Mega2560 overhead to give about the same bitrate
#ifdef HAL_SLOW_PROCESSOR
  #define XMIT_TIME 20
#else
  #define XMIT_TIME 50
#endif

char SerialST4Master::poll() {
  char c = 0;
  if (trans(&c, xmit_buffer[xmit_head])) {
    // data going out was good?
    if (!send_error) {
      if (xmit_buffer[xmit_head] != (char)0) xmit_head++;
    }
    // data coming in was good?
    if (!recv_error) {
      if (c >= (char)32) {
        recv_buffer[recv_tail] = c;
        recv_tail++;
        recv_buffer[recv_tail] = (char)0;
        return (char)0;
      } else return c;
    } else return (char)0;
  } else return (char)0;
}

bool SerialST4Master::trans(char *data_in, uint8_t data_out) {
  uint8_t s_parity = 0;
  uint8_t r_parity = 0;

  // SHC_CLOCK HIGH for more than 1500us means that a pair of data bytes is done being exchanged
//  if ((long)(micros() - lastMicros) < LOOP_TIME) return false;

  // assume no errors
  frame_error = false;
  send_error = false;
  recv_error = false;

  // start bit
  digitalWriteF(SST4_CLOCK_OUT, LOW);
  digitalWriteF(SST4_DATA_OUT, LOW);
  // yield from the st4 task at priority level 1
  // lets motor s/w timers at priority level level 0 run
  tasks.yieldMicros(XMIT_TIME);
  digitalWriteF(SST4_CLOCK_OUT, HIGH);
  if (digitalReadF(SST4_DATA_IN) != LOW) frame_error = true; // recv start bit
  tasks.yieldMicros(XMIT_TIME);
  if (frame_error) {
    DLF("WRN: SerialST4.trans(), frame/start error");
  //  lastMicros = micros();
    return false;
  }

  for (int i = 7; i >= 0; i--) {
    uint8_t state = bitRead(data_out, i);
    s_parity += state;
    digitalWriteF(SST4_CLOCK_OUT, LOW);
    digitalWriteF(SST4_DATA_OUT, state);
    tasks.yieldMicros(XMIT_TIME);
    digitalWriteF(SST4_CLOCK_OUT, HIGH);
    state = digitalReadF(SST4_DATA_IN);
    r_parity += state;
    bitWrite(*data_in, i, state);                    
    tasks.yieldMicros(XMIT_TIME);
  }

  // parity bit
  digitalWriteF(SST4_CLOCK_OUT,LOW);
  digitalWriteF(SST4_DATA_OUT, s_parity&1);
  tasks.yieldMicros(XMIT_TIME);
  digitalWriteF(SST4_CLOCK_OUT, HIGH);
  if ((r_parity&1) != digitalReadF(SST4_DATA_IN)) recv_error = true;
  tasks.yieldMicros(XMIT_TIME);

  // parity ck bit
  digitalWriteF(SST4_CLOCK_OUT, LOW);
  digitalWriteF(SST4_DATA_OUT, recv_error);                  // send local parity check
  tasks.yieldMicros(XMIT_TIME);
  digitalWriteF(SST4_CLOCK_OUT, HIGH);
  if (digitalReadF(SST4_DATA_IN) == HIGH) send_error = true; // recv remote parity, ok?
  tasks.yieldMicros(XMIT_TIME);

  // stop bit
  digitalWriteF(SST4_CLOCK_OUT, LOW);
  digitalWriteF(SST4_DATA_OUT, LOW);
  tasks.yieldMicros(XMIT_TIME);
  digitalWriteF(SST4_CLOCK_OUT, HIGH);
  if (digitalReadF(SST4_DATA_IN) != LOW) frame_error = true; // recv stop bit
  tasks.yieldMicros(XMIT_TIME);

  if (frame_error) { DLF("WRN: SerialST4.trans(), frame/stop error"); }
  if (send_error) { DLF("WRN: SerialST4.trans(), send parity error"); }
  if (recv_error) { DLF("WRN: SerialST4.trans(), recv parity error"); }

 // lastMicros = micros();
  if (frame_error) return false; else return true;
}

void SerialST4Master::begin() {
  xmit_head = 0; xmit_tail = 0; xmit_buffer[0] = 0;
  recv_head = 0; recv_tail = 0; recv_buffer[0] = 0;
  lastMicros = micros();
}

void SerialST4Master::begin(long baud) {
  begin();
  (void)(baud);
}

void SerialST4Master::end() {
  xmit_head = 0; xmit_tail = 0; xmit_buffer[0] = 0;
  recv_head = 0; recv_tail = 0; recv_buffer[0] = 0;
}

size_t SerialST4Master::write(uint8_t data) {
  unsigned long t_start = millis();
  uint8_t xh = xmit_head;
  xh--;
  while (xmit_tail == xh) {
  //  poll();
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
//    poll();
    Y; // yield from the command channel at priority level 5 (lets poll() run)
    c = xmit_buffer[xmit_head];
  } while (c != 0 || (millis() - startMs < timeout));
}

SerialST4Master serialST4;

#endif

#endif
