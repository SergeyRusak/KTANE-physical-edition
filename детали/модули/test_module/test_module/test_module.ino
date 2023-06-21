#include <Wire.h>

bool is_running = false;
bool is_solved = false;

void setup() {
  Wire.begin(20);
  Serial.begin(9600);
  Wire.onRequest(requestEvent); 
  Wire.onReceive(recieveEvent);
  Serial.println("Module is on. Awaiting init data.");
}

void loop() {
  
}

void requestEvent(){
  if (is_running){
    byte answer = 0b10000000;
    Serial.print("Recieved request. Sending message: ");
    Serial.print(answer);
    Serial.println(".");
    Wire.write(answer);
  }
}

void recieveEvent(int howMany){
  byte x = 0;
  while(Wire.available()){
    x = Wire.read();
  }

  if (x >> 5 == 0b100) {  //Если инициализационные данные
    Serial.println("Init data recieved.");
    is_running = true;
  }

  if (x >> 5 == 0b011) {  //Если системное сообщение
    Serial.println("System message recieved.");
  }
}
