#include "SPI.h"

#define SPI_END_MESSAGE 123
#define SCLK 13
#define MISO 12
#define MOSI 11
#define SS1 10
#define SS2 9
#define SS3 8
#define SS4 7
#define SS5 6
#define SS6 5


int slaveid[6] = {10,9,8,7,6,5};
unsigned long long bufferIn[6] = {0,0,0,0,0,0};
unsigned long long bufferOut[6] = {0,0,0,0,0,0};

SPICommSetup(){
  SPI.begin();
  for (int i = 0; i<6;i++){
    pinMode(slaveid[i],OUTPUT);
    }
}
byte SPICommSend(int slaveid, byte type, byte message){
  bufferOut[slaveid] = (((((bufferOut[slaveid] <<8) | SPI_END_MESSAGE) <<8) | (message)) <<8) | (type);
  return 0;
  }

int SPICommRead(int slaveid){
  bufferIn[slaveid] = bufferIn[slaveid] >> 8;
  int data = bufferIn[slaveid] & 0xFFFF;
  bufferIn[slaveid] = bufferIn[slaveid] >> 16;
  return data;
  }


byte SPICommLoop(){

  for (int i = 0; i<6; i++){
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite (slaveid[i], LOW);
    while(true){
    if (bufferOut[i]==0 && (bufferIn[i]& 0xFF == SPI_END_MESSAGE)) break;
        byte tdata = bufferOut[i] & 0xFF;
        bufferOut[i] = bufferOut[i] >> 8;
        byte rdata = SPI.transfer(tdata);
        if (rdata!=0) bufferIn[i] = (bufferIn[i]<<8) | rdata;
    }
    digitalWrite (slaveid[i], HIGH);
    SPI.endTransaction();
    }
    byte stat = 0;
    for (int i = 0; i<6; i++){
      stat  = stat << 1;
      stat |= (bufferIn[i] != 0)?(1):(0);                
    }
    return stat;
    }
