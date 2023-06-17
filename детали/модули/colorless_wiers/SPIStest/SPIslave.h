#include <SPI.h>
#define SPI_END_MESSAGE 123
#define SCLK 13
#define MISO 12
#define MOSI 11

long long bufferIn;
long long bufferOut;
bool received = false;

void SPICommSetup(){
  pinMode(MISO,OUTPUT);               
  SPCR |= _BV(SPE);
  SPDR = 0;
  bufferIn = 0;
  bufferOut = 0;                 
  received = false;
  SPI.attachInterrupt(); 
  }

ISR (SPI_STC_vect)                        
{
  if (SPDR == SPI_END_MESSAGE)
  {
    received = true;
  }else{
    bufferOut = bufferOut<<8; 
    bufferOut |= SPDR;
    } 
  SPDR = bufferIn & 0xFF;
  bufferIn = bufferIn >> 8;
}

byte SPICommSend(byte type, byte message){
  if (bufferIn >> (8*5) > 0) return 101;
  bufferIn = (((((bufferIn << 8) | SPI_END_MESSAGE) << 8) | (message)) << 8 )|(type);
  return 0;
}

int SPICommLoop(){
 if (received){
 received = false;
  int message = bufferOut & 0xFFFF;
  bufferOut = bufferOut >> 16;
  return message;
  }
  return 0;
  }
