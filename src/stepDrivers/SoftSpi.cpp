// -----------------------------------------------------------------------------------
// Simple software SPI routines (CPOL=1, CPHA=1) just for TMC stepper drivers
#include <Arduino.h>
#include "../../Constants.h"

#include "SoftSpi.h"

bool SoftSpi::init() {
  if (Pins.cs == OFF || Pins.sck == OFF || Pins.cs == OFF) return false; else return true;
}

void SoftSpi::begin()
{
  pinMode(Pins.cs, OUTPUT);
  digitalWrite(Pins.cs, HIGH);  delayMicroseconds(1);
  pinMode(Pins.sck, OUTPUT);
  digitalWrite(Pins.sck, HIGH);
  pinMode(Pins.miso, INPUT);
  pinMode(Pins.mosi, OUTPUT);   delayMicroseconds(1);
  digitalWrite(Pins.cs, LOW);   delayMicroseconds(1);
}

void SoftSpi::pause() {
  digitalWrite(Pins.cs, HIGH); delayMicroseconds(1);
  digitalWrite(Pins.cs, LOW);  delayMicroseconds(1);
}

void SoftSpi::end() {
  digitalWrite(Pins.cs, HIGH); delayMicroseconds(1);
}

uint8_t SoftSpi::transfer(uint8_t data_out)
{
  uint8_t data_in = 0;
  for(int i=7; i >= 0; i--)
  {
    digitalWrite(Pins.sck, LOW);
    digitalWrite(Pins.mosi, bitRead(data_out,i)); delayMicroseconds(1);
    digitalWrite(Pins.sck, HIGH);
    bitWrite(data_in, i, digitalReadEx(Pins.miso)); delayMicroseconds(1);
  }
  return data_in;
}

uint32_t SoftSpi::transfer32(uint32_t data_out)
{
  uint32_t data_in = 0;
  for(int i=31; i >= 0; i--)
  {
    digitalWrite(Pins.sck, LOW);
    digitalWrite(Pins.mosi, bitRead(data_out,i)); delayMicroseconds(1);
    digitalWrite(Pins.sck, HIGH);
    bitWrite(data_in, i, digitalReadEx(Pins.miso)); delayMicroseconds(1);
  }
  return data_in;
}
