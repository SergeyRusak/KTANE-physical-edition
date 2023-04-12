#include "SPI.h"

#define BUT 7     // кнопка
byte but[2];
byte c;

void setup() {
  pinMode(BUT, INPUT);
  SPI.begin();            // запускаем SPI
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);
  Serial.begin(9600);
}

void loop() {
  button();
  // тут размещаем любую обычную программу
}

void button() {
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
}
