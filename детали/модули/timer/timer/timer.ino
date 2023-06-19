#include <QuadDisplay.h>

#define DISPLAY_PIN 9
#include <Wire.h>
long time_play = 6000000;
long start_tick_time;
long cur_time;
byte mistakes = 0;

int minutes = 0;
int seconds = 0;
int mils = 0;
bool is_exploded = false;
bool is_solved = false;
bool game_stoped = false;
void setup() {
  Wire.begin(8);
  Serial.begin(9600);// join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent);
}

void loop() {
  display_update();
  if(!is_exploded && !is_solved){
  if (mils<40 && mils>30 && mistakes != 2) tone(2,2217);
  else if ((mils<10)) tone (2,1865);
  else noTone(2);
  }else{
    noTone(2);
    }

  if (is_exploded){
      tone(2,69,200);
      delay(210);
      tone(2,65,800);
      game_stoped = true;      
      
    while(true){
      displayClear(DISPLAY_PIN);
      delay(500);
      display_update();
      delay(500);
      }
      }
    
    
    if(is_solved && !game_stoped){
      tone(2,262,200);
      delay(260);
      tone(2,262,200);
      delay(210);
      tone(2,392,600);
      delay(660);
      
      game_stoped = true;      
      }
}
void display_update(){
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
  time_update();
  byte answer = (minutes / 10 == 1 || minutes % 10 == 1 || seconds / 10 == 1 || seconds % 10 == 1 || mils / 10 == 1 || mils % 10 == 1);
       answer = answer << 1 |(minutes / 10 == 4 || minutes % 10 == 4 || seconds / 10 == 4 || seconds % 10 == 4 || mils / 10 == 4 || mils % 10 == 4); 
       answer = answer << 1 |(minutes / 10 == 5 || minutes % 10 == 5 || seconds / 10 == 5 || seconds % 10 == 5 || mils / 10 == 5 || mils % 10 == 5);
       answer = answer << 1 | (time_play <= 0);
  Serial.print("time_play:");
  Serial.print(time_play);
  Serial.print("sendet:");
  Serial.println(answer,BIN);
  Wire.write(answer); // respond with message of 6 bytes
}

void receiveEvent(int howMany) {
  byte x = 0;
  while(Wire.available()){
  x = Wire.read();}
  Serial.print("received:");// receive byte as an integer
  Serial.println(x&0b00011111 *30*1000,BIN);
  
  if(x >> 5 == 0b100){time_play = (long)(x&0b00011111) * 30 *1000;}
  if(x >> 5 == 0b011){
    is_solved = x&1;
    is_exploded = x&0b10;
    mistakes = (x>>2)& 0b11;
  }
}
