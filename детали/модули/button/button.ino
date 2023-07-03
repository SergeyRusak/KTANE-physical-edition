#include <Wire.h>
#include <FastLED.h>

//Подразумевается, что рядом с нужной надписью загорится лампочка нужного цвета.
#define BTN_PIN 7
#define LED_COUNT 4
#define STRIP_PIN 3

bool is_running = false;
bool is_solved = false;
bool is_error = false;
byte mistakes = 0;

bool has_CAR = false;
bool has_FRK = false;
byte batteries = 0;

byte time_data = 0;

long btn_pressed = 0; //Миллисекунд с начала работы модуля до нажатия кнопки
byte btnclr_ID = 0;   //ID цвета кнопки (0-4)
byte text_ID = 0;     //ID текста (0-3). По факту, номер светодиода на ленте
                      //Тексты по порядку: [Нажать, Держать, Стоп, Взорвать]
byte strclr_ID = 0;   //ID цвета полосы (0-4), им горят все светодиоды в ленте при длинном нажатии.

bool need_tapping = false;

CRGB colors[5] = {CRGB::White, CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow};
CRGB strip[LED_COUNT+1] = {0,0,0,0,0};

void generate() {
  btnclr_ID = random(5);
  strclr_ID = random(5);
  text_ID = random(4);
  FastLED.addLeds<WS2811, STRIP_PIN, RGB>(strip, LED_COUNT+1);
  FastLED.setBrightness(50);
  FastLED.clear(true);
  FastLED.show();
  strip[text_ID] = colors[btnclr_ID];
  FastLED.show();
  need_tapping = calculate();
}

bool calculate() {
  if (batteries > 1 && text_ID == 3) return true;
  else if (batteries > 2 && has_FRK) return true;
  else if (btnclr_ID == 1 && text_ID == 1) return true;
  else return false;
}

void setup() {
  Wire.begin(25);
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  pinMode(BTN_PIN, INPUT);
  Wire.onRequest(requestEvent); 
  Wire.onReceive(recieveEvent);
  Serial.println("Module is on. Awaiting init data.");
}

void loop() {
  if (is_running) {
    if (digitalRead(BTN_PIN) == HIGH) {
      btn_pressed = millis();
    }
  }
}

void requestEvent(){
  if (is_running){
    byte answer = 0;
    //Сборка ответа корпусу, количество ошибок с прошлого сообщения, рашено/не решено, есть ли системные 
    answer |= (mistakes << 2) | (is_solved&1 << 1) | (is_error&1);
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
    //Вписать сюда обработку иниц. данных
    Serial.println("Init data recieved.");
    has_FRK = (bool)(x & 0b1);
    has_CAR = (bool)((x >> 1) & 0b1);
    batteries = ((x >> 2) & 0b11);
    is_running = true;
  }

  if (x >> 5 == 0b011) {  //Если системное сообщение
    Serial.println("System message recieved.");
    if ((x >> 1) & 1 == 1 || x & 1 == 1) is_running = false;
  }

  if (x >> 5 == 0b010) {  //Если обновление данных
    Serial.println("Data update recieved.");
    time_data = (x >> 1) & 0b00001110;
  }
}
