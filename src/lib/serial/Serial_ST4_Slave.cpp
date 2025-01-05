// -----------------------------------------------------------------------------------
// Serial ST4 slave

/*
ST4 port data communication scheme:

5V power ---- Teensy3.2, Nano, etc.
Gnd ---------

HC              Signal               OnStep
RAw  ----        Data         --->   Recv. data
DEs  <---        Clock        ----   Clock
DEn  <---        Data         ----   Send data
RAe  ---- 12.5Hz Square wave  --->   100% sure SHC is present, switches DEs & DEn to OUTPUT

Data is exchanged on clock edges similar to SPI so is timing insensitive (runs with interrupts enabled.)

One data byte is exchanged (in both directions w/basic error detection and recovery.)  A value 0x00 byte 
means "no data" and is ignored on both sides.  Mega2560 hardware runs at (fastest) 10mS/byte (100 Bps) and 
all others (Teensy3.x, etc.) at 2mS/byte (500 Bps.)
*/

#include "Serial_ST4_Slave.h"

#if defined(SERIAL_ST4_SLAVE) && SERIAL_ST4_SLAVE == ON

#include "../tasks/OnTask.h"

#define INTERVAL 40000 // microseconds

void Sst4::begin(long baudRate = 9600) {
  if (isActive) return;

  xmit_head = 0; xmit_tail = 0; xmit_buffer[0] = 0;
  recv_head = 0; recv_tail = 0; recv_buffer[0] = 0;
  
  VF("MSG: SerialST4Slave, start shcTone task (rate 40ms priority 0)... ");
  handle = tasks.add(0, 0, true, 0, shcTone, "tone");
  if (handle) {
    pinMode(ST4_S_PIN, INPUT_PULLUP);
    pinMode(ST4_N_PIN, INPUT_PULLUP);
    pinMode(ST4_E_PIN, OUTPUT);
    pinMode(ST4_W_PIN, OUTPUT);

    if (!tasks.requestHardwareTimer(handle, 0)) { VLF("(no hardware timer!)"); } else { VLF("success"); }
    tasks.setPeriodMicros(handle, INTERVAL);

    attachInterrupt(digitalPinToInterrupt(ST4_S_PIN), dataClock, CHANGE);

    isActive = true;
  } else {
    VLF("FAILED!");
  }
}

void Sst4::end() {
  if (!isActive) return;

  pinMode(ST4_E_PIN, INPUT_PULLUP);
  pinMode(ST4_W_PIN, INPUT_PULLUP);

  detachInterrupt(digitalPinToInterrupt(ST4_S_PIN));

  tasks.remove(handle);
  VLF("MSG: SerialST4Slave, stopped shcTone task");

  xmit_head = 0; xmit_tail = 0; xmit_buffer[0] = 0;
  recv_head = 0; recv_tail = 0; recv_buffer[0] = 0;

  isActive = false;
}

void Sst4::paused(bool state) {
  if (!isActive) return;

  if (state) tasks.setPeriodMicros(handle, INTERVAL); else tasks.setPeriod(handle, 0);
}

bool Sst4::active() {
  if (!isActive) return false;

  static unsigned long comp=0;
  bool result = false;
  noInterrupts();
  if (comp != lastMs) { result = true; comp = lastMs; }
  interrupts();
  return result;
}

size_t Sst4::write(uint8_t data) {
  if (!isActive) return 0;

  // wait for room in buffer to become available or give up
  unsigned long t_start = millis();
  uint8_t xh = xmit_head; xh--; while (xmit_tail == xh) { if ((millis() - t_start) > _timeout) return 0; }

  // is this a control code command?  is the buffer not empty?
  if (data > 0 && data < 32 && xmit_buffer[xmit_head] != 0) {
    noInterrupts();
    // insert the command into the buffer
    uint8_t hd = xmit_head;
    uint8_t hs = xmit_head; hs--;
    for (int i = 0; i < 254; i++) {  hs--; hd--; xmit_buffer[hd] = xmit_buffer[hs]; if (xmit_buffer[hs] == 0) break; }
    xmit_head++; xmit_buffer[xmit_head] = data; xmit_head--;
    xmit_tail++; xmit_buffer[xmit_tail] = 0;
    interrupts();
  } else {
    noInterrupts();
    xmit_buffer[xmit_tail] = data; xmit_tail++;
    xmit_buffer[xmit_tail] = 0;
    interrupts();
  }
  return 1;
}

