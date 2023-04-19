#include "SPI.h"

#define SCLK 13
#define MISO 12
#define MOSI 11
#define SS1 10
#define SS2 9
#define SS3 8
#define SS4 7
#define SS5 6
#define SS6 5
/*
byte but[2];
byte c;
*/
void setup() {
  SPI.begin();            // запускаем SPI
  Serial.begin(9600);
  for (int i = 5; i < 11; i++){
    pinMode(i, INPUT_PULLUP);
    if (digitalRead(i) == HIGH) {
      Serial.write(i);
      pinMode(i, OUTPUT);
      digitalWrite(i, HIGH);
    }
  }
}

void loop() {
  
}

/*void button() {
  static unsigned long timer;
  if (timer + 100 > millis()) return;  // опрос кнопок каждые 100 мс
  but[0] = but[1];
  but[1] = digitalRead(BUT);
  if (but[0] && !but[1]) {            // нажали
    digitalWrite(SS, LOW);
    c = SPI.transfer(1);              // отправляем единичку, получаем байт в ответ
    digitalWrite(SS, HIGH);
    Serial.println(c);                // печатаем полученный байт (счетчик) в монитор
  } else if (!but[0] && but[1]) {     // отжали
    digitalWrite(SS, LOW);
    c = SPI.transfer(0);              // отправляем нолик, получаем байт в ответ
    digitalWrite(SS, HIGH);
    Serial.println(c);                // печатаем полученный байт (счетчик) в монитор
  }
  timer = millis();
}*/
