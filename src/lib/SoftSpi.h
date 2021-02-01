// -----------------------------------------------------------------------------------
// Simple software SPI routines (CPOL=1, CPHA=1)
#pragma once

class SoftSpi {
  public:
    bool init(int8_t mosiPin, int8_t sckPin, int8_t csPin, int8_t misoPin) {
      this->mosi = mosiPin;
      this->sck  = sckPin;
      this->cs   = csPin;
      this->miso = misoPin;

      if (cs == OFF || sck == OFF || cs == OFF) return false; else return true;
    }

    void begin()
    {
      pinMode(cs,OUTPUT);
      digitalWrite(cs,HIGH);  delaySpi();
      pinMode(sck,OUTPUT);
      digitalWrite(sck,HIGH); Y;
      pinMode(miso,INPUT);
      pinMode(mosi,OUTPUT);   delaySpi();
      digitalWrite(cs,LOW);   delaySpi(); Y;
    }
    
    void pause() {
      digitalWrite(cs, HIGH); delaySpi();
      digitalWrite(cs, LOW);  delaySpi();
    }
    
    void end() {
      digitalWrite(cs, HIGH); delaySpi();
    }
    
    uint8_t transfer(uint8_t data_out)
    {
      uint8_t data_in = 0;
      for(int i=7; i >= 0; i--)
      {
        digitalWrite(sck,LOW);
        digitalWrite(mosi,bitRead(data_out,i));  delaySpi();
        digitalWrite(sck,HIGH);
        bitWrite(data_in,i,digitalReadEx(miso)); delaySpi(); Y;
      }
      
      return data_in;
    }
    
    uint32_t transfer32(uint32_t data_out)
    {
      uint32_t data_in = 0;
      for(int i=31; i >= 0; i--)
      {
        digitalWrite(sck,LOW);
        digitalWrite(mosi,bitRead(data_out,i)); delaySpi();
        digitalWrite(sck,HIGH);
        if (miso >= 0) bitWrite(data_in,i,digitalRead(miso)); delaySpi(); Y;
      }

      return data_in;
    }
  private:
    int8_t cs = 0;
    int8_t sck = 0;
    int8_t miso = 0;
    int8_t mosi = 0;
};
