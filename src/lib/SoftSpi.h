// -----------------------------------------------------------------------------------
// Simple software SPI routines (CPOL=1, CPHA=1) just for TMC stepper drivers
#pragma once

typedef struct DriverPins {
  int8_t m0;
  int8_t m1;
  int8_t m2;
  int8_t m3;
  int8_t decay;
} DriverPins;

#define mosi m0
#define sck  m1
#define cs   m2
#define miso m3

class SoftSpi {
  public:
    // create and set pin values
    SoftSpi(DriverPins Pins) : Pins{ Pins } {}

    // check pins and report status
    bool init();
    // setup pins and activate CS
    void begin();
    // cycle CS to reset conversation
    void pause();
    // set CS high to disable conversation
    void end();
    // send/receive an 8 bit value
    uint8_t transfer(uint8_t data_out);
    // send/receive an 32 bit value
    uint32_t transfer32(uint32_t data_out);

  private:
    const DriverPins Pins = {OFF, OFF, OFF, OFF};
};

bool SoftSpi::init() {
  if (Pins.cs == OFF || Pins.sck == OFF || Pins.cs == OFF) return false; else return true;
}

void SoftSpi::begin()
{
  pinMode(Pins.cs,OUTPUT);
  digitalWrite(Pins.cs,HIGH);  delaySpi();
  pinMode(Pins.sck,OUTPUT);
  digitalWrite(Pins.sck,HIGH);
  pinMode(Pins.miso,INPUT);
  pinMode(Pins.mosi,OUTPUT);   delaySpi();
  digitalWrite(Pins.cs,LOW);   delaySpi();
}

void SoftSpi::pause() {
  digitalWrite(Pins.cs, HIGH); delaySpi();
  digitalWrite(Pins.cs, LOW);  delaySpi();
}

void SoftSpi::end() {
  digitalWrite(Pins.cs, HIGH); delaySpi();
}

uint8_t SoftSpi::transfer(uint8_t data_out)
{
  uint8_t data_in = 0;
  for(int i=7; i >= 0; i--)
  {
    digitalWrite(Pins.sck,LOW);
    digitalWrite(Pins.mosi,bitRead(data_out,i));  delaySpi();
    digitalWrite(Pins.sck,HIGH);
    bitWrite(data_in,i,digitalReadEx(Pins.miso)); delaySpi();
  }
  return data_in;
}

uint32_t SoftSpi::transfer32(uint32_t data_out)
{
  uint32_t data_in = 0;
  for(int i=31; i >= 0; i--)
  {
    digitalWrite(Pins.sck,LOW);
    digitalWrite(Pins.mosi,bitRead(data_out,i)); delaySpi();
    digitalWrite(Pins.sck,HIGH);
    bitWrite(data_in,i,digitalReadEx(Pins.miso)); delaySpi();
  }
  return data_in;
}
