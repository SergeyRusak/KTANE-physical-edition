#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define SOLVED 0b1
#define SHUTDOWN 0b10
#define ONE_MISS 0b100
#define TWO_MISS 0b1000
#define TIME_PIN A1

byte ports_and_batteries = 0;

bool is_gameover = false;
bool is_error = false;
bool is_victory = false;

String tags[11] {"SND", "CLR", "CAR", "IND", "FRQ", "SIG", "NSA", "MSA", "TRN", "BOB", "FRK"};
byte activeTagsIDs[4];
bool tagsLights[4];

char letters[25] {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Z'};
char serial[6];

bool lastEven = false;
bool hasVowels = false;
bool hasLetters = false;

byte mistakes = 0;
byte c = 0;
byte modules[6];
byte solved[6];
int module_quantity = 0;
int solved_quantity = 0;
int loops = 0;
int stage_time = 0;

void setup() {
  stage_time = (analogRead(TIME_PIN)/53) + 1;
  Serial.println(stage_time);
  for (int i = 13; i > 4; i--){
    pinMode(i, OUTPUT);
  }
  delay(100);
  Wire.begin();       
  Serial.begin(9600);
  randomSeed(analogRead(0));
  
  generate_periphery();
  moduleSearch();
  if (!is_error){
    moduleInit();
  }
}

void loop() {
  if (!is_gameover && !is_error && !is_victory){
    for (int i = 0; i < module_quantity; i++){
      Wire.requestFrom(modules[i], 1);  
      while (Wire.available()) {
        c = Wire.read(); 
        Serial.print(modules[i]);
        Serial.print(": ");
        Serial.println(c,BIN);
      }
      if (modules[i] == 45) {
        for (int j = 0; j < module_quantity; j++){
          if (modules[j] >= 25 && modules[j] < 30) {
            Wire.beginTransmission(modules[j]);
            Wire.write(c | 0b01000000);
            Wire.endTransmission();
          }
        }
        if (c & 1 == 1 && !is_gameover){ 
          Serial.println("Time's out!");
          gameover();
        }
      } else {
        if (c&1) error(modules[i]);
        if ((c>>1)&1) solve(modules[i]);
        byte mistakes = c>>2 & 0b00111111;
        if (mistakes > 0) mistake(mistakes);
      }
    }
    Serial.println();
    loops ++;
  }
  delay(50);
}

void moduleInit(){
  for (int i = 0; i < module_quantity; i++){  //Перебирает все подключённые модули
    Serial.print("Module ID = ");
    Serial.print(modules[i]);
    Serial.print("/ ");
    if (modules[i] == 45) {
      Wire.beginTransmission(45);
      Wire.write(0b10000000 | stage_time);
      Wire.endTransmission();
      Serial.println("Timer data sent.");
    }
    //Условия для других модулей добавляются по мере создания
    if (modules[i] >= 20 & modules[i] <25){
      Wire.beginTransmission(modules[i]);
      Wire.write(0b10000000 | (lastEven & 0b1));
      Wire.endTransmission();
      Serial.print("Wires data sent.");
      Serial.println();
    }
    if (modules[i] >= 25 & modules[i] <30){
      Wire.beginTransmission(modules[i]);
      byte msg = 0b10000000;
      for (int i = 0; i < 5; i++) {
        switch (activeTagsIDs[i]){
          case 2:
            msg |= 0b10;
            break;
          case 10:
            msg |= 0b1;
            break;
        }
      }
      msg |= (ports_and_batteries >> 4) & 0b00001100;
      Wire.write(msg);
      Wire.endTransmission();
      Serial.println("Button data sent.");
    }
    if (modules[i] >= 30 & modules[i] <35){
      Wire.beginTransmission(modules[i]);
      Wire.write(0b10000000);
      Wire.endTransmission();
      Serial.println("Simon data sent.");
    }
    if (modules[i] >= 35 & modules[i] <40){
      Wire.beginTransmission(modules[i]);
      Wire.write(0b10000000);
      Wire.endTransmission();
      Serial.println("Morse data sent.");
    }
    if (modules[i] >= 40 & modules[i] <45){
      Wire.beginTransmission(modules[i]);
      Wire.write(0b10000000);
      Wire.endTransmission();
      Serial.println("Memory data sent.");
    }
  }
  Serial.println();
}

void error(byte id){
  is_error = true;
  for (byte i = 0; i < module_quantity; i++){
    if (modules[i] != id){
      Wire.beginTransmission(i);
      Wire.write(0b01100000 | SHUTDOWN );   
      Serial.print("Error. Status ");
      Serial.println(0b01100000 | SHUTDOWN);         
      Wire.endTransmission();
    }
  }
}

void moduleSearch(){
  for (byte I = 20; I < 45; I++){
    //Serial.print("Trying ID = ");
    //Serial.print(I);
    Wire.beginTransmission(I);
    int response = Wire.endTransmission();
    if (response == 0) {
      //Serial.println("Found module!");
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
    error(45);
  }
}

void mistake (byte quantity){
  if (mistakes + quantity < 3) {
    mistakes += quantity;
    byte message = 0b01000000 | mistakes;
    for (int i = 0; i< module_quantity; i++){ //Рассылка о количестве ошибок по всем модулям
      if (modules[i] == 45 || (modules[i] > 30 && modules[i] < 35)) {
        Wire.beginTransmission(modules[i]);
        Wire.write(message);
        Wire.endTransmission();
      }
    }
    Serial.print("MISTAKE! ");
    Serial.print(mistakes);
    Serial.println(" mistakes made!");
  }
  else gameover();
}

void gameover () {
  is_gameover = true;
  Serial.print("SHUTDOWN. Status ");
  Serial.println(0b01100000 | SHUTDOWN);         
  for (int i = 0; i<module_quantity; i++) {
    Wire.beginTransmission(modules[i]);
    Wire.write(0b01100000 | SHUTDOWN );   
    Wire.endTransmission();
  }
}

void solve(byte ID){
  bool already_solved = false;
  if (solved_quantity != 0) for (int i = 0; i < solved_quantity; i++){  //Проходим по списку решённых (если они есть)
    if (solved[i] == ID) already_solved = true; //Если нашли проверяемый модуль - пропускаем следующую часть функции.
  }
  if (!already_solved) {
    Serial.print("Solved module ID - ");
    Serial.println(ID);
    solved_quantity ++; //+1 к количеству решённых 
    if (solved_quantity == module_quantity-1) victory();  //Если решены все модули - победа
    else solved[solved_quantity -1] = ID; //Если нет - заносим решённый модель в список
  }
}

void victory() {
  is_victory = true;
  Serial.println("VICTORY!!!");
  for (int i = 0; i < module_quantity; i++){
    Wire.beginTransmission(modules[i]);
    Wire.write((byte)0b01100001);
    Wire.endTransmission();
  }
}

void generate_periphery() {
  Serial.print("Генерирую батареи и порты: код генерации ");
  generate_ports_and_batteries();
  Serial.print("Генерирую серийный номер: ");
  generate_serial();
  Serial.print("Генерирую индикаторы: ");
  generate_indicators();
  Serial.println();
}

void generate_ports_and_batteries() {
  ports_and_batteries = random(256);
  Serial.println(ports_and_batteries);
  
}

void generate_serial() {
  char ch;
  while (!hasLetters) {
    for (int i = 0; i < 6; i++) {
      int option = random(2);
      if (option == 0 && i!=5){
        ch = letters[random(26)];
        serial[i] = ch;
        if (!hasLetters) hasLetters = true;
        if (ch == 'A' || ch == 'E' || ch == 'I' || ch == 'O' || ch == 'U') hasVowels = true;
      }
      else {
        ch = (random(10) + '0');
        serial[i] = ch;
        if (i == 5 && (ch == '2' || ch == '4' || ch == '6' || ch == '8' || ch == '0')) lastEven = true;
      }
    Serial.println(ch);
    }
  }
  for (int i = 0; i < 6; i++) {
    Serial.print(serial[i]);
  }
  Serial.println();
  if (lastEven) {
    Serial.print("Last digit is even: ");
    Serial.println(serial[5]);
  }
}

void generate_indicators() {
  bool existFlag = false;
  byte amount = random(5);
  for (int i = 0; i < amount; i++) {
    byte ID = random(11);
    for (int j = 0; j < i; j++) {
      if (activeTagsIDs[j] == ID) existFlag = true;
    }
    if (!existFlag) activeTagsIDs[i] = ID;
    else existFlag = false;
    byte active = random(2);
    tagsLights[i] = active;
  }
  if (amount > 0) {
    for (int i = 0; i < amount; i++) {
      Serial.print(tags[activeTagsIDs[i]]);
      Serial.print(" - ");
      Serial.print(tagsLights[i]);
      Serial.print(", ");
    }
  }
  else Serial.println("Индикаторов нет");
}
