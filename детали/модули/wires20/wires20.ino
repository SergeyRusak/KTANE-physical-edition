#include <Wire.h>
#include <FastLED.h>
#define MAX_WIRES 6
#define START_WIRE 4
#define DATA_PIN 3

bool is_running = false;
bool is_solved = false;
bool is_error = false;
bool lastEven = false;

char* wires;
int wires_cnt = 0;
int ignore_wire[MAX_WIRES] = {0,0,0,0,0,0};
byte right_wire;
CRGB colors[5] = {CRGB::White, CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow};
CRGB strip[MAX_WIRES+1] = {0,0,0,0,0,0};
byte mistakes = 0;

int detect() {
  int cW(0), cR(0), cG(0), cB(0), cY(0);
  for (int i = 0; i < wires_cnt; i++) {
    switch (wires[i]){
      case 0: //белый - w
        cW += 1;
        break;
      case 1: //красный - r
        cR += 1;
        break;
      case 2: //зелёный - g
        cG += 1;
        break;
      case 3: //синий - b
        cB += 1;
        break;
      case 4: //жёлтый - y
        cY += 1;
        break;
      default:
        is_error = true;
        Serial.println("WIRES COLOR ERROR");
    }
  }
  switch (wires_cnt) {
    case 3:
      if (cR == 0) return 1;
      if (wires[2] == 0) return 3;
      if (cB >=2) {
        for (int i = wires_cnt - 1; i >= 0; i--) {
          if (wires[i] == 3) return i;
        }
      }
      return 2;
      break;
    case 4:
      if (cR >= 2 && !lastEven) return 3;
      if (cR == 0 && wires[3] == 4) return 0;
      if (cB == 1) return 1;
      if (cY >= 2) return 3;
      return 1;
      break;
    case 5:
      if (wires[4] == 2 && !lastEven) return 3;
      if (cR == 1 && cY >= 2) return 0;
      if (cG == 0) return 1;
      return 0;
      break;
    case 6:
      if (cY == 0 && !lastEven) return 2;
      if (cY == 1 && cW >= 2) return 3;
      if (cR == 0) return 5;
      return 3;
      break;
    default:
      is_error = true;
      Serial.println("WIRES COUNT ERROR");
  }
}

void generate_wires() {
  wires_cnt = random(3, 7);
  wires = new char[wires_cnt];
  for (int i = wires_cnt; i < MAX_WIRES; i++) {
    ignore_wire[i] = 1;
  }
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(strip, MAX_WIRES+1);
  FastLED.setBrightness(50);
  FastLED.clear(true);
  FastLED.show();
  for (int i = 0; i < wires_cnt; i++) {
    pinMode(i+START_WIRE, INPUT_PULLUP);
    byte rnd_col = random(0, 5);
    strip[i] = colors[rnd_col];
    wires[i] = rnd_col;
  }
  FastLED.show();
  right_wire = detect();
}

void mistake() {
  mistakes ++;
  if (!is_solved) {
    strip[MAX_WIRES] = CRGB::Red;
    FastLED.show();
  }
}

void solve() {
  is_solved = true;
  strip[MAX_WIRES] = CRGB::Green;
  FastLED.show();
}

void setup() {
  Wire.begin(20);
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  Wire.onRequest(requestEvent); 
  Wire.onReceive(recieveEvent);
  Serial.println("Module is on. Awaiting init data.");
}

void loop() {
  if (is_running) {
    if (right_wire == 0) generate_wires();
    else{
      for (int i = START_WIRE; i < START_WIRE + wires_cnt; i++) {
        if (ignore_wire[i-START_WIRE] == 1) continue;
        if (digitalRead(i) == HIGH) {
          ignore_wire[i-START_WIRE] = 1;
          if (i-START_WIRE == right_wire) {solve();}
          else {mistake();}
        }
      }
    }
    delay(100);
  }
}


void requestEvent(){
  if (is_running){
    byte answer = 0;
    answer |= (mistakes << 2) | (is_solved&1 << 1) | (is_error&1);
    Serial.print("Recieved request. Sending message: ");
    Serial.print(answer);
    Serial.println(".");
    Wire.write(answer);
    mistakes = 0;
  }
}

void recieveEvent(int howMany){
  byte x = 0;
  while(Wire.available()){
    x = Wire.read();
  }

  if (x >> 5 == 0b100) {  //Если инициализационные данные
    lastEven = x&1;
    Serial.println("Init data recieved.");
    is_running = true;
  }

  if (x >> 5 == 0b011) {  //Если системное сообщение
    Serial.println("System message recieved.");
    if ((x >> 1) & 1 == 1 || x & 1 == 1) is_running = false;
  }
}
