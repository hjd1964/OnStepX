// -----------------------------------------------------------------------------------
// Simple software SPI routines (CPOL=1, CPHA=1) just for TMC stepper drivers

#include "../../Common.h"
#include "SoftSpi.h"

bool SoftSpi::init(int16_t mosi, int16_t sck, int16_t cs, int16_t miso) {
  this->miso = miso;
  this->mosi = mosi;
  this->sck = sck;
  this->cs = cs;

  VF("MSG: SoftSpi, init MOSI="); V(mosi); VF(", SCK="); V(sck); VF(", CS="); V(cs); VF(", MISO="); VL(miso);

  if (cs == OFF || sck == OFF || cs == OFF) return false; else return true;
}

void SoftSpi::begin() {
  pinModeEx(cs, OUTPUT);
  digitalWriteEx(cs, HIGH); delayMicroseconds(1);
  pinMode(sck, OUTPUT);
  digitalWriteF(sck, HIGH);
  pinMode(miso, INPUT);
  pinMode(mosi, OUTPUT);   delayMicroseconds(1);
  digitalWriteEx(cs, LOW); delayMicroseconds(1);
}

void SoftSpi::pause() {
  digitalWriteEx(cs, HIGH); delayMicroseconds(1);
  digitalWriteEx(cs, LOW);  delayMicroseconds(1);
}

void SoftSpi::end() {
  digitalWriteEx(cs, HIGH); delayMicroseconds(1);
}

uint8_t SoftSpi::transfer(uint8_t data_out)
{
  uint8_t data_in = 0;
  for(int i = 7; i >= 0; i--) {
    digitalWriteF(sck, LOW);
    digitalWriteF(mosi, bitRead(data_out,i)); delayMicroseconds(1);
    digitalWriteF(sck, HIGH);
    bitWrite(data_in, i, digitalReadF(miso)); delayMicroseconds(1);
  }
  return data_in;
}

uint32_t SoftSpi::transfer32(uint32_t data_out)
{
  uint32_t data_in = 0;
  for(int i = 31; i >= 0; i--) {
    digitalWriteF(sck, LOW);
    digitalWriteF(mosi, bitRead(data_out,i)); delayMicroseconds(1);
    digitalWriteF(sck, HIGH);
    bitWrite(data_in, i, digitalReadF(miso)); delayMicroseconds(1);
  }
  return data_in;
}
