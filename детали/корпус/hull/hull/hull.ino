#include <Wire.h>

#define SOLVED 0b1
#define SHUTDOWN 0b10
#define ONE_MISS 0b100
#define TWO_MISS 0b1000
bool is_gameover = false;
bool is_error = false;
bool is_victory = false;

byte mistakes = 0;
byte c = 0;
byte modules[6];
byte solved[6];
int module_quantity = 0;
int solved_quantity = 0;
int loops = 0;

void setup() {
  delay(100);
  Wire.begin();       
  Serial.begin(9600);
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
      Wire.write(0b10000001);
      Wire.endTransmission();
      Serial.println("Timer data sent.");
    }
    //Условия для других модулей добавляются по мере создания
    if (modules[i] >= 20 & modules[i] <25){
      Wire.beginTransmission(modules[i]);
      Wire.write(0b10000000);
      Wire.endTransmission();
      Serial.println("Wires data sent.");
    }
    if (modules[i] >= 25 & modules[i] <30){
      Wire.beginTransmission(modules[i]);
      Wire.write(0b10000000);
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
