// -----------------------------------------------------------------------------------
// Serial ST4 slave

#include "Serial_ST4_Slave.h"

#if defined(SERIAL_ST4_SLAVE) && SERIAL_ST4_SLAVE == ON

#include "../tasks/OnTask.h"

// interval in milliseconds for generating the tone signal
#define ST4_TONE_INTERVAL 40

// interval in milliseconds for a clock timeout
#define ST4_MAX_BIT_TIME 100

#define ST4_CLOCK_PIN ST4_S_PIN
#define ST4_DATA_IN_PIN ST4_N_PIN
#define ST4_DATA_OUT_PIN ST4_W_PIN
#define ST4_TONE_PIN ST4_E_PIN

IRAM_ATTR void dataClock() { SerialST4.poll(); }

void SerialST4Slave::begin(long baudRate = 9600) {
  if (isActive) return;

  xmit_head = 0;
  xmit_tail = 0;
  xmit_buffer[0] = 0;
  recv_head = 0;
  recv_tail = 0;
  recv_buffer[0] = 0;

  VF("MSG: SerialST4Slave, start shcTone task (rate 40ms priority 0)... ");
  handle = tasks.add(0, 0, true, 0, shcTone, "tone");
  if (handle) {
    pinMode(ST4_CLOCK_PIN, INPUT_PULLUP);
    pinMode(ST4_DATA_IN_PIN, INPUT_PULLUP);
    pinMode(ST4_TONE_PIN, OUTPUT);
    pinMode(ST4_DATA_OUT_PIN, OUTPUT);

    if (!tasks.requestHardwareTimer(handle, 0)) { VLF("(no hardware timer!)"); } else { VLF("success"); }
    tasks.setPeriod(handle, ST4_TONE_INTERVAL);

    attachInterrupt(digitalPinToInterrupt(ST4_CLOCK_PIN), dataClock, CHANGE);

    isActive = true;
  } else {
    VLF("FAILED!");
  }
}

void SerialST4Slave::end() {
  if (!isActive) return;

  pinMode(ST4_TONE_PIN, INPUT_PULLUP);
  pinMode(ST4_DATA_OUT_PIN, INPUT_PULLUP);

  detachInterrupt(digitalPinToInterrupt(ST4_CLOCK_PIN));

  tasks.remove(handle);
  VLF("MSG: SerialST4Slave, stopped shcTone task");

  xmit_head = 0;
  xmit_tail = 0;
  xmit_buffer[0] = 0;
  recv_head = 0;
  recv_tail = 0;
  recv_buffer[0] = 0;

  isActive = false;
}

void SerialST4Slave::paused(bool state) {
  if (!isActive) return;

  if (state) tasks.setPeriod(handle, ST4_TONE_INTERVAL); else tasks.setPeriod(handle, 0);
}

bool SerialST4Slave::active() {
  if (!isActive) return false;

  static unsigned long comp = 0;
  bool result = false;
  noInterrupts();
  if (comp != lastTimeInMilliseconds) {
    result = true;
    comp = lastTimeInMilliseconds;
  }
  interrupts();
  return result;
}

size_t SerialST4Slave::write(uint8_t data) {
  if (!isActive) return 0;

  // wait for room in buffer to become available or give up
  unsigned long t_start = millis();
  
  uint8_t xh = xmit_head;
  xh--;
  while (xmit_tail == xh) {
    if ((millis() - t_start) > _timeout) return 0;
  }

  // is this a control code command?  is the buffer not empty?
  if (data > 0 && data < 32 && xmit_buffer[xmit_head] != 0) {
    noInterrupts();
    // insert the command into the buffer
    uint8_t hd = xmit_head;
    uint8_t hs = xmit_head;
    hs--;
    for (int i = 0; i < 254; i++) {
      xmit_buffer[--hd] = xmit_buffer[--hs];
      if (xmit_buffer[hs] == 0) break;
    }
    xmit_head++;
    xmit_buffer[xmit_head] = data;
    xmit_head--;
    xmit_buffer[++xmit_tail] = 0;
    interrupts();
  } else {
    noInterrupts();
    xmit_buffer[xmit_tail] = data;
    xmit_buffer[++xmit_tail] = 0;
    interrupts();
  }
  return 1;
}

size_t SerialST4Slave::write(const uint8_t *data, size_t quantity) {
  if (!isActive) return 0;

  // fail if trying to write more than the buffer can hold
  if ((int)quantity > 254) return 0;

  for (int i = 0; i < (int)quantity; i++) { if (!write(data[i])) return 0; }
  return 1;
}

int SerialST4Slave::available(void) {
  if (!isActive) return 0;

  int a = 0;
  noInterrupts();
  for (uint8_t b = recv_head; recv_buffer[b] != (char)0; b++) a++;
  interrupts();

  return a;
}

int SerialST4Slave::read(void) {
  if (!isActive) return -1;

  noInterrupts();
  int c = recv_buffer[recv_head]; if (c != 0) recv_head++;
  interrupts();
  if (c == 0) c = -1;

  return c;
}

