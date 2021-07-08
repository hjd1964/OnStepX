// -----------------------------------------------------------------------------------
// Simple software SPI routines (CPOL=1, CPHA=1) just for TMC stepper drivers

#include "../../Common.h"
#include "SoftSpi.h"

bool SoftSpi::init(DriverPins pins) {
  this->pins = pins;

  VF("MSG: SoftSpi, init MOSI="); V(pins.mosi); VF(", SCK="); V(pins.sck); VF(", CS="); V(pins.cs); VF(", MISO="); VL(pins.miso);

  if (pins.cs == OFF || pins.sck == OFF || pins.cs == OFF) return false; else return true;
}

void SoftSpi::begin() {
  pinMode(pins.cs, OUTPUT);
  digitalWrite(pins.cs, HIGH);  delayMicroseconds(1);
  pinMode(pins.sck, OUTPUT);
  digitalWrite(pins.sck, HIGH);
  pinMode(pins.miso, INPUT);
  pinMode(pins.mosi, OUTPUT);   delayMicroseconds(1);
  digitalWrite(pins.cs, LOW);   delayMicroseconds(1);
}

void SoftSpi::pause() {
  digitalWrite(pins.cs, HIGH); delayMicroseconds(1);
  digitalWrite(pins.cs, LOW);  delayMicroseconds(1);
}

void SoftSpi::end() {
  digitalWrite(pins.cs, HIGH); delayMicroseconds(1);
}

uint8_t SoftSpi::transfer(uint8_t data_out)
{
  uint8_t data_in = 0;
  for(int i = 7; i >= 0; i--) {
    digitalWrite(pins.sck, LOW);
    digitalWrite(pins.mosi, bitRead(data_out,i)); delayMicroseconds(1);
    digitalWrite(pins.sck, HIGH);
    bitWrite(data_in, i, digitalReadEx(pins.miso)); delayMicroseconds(1);
  }
  return data_in;
}

uint32_t SoftSpi::transfer32(uint32_t data_out)
{
  uint32_t data_in = 0;
  for(int i=31; i >= 0; i--) {
    digitalWrite(pins.sck, LOW);
    digitalWrite(pins.mosi, bitRead(data_out,i)); delayMicroseconds(1);
    digitalWrite(pins.sck, HIGH);
    bitWrite(data_in, i, digitalReadEx(pins.miso)); delayMicroseconds(1);
  }
  return data_in;
}
