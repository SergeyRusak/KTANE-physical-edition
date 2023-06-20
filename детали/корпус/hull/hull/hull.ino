#include <Wire.h>
#define SOLVED 0b1
#define EXPLODED 0b10
#define ONE_MISS 0b100
#define TWO_MISS 0b1000
#define ERR 0b10000
bool is_gameover = false;
bool is_error = false;
byte c = 0;
byte modules[6];
int module_quantity = 0;
void setup() {
  Wire.begin();       
  Serial.begin(9600);
  moduleSearch();
  if (!is_error){
    Wire.beginTransmission(45);
    Wire.write(0b10000001);
    Wire.endTransmission();
    Serial.println("Time data sent.");
  }
}

void loop() {
  if (!is_gameover){
    Wire.requestFrom(45, 1);  
    while (Wire.available()) {
      c = Wire.read(); 
      Serial.println(c,BIN);
      }
    if (c & 1 == 1 && !is_gameover){ 
      Wire.beginTransmission(45);
      Wire.write(0b01100000 | EXPLODED );   
      Serial.print("Exploded. Status ");
      Serial.println(0b01100000 | EXPLODED);         
      Wire.endTransmission();
      is_gameover = true;
    }
  }
  delay(50);
}

void sendErrors(byte id){
  for (byte i = 0; i < module_quantity; i++){
    Wire.beginTransmission(i);
    Wire.write(0b01100000 | ERR );   
    Serial.print("Error. Status ");
    Serial.println(0b01100000 | ERR);         
    Wire.endTransmission();
    is_gameover = true;
    is_error = true;
  }
}

void moduleSearch(){
  for (byte I = 20; I < 45; I++){
    Wire.beginTransmission(I);
    int response = Wire.endTransmission();
    if (response == 0) {
      modules[module_quantity] = I;
      module_quantity += 1;
    }
  }
  
  Wire.beginTransmission(45);
  int response = Wire.endTransmission();
  if (response == 0) {
    modules[module_quantity] = 45;
    module_quantity += 1;
    Serial.print("Scanning complete. Found ");
    Serial.print(module_quantity);
    Serial.println(" modules.");
  }
  else {
    Serial.println("Error. Timer module was not found.");
    is_error = true;
    is_gameover = true;
    sendErrors(45);
  }
}