int SerialST4Slave::peek(void) {
  if (!isActive) return -1;

  noInterrupts();
  int c = recv_buffer[recv_head];
  interrupts();
  if (c == 0) c = -1;

  return c;
}

void SerialST4Slave::flush(void) {
  if (!isActive) return;

  unsigned long startMs = millis();
  int c;
  do {
    noInterrupts();
    c = xmit_buffer[xmit_head];
    interrupts();
  } while (c != 0 && (millis()-startMs) < _timeout);
}

IRAM_ATTR void SerialST4Slave::poll() {
  uint8_t clockState = digitalRead(ST4_CLOCK_PIN);

  static int index = 9;

  static bool frame_error = false;
  static bool send_error = false;
  static bool recv_error = false;
  static uint8_t s_parity = 0;
  static uint8_t r_parity = 0;
  uint8_t this_bit;

  unsigned long timeInMilliseconds = millis();
  if ((long)((timeInMilliseconds - lastTimeInMilliseconds) - (unsigned long)ST4_MAX_BIT_TIME) > 0) {
    DLF("WRN: SerialST4.poll(), timeout error");
    index = 9;
  }
  lastTimeInMilliseconds = timeInMilliseconds;

  if (clockState == LOW) { if (--index == -4) index = 8; }

  switch(index*2 + (int)(clockState == LOW)) {
    case 17: // setup for another frame and send start bit (clock LOW)

      if (!send_error) {
        data_out = SerialST4.xmit_buffer[SerialST4.xmit_head]; 
        if (data_out != 0) SerialST4.xmit_head++;
      } else { DLF("WRN: SerialST4.poll(), send parity error"); }

      r_parity = 0;
      s_parity = 0;
      recv_error = false;
      send_error = false;
      frame_error = false;
      digitalWrite(ST4_DATA_OUT_PIN, LOW);
    break;
    case 16: // recv start bit (clock HIGH)
      if (digitalRead(ST4_DATA_IN_PIN) != LOW) {
        frame_error = true;
        index = 9;
        DLF("WRN: SerialST4.poll(), frame/start error");
      }
    break;
    case 15: case 13: case 11: case 9: case 7: case 5: case 3: case 1: // send data bit (clock LOW)
      this_bit = bitRead(data_out, index);
      s_parity += this_bit;
      digitalWrite(ST4_DATA_OUT_PIN, this_bit);
    break;
    case 14: case 12: case 10: case 8: case 6: case 4: case 2: case 0: // recv data bit (clock HIGH)
      this_bit = digitalRead(ST4_DATA_IN_PIN);
      r_parity += this_bit;
      bitWrite(data_in, index, this_bit);
    break;
    case -1: // send parity bit (clock LOW)
      digitalWrite(ST4_DATA_OUT_PIN, s_parity&1);
    break;
    case -2: // recv parity bit (clock HIGH)
      if ((r_parity&1) != digitalRead(ST4_DATA_IN_PIN)) recv_error = true;
    break;
    case -3: // send local parity check (clock LOW)
      digitalWrite(ST4_DATA_OUT_PIN, recv_error);
    break;
    case -4: // recv remote parity (clock HIGH)
      if (digitalRead(ST4_DATA_IN_PIN) == HIGH) send_error = true;
    break;
    case -5: // send stop bit (clock LOW)
      digitalWrite(ST4_DATA_OUT_PIN, LOW);
    break;
    case -6: // recv stop bit (clock HIGH)
      if (digitalRead(ST4_DATA_IN_PIN) != LOW) frame_error = true;

      if (!frame_error) {

//        if (data_in >= 32 || data_out >= 32) {
//          D("Got "); if (data_in >= 32) { D((char)data_in); } else D(" ");
//          D(" Sent "); if (data_out >= 32) { DL((char)data_out); } else DL(" ");
//        }

        if (!recv_error) {
          if (data_in != 0) {
            SerialST4.recv_buffer[SerialST4.recv_tail] = (char)data_in; 
            SerialST4.recv_buffer[++SerialST4.recv_tail] = (char)0;
          }
        } else { DLF("WRN: SerialST4.poll(), recv parity error"); }

      } else { DLF("WRN: SerialST4.poll(), frame/stop error"); }

    break;
  }
}

// this routine keeps a 12.5Hz "tone" on the RAe pin (always) but also on the
// RAw pin when the data comms clock from OnStep isn't running
IRAM_ATTR void shcTone() {
  static volatile bool tone_state = false;

  if (tone_state) { 
    tone_state = false;
    digitalWrite(ST4_TONE_PIN, HIGH); 
    if ((long)(millis() - SerialST4.lastTimeInMilliseconds) > 2000L) {
      digitalWrite(ST4_DATA_OUT_PIN, HIGH);
    }
  } else  {
    tone_state = true;
    digitalWrite(ST4_TONE_PIN, LOW);
    if ((long)(millis() - SerialST4.lastTimeInMilliseconds) > 2000L) {
      digitalWrite(ST4_DATA_OUT_PIN, LOW); 
    }
  }
}

SerialST4Slave SerialST4;

#endif
