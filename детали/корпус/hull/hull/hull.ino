#include <QuadDisplay.h>
#define DISPLAY_PIN 9
#include <Wire.h>

byte modules_address[5];
byte modules_count = 0;
byte modules_solved = 0;
byte mistakes_count = 2;
byte x = 0;
long time_play;
long last_millis = 0;
void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  time_play = 60000;
  scannModules(0,60);
}

void loop() {
displayClear(DISPLAY_PIN);
Wire.beginTransmission(modules_address[0]); // transmit to device #8
  Wire.write(x);              // sends one byte
  Wire.endTransmission();    // stop transmitting
  
  Wire.requestFrom(modules_address[0], 1);    // request 6 bytes from slave device #8
  while (Wire.available()) { // slave may send less than requested
    
    byte c = Wire.read();
  }
  x++;

  time_play -= ((millis()-last_millis)*(1+(0.25*mistakes_count)));
  last_millis = millis();
  int minutes = (time_play>0)?(time_play/1000/60):(0);
  int seconds = (time_play>0)?(time_play/1000 - (minutes*60)):(0);
  int mils = (time_play>0)?((time_play - (time_play/1000)*1000)/10):(0);
  if (minutes == 0){displayInt(DISPLAY_PIN, seconds*100+mils,true);}else{displayInt(DISPLAY_PIN, minutes*100+seconds,true);}
  if (mils<50 && mils>30 && mistakes_count != 2) tone(2,2217);
  else if ((mils<10)) tone (2,1865);
  else noTone(2);
  delay(50);
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
 
