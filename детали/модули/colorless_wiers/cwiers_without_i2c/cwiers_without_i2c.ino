#include <FastLED.h>
#define MAX_WIERS 6
#define START_WIRE 4
#define DATA_PIN 3

char* wiers;
CRGB strip[MAX_WIERS+1]= {CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black};
int wiers_cnt = 0;
int ignore_wire[MAX_WIERS]={0,0,0,0,0,0};
int right_wire;
bool is_mistake = false;
bool is_solve = false;
bool is_error = false;

int detect() {
  int cW(0), cR(0), cG(0), cB(0), cY(0);
  for (int i = 0; i < wiers_cnt; i++) {
    if (wiers[i] == 'w') {
      cW += 1;
    } else {
      if (wiers[i] == 'r') {
        cR += 1;
      } else {
        if (wiers[i] == 'g') {
          cG += 1;
        } else {
          if (wiers[i] == 'b') {
            cB += 1;
          } else {
            if (wiers[i] == 'y') {
              cY += 1;
            } else {
              is_error = true;
              Serial.println("color defect!");
            }
          }
        }
      }
    }
  }
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

void generate_wiers() {
  wiers_cnt = random(3, 7);
  wiers = new char[wiers_cnt];
  for (int i= wiers_cnt; i<MAX_WIERS; i++){
    ignore_wire[i]=1;
    }
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(strip, MAX_WIERS+1);
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
  }
  FastLED.show();
  right_wire = detect();
}


void mistake(){
  tone(2, 262, 500);
  if(!is_solve){
    is_mistake= true;
    strip[MAX_WIERS]=CRGB::Red;
    FastLED.show();
    }
    
  }

void solve(){
  tone(2, 880, 500);
  is_solve = true;
  strip[MAX_WIERS]=CRGB::Green;
  FastLED.show();
  }










void setup() {
  randomSeed(analogRead(A0));
}

void loop() {
  if (right_wire ==0)generate_wiers();
  else{
    for (int i = START_WIRE; i < START_WIRE + wiers_cnt; i++) {
        if(ignore_wire[i-START_WIRE]==1)continue;
        if(digitalRead(i)==HIGH){
          ignore_wire[i-START_WIRE]=1;
          if(i-START_WIRE == right_wire){solve();}else{mistake();}
          }
        
        }
    
    
    }
delay(100);
}
