#include "SPImaster.h"


void setup()
{
  Serial.begin(9600);
  SPICommSetup();
  
}
void loop()
{
  byte j = SPICommLoop();
  Serial.println(j);
  if (j>0){
    byte i = SPICommRead(0);
    Serial.print(i);
    SPICommSend(0,100,i+1);
    }
    delay(1000);
}
