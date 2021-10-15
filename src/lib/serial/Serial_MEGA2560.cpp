// -----------------------------------------------------------------------------------
// Low overhead communication routines for Serial0, Serial1

#include "Serial_MEGA2560.h"

#if defined(SERIAL_MEGA2560) && SERIAL_MEGA2560 == ON

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

#define MSB(i) (i >> 8)
#define LSB(i) (i & 0xFF)

#ifdef HAL_POLLING_MEGA2560_SERIAL_A

void PollingSerialA::begin(long baud) {
  // init the buffers
  xmit_index = 0; xmit_buffer[0] = 0; recv_head = 0; recv_tail = 0; recv_buffer[0] = 0;

  // Set baud rate
  uint16_t ubrr = F_CPU/16/baud - 1;
  UBRR0H = MSB(ubrr);
  UBRR0L = LSB(ubrr);

  // Disable U2X mode
  UCSR0A = 0;

  // Enable receiver and transmitter
  UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);

  // 8-bit, 1 stop bit, no parity, asynchronous UART
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) | (0 << USBS0)   |
            (0 << UPM01)  | (0 << UPM00)  | (0 << UMSEL01) |
            (0 << UMSEL00);
}

void PollingSerialA::end() { UCSR0B = 0; }

int PollingSerialA::read(void) {
  if (!available()) return -1;
  noInterrupts();
  recv_buffer[recv_tail] = 0;
  char c = recv_buffer[recv_head];
  interrupts();
  if (c) recv_head++;
  return c;
}

bool PollingSerialA::poll(void) {
  if (xmit_buffer[xmit_index] == 0) return false;
  if ( ( UCSR0A & (1<<UDRE0)) ) { UDR0 = xmit_buffer[xmit_index]; xmit_index++; xmit_index &= 0b111111; }
  return true;
}

PollingSerialA SerialA;

// UART receive complete interrupt handler for Serial0
ISR(USART0_RX_vect)  {
  SerialA.recv_buffer[SerialA.recv_tail] = UDR0; 
  SerialA.recv_tail++;
}

#endif

#ifdef HAL_POLLING_MEGA2560_SERIAL_B

void PollingSerialB::begin(long baud) {
  // init the buffers
  xmit_index = 0; xmit_buffer[0] = 0; recv_head = 0; recv_tail = 0; recv_buffer[0] = 0;

  // Set baud rate
  uint16_t ubrr = F_CPU/16/baud - 1;
  UBRR1H = MSB(ubrr);
  UBRR1L = LSB(ubrr);

  // Disable U2X mode
  UCSR1A = 0;

  // Enable receiver and transmitter
  UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1);

  // 8-bit, 1 stop bit, no parity, asynchronous UART
  UCSR1C = (1 <<  UCSZ11) | (1 << UCSZ10) | (0 <<  USBS1 ) |
           (0 <<   UPM11) | (0 <<  UPM10) | (0 << UMSEL11) |
           (0 << UMSEL10);
}

void PollingSerialB::end() { UCSR1B = 0; }

int PollingSerialB::read(void) {
  if (!available()) return -1;
  noInterrupts();
  recv_buffer[recv_tail] = 0;
  char c = recv_buffer[recv_head];
  interrupts();
  if (c) recv_head++;
  return c;
}

bool PollingSerialB::poll(void) {
  if (xmit_buffer[xmit_index] == 0) return false;
  if ( ( UCSR1A & (1<<UDRE1)) ) { UDR1 = xmit_buffer[xmit_index]; xmit_index++; }
  return true;
}

PollingSerialB SerialB;

// UART receive complete interrupt handler for Serial0
ISR(USART1_RX_vect)  {
  SerialB.recv_buffer[SerialB.recv_tail] = UDR1; 
  SerialB.recv_tail++; 
}

#endif

#ifdef HAL_POLLING_MEGA2560_SERIAL_C

void PollingSerialC::begin(long baud) {
  // init the buffers
  xmit_index = 0; xmit_buffer[0] = 0; recv_head = 0; recv_tail = 0; recv_buffer[0] = 0;

  // Set baud rate
  uint16_t ubrr = F_CPU/16/baud - 1;
  UBRR2H = MSB(ubrr);
  UBRR2L = LSB(ubrr);

  // Disable U2X mode
  UCSR1A = 0;

  // Enable receiver and transmitter
  UCSR2B = (1<<RXEN2) | (1<<TXEN2) | (1<<RXCIE2);

  // 8-bit, 1 stop bit, no parity, asynchronous UART
  UCSR2C = (1 <<  UCSZ21) | (1 << UCSZ20) | (0 <<  USBS2 ) |
           (0 <<   UPM21) | (0 <<  UPM20) | (0 << UMSEL21) |
           (0 << UMSEL20);
}

void PollingSerialC::end() { UCSR2B = 0; }

int PollingSerialC::read(void) {
  if (!available()) return -1;
  noInterrupts();
  recv_buffer[recv_tail] = 0;
  char c = recv_buffer[recv_head];
  interrupts();
  if (c) recv_head++;
  return c;
}

bool PollingSerialC::poll(void) {
  if (xmit_buffer[xmit_index] == 0) return false;
  if ( ( UCSR2A & (1<<UDRE2)) ) { UDR2 = xmit_buffer[xmit_index]; xmit_index++; }
  return true;
}

PollingSerialC SerialC;

// UART receive complete interrupt handler for Serial0
ISR(USART2_RX_vect)  {
  SerialC.recv_buffer[SerialC.recv_tail] = UDR2; 
  SerialC.recv_tail++; 
}

#endif

#ifdef HAL_POLLING_MEGA2560_SERIAL_D

void PollingSerialD::begin(long baud) {
  // init the buffers
  xmit_index = 0; xmit_buffer[0] = 0; recv_head = 0; recv_tail = 0; recv_buffer[0] = 0;

  // Set baud rate
  uint16_t ubrr = F_CPU/16/baud - 1;
  UBRR3H = MSB(ubrr);
  UBRR3L = LSB(ubrr);

  // Disable U2X mode
  UCSR3A = 0;

  // Enable receiver and transmitter
  UCSR3B = (1<<RXEN3) | (1<<TXEN3) | (1<<RXCIE3);

  // 8-bit, 1 stop bit, no parity, asynchronous UART
  UCSR3C = (1 <<  UCSZ31) | (1 << UCSZ30) | (0 <<  USBS3 ) |
           (0 <<   UPM31) | (0 <<  UPM30) | (0 << UMSEL31) |
           (0 << UMSEL30);
}

void PollingSerialD::end() { UCSR3B = 0; }

int PollingSerialD::read(void) {
  if (!available()) return -1;
  noInterrupts();
  recv_buffer[recv_tail] = 0;
  char c = recv_buffer[recv_head];
  interrupts();
  if (c) recv_head++;
  return c;
}

bool PollingSerialD::poll(void) {
  if (xmit_buffer[xmit_index] == 0) return false;
  if ( ( UCSR3A & (1<<UDRE3)) ) { UDR3 = xmit_buffer[xmit_index]; xmit_index++; }
  return true;
}

PollingSerialD SerialD;

// UART receive complete interrupt handler for Serial0
ISR(USART3_RX_vect)  {
  SerialD.recv_buffer[SerialD.recv_tail] = UDR3; 
  SerialD.recv_tail++; 
}

#endif

#endif

#endif
