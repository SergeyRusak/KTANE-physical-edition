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

void SPICommSetup(){
  SPI.begin();
  for (int i = 0; i<6;i++){
    pinMode(slaveid[i],OUTPUT);
    }
}
byte SPICommSend(int slaveid, int message){
  bufferOut[slaveid] = (((((bufferOut[slaveid] <<8) | SPI_END_MESSAGE) <<8) | (message)) <<(message <= 0xFF )?(8):(16));
  return 0;
  }

int SPICommRead(int slaveid){
  bufferIn[slaveid] = bufferIn[slaveid] >> 8;
  int data = bufferIn[slaveid] & 0xFF;
  bufferIn[slaveid] = bufferIn[slaveid] >> 8;
  if ((bufferIn[slaveid]&0xFF != SPI_END_MESSAGE) && (bufferIn[slaveid]&0xFF != 0)){
    data = (data << 8) | bufferIn[slaveid] & 0xFF;
    bufferIn[slaveid] = bufferIn[slaveid] >> 8;
    }
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
    for (int i = 5; i>=0; i--){
      stat  = stat << 1;
      stat |= (bufferIn[i] != SPI_END_MESSAGE)?(1):(0);                
    }
    return stat;
    }


void setup() {
            // запускаем SPI
  Serial.begin(9600);
  SPICommSetup();
  delay(1000); 
}

void loop() {
  byte stat = SPICommLoop();
  for (int i = 0; i<8; i++){

    if (stat & 1 == 1){

      int data = SPICommRead(i);
      byte type = data >> (data > 0xFF)?(8+4):(4); 
      int message = (type>>3 == 1)?(data & 0x0FFF):(0);
      if(type >> 3 ==1){
        switch (type & 0b0111)
      {
     case 1:
        //get tags
        break;  
      case 2:
        //get time
        break;
      case 3:
        //get misstakes
        break;
      case 4:
        //get serial number
        break;
      case 5:
        //get ports
        break;
      case 6:
        //get battery
        break;
      case 7:
        //get shutdown
        break;
      default:

        break;
      }
        }else{

          
          switch (type & 0b0111)
      {
      case 1:
        SPICommSend(i,((((1<<3)|1)<<4)<<8)|256);
        break;  
      case 2:
        //send time
        break;
      case 3:
        //send misstakes
        break;
      case 4:
        SPICommSend(i,((((1<<3)|1)<<4)<<8)|256);
        break;
      case 5:
        //send ports
        break;
      case 6:
        //send battery
        break;
      case 7:
        //send shutdown
        break;
      default:

        break;
      }
          
          }  
      }
    stat = stat >> 1;
  }


  //TODO: timer, sound effects + random generations
}
