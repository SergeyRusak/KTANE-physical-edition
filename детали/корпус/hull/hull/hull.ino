#include <Wire.h>
#define SOLVED 0b1
#define EXPLODED 0b10
#define ONE_MISS 0b100
#define TWO_MISS 0b1000
bool is_gameover = false;
byte c = 0;
void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);
  
  Wire.beginTransmission(8);
  Wire.write(0b10000000|10);              // sends time
  Wire.endTransmission();// start serial for output
}

void loop() {
  Wire.requestFrom(8, 1);    // request 6 bytes from slave device #8
  while (Wire.available()) { // slave may send less than requested
    c = Wire.read(); // receive a byte as character
    Serial.println(c,BIN);
    }
  if (c & 1 == 1 && !is_gameover){ 
    Wire.beginTransmission(8);
    Wire.write(0b01100000 | EXPLODED );              // sends one byte
    Wire.endTransmission();
    is_gameover = true;
    }
  

  delay(50);
}