size_t Sst4::write(const uint8_t *data, size_t quantity) {
  if (!isActive) return 0;

  // fail if trying to write more than the buffer can hold
  if ((int)quantity > 254) return 0;

  for (int i = 0; i < (int)quantity; i++) { if (!write(data[i])) return 0; }
  return 1;
}

int Sst4::available(void) {
  if (!isActive) return 0;

  int a = 0;
  noInterrupts();
  for (uint8_t b = recv_head; recv_buffer[b] != (char)0; b++) a++;
  interrupts();

  return a;
}

int Sst4::read(void) {
  if (!isActive) return -1;

  noInterrupts();
  int c = recv_buffer[recv_head]; if (c != 0) recv_head++;
  interrupts();
  if (c == 0) c = -1;

  return c;
}

int Sst4::peek(void) {
  if (!isActive) return -1;

  noInterrupts();
  int c = recv_buffer[recv_head];
  interrupts();
  if (c == 0) c = -1;

  return c;
}

void Sst4::flush(void) {
  if (!isActive) return;

  unsigned long startMs = millis();
  int c;
  do {
    noInterrupts();
    c = xmit_buffer[xmit_head];
    interrupts();
  } while (c != 0 && (millis()-startMs) < _timeout);
}

volatile uint8_t data_in = 0;
volatile uint8_t data_out = 0;

IRAM_ATTR void dataClock() {
  static volatile unsigned long t = 0;
  static volatile int i = 9;
  static volatile bool frame_error = false;
  static volatile bool send_error = false;
  static volatile bool recv_error = false;
  static volatile uint8_t s_parity = 0;
  static volatile uint8_t r_parity = 0;
  volatile uint8_t state = 0;
  
  SerialST4.lastMs = millis();
  unsigned long t1 = t;
  t = micros();
  volatile unsigned long elapsed = t - t1;

  if (digitalRead(ST4_S_PIN) == HIGH) {
    state = digitalRead(ST4_N_PIN); 
    if (i == 8) { if (state != LOW) frame_error = true; }                     // recv start bit
    if (i >= 0 && i <= 7) { r_parity += state; bitWrite(data_in, i, state); } // recv data bit
    if (i == -1) { if ((r_parity&1)!=state) recv_error=true; }                // recv parity bit
    if (i == -2) { if (state==HIGH) send_error=true; }                        // recv remote parity, ok?
    if (i == -3) {                                                            // recv stop bit
      if (state != LOW) frame_error = true;

      if (!frame_error && !recv_error) {
        if (data_in != 0) {
          SerialST4.recv_buffer[SerialST4.recv_tail] = (char)data_in; 
          SerialST4.recv_tail++;
        }
        SerialST4.recv_buffer[SerialST4.recv_tail] = (char)0;
      }
    }
  } else {
    if (elapsed > 1500L || i == -3) {
      i = 9;
      r_parity = 0;
      s_parity = 0;
      recv_error = false;

      // send the same data again?
      if (!send_error && !frame_error) {
        data_out = SerialST4.xmit_buffer[SerialST4.xmit_head]; 
        if (data_out != 0) SerialST4.xmit_head++;
      } else { send_error = false; frame_error = false; }
    }
    i--;
    if (i == 8) { digitalWrite(ST4_W_PIN, LOW); }                  // send start bit
    if (i >= 0 && i <= 7) {                                        // send data bit
      state = bitRead(data_out, i); s_parity += state; digitalWrite(ST4_W_PIN, state);
    }
    if (i == -1) { digitalWrite(ST4_W_PIN, s_parity&1); }          // send parity bit
    if (i == -2) { digitalWrite(ST4_W_PIN, recv_error); }          // send local parity check
    if (i == -3) { digitalWrite(ST4_W_PIN, LOW); }                 // send stop bit
  }
}

// this routine keeps a 12.5Hz "tone" on the RAe pin (always) but also on the
// RAw pin when the data comms clock from OnStep isn't running
IRAM_ATTR void shcTone() {
  static volatile bool tone_state = false;

  if (tone_state) { 
    tone_state = false; 
    digitalWrite(ST4_E_PIN, HIGH); 
    if (millis() - SerialST4.lastMs > 2000L) {
      digitalWrite(ST4_W_PIN, HIGH);
    }
  } else  {
    tone_state = true;
    digitalWrite(ST4_E_PIN, LOW);
    if (millis() - SerialST4.lastMs > 2000L) {
      digitalWrite(ST4_W_PIN, LOW); 
    }
  }
}

Sst4 SerialST4;

#endif
