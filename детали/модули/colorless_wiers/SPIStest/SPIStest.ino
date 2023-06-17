#include "SPIslave.h"


void setup()
{
 SPICommSetup();
}
void loop()
{
  int message = SPICommLoop();
  if (message !=0 && ((message >>8)& 0xFF)==100){
    byte i = message & 0xFF;
    SPICommSend(100,i+1);
    }
}
