#include <QuadDisplay.h>
#include <Wire.h>

#define MISTAKE1 7
#define MISTAKE2 6
#define DISPLAY_PIN 9

long time_play = 0;
long start_tick_time;
long cur_time;
byte mistakes = 0;

int minutes = 0;
int seconds = 0;
int mils = 0;

bool is_exploded = false;
bool is_solved = false;
bool game_stopped = true;

void setup() {
  Wire.begin(45);
  pinMode(MISTAKE1, OUTPUT);
  pinMode(MISTAKE2, OUTPUT);
  Serial.begin(9600);
  Wire.onRequest(requestEvent); 
  Wire.onReceive(receiveEvent);
  Serial.println("Timer is on. Awaiting time data.");
}

void loop() {
  if (!game_stopped){
    display_update();
    if(!is_exploded && !is_solved){ //Если игра не завершена тем или иным образом
      if (mils<40 && mils>30 && mistakes < 2) tone(2,2217);  //Играет высокий тон
      else if ((mils<10)) tone (2,1865);  //Играет низкий тон
      else noTone(2); //замолкает
    } else {
      noTone(2);  //Если игра закончена - просто молчит
    }
  
    if (is_exploded){
      tone(2,69,200); //Играет мелодию проигрыша
      delay(210);
      tone(2,65,800);
      game_stopped = true; //Ставит флаг о проигрыше
      while(true){  //Уходит в постоянное мигание
        displayClear(DISPLAY_PIN);
        delay(500);
        display_update();
        delay(500);
      }
    }

    if(is_solved){  //Если игра выиграна
      tone(2,262,200);  //Играет победную мелодию
      delay(260);
      tone(2,262,200);
      delay(210);
      tone(2,392,600);
      delay(660);
      game_stopped = true;  //Останавливает игру    
    }
  }
}

void display_update(){
  displayClear(DISPLAY_PIN);
  displayInt(DISPLAY_PIN, (minutes!=0)?(minutes*100+seconds):(seconds*100+mils),true);
  }

  
void time_update(){
  if (is_exploded || is_solved) return;
  cur_time = start_tick_time;
  start_tick_time = millis();
  time_play -= (start_tick_time - cur_time) * (1 + 0.25*mistakes);
  minutes = (time_play>0)?(time_play/1000/60):(0);
  seconds = (time_play>0)?(time_play/1000 - (minutes*60)):(0);
  mils = (time_play>0)?((time_play - (time_play/1000)*1000)/10):(0);
  }

void requestEvent() {
  if (!game_stopped){
    time_update();
    byte answer = (minutes / 10 == 1 || minutes % 10 == 1 || seconds / 10 == 1 || seconds % 10 == 1 || mils / 10 == 1 || mils % 10 == 1);
         answer = answer << 1 |(minutes / 10 == 4 || minutes % 10 == 4 || seconds / 10 == 4 || seconds % 10 == 4 || mils / 10 == 4 || mils % 10 == 4); 
         answer = answer << 1 |(minutes / 10 == 5 || minutes % 10 == 5 || seconds / 10 == 5 || seconds % 10 == 5 || mils / 10 == 5 || mils % 10 == 5);
         answer = answer << 1 | (time_play <= 0);
    Serial.print("time remaining (ms.):");
    Serial.print(time_play);
    Serial.print("  / sending data:");
    Serial.println(answer,BIN);
    Wire.write(answer); 
  }
}

void receiveEvent(int howMany) {
  byte x = 0;
  while(Wire.available()){
    x = Wire.read();
  }
   
  if(x >> 5 == 0b100){
    start_tick_time = millis();
    Serial.print("received time data: ");
    time_play = (long)(x&0b00011111) * 30 *1000;
    Serial.println(time_play);
    
    game_stopped = false;
  }
  
  if(x >> 5 == 0b011){
    Serial.print("System message recieved. ");
    Serial.print("SOLVED: ");
    is_solved = x&1;
    Serial.print(is_solved);
    Serial.print("/ EXPLODED: ");
    is_exploded = x&0b10;
    Serial.print(is_exploded);
    Serial.print("/ Mistakes amount:");
    mistakes = (x>>2)& 0b11;
    Serial.print(mistakes);
  }

  if (x >> 5 == 0b010) {
    Serial.print("Mistakes arrived.");
    tone(2, 250, 250);
    byte ans_mistakes = x & 0b11111;
    for (int i = 0; i<ans_mistakes; i++) {
      mistakes ++;
      if (mistakes == 1) digitalWrite(MISTAKE1, HIGH);
      if (mistakes == 2) digitalWrite(MISTAKE2, HIGH);
    }
  }
}
