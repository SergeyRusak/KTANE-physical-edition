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
    Serial.print("Recieved request. ");
    byte answer = 0;
    //формирование ответа корпусу
    Serial.print("Sending message: ");
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
    //Вписать сюда обработку иниц. данных
    Serial.println("Init data recieved.");
    is_running = true;
  }

  if (x >> 5 == 0b011) {  //Если системное сообщение
    Serial.println("System message recieved.");
    if ((x >> 1) & 1 == 1 || x & 1 == 1) is_running = false;  //При победе или сигнале на выключение - выключиться
  }

  if (x >> 5 == 0b010) {  //Если обновление данных (вроде таймера или количества ошибок)
    Serial.println("Data update recieved.");
    //Обновлять здесь данные, если такие есть.
  }
}
