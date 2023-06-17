#include <Wire.h>

byte modules_address[5];
byte modules_count = 0;
byte modules_solved = 0;
byte x = 0;
void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output

  scannModules(0,60);
}

void loop() {

Wire.beginTransmission(modules_address[0]); // transmit to device #8
  Wire.write(x);              // sends one byte
  Wire.endTransmission();    // stop transmitting
  
  Wire.requestFrom(modules_address[0], 1);    // request 6 bytes from slave device #8
  while (Wire.available()) { // slave may send less than requested
    
    byte c = Wire.read();
    Serial.print("(");
    Serial.print(c); // print the number
    Serial.print("=");
    Serial.print(x);  
    Serial.print(")");
    Serial.println((c==x)?("True"):("false"));  
  }
  x++;
  delay(500);
 }

byte scannModules (byte start_addr, byte end_addr){
  Serial.println("Scanning...");
  for (byte address = start_addr; address <= end_addr; ++address) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      Serial.print(address, HEX);
      Serial.println("  !");
      if (address != 11){
        modules_address[modules_count]= address;
        modules_count++;
        }
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      Serial.println(address, HEX);
    }
  }
  if (modules_count == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.println("done\n");
  }
  
  return modules_count;
  }
 
