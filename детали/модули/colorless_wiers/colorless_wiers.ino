#include <FastLED.h>
#include <SPI.h>


#define SPI_END_MESSAGE 123
#define SCLK 13
#define MISO 12
#define MOSI 11
#define DATA_PIN 0
#define LED_MISTAKE 3
#define LED_SOLVE 2
#define LED_ERROR 1
#define MAX_WIERS 6
#define START_WIRE 4


unsigned long long bufferIn;
unsigned long long bufferOut;
bool received = false;

char* wiers;
CRGB strip[MAX_WIERS+1];
int wiers_cnt = 0;
int ignore_wire[MAX_WIERS]={};
int right_wire;
bool is_mistake = false;
bool is_solve = false;
bool is_error = false;
bool is_setup_recive = true;
bool startg= false;
int tags = 0;
byte tags_H = 0;
int serial = 0;
byte serial_H = 0;


void SPICommSetup(){
  Serial.begin(9600);
  pinMode(MISO,OUTPUT);               
  SPCR |= _BV(SPE);
  SPDR = 0;
  bufferIn = 0;
  bufferOut = 0;                 
  received = false;
  SPI.attachInterrupt(); 
  }

ISR (SPI_STC_vect)                        
{
  if (SPDR == SPI_END_MESSAGE)
  {
    received = true;
  }else{
    bufferOut = bufferOut<<8; 
    bufferOut |= SPDR;
    } 
  SPDR = bufferIn & 0xFF;
  bufferIn = bufferIn >> 8;
}

byte SPICommSend(int message){
  if (bufferIn >> (8*5) > 0) return 101;
  bufferIn = ((((bufferIn << 8) | SPI_END_MESSAGE) << (message <= 0xFF)?(8):(16)) | message);
  return 0;
}

int SPICommLoop(){
 if (received){
 received = false;
  int message = bufferOut & 0xFF;
  if (!((message >> 7 == 0) && ((message & 0x0F) ==0))){
    message = bufferOut & 0xFFFF;
    bufferOut = bufferOut >> 8;
  }
  bufferOut = bufferOut >> 8;
  return message;
  }
  return 0;
  }


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
              Serial.println("color deyect!");
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



void generate_wiers(int tags_i, int serial_i) {
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
  }
  FastLED.show();
  right_wire = detect();
}



void setup() {
  init_system();
  SPICommSetup();      

  SPICommSend(((0<<3)|4)<<4); // request for serial
  SPICommSend(((0<<3)|1)<<4); // request for tags
}


void mistake(){
  if(!is_solve){
    digitalWrite(LED_MISTAKE,LOW);
    SPICommSend(((((1<<3)|3)<<4)<<8)|1);
    is_mistake= true;
    }
  }

void solve(){
  is_solve = true;
  SPICommSend(((((1<<3)|3)<<4)<<8)|2);
  if(is_mistake){
    digitalWrite(LED_MISTAKE,HIGH);
  }
  digitalWrite(LED_SOLVE,LOW);  
  }

void loop() {
  if(!is_error){
    
int m = SPICommLoop();
if (m !=0){
  byte qa = m >> (m > 0xFF)?(15):(7);
  byte type = (m >>(m > 0xFF)?(8):(4)) & 0b0111;
  byte mess = (m > 0xFF)?(m & 0x0FFFF):(0);
  
  if (qa = 1){
    if (type == 1){tags = mess; tags_H = 1;}
    if (type == 4){serial = mess; serial_H = 1;}
    }
    else{
      if (type = 7) {}
      }



  
  }
  if (tags_H !=0 && serial_H != 0 ){
      generate_wiers(tags, serial);
      startg = true;          
      }
    }

    if(startg){
      for (int i = START_WIRE; i < START_WIRE + wiers_cnt; i++) {
        if(ignore_wire[i-START_WIRE]==1)continue;
        if(digitalRead(i)==HIGH){
          ignore_wire[i-START_WIRE]=1;
          if(i-START_WIRE == right_wire){solve();}else{mistake();}
          }
        
        }
    }else{
    SPICommSend(((0<<3)|7)<<4);
    digitalWrite(LED_ERROR,LOW);
    }
  

}
