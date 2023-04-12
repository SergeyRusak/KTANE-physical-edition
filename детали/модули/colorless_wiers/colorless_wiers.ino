#include <FastLED.h>



#define DATA_PIN 0
#define CLOCK_PIN 13
#define LED_MISTAKE 3
#define LED_SOLVE 2
#define LED_ERROR 1
#define MAX_WIERS 6
#define START_WIRE 4



char* wiers;
CRGB strip[MAX_WIERS];
int wiers_cnt = 0;
int ignore_wire[MAX_WIERS]={};
int right_wire;
bool is_mistake = false;
bool is_solve = false;
bool is_error = false;






int detect() {
  int cW(0), cR(0), cG(0), cB(0), cY(0);
  for (int i = 0; i < wiers_cnt; i++) {
    Serial.print(wiers[i] + "=");
    if (wiers[i] == 'w') {
      cW += 1;Serial.print("White ");
    } else {
      if (wiers[i] == 'r') {
        cR += 1; Serial.print("Red ");
      } else {
        if (wiers[i] == 'g') {
          cG += 1; Serial.print("Green ");
        } else {
          if (wiers[i] == 'b') {
            cB += 1; Serial.print("Blue ");
          } else {
            if (wiers[i] == 'y') {
              cY += 1; Serial.print("Yellow ");
            } else {
              is_error = true;
            }
          }
        }
      }
    }
  }
  //Serial.println("cW:" + String(cW) + " cR:" + String(cR) + " cG:" + String(cG) + " cB:" + String(cB) + " cY:" + String(cY));
  switch (wiers_cnt) {
    case 3:
      if (cR == 0)return 1;
      if (wiers[2] == 'w') return 3;
      if (cB >= 2) {
        for (int i = wiers_cnt - 1; i >= 0; i--) {
          if (wiers[i] == 'b') return i;
        }
      }
      return 2;
      break;
    case 4:
      if (cR >= 2) return 3;
      if (cR == 0 && wiers[3] == 'y') return 0;
      if (cB == 1) return 1;
      if (cY >= 2) return 3;
      return 1;
      break;
    case 5:
      if (wiers[4] == 'g') return 3;
      if (cR == 1 && cY >= 2) return 0;
      if (cG == 0)return 1;
      return 0;
      break;
    case 6:
      if (cY == 0) return 2;
      if (cY == 1 && cW >= 2) return 3;
      if (cR == 0) return 5;
      return 3;
      break;
    default:
      is_error = true;
  }




}
void init_system() {
  //Serial.begin(9600);
  randomSeed(analogRead(A0));
  pinMode(LED_MISTAKE, OUTPUT);
  digitalWrite(LED_MISTAKE, HIGH);
  pinMode(LED_SOLVE, OUTPUT);
  digitalWrite(LED_SOLVE, HIGH);
  pinMode(LED_ERROR, OUTPUT);
  digitalWrite(LED_ERROR, HIGH);
}



void generate_wiers() {
  wiers_cnt = random(3, 7);
  wiers = new char[wiers_cnt];
  for (int i= wiers_cnt; i<MAX_WIERS; i++){
    ignore_wire[i]=1;
    }
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(strip, MAX_WIERS);
  FastLED.setBrightness(50);
  FastLED.clear(true);
  FastLED.show();
  for (int i = 0; i <wiers_cnt; i++) {
    pinMode(i+START_WIRE, INPUT_PULLUP);
    switch (random(0, 5)) {
      case 0:
      strip[i]= CRGB::White;
      wiers[i]='w';
        break;
      case 1:
      strip[i]= CRGB::Red;
      wiers[i]='r';
        break;
      case 2:
      strip[i]= CRGB::Green;
      wiers[i]='g';
        break;
      case 3:
      strip[i]= CRGB::Blue;
      wiers[i]='b';
        break;
      case 4:
      strip[i]= CRGB::Yellow;
      wiers[i]='y';
        break;
      default:
        is_error = true;
    }
    //Serial.print(wiers[i]);
  }
  //Serial.println();
  FastLED.show();
  right_wire = detect();
  //Serial.println("Right:" + String(right_wire));
}



void setup() {
  init_system();
  generate_wiers();
  delay(2000);



}
void mistake(){
  if(!is_solve){
    digitalWrite(LED_MISTAKE,LOW);
    is_mistake= true;
    }
  }
void solve(){
  is_solve = true;
  if(is_mistake){
    digitalWrite(LED_MISTAKE,HIGH);
  }
  digitalWrite(LED_SOLVE,LOW);
  
  }

void loop() {
  if(!is_error){
  for (int i = START_WIRE; i < START_WIRE + wiers_cnt; i++) {
    if(ignore_wire[i-START_WIRE]==1)continue;
    if(digitalRead(i)==HIGH){
      ignore_wire[i]=1;
      if(i-START_WIRE == right_wire){solve();}else{mistake();}}
    
    }
  }else{digitalWrite(LED_ERROR,LOW);}
  

}
