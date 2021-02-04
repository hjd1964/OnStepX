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
    SoftSpi(DriverPins Pins) : Pins{ Pins } {}

    bool init() {
      Serial.begin(115200);
      Serial.println();
      Serial.print("Pin m0 = "); Serial.println(Pins.m0);
      Serial.print("Pin m1 = "); Serial.println(Pins.m1);
      Serial.print("Pin m2 = "); Serial.println(Pins.m2);
      delay(100);
      if (Pins.cs == OFF || Pins.sck == OFF || Pins.cs == OFF) return false; else return true;
    }

    void begin()
    {
      pinMode(Pins.cs,OUTPUT);
      digitalWrite(Pins.cs,HIGH);  delaySpi();
      pinMode(Pins.sck,OUTPUT);
      digitalWrite(Pins.sck,HIGH);
      pinMode(Pins.miso,INPUT);
      pinMode(Pins.mosi,OUTPUT);   delaySpi();
      digitalWrite(Pins.cs,LOW);   delaySpi();
    }
    
    void pause() {
      digitalWrite(Pins.cs, HIGH); delaySpi();
      digitalWrite(Pins.cs, LOW);  delaySpi();
    }
    
    void end() {
      digitalWrite(Pins.cs, HIGH); delaySpi();
    }
    
    uint8_t transfer(uint8_t data_out)
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
    
    uint32_t transfer32(uint32_t data_out)
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
  private:
    const DriverPins Pins = {OFF, OFF, OFF, OFF};
};
